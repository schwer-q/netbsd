/*	$NetBSD: pam_lastlog.c,v 1.4 2005/02/04 15:11:35 he Exp $	*/

/*-
 * Copyright (c) 1980, 1987, 1988, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 2001 Mark R V Murray
 * All rights reserved.
 * Copyright (c) 2001 Networks Associates Technology, Inc.
 * All rights reserved.
 * Copyright (c) 2004 Joe R. Doupnik
 * All rights reserved.
 *
 * Portions of this software were developed for the FreeBSD Project by
 * ThinkSec AS and NAI Labs, the Security Research Division of Network
 * Associates, Inc.  under DARPA/SPAWAR contract N66001-01-C-8035
 * ("CBOSS"), as part of the DARPA CHATS research program.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifdef __FreeBSD__
__FBSDID("$FreeBSD: src/lib/libpam/modules/pam_lastlog/pam_lastlog.c,v 1.20 2004/01/26 19:28:37 des Exp $");
#else
__RCSID("$NetBSD: pam_lastlog.c,v 1.4 2005/02/04 15:11:35 he Exp $");
#endif

#include <sys/param.h>

#include <fcntl.h>
#include <util.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#ifdef SUPPORT_UTMP
#include <utmp.h>
static void doutmp(const char *, const char *, const char *,
    const struct timeval *);
static void dolastlog(int, const struct passwd *, const char *, const char *,
    const struct timeval *);
#endif

#ifdef SUPPORT_UTMPX
#include <utmpx.h>
static void doutmpx(const char *, const char *, const char *,
    const struct sockaddr_storage *ss, const struct timeval *);
static void dolastlogx(int, const struct passwd *, const char *, const char *,
    const struct sockaddr_storage *ss, const struct timeval *);
#endif

#define PAM_SM_SESSION

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_mod_misc.h>

PAM_EXTERN int
pam_sm_open_session(pam_handle_t *pamh, int flags,
    int argc __unused, const char *argv[] __unused)
{
	struct passwd *pwd;
	struct timeval now;
	const char *user, *rhost, *tty;
	const void *vrhost, *vtty, *vss;
	const struct sockaddr_storage *ss;
	int pam_err;

	pam_err = pam_get_user(pamh, &user, NULL);
	if (pam_err != PAM_SUCCESS)
		return pam_err;

	if (user == NULL || (pwd = getpwnam(user)) == NULL)
		return PAM_SERVICE_ERR;

	PAM_LOG("Got user: %s", user);

	pam_err = pam_get_item(pamh, PAM_RHOST, &vrhost);
	if (pam_err != PAM_SUCCESS)
		goto err;
	rhost = (const char *)vrhost;

	pam_err = pam_get_item(pamh, PAM_SOCKADDR, &vss);
	if (pam_err != PAM_SUCCESS)
		goto err;
	ss = (const struct sockaddr_storage *)vss;

	pam_err = pam_get_item(pamh, PAM_TTY, &vtty);
	if (pam_err != PAM_SUCCESS)
		goto err;
	tty = (const char *)vtty;

	if (tty == NULL) {
		pam_err = PAM_SERVICE_ERR;
		goto err;
	}

	if (strncmp(tty, _PATH_DEV, strlen(_PATH_DEV)) == 0)
		tty = tty + strlen(_PATH_DEV);

	if (*tty == '\0') {
		pam_err = PAM_SERVICE_ERR;
		goto err;
	}

	(void)gettimeofday(&now, NULL);

#ifdef SUPPORT_UTMPX
	doutmpx(user, rhost, tty, ss, &now);
	dolastlogx(1, pwd, rhost, tty, ss, &now);
#endif
#ifdef SUPPORT_UTMP
	doutmp(user, rhost, tty, &now);
	dolastlog(1, pwd, rhost, tty, &now);
#endif
err:
	if (openpam_get_option(pamh, "no_fail"))
		return PAM_SUCCESS;
	return pam_err;
}

PAM_EXTERN int
pam_sm_close_session(pam_handle_t *pamh __unused, int flags __unused,
    int argc __unused, const char *argv[] __unused)
{
	const void *vtty;
	const char *tty;

	pam_get_item(pamh, PAM_TTY, &vtty);
	if (vtty == NULL)
		return PAM_SERVICE_ERR;
	tty = (const char *)vtty;

	if (strncmp(tty, _PATH_DEV, strlen(_PATH_DEV)) == 0)
		tty = tty + strlen(_PATH_DEV);

	if (*tty == '\0')
		return PAM_SERVICE_ERR;

#ifdef SUPPORT_UTMPX
	if (logoutx(tty, 0, DEAD_PROCESS))
		logwtmpx(tty, "", "", 0, DEAD_PROCESS);
	else
                syslog(LOG_NOTICE, "%s(): no utmpx record for %s",
		    __func__, tty);
#endif

#ifdef SUPPORT_UTMP
	if (logout(tty))
		logwtmp(tty, "", "");
	else
                syslog(LOG_NOTICE, "%s(): no utmp record for %s",
		    __func__, tty);
#endif
        return PAM_SUCCESS;
}

#ifdef SUPPORT_UTMPX
static void
doutmpx(const char *username, const char *hostname, const char *tty,
    const struct sockaddr_storage *ss, const struct timeval *now)
{
	struct utmpx utmpx;
	const char *t;

	memset((void *)&utmpx, 0, sizeof(utmpx));
	utmpx.ut_tv = *now;
	(void)strncpy(utmpx.ut_name, username, sizeof(utmpx.ut_name));
	if (hostname) {
		(void)strncpy(utmpx.ut_host, hostname, sizeof(utmpx.ut_host));
		if (ss)
			utmpx.ut_ss = *ss;
	}
	(void)strncpy(utmpx.ut_line, tty, sizeof(utmpx.ut_line));
	utmpx.ut_type = USER_PROCESS;
	utmpx.ut_pid = getpid();
	t = tty + strlen(tty);
	if (t - tty >= sizeof(utmpx.ut_id)) {
		(void)strncpy(utmpx.ut_id, t - sizeof(utmpx.ut_id),
		    sizeof(utmpx.ut_id));
	} else {
		(void)strncpy(utmpx.ut_id, tty, sizeof(utmpx.ut_id));
	}
	if (pututxline(&utmpx) == NULL)
		syslog(LOG_NOTICE, "Cannot update utmpx %m");
	endutxent();
	if (updwtmpx(_PATH_WTMPX, &utmpx) != 0)
		syslog(LOG_NOTICE, "Cannot update wtmpx %m");
}

static void
dolastlogx(int quiet, const struct passwd *pwd, const char *hostname,
    const char *tty, const struct sockaddr_storage *ss,
    const struct timeval *now)
{
	struct lastlogx ll;
	if (getlastlogx(_PATH_LASTLOGX, pwd->pw_uid, &ll) != NULL) {
		time_t t = (time_t)ll.ll_tv.tv_sec;
		if (!quiet) {
			(void)printf("Last login: %.24s ", ctime(&t));
			if (*ll.ll_host != '\0')
				(void)printf("from %.*s ",
				    (int)sizeof(ll.ll_host), ll.ll_host);
			(void)printf("on %.*s\n", (int)sizeof(ll.ll_line),
			    ll.ll_line);
		}
	}
	ll.ll_tv = *now;
	(void)strncpy(ll.ll_line, tty, sizeof(ll.ll_line));
	if (hostname) {
		(void)strncpy(ll.ll_host, hostname, sizeof(ll.ll_host));
		if (ss)
			ll.ll_ss = *ss;
	}
	if (updlastlogx(_PATH_LASTLOGX, pwd->pw_uid, &ll) != 0)
		syslog(LOG_NOTICE, "Cannot update lastlogx %m");
	PAM_LOG("Login recorded in %s", _PATH_LASTLOGX);
}
#endif

#ifdef SUPPORT_UTMP
static void
doutmp(const char *username, const char *hostname, const char *tty,
    const struct timeval *now)
{
	struct utmp utmp;

	(void)memset((void *)&utmp, 0, sizeof(utmp));
	utmp.ut_time = now->tv_sec;
	(void)strncpy(utmp.ut_name, username, sizeof(utmp.ut_name));
	if (hostname)
		(void)strncpy(utmp.ut_host, hostname, sizeof(utmp.ut_host));
	(void)strncpy(utmp.ut_line, tty, sizeof(utmp.ut_line));
	login(&utmp);
}

static void
dolastlog(int quiet, const struct passwd *pwd, const char *hostname,
    const char *tty, const struct timeval *now)
{
	struct lastlog ll;
	int fd;

	if ((fd = open(_PATH_LASTLOG, O_RDWR, 0)) >= 0) {
		(void)lseek(fd, (off_t)(pwd->pw_uid * sizeof(ll)), SEEK_SET);
		if (!quiet) {
			if (read(fd, (char *)&ll, sizeof(ll)) == sizeof(ll) &&
			    ll.ll_time != 0) {
				(void)printf("Last login: %.24s ",
				    ctime(&ll.ll_time));
				if (*ll.ll_host != '\0')
					(void)printf("from %.*s ",
					    (int)sizeof(ll.ll_host),
					    ll.ll_host);
				(void)printf("on %.*s\n",
				    (int)sizeof(ll.ll_line), ll.ll_line);
			}
			(void)lseek(fd, (off_t)(pwd->pw_uid * sizeof(ll)),
			    SEEK_SET);
		}
		(void)memset((void *)&ll, 0, sizeof(ll));
		ll.ll_time = now->tv_sec;
		(void)strncpy(ll.ll_line, tty, sizeof(ll.ll_line));
		if (hostname)
			(void)strncpy(ll.ll_host, hostname, sizeof(ll.ll_host));
		(void)write(fd, (char *)&ll, sizeof(ll));
		(void)close(fd);
	}
	PAM_LOG("Login recorded in %s", _PATH_LASTLOG);
}
#endif

PAM_MODULE_ENTRY("pam_lastlog");
