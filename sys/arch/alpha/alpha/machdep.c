/* $NetBSD: machdep.c,v 1.279 2003/12/30 12:33:13 pk Exp $ */

/*-
 * Copyright (c) 1998, 1999, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center and by Chris G. Demetriou.
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
 * Copyright (c) 1994, 1995, 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include "opt_ddb.h"
#include "opt_kgdb.h"
#include "opt_multiprocessor.h"
#include "opt_dec_3000_300.h"
#include "opt_dec_3000_500.h"
#include "opt_compat_osf1.h"
#include "opt_compat_netbsd.h"
#include "opt_execfmt.h"

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: machdep.c,v 1.279 2003/12/30 12:33:13 pk Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signalvar.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/ras.h>
#include <sys/sa.h>
#include <sys/savar.h>
#include <sys/sched.h>
#include <sys/buf.h>
#include <sys/reboot.h>
#include <sys/device.h>
#include <sys/file.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/mman.h>
#include <sys/msgbuf.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/user.h>
#include <sys/exec.h>
#include <sys/exec_ecoff.h>
#include <sys/core.h>
#include <sys/kcore.h>
#include <sys/ucontext.h>
#include <sys/conf.h>
#include <sys/ksyms.h>
#include <machine/kcore.h>
#include <machine/fpu.h>

#include <sys/mount.h>
#include <sys/sa.h>
#include <sys/syscallargs.h>

#include <uvm/uvm_extern.h>
#include <sys/sysctl.h>

#include <dev/cons.h>

#include <machine/autoconf.h>
#include <machine/cpu.h>
#include <machine/reg.h>
#include <machine/rpb.h>
#include <machine/prom.h>
#include <machine/cpuconf.h>
#include <machine/ieeefp.h>

#ifdef DDB
#include <machine/db_machdep.h>
#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_extern.h>
#include <ddb/db_interface.h>
#endif

#ifdef KGDB
#include <sys/kgdb.h>
#endif

#ifdef DEBUG
#include <machine/sigdebug.h>
#endif

#include <machine/alpha.h>

#include "ksyms.h"

struct vm_map *exec_map = NULL;
struct vm_map *mb_map = NULL;
struct vm_map *phys_map = NULL;

caddr_t msgbufaddr;

int	maxmem;			/* max memory per process */

int	totalphysmem;		/* total amount of physical memory in system */
int	physmem;		/* physical memory used by NetBSD + some rsvd */
int	resvmem;		/* amount of memory reserved for PROM */
int	unusedmem;		/* amount of memory for OS that we don't use */
int	unknownmem;		/* amount of memory with an unknown use */

int	cputype;		/* system type, from the RPB */

int	bootdev_debug = 0;	/* patchable, or from DDB */

/*
 * XXX We need an address to which we can assign things so that they
 * won't be optimized away because we didn't use the value.
 */
u_int32_t no_optimize;

/* the following is used externally (sysctl_hw) */
char	machine[] = MACHINE;		/* from <machine/param.h> */
char	machine_arch[] = MACHINE_ARCH;	/* from <machine/param.h> */
char	cpu_model[128];

struct	user *proc0paddr;

/* Number of machine cycles per microsecond */
u_int64_t	cycles_per_usec;

/* number of cpus in the box.  really! */
int		ncpus;

struct bootinfo_kernel bootinfo;

/* For built-in TCDS */
#if defined(DEC_3000_300) || defined(DEC_3000_500)
u_int8_t	dec_3000_scsiid[2], dec_3000_scsifast[2];
#endif

struct platform platform;

#if NKSYMS || defined(DDB) || defined(LKM)
/* start and end of kernel symbol table */
void	*ksym_start, *ksym_end;
#endif

/* for cpu_sysctl() */
int	alpha_unaligned_print = 1;	/* warn about unaligned accesses */
int	alpha_unaligned_fix = 1;	/* fix up unaligned accesses */
int	alpha_unaligned_sigbus = 0;	/* don't SIGBUS on fixed-up accesses */
int	alpha_fp_sync_complete = 0;	/* fp fixup if sync even without /s */

/*
 * XXX This should be dynamically sized, but we have the chicken-egg problem!
 * XXX it should also be larger than it is, because not all of the mddt
 * XXX clusters end up being used for VM.
 */
phys_ram_seg_t mem_clusters[VM_PHYSSEG_MAX];	/* low size bits overloaded */
int	mem_cluster_cnt;

int	cpu_dump __P((void));
int	cpu_dumpsize __P((void));
u_long	cpu_dump_mempagecnt __P((void));
void	dumpsys __P((void));
void	identifycpu __P((void));
void	printregs __P((struct reg *));

