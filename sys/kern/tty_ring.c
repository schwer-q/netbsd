/*
 * Copyright (c) 1989, 1990, 1991, 1992 William F. Jolitz, TeleMuse
 * All rights reserved.
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
 *	This software is a component of "386BSD" developed by 
 *	William F. Jolitz, TeleMuse.
 * 4. Neither the name of the developer nor the name "386BSD"
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS A COMPONENT OF 386BSD DEVELOPED BY WILLIAM F. JOLITZ 
 * AND IS INTENDED FOR RESEARCH AND EDUCATIONAL PURPOSES ONLY. THIS 
 * SOFTWARE SHOULD NOT BE CONSIDERED TO BE A COMMERCIAL PRODUCT. 
 * THE DEVELOPER URGES THAT USERS WHO REQUIRE A COMMERCIAL PRODUCT 
 * NOT MAKE USE OF THIS WORK.
 *
 * FOR USERS WHO WISH TO UNDERSTAND THE 386BSD SYSTEM DEVELOPED
 * BY WILLIAM F. JOLITZ, WE RECOMMEND THE USER STUDY WRITTEN 
 * REFERENCES SUCH AS THE  "PORTING UNIX TO THE 386" SERIES 
 * (BEGINNING JANUARY 1991 "DR. DOBBS JOURNAL", USA AND BEGINNING 
 * JUNE 1991 "UNIX MAGAZIN", GERMANY) BY WILLIAM F. JOLITZ AND 
 * LYNNE GREER JOLITZ, AS WELL AS OTHER BOOKS ON UNIX AND THE 
 * ON-LINE 386BSD USER MANUAL BEFORE USE. A BOOK DISCUSSING THE INTERNALS 
 * OF 386BSD ENTITLED "386BSD FROM THE INSIDE OUT" WILL BE AVAILABLE LATE 1992.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPER ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE DEVELOPER BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id: tty_ring.c,v 1.7 1993/05/27 09:29:40 deraadt Exp $
 */

#include "param.h"
#include "systm.h"
#include "buf.h"
#include "ioctl.h"
#include "tty.h"

/*
 * XXX - put this in tty.h someday.
 */
size_t rb_read __P((struct ringb *from, rbchar *bf, size_t nto));
size_t rb_write __P((struct ringb *to, rbchar *buf, size_t nfrom));
size_t rb_cread __P((struct ringb *from, char *bf, size_t nto));
size_t rb_cwrite __P((struct ringb *to, char *buf, size_t nfrom));

putc(c, rbp)
struct ringb *rbp;
{
	rbchar *nxtp;

	/* ring buffer full? */
	if ( (nxtp = RB_SUCC(rbp, rbp->rb_tl)) == rbp->rb_hd)
		return (-1);

	/* stuff character */
	*rbp->rb_tl = c;
	rbp->rb_tl = nxtp;
	return(0);
}

getc(rbp)
struct ringb *rbp;
{
	rbchar c;

	/* ring buffer empty? */
	if (rbp->rb_hd == rbp->rb_tl)
		return(-1);

	/* fetch character, locate next character */
	c = *(rbchar *) rbp->rb_hd;
	rbp->rb_hd = RB_SUCC(rbp, rbp->rb_hd);
	return (c);
}

nextc(cpp, rbp)
struct ringb *rbp;
rbchar **cpp;
{
	rbchar *cp;

	if (*cpp == rbp->rb_tl)
		return (0);
	else {
		cp = *cpp;
		*cpp = RB_SUCC(rbp, cp);
		return(*cp);
	}
}

ungetc(c, rbp)
struct ringb *rbp;
{
	rbchar *backp;

	/* ring buffer full? */
	if ( (backp = RB_PRED(rbp, rbp->rb_hd)) == rbp->rb_tl)
		return (-1);
	rbp->rb_hd = backp;

	/* stuff character */
	*rbp->rb_hd = c;
	return(0);
}

unputc(rbp)
struct ringb *rbp;
{
	rbchar *backp;
	int c;

	/* ring buffer empty? */
	if (rbp->rb_hd == rbp->rb_tl)
		return(-1);

	/* backup buffer and dig out previous character */
	backp = RB_PRED(rbp, rbp->rb_tl);
	c = *(rbchar *)backp;
	rbp->rb_tl = backp;

	return(c);
}

#define	peekc(rbp)	(*(rbp)->rb_hd)

initrb(rbp)
struct ringb *rbp;
{
	rbp->rb_hd = rbp->rb_tl = rbp->rb_buf;
}

rbpack(s, ss, n)
char *s;
rbchar *ss;
int n;
{
	while(n--)
		*ss++ = (rbchar)(*s++);
	return n;
}

rbunpack(ss, s, n)
rbchar *ss;
char *s;
int n;
{
	while(n--)
		*s++ = (char)(*ss++);
	return n;
}

/*
 * Example code for contiguous operations:
	...
	nc = RB_CONTIGPUT(&rb);
	if (nc) {
		if (nc > 9)
			nc = 9;
		rbpack("ABCDEFGHI", rb.rb_tl, nc);
		rb.rb_tl += nc;
		rb.rb_tl = RB_ROLLOVER(&rb, rb.rb_tl);
	}
	...
	...
	nc = RB_CONTIGGET(&rb);
	if (nc) {
		int i;

		if (nc > 79)
			nc = 79;
		rbunpack(rb.rb_hd, stringbuf, nc);
		rb.rb_hd += nc;
		rb.rb_hd = RB_ROLLOVER(&rb, rb.rb_hd);
		stringbuf[nc] = 0;
		printf("%s|", stringbuf);
	}
	...
 */

