/*	$NetBSD: lmtp_trouble.c,v 1.1.1.6 2005/12/01 21:44:26 rpaulo Exp $	*/

/*++
/* NAME
/*	lmtp_trouble 3
/* SUMMARY
/*	error handler policies
/* SYNOPSIS
/*	#include "lmtp.h"
/*
/*	int	lmtp_site_fail(state, code, format, ...)
/*	LMTP_STATE *state;
/*	int	code;
/*	char	*format;
/*
/*	int	lmtp_mesg_fail(state, code, format, ...)
/*	LMTP_STATE *state;
/*	int	code;
/*	char	*format;
/*
/*	void	lmtp_rcpt_fail(state, code, recipient, format, ...)
/*	LMTP_STATE *state;
/*	int	code;
/*	RECIPIENT *recipient;
/*	char	*format;
/*
/*	int	lmtp_stream_except(state, exception, description)
/*	LMTP_STATE *state;
/*	int	exception;
/*	char	*description;
/* DESCRIPTION
/*	This module handles all non-fatal errors that can happen while
/*	attempting to deliver mail via LMTP, and implements the policy
/*	of how to deal with the error. Depending on the nature of
/*	the problem, delivery of a single message is deferred, delivery
/*	of all messages to the same domain is deferred, or one or more
/*	recipients are given up as non-deliverable and a bounce log is
/*	updated.
/*
/*	In addition, when an unexpected response code is seen such
/*	as 3xx where only 4xx or 5xx are expected, or any error code
/*	that suggests a syntax error or something similar, the
/*	protocol error flag is set so that the postmaster receives
/*	a transcript of the session. No notification is generated for
/*	what appear to be configuration errors - very likely, they
/*	would suffer the same problem and just cause more trouble.
/*
/*	lmtp_site_fail() handles the case where the program fails to
/*	complete the initial LMTP handshake: the server is not reachable,
/*	is not running, does not want talk to us, or we talk to ourselves.
/*	The \fIcode\fR gives an error status code; the \fIformat\fR
/*	argument gives a textual description.  The policy is: soft
/*	error: defer delivery of all messages to this domain; hard
/*	error: bounce all recipients of this message.
/*	The result is non-zero.
/*
/*	lmtp_mesg_fail() handles the case where the lmtp server
/*	does not accept the sender address or the message data.
/*	The policy is: soft errors: defer delivery of this message;
/*	hard error: bounce all recipients of this message.
/*	The result is non-zero.
/*
/*	lmtp_rcpt_fail() handles the case where a recipient is not
/*	accepted by the server for reasons other than that the server
/*	recipient limit is reached. The policy is: soft error: defer
/*	delivery to this recipient; hard error: bounce this recipient.
/*
/*	lmtp_stream_except() handles the exceptions generated by
/*	the smtp_stream(3) module (i.e. timeouts and I/O errors).
/*	The \fIexception\fR argument specifies the type of problem.
/*	The \fIdescription\fR argument describes at what stage of
/*	the LMTP dialog the problem happened. The policy is to defer
/*	delivery of all messages to the same domain. The result is non-zero.
/* DIAGNOSTICS
/*	Panic: unknown exception code.
/* SEE ALSO
/*	lmtp_proto(3) lmtp high-level protocol
/*	smtp_stream(3) lmtp low-level protocol
/*	defer(3) basic message defer interface
/*	bounce(3) basic message bounce interface
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*
/*	Alterations for LMTP by:
/*	Philip A. Prindeville
/*	Mirapoint, Inc.
/*	USA.
/*
/*	Additional work on LMTP by:
/*	Amos Gouaux
/*	University of Texas at Dallas
/*	P.O. Box 830688, MC34
/*	Richardson, TX 75083, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <stdlib.h>			/* 44BSD stdarg.h uses abort() */
#include <stdarg.h>

/* Utility library. */

#include <msg.h>
#include <vstring.h>
#include <stringops.h>
#include <mymalloc.h>

/* Global library. */

#include <smtp_stream.h>
#include <deliver_request.h>
#include <deliver_completed.h>
#include <bounce.h>
#include <defer.h>
#include <mail_error.h>

/* Application-specific. */

#include "lmtp.h"

#define LMTP_SOFT(code) (((code) / 100) == 4)
#define LMTP_HARD(code) (((code) / 100) == 5)

/* lmtp_check_code - check response code */

static void lmtp_check_code(LMTP_STATE *state, int code)
{

    /*
     * The intention of this stuff is to alert the postmaster when the local
     * Postfix LMTP client screws up, protocol wise. RFC 821 says that x0z
     * replies "refer to syntax errors, syntactically correct commands that
     * don't fit any functional category, and unimplemented or superfluous
     * commands". Unfortunately, this also triggers postmaster notices when
     * remote servers screw up, protocol wise. This is becoming a common
     * problem now that response codes are configured manually as part of
     * anti-UCE systems, by people who aren't aware of RFC details.
     */
    if ((!LMTP_SOFT(code) && !LMTP_HARD(code))
	|| code == 555			/* RFC 1869, section 6.1. */
	|| (code >= 500 && code < 510))
	state->error_mask |= MAIL_ERROR_PROTOCOL;
}

/* lmtp_site_fail - defer site or bounce recipients */