void
alpha_init(pfn, ptb, bim, bip, biv)
	u_long pfn;		/* first free PFN number */
	u_long ptb;		/* PFN of current level 1 page table */
	u_long bim;		/* bootinfo magic */
	u_long bip;		/* bootinfo pointer */
	u_long biv;		/* bootinfo version */
{
	extern char kernel_text[], _end[];
	struct mddt *mddtp;
	struct mddt_cluster *memc;
	int i, mddtweird;
	struct vm_physseg *vps;
	vaddr_t kernstart, kernend;
	paddr_t kernstartpfn, kernendpfn, pfn0, pfn1;
	cpuid_t cpu_id;
	struct cpu_info *ci;
	char *p;
	const char *bootinfo_msg;
	const struct cpuinit *c;

	/* NO OUTPUT ALLOWED UNTIL FURTHER NOTICE */

	/*
	 * Turn off interrupts (not mchecks) and floating point.
	 * Make sure the instruction and data streams are consistent.
	 */
	(void)alpha_pal_swpipl(ALPHA_PSL_IPL_HIGH);
	alpha_pal_wrfen(0);
	ALPHA_TBIA();
	alpha_pal_imb();

	/* Initialize the SCB. */
	scb_init();

	cpu_id = cpu_number();

#if defined(MULTIPROCESSOR)
	/*
	 * Set our SysValue to the address of our cpu_info structure.
	 * Secondary processors do this in their spinup trampoline.
	 */
	alpha_pal_wrval((u_long)&cpu_info_primary);
	cpu_info[cpu_id] = &cpu_info_primary;
#endif

	ci = curcpu();
	ci->ci_cpuid = cpu_id;

	/*
	 * Get critical system information (if possible, from the
	 * information provided by the boot program).
	 */
	bootinfo_msg = NULL;
	if (bim == BOOTINFO_MAGIC) {
		if (biv == 0) {		/* backward compat */
			biv = *(u_long *)bip;
			bip += 8;
		}
		switch (biv) {
		case 1: {
			struct bootinfo_v1 *v1p = (struct bootinfo_v1 *)bip;

			bootinfo.ssym = v1p->ssym;
			bootinfo.esym = v1p->esym;
			/* hwrpb may not be provided by boot block in v1 */
			if (v1p->hwrpb != NULL) {
				bootinfo.hwrpb_phys =
				    ((struct rpb *)v1p->hwrpb)->rpb_phys;
				bootinfo.hwrpb_size = v1p->hwrpbsize;
			} else {
				bootinfo.hwrpb_phys =
				    ((struct rpb *)HWRPB_ADDR)->rpb_phys;
				bootinfo.hwrpb_size =
				    ((struct rpb *)HWRPB_ADDR)->rpb_size;
			}
			memcpy(bootinfo.boot_flags, v1p->boot_flags,
			    min(sizeof v1p->boot_flags,
			      sizeof bootinfo.boot_flags));
			memcpy(bootinfo.booted_kernel, v1p->booted_kernel,
			    min(sizeof v1p->booted_kernel,
			      sizeof bootinfo.booted_kernel));
			/* booted dev not provided in bootinfo */
			init_prom_interface((struct rpb *)
			    ALPHA_PHYS_TO_K0SEG(bootinfo.hwrpb_phys));
                	prom_getenv(PROM_E_BOOTED_DEV, bootinfo.booted_dev,
			    sizeof bootinfo.booted_dev);
			break;
		}
		default:
			bootinfo_msg = "unknown bootinfo version";
			goto nobootinfo;
		}
	} else {
		bootinfo_msg = "boot program did not pass bootinfo";
nobootinfo:
		bootinfo.ssym = (u_long)_end;
		bootinfo.esym = (u_long)_end;
		bootinfo.hwrpb_phys = ((struct rpb *)HWRPB_ADDR)->rpb_phys;
		bootinfo.hwrpb_size = ((struct rpb *)HWRPB_ADDR)->rpb_size;
		init_prom_interface((struct rpb *)HWRPB_ADDR);
		prom_getenv(PROM_E_BOOTED_OSFLAGS, bootinfo.boot_flags,
		    sizeof bootinfo.boot_flags);
		prom_getenv(PROM_E_BOOTED_FILE, bootinfo.booted_kernel,
		    sizeof bootinfo.booted_kernel);
		prom_getenv(PROM_E_BOOTED_DEV, bootinfo.booted_dev,
		    sizeof bootinfo.booted_dev);
	}

	/*
	 * Initialize the kernel's mapping of the RPB.  It's needed for
	 * lots of things.
	 */
	hwrpb = (struct rpb *)ALPHA_PHYS_TO_K0SEG(bootinfo.hwrpb_phys);

#if defined(DEC_3000_300) || defined(DEC_3000_500)
	if (hwrpb->rpb_type == ST_DEC_3000_300 ||
	    hwrpb->rpb_type == ST_DEC_3000_500) {
		prom_getenv(PROM_E_SCSIID, dec_3000_scsiid,
		    sizeof(dec_3000_scsiid));
		prom_getenv(PROM_E_SCSIFAST, dec_3000_scsifast,
		    sizeof(dec_3000_scsifast));
	}
#endif

	/*
	 * Remember how many cycles there are per microsecond, 
	 * so that we can use delay().  Round up, for safety.
	 */
	cycles_per_usec = (hwrpb->rpb_cc_freq + 999999) / 1000000;

	/*
	 * Initialize the (temporary) bootstrap console interface, so
	 * we can use printf until the VM system starts being setup.
	 * The real console is initialized before then.
	 */
	init_bootstrap_console();

	/* OUTPUT NOW ALLOWED */

	/* delayed from above */
	if (bootinfo_msg)
		printf("WARNING: %s (0x%lx, 0x%lx, 0x%lx)\n",
		    bootinfo_msg, bim, bip, biv);

	/* Initialize the trap vectors on the primary processor. */
	trap_init();

	/*
	 * Find out this system's page size, and initialize
	 * PAGE_SIZE-dependent variables.
	 */
	if (hwrpb->rpb_page_size != ALPHA_PGBYTES)
		panic("page size %lu != %d?!", hwrpb->rpb_page_size,
		    ALPHA_PGBYTES);
	uvmexp.pagesize = hwrpb->rpb_page_size;
	uvm_setpagesize();

	/*
	 * Find out what hardware we're on, and do basic initialization.
	 */
	cputype = hwrpb->rpb_type;
	if (cputype < 0) {
		/*
		 * At least some white-box systems have SRM which
		 * reports a systype that's the negative of their
		 * blue-box counterpart.
		 */
		cputype = -cputype;
	}
	c = platform_lookup(cputype);
	if (c == NULL) {
		platform_not_supported();
		/* NOTREACHED */
	}
	(*c->init)();
	strcpy(cpu_model, platform.model);

	/*
	 * Initialize the real console, so that the bootstrap console is
	 * no longer necessary.
	 */
	(*platform.cons_init)();

#ifdef DIAGNOSTIC
	/* Paranoid sanity checking */

	/* We should always be running on the primary. */
	assert(hwrpb->rpb_primary_cpu_id == cpu_id);

	/*
	 * On single-CPU systypes, the primary should always be CPU 0,
	 * except on Alpha 8200 systems where the CPU id is related
	 * to the VID, which is related to the Turbo Laser node id.
	 */
	if (cputype != ST_DEC_21000)
		assert(hwrpb->rpb_primary_cpu_id == 0);
#endif

	/* NO MORE FIRMWARE ACCESS ALLOWED */
#ifdef _PMAP_MAY_USE_PROM_CONSOLE
	/*
	 * XXX (unless _PMAP_MAY_USE_PROM_CONSOLE is defined and
	 * XXX pmap_uses_prom_console() evaluates to non-zero.)
	 */
#endif

	/*
	 * Find the beginning and end of the kernel (and leave a
	 * bit of space before the beginning for the bootstrap
	 * stack).
	 */
	kernstart = trunc_page((vaddr_t)kernel_text) - 2 * PAGE_SIZE;
#if NKSYMS || defined(DDB) || defined(LKM)
	ksym_start = (void *)bootinfo.ssym;
	ksym_end   = (void *)bootinfo.esym;
	kernend = (vaddr_t)round_page((vaddr_t)ksym_end);
#else
	kernend = (vaddr_t)round_page((vaddr_t)_end);
#endif

	kernstartpfn = atop(ALPHA_K0SEG_TO_PHYS(kernstart));
	kernendpfn = atop(ALPHA_K0SEG_TO_PHYS(kernend));

	/*
	 * Find out how much memory is available, by looking at
	 * the memory cluster descriptors.  This also tries to do
	 * its best to detect things things that have never been seen
	 * before...
	 */
	mddtp = (struct mddt *)(((caddr_t)hwrpb) + hwrpb->rpb_memdat_off);

	/* MDDT SANITY CHECKING */
	mddtweird = 0;
	if (mddtp->mddt_cluster_cnt < 2) {
		mddtweird = 1;
		printf("WARNING: weird number of mem clusters: %lu\n",
		    mddtp->mddt_cluster_cnt);
	}

#if 0
	printf("Memory cluster count: %d\n", mddtp->mddt_cluster_cnt);
#endif

	for (i = 0; i < mddtp->mddt_cluster_cnt; i++) {
		memc = &mddtp->mddt_clusters[i];
#if 0
		printf("MEMC %d: pfn 0x%lx cnt 0x%lx usage 0x%lx\n", i,
		    memc->mddt_pfn, memc->mddt_pg_cnt, memc->mddt_usage);
#endif
		totalphysmem += memc->mddt_pg_cnt;
		if (mem_cluster_cnt < VM_PHYSSEG_MAX) {	/* XXX */
			mem_clusters[mem_cluster_cnt].start =
			    ptoa(memc->mddt_pfn);
			mem_clusters[mem_cluster_cnt].size =
			    ptoa(memc->mddt_pg_cnt);
			if (memc->mddt_usage & MDDT_mbz ||
			    memc->mddt_usage & MDDT_NONVOLATILE || /* XXX */
			    memc->mddt_usage & MDDT_PALCODE)
				mem_clusters[mem_cluster_cnt].size |=
				    PROT_READ;
			else
				mem_clusters[mem_cluster_cnt].size |=
				    PROT_READ | PROT_WRITE | PROT_EXEC;
			mem_cluster_cnt++;
		}

		if (memc->mddt_usage & MDDT_mbz) {
			mddtweird = 1;
			printf("WARNING: mem cluster %d has weird "
			    "usage 0x%lx\n", i, memc->mddt_usage);
			unknownmem += memc->mddt_pg_cnt;
			continue;
		}
		if (memc->mddt_usage & MDDT_NONVOLATILE) {
			/* XXX should handle these... */
			printf("WARNING: skipping non-volatile mem "
			    "cluster %d\n", i);
			unusedmem += memc->mddt_pg_cnt;
			continue;
		}
		if (memc->mddt_usage & MDDT_PALCODE) {
			resvmem += memc->mddt_pg_cnt;
			continue;
		}

		/*
		 * We have a memory cluster available for system
		 * software use.  We must determine if this cluster
		 * holds the kernel.
		 */
#ifdef _PMAP_MAY_USE_PROM_CONSOLE
		/*
		 * XXX If the kernel uses the PROM console, we only use the
		 * XXX memory after the kernel in the first system segment,
		 * XXX to avoid clobbering prom mapping, data, etc.
		 */
	    if (!pmap_uses_prom_console() || physmem == 0) {
#endif /* _PMAP_MAY_USE_PROM_CONSOLE */
		physmem += memc->mddt_pg_cnt;
		pfn0 = memc->mddt_pfn;
		pfn1 = memc->mddt_pfn + memc->mddt_pg_cnt;
		if (pfn0 <= kernstartpfn && kernendpfn <= pfn1) {
			/*
			 * Must compute the location of the kernel
			 * within the segment.
			 */
#if 0
			printf("Cluster %d contains kernel\n", i);
#endif
#ifdef _PMAP_MAY_USE_PROM_CONSOLE
		    if (!pmap_uses_prom_console()) {
#endif /* _PMAP_MAY_USE_PROM_CONSOLE */
			if (pfn0 < kernstartpfn) {
				/*
				 * There is a chunk before the kernel.
				 */
#if 0
				printf("Loading chunk before kernel: "
				    "0x%lx / 0x%lx\n", pfn0, kernstartpfn);
#endif
				uvm_page_physload(pfn0, kernstartpfn,
				    pfn0, kernstartpfn, VM_FREELIST_DEFAULT);
			}
#ifdef _PMAP_MAY_USE_PROM_CONSOLE
		    }
#endif /* _PMAP_MAY_USE_PROM_CONSOLE */
			if (kernendpfn < pfn1) {
				/*
				 * There is a chunk after the kernel.
				 */
#if 0
				printf("Loading chunk after kernel: "
				    "0x%lx / 0x%lx\n", kernendpfn, pfn1);
#endif
				uvm_page_physload(kernendpfn, pfn1,
				    kernendpfn, pfn1, VM_FREELIST_DEFAULT);
			}
		} else {
			/*
			 * Just load this cluster as one chunk.
			 */
#if 0
			printf("Loading cluster %d: 0x%lx / 0x%lx\n", i,
			    pfn0, pfn1);
#endif
			uvm_page_physload(pfn0, pfn1, pfn0, pfn1,
			    VM_FREELIST_DEFAULT);
		}
#ifdef _PMAP_MAY_USE_PROM_CONSOLE
	    }
#endif /* _PMAP_MAY_USE_PROM_CONSOLE */
	}

	/*
	 * Dump out the MDDT if it looks odd...
	 */
	if (mddtweird) {
		printf("\n");
		printf("complete memory cluster information:\n");
		for (i = 0; i < mddtp->mddt_cluster_cnt; i++) {
			printf("mddt %d:\n", i);
			printf("\tpfn %lx\n",
			    mddtp->mddt_clusters[i].mddt_pfn);
			printf("\tcnt %lx\n",
			    mddtp->mddt_clusters[i].mddt_pg_cnt);
			printf("\ttest %lx\n",
			    mddtp->mddt_clusters[i].mddt_pg_test);
			printf("\tbva %lx\n",
			    mddtp->mddt_clusters[i].mddt_v_bitaddr);
			printf("\tbpa %lx\n",
			    mddtp->mddt_clusters[i].mddt_p_bitaddr);
			printf("\tbcksum %lx\n",
			    mddtp->mddt_clusters[i].mddt_bit_cksum);
			printf("\tusage %lx\n",
			    mddtp->mddt_clusters[i].mddt_usage);
		}
		printf("\n");
	}

	if (totalphysmem == 0)
		panic("can't happen: system seems to have no memory!");
	maxmem = physmem;
#if 0
	printf("totalphysmem = %d\n", totalphysmem);
	printf("physmem = %d\n", physmem);
	printf("resvmem = %d\n", resvmem);
	printf("unusedmem = %d\n", unusedmem);
	printf("unknownmem = %d\n", unknownmem);
#endif

	/*
	 * Initialize error message buffer (at end of core).
	 */
	{
		vsize_t sz = (vsize_t)round_page(MSGBUFSIZE);
		vsize_t reqsz = sz;

		vps = &vm_physmem[vm_nphysseg - 1];

		/* shrink so that it'll fit in the last segment */
		if ((vps->avail_end - vps->avail_start) < atop(sz))
			sz = ptoa(vps->avail_end - vps->avail_start);

		vps->end -= atop(sz);
		vps->avail_end -= atop(sz);
		msgbufaddr = (caddr_t) ALPHA_PHYS_TO_K0SEG(ptoa(vps->end));
		initmsgbuf(msgbufaddr, sz);

		/* Remove the last segment if it now has no pages. */
		if (vps->start == vps->end)
			vm_nphysseg--;

		/* warn if the message buffer had to be shrunk */
		if (sz != reqsz)
			printf("WARNING: %ld bytes not available for msgbuf "
			    "in last cluster (%ld used)\n", reqsz, sz);

	}

	/*
	 * NOTE: It is safe to use uvm_pageboot_alloc() before
	 * pmap_bootstrap() because our pmap_virtual_space()
	 * returns compile-time constants.
	 */

	/*
	 * Init mapping for u page(s) for proc 0
	 */
	lwp0.l_addr = proc0paddr =
	    (struct user *)uvm_pageboot_alloc(UPAGES * PAGE_SIZE);

	/*
	 * Initialize the virtual memory system, and set the
	 * page table base register in proc 0's PCB.
	 */
	pmap_bootstrap(ALPHA_PHYS_TO_K0SEG(ptb << PGSHIFT),
	    hwrpb->rpb_max_asn, hwrpb->rpb_pcs_cnt);

	/*
	 * Initialize the rest of proc 0's PCB, and cache its physical
	 * address.
	 */
	lwp0.l_md.md_pcbpaddr =
	    (struct pcb *)ALPHA_K0SEG_TO_PHYS((vaddr_t)&proc0paddr->u_pcb);

	/*
	 * Set the kernel sp, reserving space for an (empty) trapframe,
	 * and make proc0's trapframe pointer point to it for sanity.
	 */
	proc0paddr->u_pcb.pcb_hw.apcb_ksp =
	    (u_int64_t)proc0paddr + USPACE - sizeof(struct trapframe);
	lwp0.l_md.md_tf =
	    (struct trapframe *)proc0paddr->u_pcb.pcb_hw.apcb_ksp;
	simple_lock_init(&proc0paddr->u_pcb.pcb_fpcpu_slock);

	/*
	 * Initialize the primary CPU's idle PCB to proc0's.  In a
	 * MULTIPROCESSOR configuration, each CPU will later get
	 * its own idle PCB when autoconfiguration runs.
	 */
	ci->ci_idle_pcb = &proc0paddr->u_pcb;
	ci->ci_idle_pcb_paddr = (u_long)lwp0.l_md.md_pcbpaddr;

	/* Indicate that proc0 has a CPU. */
	lwp0.l_cpu = ci;

	/*
	 * Look at arguments passed to us and compute boothowto.
	 */

	boothowto = RB_SINGLE;
#ifdef KADB
	boothowto |= RB_KDB;
#endif
	for (p = bootinfo.boot_flags; p && *p != '\0'; p++) {
		/*
		 * Note that we'd really like to differentiate case here,
		 * but the Alpha AXP Architecture Reference Manual
		 * says that we shouldn't.
		 */
		switch (*p) {
		case 'a': /* autoboot */
		case 'A':
			boothowto &= ~RB_SINGLE;
			break;

#ifdef DEBUG
		case 'c': /* crash dump immediately after autoconfig */
		case 'C':
			boothowto |= RB_DUMP;
			break;
#endif

#if defined(KGDB) || defined(DDB)
		case 'd': /* break into the kernel debugger ASAP */
		case 'D':
			boothowto |= RB_KDB;
			break;
#endif

		case 'h': /* always halt, never reboot */
		case 'H':
			boothowto |= RB_HALT;
			break;

#if 0
		case 'm': /* mini root present in memory */
		case 'M':
			boothowto |= RB_MINIROOT;
			break;
#endif

		case 'n': /* askname */
		case 'N':
			boothowto |= RB_ASKNAME;
			break;

		case 's': /* single-user (default, supported for sanity) */
		case 'S':
			boothowto |= RB_SINGLE;
			break;

		case 'q': /* quiet boot */
		case 'Q':
			boothowto |= AB_QUIET;
			break;
			
		case 'v': /* verbose boot */
		case 'V':
			boothowto |= AB_VERBOSE;
			break;

		case '-':
			/*
			 * Just ignore this.  It's not required, but it's
			 * common for it to be passed regardless.
			 */
			break;

		default:
			printf("Unrecognized boot flag '%c'.\n", *p);
			break;
		}
	}


	/*
	 * Figure out the number of cpus in the box, from RPB fields.
	 * Really.  We mean it.
	 */
	for (i = 0; i < hwrpb->rpb_pcs_cnt; i++) {
		struct pcs *pcsp;

		pcsp = LOCATE_PCS(hwrpb, i);
		if ((pcsp->pcs_flags & PCS_PP) != 0)
			ncpus++;
	}

	/*
	 * Initialize debuggers, and break into them if appropriate.
	 */
#if NKSYMS || defined(DDB) || defined(LKM)
	ksyms_init((int)((u_int64_t)ksym_end - (u_int64_t)ksym_start),
	    ksym_start, ksym_end);
#endif

	if (boothowto & RB_KDB) {
#if defined(KGDB)
		kgdb_debug_init = 1;
		kgdb_connect(1);
#elif defined(DDB)
		Debugger();
#endif
	}

	/*
	 * Figure out our clock frequency, from RPB fields.
	 */
	hz = hwrpb->rpb_intr_freq >> 12;
	if (!(60 <= hz && hz <= 10240)) {
		hz = 1024;
#ifdef DIAGNOSTIC
		printf("WARNING: unbelievable rpb_intr_freq: %ld (%d hz)\n",
			hwrpb->rpb_intr_freq, hz);
#endif
	}
}