/*
 * Concatenate ring buffers.
 */
catb(from, to)
struct ringb *from, *to;
{
	size_t nfromleft;
	size_t nfromright;

	nfromright = RB_CONTIGGET(from);
	rb_write(to, from->rb_hd, nfromright);
	from->rb_hd += nfromright;
	from->rb_hd = RB_ROLLOVER(from, from->rb_hd);
	nfromleft = RB_CONTIGGET(from);
	rb_write(to, from->rb_hd, nfromleft);
	from->rb_hd += nfromleft;
}

/*
 * Copy from ring buffer to ordinary buffer, returning count of characters
 * read.
 */
size_t rb_read(from, buf, nto)
struct ringb *from;
rbchar *buf;
size_t nto;
{
	size_t nright, nleft;

	if (from->rb_tl >= from->rb_hd) {
		nright = from->rb_tl - from->rb_hd;
		if (!nright)
			return (0);
		nleft = 0;
	} else {
		nleft = from->rb_tl - from->rb_buf;
		nright = RBSZ - (from->rb_hd - from->rb_buf);
	}
	if (nright >= nto) {
		noleft:
		bcopy(from->rb_hd, buf, nto * sizeof(rbchar));
		from->rb_hd = RB_ROLLOVER(from, from->rb_hd + nto);
		return (nto);
	}
	if (!nleft) {
		nto = nright;
		goto noleft;
	}
	bcopy(from->rb_hd, buf, nright * sizeof(rbchar));
	nto -= nright;
	if (nto > nleft)
		nto = nleft;
	bcopy(from->rb_buf, buf+nright, nto * sizeof(rbchar));
	from->rb_hd = from->rb_buf + nto;
	return (nright + nto);
}

/*
 * Copy ordinary buffer to ring buffer, return count of what fitted.
 */
size_t rb_write(to, buf, nfrom)
struct ringb *to;
rbchar *buf;
size_t nfrom;
{
	rbchar *toleft;
	size_t ntoleft;
	size_t ntoright;

	ntoright = RB_CONTIGPUT(to);
	if (nfrom < ntoright) {
		bcopy(buf, to->rb_tl, nfrom * sizeof(rbchar));
		to->rb_tl += nfrom;
		return (nfrom);
	}
	bcopy(buf, to->rb_tl, ntoright * sizeof(rbchar));
	nfrom -= ntoright;
	toleft = to->rb_buf;	/* fast RB_ROLLOVER */
	ntoleft = to->rb_hd - toleft;	/* fast RB_CONTIGPUT */
	if (nfrom > ntoleft)
		nfrom = ntoleft;
	bcopy(buf + ntoright, toleft, nfrom * sizeof(rbchar));
	to->rb_tl = toleft + nfrom;
	return (ntoright + nfrom);
}

/*
 * Copy from ring buffer to ordinary buffer, returning count of characters
 * read.
 */
size_t rb_cread(from, buf, nto)
struct ringb *from;
char *buf;
size_t nto;
{
	size_t nright, nleft;

	if (from->rb_tl >= from->rb_hd) {
		nright = from->rb_tl - from->rb_hd;
		if (!nright)
			return (0);
		nleft = 0;
	} else {
		nleft = from->rb_tl - from->rb_buf;
		nright = RBSZ - (from->rb_hd - from->rb_buf);
	}
	if (nright >= nto) {
		noleft:
		rbunpack(from->rb_hd, buf, nto);
		from->rb_hd = RB_ROLLOVER(from, from->rb_hd + nto);
		return (nto);
	}
	if (!nleft) {
		nto = nright;
		goto noleft;
	}
	rbunpack(from->rb_hd, buf, nright);
	nto -= nright;
	if (nto > nleft)
		nto = nleft;
	rbunpack(from->rb_buf, buf+nright, nto);
	from->rb_hd = from->rb_buf + nto;
	return (nright + nto);
}

/*
 * Copy ordinary buffer to ring buffer, return count of what fitted.
 */
size_t rb_cwrite(to, buf, nfrom)
struct ringb *to;
char *buf;
size_t nfrom;
{
	rbchar *toleft;
	size_t ntoleft;
	size_t ntoright;

	ntoright = RB_CONTIGPUT(to);
	if (nfrom < ntoright) {
		rbpack(buf, to->rb_tl, nfrom);
		to->rb_tl += nfrom;
		return (nfrom);
	}
	rbpack(buf, to->rb_tl, ntoright);
	nfrom -= ntoright;
	toleft = to->rb_buf;	/* fast RB_ROLLOVER */
	ntoleft = to->rb_hd - toleft;	/* fast RB_CONTIGPUT */
	if (nfrom > ntoleft)
		nfrom = ntoleft;
	rbpack(buf + ntoright, toleft, nfrom);
	to->rb_tl = toleft + nfrom;
	return (ntoright + nfrom);
}
