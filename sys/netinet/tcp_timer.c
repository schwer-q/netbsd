/*	$NetBSD: tcp_timer.c,v 1.31 1998/03/19 22:29:34 kml Exp $	*/

/*-
 * Copyright (c) 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe and Kevin M. Lahey of the Numerical Aerospace Simulation
 * Facility, NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1988, 1990, 1993, 1995
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)tcp_timer.c	8.2 (Berkeley) 5/24/95
 */

#include "opt_tcp_compat_42.h"

#ifndef TUBA_INCLUDE
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/errno.h>

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in_pcb.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/tcpip.h>

int	tcp_keepidle = TCPTV_KEEP_IDLE;
int	tcp_keepintvl = TCPTV_KEEPINTVL;
int	tcp_keepcnt = TCPTV_KEEPCNT;		/* max idle probes */
int	tcp_maxpersistidle = TCPTV_KEEP_IDLE;	/* max idle time in persist */
int	tcp_maxidle;
#else /* TUBA_INCLUDE */

extern	int tcp_keepcnt;
extern	int tcp_maxpersistidle;
#endif /* TUBA_INCLUDE */

struct tcp_delack_head tcp_delacks;

/*
 * Fast timeout routine for processing delayed acks
 */
void
tcp_fasttimo()
{
	register struct tcpcb *tp, *ntp;
	int s;

	s = splsoftnet();
	for (tp = tcp_delacks.lh_first; tp != NULL; tp = ntp) {
		/*
		 * If tcp_output() can't transmit the ACK for whatever
		 * reason, it will remain on the queue for the next
		 * time the heartbeat ticks.
		 */
		ntp = tp->t_delack.le_next;
		tp->t_flags |= TF_ACKNOW;
		(void) tcp_output(tp);
	}
	splx(s);
}

/*
 * Tcp protocol timeout routine called every 500 ms.
 * Updates the timers in all active tcb's and
 * causes finite state machine actions if timers expire.
 */
void
tcp_slowtimo()
{
	register struct inpcb *inp, *ninp;
	register struct tcpcb *tp;
	int s;
	register long i;
	static int syn_cache_last = 0;

	s = splsoftnet();
	tcp_maxidle = tcp_keepcnt * tcp_keepintvl;
	/*
	 * Search through tcb's and update active timers.
	 */
	inp = tcbtable.inpt_queue.cqh_first;
	if (inp == (struct inpcb *)0) {				/* XXX */
		splx(s);
		return;
	}
	for (; inp != (struct inpcb *)&tcbtable.inpt_queue; inp = ninp) {
		ninp = inp->inp_queue.cqe_next;
		tp = intotcpcb(inp);
		if (tp == 0 || tp->t_state == TCPS_LISTEN)
			continue;
		for (i = 0; i < TCPT_NTIMERS; i++) {
			if (tp->t_timer[i] && --tp->t_timer[i] == 0) {
				(void) tcp_usrreq(tp->t_inpcb->inp_socket,
				    PRU_SLOWTIMO, (struct mbuf *)0,
				    (struct mbuf *)i, (struct mbuf *)0,
				    (struct proc *)0);
				/* XXX NOT MP SAFE */
				if ((ninp == (void *)&tcbtable.inpt_queue &&
				    tcbtable.inpt_queue.cqh_last != inp) ||
				    ninp->inp_queue.cqe_prev != inp)
					goto tpgone;
			}
		}
		tp->t_idle++;
		if (tp->t_rtt)
			tp->t_rtt++;
tpgone:
		;
	}
#if NRND == 0 /* Do we need to do this when using random() ? */
	tcp_iss_seq += TCP_ISSINCR;			/* increment iss */
#ifdef TCP_COMPAT_42
	if ((int)tcp_iss_seq < 0)
		tcp_iss_seq = 0;			/* XXX */
#endif
#endif
	tcp_now++;					/* for timestamps */
	if (++syn_cache_last >= tcp_syn_cache_interval) {
		syn_cache_timer(syn_cache_last);
		syn_cache_last = 0;
	}
	splx(s);
}
#ifndef TUBA_INCLUDE

/*
 * Cancel all timers for TCP tp.
 */
void
tcp_canceltimers(tp)
	struct tcpcb *tp;
{
	register int i;

	for (i = 0; i < TCPT_NTIMERS; i++)
		tp->t_timer[i] = 0;
}

int	tcp_backoff[TCP_MAXRXTSHIFT + 1] =
    { 1, 2, 4, 8, 16, 32, 64, 64, 64, 64, 64, 64, 64 };

int	tcp_totbackoff = 511;	/* sum of tcp_backoff[] */

/*
 * TCP timer processing.
 */