void
consinit()
{

	/*
	 * Everything related to console initialization is done
	 * in alpha_init().
	 */
#if defined(DIAGNOSTIC) && defined(_PMAP_MAY_USE_PROM_CONSOLE)
	printf("consinit: %susing prom console\n",
	    pmap_uses_prom_console() ? "" : "not ");
#endif
}

#include "pckbc.h"
#include "pckbd.h"
#if (NPCKBC > 0) && (NPCKBD == 0)

#include <dev/ic/pckbcvar.h>

/*
 * This is called by the pbkbc driver if no pckbd is configured.
 * On the i386, it is used to glue in the old, deprecated console
 * code.  On the Alpha, it does nothing.
 */
int
pckbc_machdep_cnattach(kbctag, kbcslot)
	pckbc_tag_t kbctag;
	pckbc_slot_t kbcslot;
{

	return (ENXIO);
}
#endif /* NPCKBC > 0 && NPCKBD == 0 */

void
cpu_startup()
{
	vaddr_t minaddr, maxaddr;
	char pbuf[9];
#if defined(DEBUG)
	extern int pmapdebug;
	int opmapdebug = pmapdebug;

	pmapdebug = 0;
#endif

	/*
	 * Good {morning,afternoon,evening,night}.
	 */
	printf(version);
	identifycpu();
	format_bytes(pbuf, sizeof(pbuf), ptoa(totalphysmem));
	printf("total memory = %s\n", pbuf);
	format_bytes(pbuf, sizeof(pbuf), ptoa(resvmem));
	printf("(%s reserved for PROM, ", pbuf);
	format_bytes(pbuf, sizeof(pbuf), ptoa(physmem));
	printf("%s used by NetBSD)\n", pbuf);
	if (unusedmem) {
		format_bytes(pbuf, sizeof(pbuf), ptoa(unusedmem));
		printf("WARNING: unused memory = %s\n", pbuf);
	}
	if (unknownmem) {
		format_bytes(pbuf, sizeof(pbuf), ptoa(unknownmem));
		printf("WARNING: %s of memory with unknown purpose\n", pbuf);
	}

	minaddr = 0;

	/*
	 * Allocate a submap for exec arguments.  This map effectively
	 * limits the number of processes exec'ing at any time.
	 */
	exec_map = uvm_km_suballoc(kernel_map, &minaddr, &maxaddr,
				   16 * NCARGS, VM_MAP_PAGEABLE, FALSE, NULL);

	/*
	 * Allocate a submap for physio
	 */
	phys_map = uvm_km_suballoc(kernel_map, &minaddr, &maxaddr,
				   VM_PHYS_SIZE, 0, FALSE, NULL);

	/*
	 * No need to allocate an mbuf cluster submap.  Mbuf clusters
	 * are allocated via the pool allocator, and we use K0SEG to
	 * map those pages.
	 */

#if defined(DEBUG)
	pmapdebug = opmapdebug;
#endif
	format_bytes(pbuf, sizeof(pbuf), ptoa(uvmexp.free));
	printf("avail memory = %s\n", pbuf);
#if 0
	{
		extern u_long pmap_pages_stolen;

		format_bytes(pbuf, sizeof(pbuf), pmap_pages_stolen * PAGE_SIZE);
		printf("stolen memory for VM structures = %s\n", pbuf);
	}
#endif

	/*
	 * Set up the HWPCB so that it's safe to configure secondary
	 * CPUs.
	 */
	hwrpb_primary_init();
}

