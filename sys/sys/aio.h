/*	$NetBSD: aio.h,v 1.1 2007/04/30 14:44:31 rmind Exp $	*/

/*
 * Copyright (c) 2007, Mindaugas Rasiukevicius <rmind at NetBSD org>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
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

#ifndef _SYS_AIO_H_
#define _SYS_AIO_H_

/* Returned by aio_cancel() */
#define AIO_CANCELED		0x1
#define AIO_NOTCANCELED		0x2
#define AIO_ALLDONE		0x3

/* LIO opcodes */
#define LIO_NOP			0x0
#define LIO_WRITE		0x1
#define LIO_READ		0x2

/* LIO modes */
#define LIO_NOWAIT		0x0
#define LIO_WAIT		0x1

/*
 * Asynchronous I/O structure.
 * Defined in the Base Definitions volume of IEEE Std 1003.1-2001 .
 */
struct aiocb {
	int aio_fildes;			/* File descriptor */
	off_t aio_offset;		/* File offset */
	volatile void *aio_buf;		/* I/O buffer in process space */
	size_t aio_nbytes;		/* Length of transfer */
	int aio_reqprio;		/* Request priority offset */
	struct sigevent aio_sigevent;	/* Signal to deliver */
	int aio_lio_opcode;		/* LIO opcode */

	/* Internal kernel variables */
	char _state;			/* State of the job */
	int _errno;			/* Error value */
	ssize_t _retval;		/* Return value */
};

/* Internal kernel data */
#ifdef _KERNEL

/* Maximal number of allowed AIO operations */
/* XXX: These will be changeable via sysctl */
#define AIO_LISTIO_MAX		512
#define AIO_MAX			AIO_LISTIO_MAX * 16

#include <sys/condvar.h>
#include <sys/lwp.h>
#include <sys/mutex.h>
#include <sys/pool.h>
#include <sys/queue.h>

/* Operations (as flags) */
#define AIO_LIO			0x00
#define AIO_READ		0x01
#define AIO_WRITE		0x02
#define AIO_SYNC		0x04
#define AIO_DSYNC		0x08

/* Job states */
#define JOB_NONE		0x0
#define JOB_WIP			0x1
#define JOB_DONE		0x2

/* Structure of AIO job */
struct aio_job {
	int aio_op;		/* Operation code */
	struct aiocb aiocbp;	/* AIO data structure */
	void *aiocb_uptr;	/* User-space pointer for identification of job */
	TAILQ_ENTRY(aio_job) list;
	struct lio_req *lio;
};

/* LIO structure */
struct lio_req {
	u_int refcnt;		/* Reference counter */
	struct sigevent sig;	/* Signal of lio_listio() calls */
	bool dofree;		/* Worker will free the structure */
};

/* Structure of AIO data for process */
struct aioproc {
	kmutex_t aio_mtx;		/* Protects the entire structure */
	kcondvar_t aio_worker_cv;	/* Signals on a new job */
	kcondvar_t done_cv;		/* Signals when the job is done */
	struct aio_job *curjob;		/* Currently processing AIO job */
	struct pool jobs_pool;		/* Jobs pool */
	unsigned int jobs_count;	/* Count of the jobs */
	TAILQ_HEAD(, aio_job) jobs_queue;	/* Queue of the AIO jobs */
	struct pool lio_pool;		/* List of LIO operations */
	struct lwp *aio_worker;		/* AIO worker thread */
};

/* Prototypes */
int aio_init(struct proc *);
void aio_exit(struct proc *);
#if defined(DDB)
void aio_print_jobs(void (*pr)(const char *, ...));
#endif

#endif /* _KERNEL */

#endif /* _SYS_AIO_H_ */