struct tcpcb *
tcp_timers(tp, timer)
	register struct tcpcb *tp;
	int timer;
{
	short	rto;

	switch (timer) {

	/*
	 * 2 MSL timeout in shutdown went off.  If we're closed but
	 * still waiting for peer to close and connection has been idle
	 * too long, or if 2MSL time is up from TIME_WAIT, delete connection
	 * control block.  Otherwise, check again in a bit.
	 */
	case TCPT_2MSL:
		if (tp->t_state != TCPS_TIME_WAIT &&
		    tp->t_idle <= tcp_maxidle)
			tp->t_timer[TCPT_2MSL] = tcp_keepintvl;
		else
			tp = tcp_close(tp);
		break;

	/*
	 * Retransmission timer went off.  Message has not
	 * been acked within retransmit interval.  Back off
	 * to a longer retransmit interval and retransmit one segment.
	 */
	case TCPT_REXMT:
		if (++tp->t_rxtshift > TCP_MAXRXTSHIFT) {
			tp->t_rxtshift = TCP_MAXRXTSHIFT;
			tcpstat.tcps_timeoutdrop++;
			tp = tcp_drop(tp, tp->t_softerror ?
			    tp->t_softerror : ETIMEDOUT);
			break;
		}
		tcpstat.tcps_rexmttimeo++;
		rto = TCP_REXMTVAL(tp);
		if (rto < tp->t_rttmin)
			rto = tp->t_rttmin;
		TCPT_RANGESET(tp->t_rxtcur, rto * tcp_backoff[tp->t_rxtshift],
		    tp->t_rttmin, TCPTV_REXMTMAX);
		tp->t_timer[TCPT_REXMT] = tp->t_rxtcur;
		/*
		 * If losing, let the lower level know and try for
		 * a better route.  Also, if we backed off this far,
		 * our srtt estimate is probably bogus.  Clobber it
		 * so we'll take the next rtt measurement as our srtt;
		 * move the current srtt into rttvar to keep the current
		 * retransmit times until then.
		 */
		if (tp->t_rxtshift > TCP_MAXRXTSHIFT / 4) {
			in_losing(tp->t_inpcb);
			tp->t_rttvar += (tp->t_srtt >> TCP_RTT_SHIFT);
			tp->t_srtt = 0;
		}
		tp->snd_nxt = tp->snd_una;
		/*
		 * If timing a segment in this window, stop the timer.
		 */
		tp->t_rtt = 0;
		/*
		 * Close the congestion window down to the initial window
		 * (we'll open it by one segment for each ack we get).
		 * Since we probably have a window's worth of unacked
		 * data accumulated, this "slow start" keeps us from
		 * dumping all that data as back-to-back packets (which
		 * might overwhelm an intermediate gateway).
		 *
		 * There are two phases to the opening: Initially we
		 * open by one mss on each ack.  This makes the window
		 * size increase exponentially with time.  If the
		 * window is larger than the path can handle, this
		 * exponential growth results in dropped packet(s)
		 * almost immediately.  To get more time between 
		 * drops but still "push" the network to take advantage
		 * of improving conditions, we switch from exponential
		 * to linear window opening at some threshhold size.
		 * For a threshhold, we use half the current window
		 * size, truncated to a multiple of the mss.
		 *
		 * (the minimum cwnd that will give us exponential
		 * growth is 2 mss.  We don't allow the threshhold
		 * to go below this.)
		 */
		{
		u_int win = min(tp->snd_wnd, tp->snd_cwnd) / 2 / tp->t_segsz;
		if (win < 2)
			win = 2;
		tp->snd_cwnd = TCP_INITIAL_WINDOW(tp->t_segsz);
		tp->snd_ssthresh = win * tp->t_segsz;
		tp->t_dupacks = 0;
		}
		(void) tcp_output(tp);
		break;

	/*
	 * Persistance timer into zero window.
	 * Force a byte to be output, if possible.
	 */
	case TCPT_PERSIST:
		/*
		 * Hack: if the peer is dead/unreachable, we do not
		 * time out if the window is closed.  After a full
		 * backoff, drop the connection if the idle time
		 * (no responses to probes) reaches the maximum
		 * backoff that we would use if retransmitting.
		 */
		rto = TCP_REXMTVAL(tp);
		if (rto < tp->t_rttmin)
			rto = tp->t_rttmin;
		if (tp->t_rxtshift == TCP_MAXRXTSHIFT &&
		    (tp->t_idle >= tcp_maxpersistidle ||
		    tp->t_idle >= rto * tcp_totbackoff)) {
			tcpstat.tcps_persistdrops++;
			tp = tcp_drop(tp, ETIMEDOUT);
			break;
		}
		tcpstat.tcps_persisttimeo++;
		tcp_setpersist(tp);
		tp->t_force = 1;
		(void) tcp_output(tp);
		tp->t_force = 0;
		break;

	/*
	 * Keep-alive timer went off; send something
	 * or drop connection if idle for too long.
	 */
	case TCPT_KEEP:
		tcpstat.tcps_keeptimeo++;
		if (TCPS_HAVEESTABLISHED(tp->t_state) == 0)
			goto dropit;
		if (tp->t_inpcb->inp_socket->so_options & SO_KEEPALIVE &&
		    tp->t_state <= TCPS_CLOSE_WAIT) {
		    	if (tp->t_idle >= tcp_keepidle + tcp_maxidle)
				goto dropit;
			/*
			 * Send a packet designed to force a response
			 * if the peer is up and reachable:
			 * either an ACK if the connection is still alive,
			 * or an RST if the peer has closed the connection
			 * due to timeout or reboot.
			 * Using sequence number tp->snd_una-1
			 * causes the transmitted zero-length segment
			 * to lie outside the receive window;
			 * by the protocol spec, this requires the
			 * correspondent TCP to respond.
			 */
			tcpstat.tcps_keepprobe++;
#ifdef TCP_COMPAT_42
			/*
			 * The keepalive packet must have nonzero length
			 * to get a 4.2 host to respond.
			 */
			(void)tcp_respond(tp, tp->t_template,
			    (struct mbuf *)NULL, tp->rcv_nxt - 1,
			    tp->snd_una - 1, 0);
#else
			(void)tcp_respond(tp, tp->t_template,
			    (struct mbuf *)NULL, tp->rcv_nxt,
			    tp->snd_una - 1, 0);
#endif
			tp->t_timer[TCPT_KEEP] = tcp_keepintvl;
		} else
			tp->t_timer[TCPT_KEEP] = tcp_keepidle;
		break;
	dropit:
		tcpstat.tcps_keepdrops++;
		tp = tcp_drop(tp, ETIMEDOUT);
		break;
	}
	return (tp);
}
#endif /* TUBA_INCLUDE */