/*
 * Retrieve the platform name from the DSR.
 */
const char *
alpha_dsr_sysname()
{
	struct dsrdb *dsr;
	const char *sysname;

	/*
	 * DSR does not exist on early HWRPB versions.
	 */
	if (hwrpb->rpb_version < HWRPB_DSRDB_MINVERS)
		return (NULL);

	dsr = (struct dsrdb *)(((caddr_t)hwrpb) + hwrpb->rpb_dsrdb_off);
	sysname = (const char *)((caddr_t)dsr + (dsr->dsr_sysname_off +
	    sizeof(u_int64_t)));
	return (sysname);
}

/*
 * Lookup the system specified system variation in the provided table,
 * returning the model string on match.
 */
const char *
alpha_variation_name(variation, avtp)
	u_int64_t variation;
	const struct alpha_variation_table *avtp;
{
	int i;

	for (i = 0; avtp[i].avt_model != NULL; i++)
		if (avtp[i].avt_variation == variation)
			return (avtp[i].avt_model);
	return (NULL);
}

/*
 * Generate a default platform name based for unknown system variations.
 */
const char *
alpha_unknown_sysname()
{
	static char s[128];		/* safe size */

	sprintf(s, "%s family, unknown model variation 0x%lx",
	    platform.family, hwrpb->rpb_variation & SV_ST_MASK);
	return ((const char *)s);
}

void
identifycpu()
{
	char *s;
	int i;

	/*
	 * print out CPU identification information.
	 */
	printf("%s", cpu_model);
	for(s = cpu_model; *s; ++s)
		if(strncasecmp(s, "MHz", 3) == 0)
			goto skipMHz;
	printf(", %ldMHz", hwrpb->rpb_cc_freq / 1000000);
skipMHz:
	printf(", s/n ");
	for (i = 0; i < 10; i++)
		printf("%c", hwrpb->rpb_ssn[i]);
	printf("\n");
	printf("%ld byte page size, %d processor%s.\n",
	    hwrpb->rpb_page_size, ncpus, ncpus == 1 ? "" : "s");
#if 0
	/* this isn't defined for any systems that we run on? */
	printf("serial number 0x%lx 0x%lx\n",
	    ((long *)hwrpb->rpb_ssn)[0], ((long *)hwrpb->rpb_ssn)[1]);

	/* and these aren't particularly useful! */
	printf("variation: 0x%lx, revision 0x%lx\n",
	    hwrpb->rpb_variation, *(long *)hwrpb->rpb_revision);
#endif
}

int	waittime = -1;
struct pcb dumppcb;

void
cpu_reboot(howto, bootstr)
	int howto;
	char *bootstr;
{
#if defined(MULTIPROCESSOR)
	u_long cpu_id = cpu_number();
	u_long wait_mask = (1UL << cpu_id) |
			   (1UL << hwrpb->rpb_primary_cpu_id);
	int i;
#endif

	/* If "always halt" was specified as a boot flag, obey. */
	if ((boothowto & RB_HALT) != 0)
		howto |= RB_HALT;

	boothowto = howto;

	/* If system is cold, just halt. */
	if (cold) {
		boothowto |= RB_HALT;
		goto haltsys;
	}

	if ((boothowto & RB_NOSYNC) == 0 && waittime < 0) {
		waittime = 0;
		vfs_shutdown();
		/*
		 * If we've been adjusting the clock, the todr
		 * will be out of synch; adjust it now.
		 */
		resettodr();
	}

	/* Disable interrupts. */
	splhigh();

#if defined(MULTIPROCESSOR)
	/*
	 * Halt all other CPUs.  If we're not the primary, the
	 * primary will spin, waiting for us to halt.
	 */
	alpha_broadcast_ipi(ALPHA_IPI_HALT);

	for (i = 0; i < 10000; i++) {
		alpha_mb();
		if (cpus_running == wait_mask)
			break;
		delay(1000);
	}
	alpha_mb();
	if (cpus_running != wait_mask)
		printf("WARNING: Unable to halt secondary CPUs (0x%lx)\n",
		    cpus_running);
#endif /* MULTIPROCESSOR */

	/* If rebooting and a dump is requested do it. */
#if 0
	if ((boothowto & (RB_DUMP | RB_HALT)) == RB_DUMP)
#else
	if (boothowto & RB_DUMP)
#endif
		dumpsys();

haltsys:

	/* run any shutdown hooks */
	doshutdownhooks();

#ifdef BOOTKEY
	printf("hit any key to %s...\n", howto & RB_HALT ? "halt" : "reboot");
	cnpollc(1);	/* for proper keyboard command handling */
	cngetc();
	cnpollc(0);
	printf("\n");
#endif

	/* Finally, powerdown/halt/reboot the system. */
	if ((boothowto & RB_POWERDOWN) == RB_POWERDOWN &&
	    platform.powerdown != NULL) {
		(*platform.powerdown)();
		printf("WARNING: powerdown failed!\n");
	}
	printf("%s\n\n", (boothowto & RB_HALT) ? "halted." : "rebooting...");
#if defined(MULTIPROCESSOR)
	if (cpu_id != hwrpb->rpb_primary_cpu_id)
		cpu_halt();
	else
#endif
		prom_halt(boothowto & RB_HALT);
	/*NOTREACHED*/
}