int     lmtp_site_fail(LMTP_STATE *state, int code, char *format,...)
{
    DELIVER_REQUEST *request = state->request;
    LMTP_SESSION *session = state->session;
    RECIPIENT *rcpt;
    int     status;
    int     nrcpt;
    int     soft_error = LMTP_SOFT(code);
    va_list ap;
    VSTRING *why = vstring_alloc(100);

    /*
     * Initialize.
     */
    va_start(ap, format);
    vstring_vsprintf(why, format, ap);
    va_end(ap);

    /*
     * If this is a soft error, postpone further deliveries to this domain.
     * Otherwise, generate a bounce record for each recipient.
     */
    for (nrcpt = 0; nrcpt < request->rcpt_list.len; nrcpt++) {
	rcpt = request->rcpt_list.info + nrcpt;
	if (rcpt->offset == 0)
	    continue;
	status = (soft_error ? defer_append : bounce_append)
	    (DEL_REQ_TRACE_FLAGS(request->flags), request->queue_id,
	     rcpt->orig_addr, rcpt->address, rcpt->offset,
	     session ? session->namaddr : "none",
	     request->arrival_time, "%s", vstring_str(why));
	if (status == 0) {
	    deliver_completed(state->src, rcpt->offset);
	    rcpt->offset = 0;
	}
	state->status |= status;
    }
    if (soft_error && request->hop_status == 0)
	request->hop_status = mystrdup(vstring_str(why));

    /*
     * Cleanup.
     */
    vstring_free(why);
    return (-1);
}

/* lmtp_mesg_fail - defer message or bounce all recipients */

int     lmtp_mesg_fail(LMTP_STATE *state, int code, char *format,...)
{
    DELIVER_REQUEST *request = state->request;
    LMTP_SESSION *session = state->session;
    RECIPIENT *rcpt;
    int     status;
    int     nrcpt;
    va_list ap;
    VSTRING *why = vstring_alloc(100);

    /*
     * Initialize.
     */
    va_start(ap, format);
    vstring_vsprintf(why, format, ap);
    va_end(ap);

    /*
     * If this is a soft error, postpone delivery of this message. Otherwise,
     * generate a bounce record for each recipient.
     */
    for (nrcpt = 0; nrcpt < request->rcpt_list.len; nrcpt++) {
	rcpt = request->rcpt_list.info + nrcpt;
	if (rcpt->offset == 0)
	    continue;
	status = (LMTP_SOFT(code) ? defer_append : bounce_append)
	    (DEL_REQ_TRACE_FLAGS(request->flags), request->queue_id,
	     rcpt->orig_addr, rcpt->address, rcpt->offset,
	     session->namaddr, request->arrival_time,
	     "%s", vstring_str(why));
	if (status == 0) {
	    deliver_completed(state->src, rcpt->offset);
	    rcpt->offset = 0;
	}
	state->status |= status;
    }
    lmtp_check_code(state, code);

    /*
     * Cleanup.
     */
    vstring_free(why);
    return (-1);
}

/* lmtp_rcpt_fail - defer or bounce recipient */

void    lmtp_rcpt_fail(LMTP_STATE *state, int code, RECIPIENT *rcpt,
		               char *format,...)
{
    DELIVER_REQUEST *request = state->request;
    LMTP_SESSION *session = state->session;
    int     status;
    va_list ap;

    /*
     * If this is a soft error, postpone delivery to this recipient.
     * Otherwise, generate a bounce record for this recipient.
     */
    va_start(ap, format);
    status = (LMTP_SOFT(code) ? vdefer_append : vbounce_append)
	(DEL_REQ_TRACE_FLAGS(request->flags), request->queue_id,
	 rcpt->orig_addr, rcpt->address, rcpt->offset,
	 session->namaddr, request->arrival_time, format, ap);
    va_end(ap);
    if (status == 0) {
	deliver_completed(state->src, rcpt->offset);
	rcpt->offset = 0;
    }
    lmtp_check_code(state, code);
    state->status |= status;
}

/* lmtp_stream_except - defer domain after I/O problem */

int     lmtp_stream_except(LMTP_STATE *state, int code, char *description)
{
    DELIVER_REQUEST *request = state->request;
    LMTP_SESSION *session = state->session;
    RECIPIENT *rcpt;
    int     nrcpt;
    VSTRING *why = vstring_alloc(100);

    /*
     * Initialize.
     */
    switch (code) {
    default:
	msg_panic("lmtp_stream_except: unknown exception %d", code);
    case SMTP_ERR_EOF:
	vstring_sprintf(why, "lost connection with %s while %s",
			session->namaddr, description);
	break;
    case SMTP_ERR_TIME:
	vstring_sprintf(why, "conversation with %s timed out while %s",
			session->namaddr, description);
	break;
    case SMTP_ERR_PROTO:
	vstring_sprintf(why, "remote protocol error in reply from %s while %s",
			session->namaddr, description);
	break;
    }

    /*
     * At this point, the status of individual recipients remains unresolved.
     * All we know is that we should stay away from this host for a while.
     */
    for (nrcpt = 0; nrcpt < request->rcpt_list.len; nrcpt++) {
	rcpt = request->rcpt_list.info + nrcpt;
	if (rcpt->offset == 0)
	    continue;
	state->status |= defer_append(DEL_REQ_TRACE_FLAGS(request->flags),
				      request->queue_id,
				      rcpt->orig_addr, rcpt->address,
				      rcpt->offset, session->namaddr,
				      request->arrival_time,
				      "%s", vstring_str(why));
    }
    if (request->hop_status == 0)
	request->hop_status = mystrdup(vstring_str(why));

    /*
     * Cleanup.
     */
    vstring_free(why);
    return (-1);
}