/*
 * These variables are needed by /sbin/savecore
 */
u_int32_t dumpmag = 0x8fca0101;	/* magic number */
int 	dumpsize = 0;		/* pages */
long	dumplo = 0; 		/* blocks */

/*
 * cpu_dumpsize: calculate size of machine-dependent kernel core dump headers.
 */
int
cpu_dumpsize()
{
	int size;

	size = ALIGN(sizeof(kcore_seg_t)) + ALIGN(sizeof(cpu_kcore_hdr_t)) +
	    ALIGN(mem_cluster_cnt * sizeof(phys_ram_seg_t));
	if (roundup(size, dbtob(1)) != dbtob(1))
		return -1;

	return (1);
}

/*
 * cpu_dump_mempagecnt: calculate size of RAM (in pages) to be dumped.
 */
u_long
cpu_dump_mempagecnt()
{
	u_long i, n;

	n = 0;
	for (i = 0; i < mem_cluster_cnt; i++)
		n += atop(mem_clusters[i].size);
	return (n);
}

/*
 * cpu_dump: dump machine-dependent kernel core dump headers.
 */
int
cpu_dump()
{
	int (*dump) __P((dev_t, daddr_t, caddr_t, size_t));
	char buf[dbtob(1)];
	kcore_seg_t *segp;
	cpu_kcore_hdr_t *cpuhdrp;
	phys_ram_seg_t *memsegp;
	const struct bdevsw *bdev;
	int i;

	bdev = bdevsw_lookup(dumpdev);
	if (bdev == NULL)
		return (ENXIO);
	dump = bdev->d_dump;

	memset(buf, 0, sizeof buf);
	segp = (kcore_seg_t *)buf;
	cpuhdrp = (cpu_kcore_hdr_t *)&buf[ALIGN(sizeof(*segp))];
	memsegp = (phys_ram_seg_t *)&buf[ ALIGN(sizeof(*segp)) +
	    ALIGN(sizeof(*cpuhdrp))];

	/*
	 * Generate a segment header.
	 */
	CORE_SETMAGIC(*segp, KCORE_MAGIC, MID_MACHINE, CORE_CPU);
	segp->c_size = dbtob(1) - ALIGN(sizeof(*segp));

	/*
	 * Add the machine-dependent header info.
	 */
	cpuhdrp->lev1map_pa = ALPHA_K0SEG_TO_PHYS((vaddr_t)kernel_lev1map);
	cpuhdrp->page_size = PAGE_SIZE;
	cpuhdrp->nmemsegs = mem_cluster_cnt;

	/*
	 * Fill in the memory segment descriptors.
	 */
	for (i = 0; i < mem_cluster_cnt; i++) {
		memsegp[i].start = mem_clusters[i].start;
		memsegp[i].size = mem_clusters[i].size & ~PAGE_MASK;
	}

	return (dump(dumpdev, dumplo, (caddr_t)buf, dbtob(1)));
}

/*
 * This is called by main to set dumplo and dumpsize.
 * Dumps always skip the first PAGE_SIZE of disk space
 * in case there might be a disk label stored there.
 * If there is extra space, put dump at the end to
 * reduce the chance that swapping trashes it.
 */
void
cpu_dumpconf()
{
	const struct bdevsw *bdev;
	int nblks, dumpblks;	/* size of dump area */

	if (dumpdev == NODEV)
		goto bad;
	bdev = bdevsw_lookup(dumpdev);
	if (bdev == NULL)
		panic("dumpconf: bad dumpdev=0x%x", dumpdev);
	if (bdev->d_psize == NULL)
		goto bad;
	nblks = (*bdev->d_psize)(dumpdev);
	if (nblks <= ctod(1))
		goto bad;

	dumpblks = cpu_dumpsize();
	if (dumpblks < 0)
		goto bad;
	dumpblks += ctod(cpu_dump_mempagecnt());

	/* If dump won't fit (incl. room for possible label), punt. */
	if (dumpblks > (nblks - ctod(1)))
		goto bad;

	/* Put dump at end of partition */
	dumplo = nblks - dumpblks;

	/* dumpsize is in page units, and doesn't include headers. */
	dumpsize = cpu_dump_mempagecnt();
	return;

bad:
	dumpsize = 0;
	return;
}

/*
 * Dump the kernel's image to the swap partition.
 */
#define	BYTES_PER_DUMP	PAGE_SIZE

void
dumpsys()
{
	const struct bdevsw *bdev;
	u_long totalbytesleft, bytes, i, n, memcl;
	u_long maddr;
	int psize;
	daddr_t blkno;
	int (*dump) __P((dev_t, daddr_t, caddr_t, size_t));
	int error;

	/* Save registers. */
	savectx(&dumppcb);

	if (dumpdev == NODEV)
		return;
	bdev = bdevsw_lookup(dumpdev);
	if (bdev == NULL || bdev->d_psize == NULL)
		return;

	/*
	 * For dumps during autoconfiguration,
	 * if dump device has already configured...
	 */
	if (dumpsize == 0)
		cpu_dumpconf();
	if (dumplo <= 0) {
		printf("\ndump to dev %u,%u not possible\n", major(dumpdev),
		    minor(dumpdev));
		return;
	}
	printf("\ndumping to dev %u,%u offset %ld\n", major(dumpdev),
	    minor(dumpdev), dumplo);

	psize = (*bdev->d_psize)(dumpdev);
	printf("dump ");
	if (psize == -1) {
		printf("area unavailable\n");
		return;
	}

	/* XXX should purge all outstanding keystrokes. */

	if ((error = cpu_dump()) != 0)
		goto err;

	totalbytesleft = ptoa(cpu_dump_mempagecnt());
	blkno = dumplo + cpu_dumpsize();
	dump = bdev->d_dump;
	error = 0;

	for (memcl = 0; memcl < mem_cluster_cnt; memcl++) {
		maddr = mem_clusters[memcl].start;
		bytes = mem_clusters[memcl].size & ~PAGE_MASK;

		for (i = 0; i < bytes; i += n, totalbytesleft -= n) {

			/* Print out how many MBs we to go. */
			if ((totalbytesleft % (1024*1024)) == 0)
				printf("%ld ", totalbytesleft / (1024 * 1024));

			/* Limit size for next transfer. */
			n = bytes - i;
			if (n > BYTES_PER_DUMP)
				n =  BYTES_PER_DUMP;
	
			error = (*dump)(dumpdev, blkno,
			    (caddr_t)ALPHA_PHYS_TO_K0SEG(maddr), n);
			if (error)
				goto err;
			maddr += n;
			blkno += btodb(n);			/* XXX? */

			/* XXX should look for keystrokes, to cancel. */
		}
	}

err:
	switch (error) {

	case ENXIO:
		printf("device bad\n");
		break;

	case EFAULT:
		printf("device not ready\n");
		break;

	case EINVAL:
		printf("area improper\n");
		break;

	case EIO:
		printf("i/o error\n");
		break;

	case EINTR:
		printf("aborted from console\n");
		break;

	case 0:
		printf("succeeded\n");
		break;

	default:
		printf("error %d\n", error);
		break;
	}
	printf("\n\n");
	delay(1000);
}

void
frametoreg(framep, regp)
	const struct trapframe *framep;
	struct reg *regp;
{

	regp->r_regs[R_V0] = framep->tf_regs[FRAME_V0];
	regp->r_regs[R_T0] = framep->tf_regs[FRAME_T0];
	regp->r_regs[R_T1] = framep->tf_regs[FRAME_T1];
	regp->r_regs[R_T2] = framep->tf_regs[FRAME_T2];
	regp->r_regs[R_T3] = framep->tf_regs[FRAME_T3];
	regp->r_regs[R_T4] = framep->tf_regs[FRAME_T4];
	regp->r_regs[R_T5] = framep->tf_regs[FRAME_T5];
	regp->r_regs[R_T6] = framep->tf_regs[FRAME_T6];
	regp->r_regs[R_T7] = framep->tf_regs[FRAME_T7];
	regp->r_regs[R_S0] = framep->tf_regs[FRAME_S0];
	regp->r_regs[R_S1] = framep->tf_regs[FRAME_S1];
	regp->r_regs[R_S2] = framep->tf_regs[FRAME_S2];
	regp->r_regs[R_S3] = framep->tf_regs[FRAME_S3];
	regp->r_regs[R_S4] = framep->tf_regs[FRAME_S4];
	regp->r_regs[R_S5] = framep->tf_regs[FRAME_S5];
	regp->r_regs[R_S6] = framep->tf_regs[FRAME_S6];
	regp->r_regs[R_A0] = framep->tf_regs[FRAME_A0];
	regp->r_regs[R_A1] = framep->tf_regs[FRAME_A1];
	regp->r_regs[R_A2] = framep->tf_regs[FRAME_A2];
	regp->r_regs[R_A3] = framep->tf_regs[FRAME_A3];
	regp->r_regs[R_A4] = framep->tf_regs[FRAME_A4];
	regp->r_regs[R_A5] = framep->tf_regs[FRAME_A5];
	regp->r_regs[R_T8] = framep->tf_regs[FRAME_T8];
	regp->r_regs[R_T9] = framep->tf_regs[FRAME_T9];
	regp->r_regs[R_T10] = framep->tf_regs[FRAME_T10];
	regp->r_regs[R_T11] = framep->tf_regs[FRAME_T11];
	regp->r_regs[R_RA] = framep->tf_regs[FRAME_RA];
	regp->r_regs[R_T12] = framep->tf_regs[FRAME_T12];
	regp->r_regs[R_AT] = framep->tf_regs[FRAME_AT];
	regp->r_regs[R_GP] = framep->tf_regs[FRAME_GP];
	/* regp->r_regs[R_SP] = framep->tf_regs[FRAME_SP]; XXX */
	regp->r_regs[R_ZERO] = 0;
}

void
regtoframe(regp, framep)
	const struct reg *regp;
	struct trapframe *framep;
{

	framep->tf_regs[FRAME_V0] = regp->r_regs[R_V0];
	framep->tf_regs[FRAME_T0] = regp->r_regs[R_T0];
	framep->tf_regs[FRAME_T1] = regp->r_regs[R_T1];
	framep->tf_regs[FRAME_T2] = regp->r_regs[R_T2];
	framep->tf_regs[FRAME_T3] = regp->r_regs[R_T3];
	framep->tf_regs[FRAME_T4] = regp->r_regs[R_T4];
	framep->tf_regs[FRAME_T5] = regp->r_regs[R_T5];
	framep->tf_regs[FRAME_T6] = regp->r_regs[R_T6];
	framep->tf_regs[FRAME_T7] = regp->r_regs[R_T7];
	framep->tf_regs[FRAME_S0] = regp->r_regs[R_S0];
	framep->tf_regs[FRAME_S1] = regp->r_regs[R_S1];
	framep->tf_regs[FRAME_S2] = regp->r_regs[R_S2];
	framep->tf_regs[FRAME_S3] = regp->r_regs[R_S3];
	framep->tf_regs[FRAME_S4] = regp->r_regs[R_S4];
	framep->tf_regs[FRAME_S5] = regp->r_regs[R_S5];
	framep->tf_regs[FRAME_S6] = regp->r_regs[R_S6];
	framep->tf_regs[FRAME_A0] = regp->r_regs[R_A0];
	framep->tf_regs[FRAME_A1] = regp->r_regs[R_A1];
	framep->tf_regs[FRAME_A2] = regp->r_regs[R_A2];
	framep->tf_regs[FRAME_A3] = regp->r_regs[R_A3];
	framep->tf_regs[FRAME_A4] = regp->r_regs[R_A4];
	framep->tf_regs[FRAME_A5] = regp->r_regs[R_A5];
	framep->tf_regs[FRAME_T8] = regp->r_regs[R_T8];
	framep->tf_regs[FRAME_T9] = regp->r_regs[R_T9];
	framep->tf_regs[FRAME_T10] = regp->r_regs[R_T10];
	framep->tf_regs[FRAME_T11] = regp->r_regs[R_T11];
	framep->tf_regs[FRAME_RA] = regp->r_regs[R_RA];
	framep->tf_regs[FRAME_T12] = regp->r_regs[R_T12];
	framep->tf_regs[FRAME_AT] = regp->r_regs[R_AT];
	framep->tf_regs[FRAME_GP] = regp->r_regs[R_GP];
	/* framep->tf_regs[FRAME_SP] = regp->r_regs[R_SP]; XXX */
	/* ??? = regp->r_regs[R_ZERO]; */
}

void
printregs(regp)
	struct reg *regp;
{
	int i;

	for (i = 0; i < 32; i++)
		printf("R%d:\t0x%016lx%s", i, regp->r_regs[i],
		   i & 1 ? "\n" : "\t");
}

void
regdump(framep)
	struct trapframe *framep;
{
	struct reg reg;

	frametoreg(framep, &reg);
	reg.r_regs[R_SP] = alpha_pal_rdusp();

	printf("REGISTERS:\n");
	printregs(&reg);
}



void *
getframe(const struct lwp *l, int sig, int *onstack)
{
	void * frame;
	struct proc *p;

	p = l->l_proc;

	/* Do we need to jump onto the signal stack? */
	*onstack =
	    (p->p_sigctx.ps_sigstk.ss_flags & (SS_DISABLE | SS_ONSTACK)) == 0 &&
	    (SIGACTION(p, sig).sa_flags & SA_ONSTACK) != 0;

	if (*onstack)
		frame = (void *)((caddr_t)p->p_sigctx.ps_sigstk.ss_sp +
					p->p_sigctx.ps_sigstk.ss_size);
	else
		frame = (void *)(alpha_pal_rdusp());
	return (frame);
}	

void
buildcontext(struct lwp *l, const void *catcher, const void *tramp, const void *fp)
{
	struct trapframe *tf = l->l_md.md_tf;

	tf->tf_regs[FRAME_RA] = (u_int64_t)tramp;
	tf->tf_regs[FRAME_PC] = (u_int64_t)catcher;
	tf->tf_regs[FRAME_T12] = (u_int64_t)catcher;
	alpha_pal_wrusp((unsigned long)fp);
}


/*
 * Send an interrupt to process, new style
 */
void
sendsig_siginfo(const ksiginfo_t *ksi, const sigset_t *mask)
{
	struct lwp *l = curlwp;
	struct proc *p = l->l_proc;
	struct sigacts *ps = p->p_sigacts;
	int onstack, sig = ksi->ksi_signo;
	struct sigframe_siginfo *fp, frame;
	struct trapframe *tf;
	sig_t catcher = SIGACTION(p, ksi->ksi_signo).sa_handler;

	fp = (struct sigframe_siginfo *)getframe(l,ksi->ksi_signo,&onstack);
	tf = l->l_md.md_tf;

	/* Allocate space for the signal handler context. */
	fp--;

	/* Build stack frame for signal trampoline. */
	switch (ps->sa_sigdesc[sig].sd_vers) {
	case 0:		/* handled by sendsig_sigcontext */
	case 1:		/* handled by sendsig_sigcontext */
	default:	/* unknown version */
		printf("nsendsig: bad version %d\n",
		    ps->sa_sigdesc[sig].sd_vers);
		sigexit(l, SIGILL);
	case 2:
		break;
	}

#ifdef DEBUG
	if ((sigdebug & SDB_KSTACK) && p->p_pid == sigpid)
		printf("sendsig_siginfo(%d): sig %d ssp %p usp %p\n", p->p_pid,
		    sig, &onstack, fp);
#endif

	/* Build stack frame for signal trampoline. */

	frame.sf_si._info = ksi->ksi_info;
	frame.sf_uc.uc_flags = _UC_SIGMASK;
	frame.sf_uc.uc_sigmask = *mask;
	frame.sf_uc.uc_link = NULL;
	memset(&frame.sf_uc.uc_stack, 0, sizeof(frame.sf_uc.uc_stack));
	cpu_getmcontext(l, &frame.sf_uc.uc_mcontext, &frame.sf_uc.uc_flags);

	if (copyout(&frame, fp, sizeof(frame)) != 0) {
		/*
		 * Process has trashed its stack; give it an illegal
		 * instruction to halt it in its tracks.
		 */
#ifdef DEBUG
		if ((sigdebug & SDB_KSTACK) && p->p_pid == sigpid)
			printf("sendsig_siginfo(%d): copyout failed on sig %d\n",
			    p->p_pid, sig);
#endif
		sigexit(l, SIGILL);
		/* NOTREACHED */
	}

#ifdef DEBUG
	if (sigdebug & SDB_FOLLOW)
		printf("sendsig_siginfo(%d): sig %d usp %p code %x\n",
		       p->p_pid, sig, fp, ksi->ksi_code);
#endif

	/*
	 * Set up the registers to directly invoke the signal handler.  The
	 * signal trampoline is then used to return from the signal.  Note
	 * the trampoline version numbers are coordinated with machine-
	 * dependent code in libc.
	 */
	
	tf->tf_regs[FRAME_A0] = sig;
	tf->tf_regs[FRAME_A1] = (u_int64_t)&fp->sf_si;
	tf->tf_regs[FRAME_A2] = (u_int64_t)&fp->sf_uc;

	buildcontext(l,catcher,ps->sa_sigdesc[sig].sd_tramp,fp);

	/* Remember that we're now on the signal stack. */
	if (onstack)
		p->p_sigctx.ps_sigstk.ss_flags |= SS_ONSTACK;

#ifdef DEBUG
	if (sigdebug & SDB_FOLLOW)
		printf("sendsig_siginfo(%d): pc %lx, catcher %lx\n", p->p_pid,
		    tf->tf_regs[FRAME_PC], tf->tf_regs[FRAME_A3]);
	if ((sigdebug & SDB_KSTACK) && p->p_pid == sigpid)
		printf("sendsig_siginfo(%d): sig %d returns\n",
		    p->p_pid, sig);
#endif
}


void
sendsig(const ksiginfo_t *ksi, const sigset_t *mask)
{
#ifdef COMPAT_16
	if (curproc->p_sigacts->sa_sigdesc[ksi->ksi_signo].sd_vers < 2) {
		sendsig_sigcontext(ksi, mask);
	} else {
#endif
#ifdef DEBUG
	if (sigdebug & SDB_FOLLOW)
		printf("sendsig: sendsig called: sig %d vers %d\n",
		       ksi->ksi_signo,
		       curproc->p_sigacts->sa_sigdesc[ksi->ksi_signo].sd_vers);
#endif
		sendsig_siginfo(ksi, mask);
#ifdef COMPAT_16
	}
#endif
}

void 
cpu_upcall(struct lwp *l, int type, int nevents, int ninterrupted, void *sas, void *ap, void *sp, sa_upcall_t upcall)
{
       	struct trapframe *tf;

	tf = l->l_md.md_tf;

	tf->tf_regs[FRAME_PC] = (u_int64_t)upcall;
	tf->tf_regs[FRAME_RA] = 0;
	tf->tf_regs[FRAME_A0] = type;
	tf->tf_regs[FRAME_A1] = (u_int64_t)sas;
	tf->tf_regs[FRAME_A2] = nevents;
	tf->tf_regs[FRAME_A3] = ninterrupted;
	tf->tf_regs[FRAME_A4] = (u_int64_t)ap;
	tf->tf_regs[FRAME_T12] = (u_int64_t)upcall;  /* t12 is pv */
	alpha_pal_wrusp((unsigned long)sp);
}

/*
 * machine dependent system variables.
 */
SYSCTL_SETUP(sysctl_machdep_setup, "sysctl machdep subtree setup")
{

	sysctl_createv(SYSCTL_PERMANENT,
		       CTLTYPE_NODE, "machdep", NULL,
		       NULL, 0, NULL, 0,
		       CTL_MACHDEP, CTL_EOL);

	sysctl_createv(SYSCTL_PERMANENT,
		       CTLTYPE_STRUCT, "console_device", NULL,
		       sysctl_consdev, 0, NULL, sizeof(dev_t),
		       CTL_MACHDEP, CPU_CONSDEV, CTL_EOL);
	sysctl_createv(SYSCTL_PERMANENT,
		       CTLTYPE_STRING, "root_device", NULL,
		       sysctl_root_device, 0, NULL, 0,
		       CTL_MACHDEP, CPU_ROOT_DEVICE, CTL_EOL);
	sysctl_createv(SYSCTL_PERMANENT|SYSCTL_READWRITE,
		       CTLTYPE_INT, "unaligned_print", NULL,
		       NULL, 0, &alpha_unaligned_print, 0,
		       CTL_MACHDEP, CPU_UNALIGNED_PRINT, CTL_EOL);
	sysctl_createv(SYSCTL_PERMANENT|SYSCTL_READWRITE,
		       CTLTYPE_INT, "unaligned_fix", NULL,
		       NULL, 0, &alpha_unaligned_fix, 0,
		       CTL_MACHDEP, CPU_UNALIGNED_FIX, CTL_EOL);
	sysctl_createv(SYSCTL_PERMANENT|SYSCTL_READWRITE,
		       CTLTYPE_INT, "unaligned_sigbus", NULL,
		       NULL, 0, &alpha_unaligned_sigbus, 0,
		       CTL_MACHDEP, CPU_UNALIGNED_SIGBUS, CTL_EOL);
	sysctl_createv(SYSCTL_PERMANENT,
		       CTLTYPE_STRING, "booted_kernel", NULL,
		       NULL, 0, bootinfo.booted_kernel, 0,
		       CTL_MACHDEP, CPU_BOOTED_KERNEL, CTL_EOL);
	sysctl_createv(SYSCTL_PERMANENT|SYSCTL_READWRITE,
		       CTLTYPE_INT, "fp_sync_complete", NULL,
		       NULL, 0, &alpha_fp_sync_complete, 0,
		       CTL_MACHDEP, CPU_FP_SYNC_COMPLETE, CTL_EOL);
}

/*
 * Set registers on exec.
 */
void
setregs(l, pack, stack)
	register struct lwp *l;
	struct exec_package *pack;
	u_long stack;
{
	struct trapframe *tfp = l->l_md.md_tf;
#ifdef DEBUG
	int i;
#endif

#ifdef DEBUG
	/*
	 * Crash and dump, if the user requested it.
	 */
	if (boothowto & RB_DUMP)
		panic("crash requested by boot flags");
#endif

#ifdef DEBUG
	for (i = 0; i < FRAME_SIZE; i++)
		tfp->tf_regs[i] = 0xbabefacedeadbeef;
#else
	memset(tfp->tf_regs, 0, FRAME_SIZE * sizeof tfp->tf_regs[0]);
#endif
	memset(&l->l_addr->u_pcb.pcb_fp, 0, sizeof l->l_addr->u_pcb.pcb_fp);
	alpha_pal_wrusp(stack);
	tfp->tf_regs[FRAME_PS] = ALPHA_PSL_USERSET;
	tfp->tf_regs[FRAME_PC] = pack->ep_entry & ~3;

	tfp->tf_regs[FRAME_A0] = stack;			/* a0 = sp */
	tfp->tf_regs[FRAME_A1] = 0;			/* a1 = rtld cleanup */
	tfp->tf_regs[FRAME_A2] = 0;			/* a2 = rtld object */
	tfp->tf_regs[FRAME_A3] = (u_int64_t)l->l_proc->p_psstr;	/* a3 = ps_strings */
	tfp->tf_regs[FRAME_T12] = tfp->tf_regs[FRAME_PC];	/* a.k.a. PV */

	l->l_md.md_flags &= ~MDP_FPUSED;
	if (__predict_true((l->l_md.md_flags & IEEE_INHERIT) == 0)) {
		l->l_md.md_flags &= ~MDP_FP_C;
		l->l_addr->u_pcb.pcb_fp.fpr_cr = FPCR_DYN(FP_RN);
	}
	if (l->l_addr->u_pcb.pcb_fpcpu != NULL)
		fpusave_proc(l, 0);
}

/*
 * Release the FPU.
 */
void
fpusave_cpu(struct cpu_info *ci, int save)
{
	struct lwp *l;
#if defined(MULTIPROCESSOR)
	int s;
#endif

	KDASSERT(ci == curcpu());

#if defined(MULTIPROCESSOR)
	atomic_setbits_ulong(&ci->ci_flags, CPUF_FPUSAVE);
#endif

	l = ci->ci_fpcurlwp;
	if (l == NULL)
		goto out;

	if (save) {
		alpha_pal_wrfen(1);
		savefpstate(&l->l_addr->u_pcb.pcb_fp);
	}

	alpha_pal_wrfen(0);

	FPCPU_LOCK(&l->l_addr->u_pcb, s);

	l->l_addr->u_pcb.pcb_fpcpu = NULL;
	ci->ci_fpcurlwp = NULL;

	FPCPU_UNLOCK(&l->l_addr->u_pcb, s);

 out:
#if defined(MULTIPROCESSOR)
	atomic_clearbits_ulong(&ci->ci_flags, CPUF_FPUSAVE);
#endif
	return;
}

/*
 * Synchronize FP state for this process.
 */
void
fpusave_proc(struct lwp *l, int save)
{
	struct cpu_info *ci = curcpu();
	struct cpu_info *oci;
#if defined(MULTIPROCESSOR)
	u_long ipi = save ? ALPHA_IPI_SYNCH_FPU : ALPHA_IPI_DISCARD_FPU;
	int s, spincount;
#endif

	KDASSERT(l->l_addr != NULL);

	FPCPU_LOCK(&l->l_addr->u_pcb, s);

	oci = l->l_addr->u_pcb.pcb_fpcpu;
	if (oci == NULL) {
		FPCPU_UNLOCK(&l->l_addr->u_pcb, s);
		return;
	}

#if defined(MULTIPROCESSOR)
	if (oci == ci) {
		KASSERT(ci->ci_fpcurlwp == l);
		FPCPU_UNLOCK(&l->l_addr->u_pcb, s);
		fpusave_cpu(ci, save);
		return;
	}

	KASSERT(oci->ci_fpcurlwp == l);
	alpha_send_ipi(oci->ci_cpuid, ipi);
	FPCPU_UNLOCK(&l->l_addr->u_pcb, s);

	spincount = 0;
	while (l->l_addr->u_pcb.pcb_fpcpu != NULL) {
		spincount++;
		delay(1000);	/* XXX */
		if (spincount > 10000)
			panic("fpsave ipi didn't");
	}
#else
	KASSERT(ci->ci_fpcurlwp == l);
	FPCPU_UNLOCK(&l->l_addr->u_pcb, s);
	fpusave_cpu(ci, save);
#endif /* MULTIPROCESSOR */
}

/*
 * Wait "n" microseconds.
 */
void
delay(n)
	unsigned long n;
{
	unsigned long pcc0, pcc1, curcycle, cycles, usec;

	if (n == 0)
		return;

	pcc0 = alpha_rpcc() & 0xffffffffUL;
	cycles = 0;
	usec = 0;

	while (usec <= n) {
		/*
		 * Get the next CPU cycle count- assumes that we cannot
		 * have had more than one 32 bit overflow.
		 */
		pcc1 = alpha_rpcc() & 0xffffffffUL;
		if (pcc1 < pcc0)
			curcycle = (pcc1 + 0x100000000UL) - pcc0;
		else
			curcycle = pcc1 - pcc0;

		/*
		 * We now have the number of processor cycles since we
		 * last checked. Add the current cycle count to the
		 * running total. If it's over cycles_per_usec, increment
		 * the usec counter.
		 */
		cycles += curcycle;
		while (cycles > cycles_per_usec) {
			usec++;
			cycles -= cycles_per_usec;
		}
		pcc0 = pcc1;
	}
}

#ifdef EXEC_ECOFF
void
cpu_exec_ecoff_setregs(l, epp, stack)
	struct lwp *l;
	struct exec_package *epp;
	u_long stack;
{
	struct ecoff_exechdr *execp = (struct ecoff_exechdr *)epp->ep_hdr;

	l->l_md.md_tf->tf_regs[FRAME_GP] = execp->a.gp_value;
}

/*
 * cpu_exec_ecoff_hook():
 *	cpu-dependent ECOFF format hook for execve().
 * 
 * Do any machine-dependent diddling of the exec package when doing ECOFF.
 *
 */
int
cpu_exec_ecoff_probe(p, epp)
	struct proc *p;
	struct exec_package *epp;
{
	struct ecoff_exechdr *execp = (struct ecoff_exechdr *)epp->ep_hdr;
	int error;

	if (execp->f.f_magic == ECOFF_MAGIC_NETBSD_ALPHA)
		error = 0;
	else
		error = ENOEXEC;

	return (error);
}
#endif /* EXEC_ECOFF */

int
alpha_pa_access(pa)
	u_long pa;
{
	int i;

	for (i = 0; i < mem_cluster_cnt; i++) {
		if (pa < mem_clusters[i].start)
			continue;
		if ((pa - mem_clusters[i].start) >=
		    (mem_clusters[i].size & ~PAGE_MASK))
			continue;
		return (mem_clusters[i].size & PAGE_MASK);	/* prot */
	}

	/*
	 * Address is not a memory address.  If we're secure, disallow
	 * access.  Otherwise, grant read/write.
	 */
	if (securelevel > 0)
		return (PROT_NONE);
	else
		return (PROT_READ | PROT_WRITE);
}

/* XXX XXX BEGIN XXX XXX */
paddr_t alpha_XXX_dmamap_or;					/* XXX */
								/* XXX */
paddr_t								/* XXX */
alpha_XXX_dmamap(v)						/* XXX */
	vaddr_t v;						/* XXX */
{								/* XXX */
								/* XXX */
	return (vtophys(v) | alpha_XXX_dmamap_or);		/* XXX */
}								/* XXX */
/* XXX XXX END XXX XXX */

char *
dot_conv(x)
	unsigned long x;
{
	int i;
	char *xc;
	static int next;
	static char space[2][20];

	xc = space[next ^= 1] + sizeof space[0];
	*--xc = '\0';
	for (i = 0;; ++i) {
		if (i && (i & 3) == 0)
			*--xc = '.';
		*--xc = "0123456789abcdef"[x & 0xf];
		x >>= 4;
		if (x == 0)
			break;
	}
	return xc;
}

void
cpu_getmcontext(l, mcp, flags)
	struct lwp *l;
	mcontext_t *mcp;
	unsigned int *flags;
{
	struct trapframe *frame = l->l_md.md_tf;
	__greg_t *gr = mcp->__gregs;
	__greg_t ras_pc;

	/* Save register context. */
	frametoreg(frame, (struct reg *)gr);
	/* XXX if there's a better, general way to get the USP of
	 * an LWP that might or might not be curlwp, I'd like to know
	 * about it.
	 */
	if (l == curlwp) {
		gr[_REG_SP] = alpha_pal_rdusp();
		gr[_REG_UNIQUE] = alpha_pal_rdunique();
	} else {
		gr[_REG_SP] = l->l_addr->u_pcb.pcb_hw.apcb_usp;
		gr[_REG_UNIQUE] = l->l_addr->u_pcb.pcb_hw.apcb_unique;
	}
	gr[_REG_PC] = frame->tf_regs[FRAME_PC];
	gr[_REG_PS] = frame->tf_regs[FRAME_PS];

	if ((ras_pc = (__greg_t)ras_lookup(l->l_proc,
	    (caddr_t) gr[_REG_PC])) != -1)
		gr[_REG_PC] = ras_pc;

	*flags |= _UC_CPU | _UC_UNIQUE;

	/* Save floating point register context, if any, and copy it. */
	if (l->l_md.md_flags & MDP_FPUSED) {
		fpusave_proc(l, 1);
		(void)memcpy(&mcp->__fpregs, &l->l_addr->u_pcb.pcb_fp,
		    sizeof (mcp->__fpregs));
		mcp->__fpregs.__fp_fpcr = alpha_read_fp_c(l);
		*flags |= _UC_FPU;
	}
}


int
cpu_setmcontext(l, mcp, flags)
	struct lwp *l;
	const mcontext_t *mcp;
	unsigned int flags;
{
	struct trapframe *frame = l->l_md.md_tf;
	const __greg_t *gr = mcp->__gregs;

	/* Restore register context, if any. */
	if (flags & _UC_CPU) {
		/* Check for security violations first. */
		if ((gr[_REG_PS] & ALPHA_PSL_USERSET) != ALPHA_PSL_USERSET ||
		    (gr[_REG_PS] & ALPHA_PSL_USERCLR) != 0)
			return (EINVAL);

		regtoframe((struct reg *)gr, l->l_md.md_tf);
		if (l == curlwp)
			alpha_pal_wrusp(gr[_REG_SP]);
		else
			l->l_addr->u_pcb.pcb_hw.apcb_usp = gr[_REG_SP];
		frame->tf_regs[FRAME_PC] = gr[_REG_PC];
		frame->tf_regs[FRAME_PS] = gr[_REG_PS];
	}
	if (flags & _UC_UNIQUE) {
		if (l == curlwp)
			alpha_pal_wrunique(gr[_REG_UNIQUE]);
		else
			l->l_addr->u_pcb.pcb_hw.apcb_unique = gr[_REG_UNIQUE];
	}
	/* Restore floating point register context, if any. */
	if (flags & _UC_FPU) {
		/* If we have an FP register context, get rid of it. */
		if (l->l_addr->u_pcb.pcb_fpcpu != NULL)
			fpusave_proc(l, 0);
		(void)memcpy(&l->l_addr->u_pcb.pcb_fp, &mcp->__fpregs,
		    sizeof (l->l_addr->u_pcb.pcb_fp));
		l->l_md.md_flags = mcp->__fpregs.__fp_fpcr & MDP_FP_C;
		l->l_md.md_flags |= MDP_FPUSED;
	}

	return (0);
}
