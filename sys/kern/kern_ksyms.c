/*	$NetBSD: kern_ksyms.c,v 1.56 2010/03/12 21:43:11 darran Exp $	*/

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software developed for The NetBSD Foundation
 * by Andrew Doran.
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
 * Copyright (c) 2001, 2003 Anders Magnusson (ragge@ludd.luth.se).
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Code to deal with in-kernel symbol table management + /dev/ksyms.
 *
 * For each loaded module the symbol table info is kept track of by a
 * struct, placed in a circular list. The first entry is the kernel
 * symbol table.
 */

/*
 * TODO:
 *
 *	Add support for mmap, poll.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: kern_ksyms.c,v 1.56 2010/03/12 21:43:11 darran Exp $");

#if defined(_KERNEL) && defined(_KERNEL_OPT)
#include "opt_ddb.h"
#include "opt_ddbparam.h"	/* for SYMTAB_SPACE */
#include "opt_dtrace.h"
#endif

#define _KSYMS_PRIVATE

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/exec.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/kmem.h>
#include <sys/proc.h>
#include <sys/atomic.h>
#include <sys/ksyms.h>

#include <uvm/uvm_extern.h>

#ifdef DDB
#include <ddb/db_output.h>
#endif

#include "ksyms.h"

#define KSYMS_MAX_ID	65536
#ifdef KDTRACE_HOOKS
static uint32_t ksyms_nmap[KSYMS_MAX_ID];	/* sorted symbol table map */
#else
static uint32_t *ksyms_nmap = NULL;
#endif

static int ksyms_maxlen;
static bool ksyms_isopen;
static bool ksyms_initted;
static struct ksyms_hdr ksyms_hdr;
static kmutex_t ksyms_lock;

void ksymsattach(int);
static void ksyms_hdr_init(void *);
static void ksyms_sizes_calc(void);

#ifdef KSYMS_DEBUG
#define	FOLLOW_CALLS		1
#define	FOLLOW_MORE_CALLS	2
#define	FOLLOW_DEVKSYMS		4
static int ksyms_debug;
#endif

#ifdef SYMTAB_SPACE
#define		SYMTAB_FILLER	"|This is the symbol table!"

char		db_symtab[SYMTAB_SPACE] = SYMTAB_FILLER;
int		db_symtabsize = SYMTAB_SPACE;
#endif

int ksyms_symsz;
int ksyms_strsz;
int ksyms_ctfsz;
TAILQ_HEAD(, ksyms_symtab) ksyms_symtabs =
    TAILQ_HEAD_INITIALIZER(ksyms_symtabs);
static struct ksyms_symtab kernel_symtab;

static int
ksyms_verify(void *symstart, void *strstart)
{
#if defined(DIAGNOSTIC) || defined(DEBUG)
	if (symstart == NULL)
		printf("ksyms: Symbol table not found\n");
	if (strstart == NULL)
		printf("ksyms: String table not found\n");
	if (symstart == NULL || strstart == NULL)
		printf("ksyms: Perhaps the kernel is stripped?\n");
#endif
	if (symstart == NULL || strstart == NULL)
		return 0;
	return 1;
}

/*
 * Finds a certain symbol name in a certain symbol table.
 */
static Elf_Sym *
findsym(const char *name, struct ksyms_symtab *table, int type)
{
	Elf_Sym *sym, *maxsym;
	int low, mid, high, nglob;
	char *str, *cmp;

	sym = table->sd_symstart;
	str = table->sd_strstart - table->sd_usroffset;
	nglob = table->sd_nglob;
	low = 0;
	high = nglob;

	/*
	 * Start with a binary search of all global symbols in this table.
	 * Global symbols must have unique names.
	 */
	while (low < high) {
		mid = (low + high) >> 1;
		cmp = sym[mid].st_name + str;
		if (cmp[0] < name[0] || strcmp(cmp, name) < 0) {
			low = mid + 1; 
		} else {
			high = mid;
		}
	}
	KASSERT(low == high);
	if (__predict_true(low < nglob &&
	    strcmp(sym[low].st_name + str, name) == 0)) {
		KASSERT(ELF_ST_BIND(sym[low].st_info) == STB_GLOBAL);
		return &sym[low];
	}

	/*
	 * Perform a linear search of local symbols (rare).  Many local
	 * symbols with the same name can exist so are not included in
	 * the binary search.
	 */
	if (type != KSYMS_EXTERN) {
		maxsym = sym + table->sd_symsize / sizeof(Elf_Sym);
		for (sym += nglob; sym < maxsym; sym++) {
			if (strcmp(name, sym->st_name + str) == 0) {
				return sym;
			}
		}
	}
	return NULL;
}

/*
 * The "attach" is in reality done in ksyms_init().
 */
void
ksymsattach(int arg)
{

}

void
ksyms_init(void)
{

#ifdef SYMTAB_SPACE
	if (!ksyms_initted &&
	    strncmp(db_symtab, SYMTAB_FILLER, sizeof(SYMTAB_FILLER))) {
		ksyms_addsyms_elf(db_symtabsize, db_symtab,
		    db_symtab + db_symtabsize);
	}
#endif

	mutex_init(&ksyms_lock, MUTEX_DEFAULT, IPL_NONE);
}

/*
 * Add a symbol table.
 * This is intended for use when the symbol table and its corresponding
 * string table are easily available.  If they are embedded in an ELF
 * image, use addsymtab_elf() instead.
 *
 * name - Symbol's table name.
 * symstart, symsize - Address and size of the symbol table.
 * strstart, strsize - Address and size of the string table.
 * tab - Symbol table to be updated with this information.
 * newstart - Address to which the symbol table has to be copied during
 *            shrinking.  If NULL, it is not moved.
 */
static const char *addsymtab_strstart;

static int
addsymtab_compar(const void *a, const void *b)
{
	const Elf_Sym *sa, *sb;

	sa = a;
	sb = b;

	/*
	 * Split the symbol table into two, with globals at the start
	 * and locals at the end.
	 */
	if (ELF_ST_BIND(sa->st_info) != ELF_ST_BIND(sb->st_info)) {
		if (ELF_ST_BIND(sa->st_info) == STB_GLOBAL) {
			return -1;
		}
		if (ELF_ST_BIND(sb->st_info) == STB_GLOBAL) {
			return 1;
		}
	}

	/* Within each band, sort by name. */
	return strcmp(sa->st_name + addsymtab_strstart,
	    sb->st_name + addsymtab_strstart);
}

static void
addsymtab(const char *name, void *symstart, size_t symsize,
	  void *strstart, size_t strsize, struct ksyms_symtab *tab,
	  void *newstart, void *ctfstart, size_t ctfsize, uint32_t *nmap)
{
	Elf_Sym *sym, *nsym, ts;
	int i, j, n, nglob;
	char *str;
	int nsyms = symsize / sizeof(Elf_Sym);

	/* sanity check for pre-malloc map table used during startup */
	if ((nmap == ksyms_nmap) && (nsyms >= KSYMS_MAX_ID)) {
	    printf("kern_ksyms: ERROR %d > %d, increase KSYMS_MAX_ID\n",
		    nsyms, KSYMS_MAX_ID);

	    /* truncate for now */
	    nsyms = KSYMS_MAX_ID-1;
	}

	tab->sd_symstart = symstart;
	tab->sd_symsize = symsize;
	tab->sd_strstart = strstart;
	tab->sd_strsize = strsize;
	tab->sd_name = name;
	tab->sd_minsym = UINTPTR_MAX;
	tab->sd_maxsym = 0;
	tab->sd_usroffset = 0;
	tab->sd_gone = false;
	tab->sd_ctfstart = ctfstart;
	tab->sd_ctfsize = ctfsize;
	tab->sd_nmap = nmap;
	tab->sd_nmapsize = nsyms;
#ifdef KSYMS_DEBUG
	printf("newstart %p sym %p ksyms_symsz %d str %p strsz %d send %p\n",
	    newstart, symstart, symsize, strstart, strsize,
	    tab->sd_strstart + tab->sd_strsize);
#endif

	if (nmap) {
		memset(nmap, 0, nsyms * sizeof(uint32_t));
	}

	/* Pack symbol table by removing all file name references. */
	sym = tab->sd_symstart;
	nsym = (Elf_Sym *)newstart;
	str = tab->sd_strstart;
	nglob = 0;
	for (i = n = 0; i < nsyms; i++) {

	    	/* This breaks CTF mapping, so don't do it when
		 * DTrace is enabled
		 */
#ifndef KDTRACE_HOOKS
		/*
		 * Remove useless symbols.
		 * Should actually remove all typeless symbols.
		 */
		if (sym[i].st_name == 0)
			continue; /* Skip nameless entries */
		if (sym[i].st_shndx == SHN_UNDEF)
			continue; /* Skip external references */
		if (ELF_ST_TYPE(sym[i].st_info) == STT_FILE)
			continue; /* Skip filenames */
		if (ELF_ST_TYPE(sym[i].st_info) == STT_NOTYPE &&
		    sym[i].st_value == 0 &&
		    strcmp(str + sym[i].st_name, "*ABS*") == 0)
			continue; /* XXX */
		if (ELF_ST_TYPE(sym[i].st_info) == STT_NOTYPE &&
		    strcmp(str + sym[i].st_name, "gcc2_compiled.") == 0)
			continue; /* XXX */
#endif

		/* Save symbol. Set it as an absolute offset */
		nsym[n] = sym[i];

		if (nmap != NULL) {
			/* Save the size, replace it with the symbol id so
			 * the mapping can be done after the cleanup and sort.
			 */
			nmap[i] = nsym[n].st_size;
			nsym[n].st_size = i+1;	/* zero is reserved */
		}

		nsym[n].st_shndx = SHBSS;
		j = strlen(nsym[n].st_name + str) + 1;
		if (j > ksyms_maxlen)
			ksyms_maxlen = j;
		nglob += (ELF_ST_BIND(nsym[n].st_info) == STB_GLOBAL);

		/* Compute min and max symbols. */
		if (nsym[n].st_value < tab->sd_minsym) {
		    	tab->sd_minsym = nsym[n].st_value;
		}
		if (nsym[n].st_value > tab->sd_maxsym) {
		    	tab->sd_maxsym = nsym[n].st_value;
		}
		n++;
	}

	/* Fill the rest of the record, and sort the symbols. */
	tab->sd_symstart = nsym;
	tab->sd_symsize = n * sizeof(Elf_Sym);
	tab->sd_nglob = nglob;
	addsymtab_strstart = str;
	if (kheapsort(nsym, n, sizeof(Elf_Sym), addsymtab_compar, &ts) != 0)
		panic("addsymtab");

	/* 
	 * Build the mapping from original symbol id to new symbol table.
	 * Deleted symbols will have a zero map, indices will be one based
	 * instead of zero based.
	 * Resulting map is sd_nmap[original_index] = new_index + 1
	 */
	if (nmap != NULL) {
		int new;
		for (new=0; new<n; new++) {
			uint32_t orig = nsym[new].st_size - 1;
			uint32_t size = nmap[orig];
	
			nmap[orig] = new + 1;

			/* restore the size */
			nsym[new].st_size = size;
		}
	}

	/* ksymsread() is unlocked, so membar. */
	membar_producer();
	TAILQ_INSERT_TAIL(&ksyms_symtabs, tab, sd_queue);
	ksyms_sizes_calc();
	ksyms_initted = true;
}

/*
 * Setup the kernel symbol table stuff.
 */
void
ksyms_addsyms_elf(int symsize, void *start, void *end)
{
	int i, j;
	Elf_Shdr *shdr;
	char *symstart = NULL, *strstart = NULL;
	size_t strsize = 0;
	Elf_Ehdr *ehdr;
	char *ctfstart = NULL;
	size_t ctfsize = 0;
	char *shstr = NULL;

	if (symsize <= 0) {
		printf("[ Kernel symbol table missing! ]\n");
		return;
	}

	/* Sanity check */
	if (ALIGNED_POINTER(start, long) == 0) {
		printf("[ Kernel symbol table has bad start address %p ]\n",
		    start);
		return;
	}

	ehdr = (Elf_Ehdr *)start;

	/* check if this is a valid ELF header */
	/* No reason to verify arch type, the kernel is actually running! */
	if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) ||
	    ehdr->e_ident[EI_CLASS] != ELFCLASS ||
	    ehdr->e_version > 1) {
		printf("[ Kernel symbol table invalid! ]\n");
		return; /* nothing to do */
	}

	/* Loaded header will be scratched in addsymtab */
	ksyms_hdr_init(start);

	/* Find the symbol table and the corresponding string table. */
	shdr = (Elf_Shdr *)((uint8_t *)start + ehdr->e_shoff);
	for (i = 1; i < ehdr->e_shnum; i++) {
		if (shdr[i].sh_type != SHT_SYMTAB)
			continue;
		if (shdr[i].sh_offset == 0)
			continue;
		symstart = (uint8_t *)start + shdr[i].sh_offset;
		symsize = shdr[i].sh_size;
		j = shdr[i].sh_link;
		if (shdr[j].sh_offset == 0)
			continue; /* Can this happen? */
		strstart = (uint8_t *)start + shdr[j].sh_offset;
		strsize = shdr[j].sh_size;
		break;
	}

	/* Find the CTF section */
	shdr = (Elf_Shdr *)((uint8_t *)start + ehdr->e_shoff);
	if (ehdr->e_shstrndx != 0) {
		shstr = (uint8_t*)start + shdr[ehdr->e_shstrndx].sh_offset;
		for (i = 1; i < ehdr->e_shnum; i++) {
			if (shdr[i].sh_type != SHT_PROGBITS)
				continue;
			if (strncmp(".SUNW_ctf", &shstr[shdr[i].sh_name] ,10) != 0)
				continue;
			ctfstart = (uint8_t *)start + shdr[i].sh_offset;
			ctfsize = shdr[i].sh_size;
			ksyms_ctfsz = ctfsize;
#ifdef DEBUG
			aprint_normal("Found CTF at 0x%x, size 0x%x\n",
				(uint32_t)ctfstart, ctfsize);
#endif
			break;
		}
	}

	if (!ksyms_verify(symstart, strstart))
		return;

	addsymtab("netbsd", symstart, symsize, strstart, strsize,
	    &kernel_symtab, start, ctfstart, ctfsize, ksyms_nmap);

#ifdef DEBUG
	aprint_normal("Loaded initial symtab at %p, strtab at %p, # entries %ld\n",
	    kernel_symtab.sd_symstart, kernel_symtab.sd_strstart,
	    (long)kernel_symtab.sd_symsize/sizeof(Elf_Sym));
#endif
}

/*
 * Setup the kernel symbol table stuff.
 * Use this when the address of the symbol and string tables are known;
 * otherwise use ksyms_init with an ELF image.
 * We need to pass a minimal ELF header which will later be completed by
 * ksyms_hdr_init and handed off to userland through /dev/ksyms.  We use
 * a void *rather than a pointer to avoid exposing the Elf_Ehdr type.
 */
void
ksyms_addsyms_explicit(void *ehdr, void *symstart, size_t symsize,
		    void *strstart, size_t strsize)
{

	if (!ksyms_verify(symstart, strstart))
		return;

	ksyms_hdr_init(ehdr);
	addsymtab("netbsd", symstart, symsize, strstart, strsize,
	    &kernel_symtab, symstart, NULL, 0, ksyms_nmap);
}

/*
 * Get the value associated with a symbol.
 * "mod" is the module name, or null if any module.
 * "sym" is the symbol name.
 * "val" is a pointer to the corresponding value, if call succeeded.
 * Returns 0 if success or ENOENT if no such entry.
 *
 * Call with ksyms_lock, unless known that the symbol table can't change.
 */
int
ksyms_getval_unlocked(const char *mod, const char *sym, unsigned long *val,
		      int type)
{
	struct ksyms_symtab *st;
	Elf_Sym *es;

#ifdef KSYMS_DEBUG
	if (ksyms_debug & FOLLOW_CALLS)
		printf("ksyms_getval_unlocked: mod %s sym %s valp %p\n",
		    mod, sym, val);
#endif

	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (__predict_false(st->sd_gone))
			continue;
		if (mod != NULL && strcmp(st->sd_name, mod))
			continue;
		if ((es = findsym(sym, st, type)) != NULL) {
			*val = es->st_value;
			return 0;
		}
	}
	return ENOENT;
}

int
ksyms_getval(const char *mod, const char *sym, unsigned long *val, int type)
{
	int rc;

	if (!ksyms_initted)
		return ENOENT;

	mutex_enter(&ksyms_lock);
	rc = ksyms_getval_unlocked(mod, sym, val, type);
	mutex_exit(&ksyms_lock);
	return rc;
}

struct ksyms_symtab *
ksyms_get_mod(const char *mod)
{
	struct ksyms_symtab *st;

	mutex_enter(&ksyms_lock);
	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (__predict_false(st->sd_gone))
			continue;
		if (mod != NULL && strcmp(st->sd_name, mod))
			continue;
		break;
	}
	mutex_exit(&ksyms_lock);

	return st;
}


/*
 * ksyms_mod_foreach()
 *
 * Iterate over the symbol table of the specified module, calling the callback
 * handler for each symbol. Stop iterating if the handler return is non-zero.
 *
 */

int
ksyms_mod_foreach(const char *mod, ksyms_callback_t callback, void *opaque)
{
	struct ksyms_symtab *st;
	Elf_Sym *sym, *maxsym;
	char *str;
	int symindx;

	if (!ksyms_initted)
		return ENOENT;

	mutex_enter(&ksyms_lock);

	/* find the module */
	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (__predict_false(st->sd_gone))
			continue;
		if (mod != NULL && strcmp(st->sd_name, mod))
			continue;

		sym = st->sd_symstart;
		str = st->sd_strstart - st->sd_usroffset;

		/* now iterate through the symbols */
		maxsym = sym + st->sd_symsize / sizeof(Elf_Sym);
		for (symindx=0; sym < maxsym; sym++,symindx++) {
			if (callback(str + sym->st_name, symindx,
				    (void *)sym->st_value,
				    sym->st_size,
				    sym->st_info,
				    opaque) != 0) {
				break;
			}
		}
	}
	mutex_exit(&ksyms_lock);

	return 0;
}

/*
 * Get "mod" and "symbol" associated with an address.
 * Returns 0 if success or ENOENT if no such entry.
 *
 * Call with ksyms_lock, unless known that the symbol table can't change.
 */
int
ksyms_getname(const char **mod, const char **sym, vaddr_t v, int f)
{
	struct ksyms_symtab *st;
	Elf_Sym *les, *es = NULL;
	vaddr_t laddr = 0;
	const char *lmod = NULL;
	char *stable = NULL;
	int type, i, sz;

	if (!ksyms_initted)
		return ENOENT;

	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (st->sd_gone)
			continue;
		if (v < st->sd_minsym || v > st->sd_maxsym)
			continue;
		sz = st->sd_symsize/sizeof(Elf_Sym);
		for (i = 0; i < sz; i++) {
			les = st->sd_symstart + i;
			type = ELF_ST_TYPE(les->st_info);

			if ((f & KSYMS_PROC) && (type != STT_FUNC))
				continue;

			if (type == STT_NOTYPE)
				continue;

			if (((f & KSYMS_ANY) == 0) &&
			    (type != STT_FUNC) && (type != STT_OBJECT))
				continue;

			if ((les->st_value <= v) && (les->st_value > laddr)) {
				laddr = les->st_value;
				es = les;
				lmod = st->sd_name;
				stable = st->sd_strstart - st->sd_usroffset;
			}
		}
	}
	if (es == NULL)
		return ENOENT;
	if ((f & KSYMS_EXACT) && (v != es->st_value))
		return ENOENT;
	if (mod)
		*mod = lmod;
	if (sym)
		*sym = stable + es->st_name;
	return 0;
}

/*
 * Add a symbol table from a loadable module.
 */
void
ksyms_modload(const char *name, void *symstart, vsize_t symsize,
	      char *strstart, vsize_t strsize)
{
	struct ksyms_symtab *st;

	st = kmem_zalloc(sizeof(*st), KM_SLEEP);
	mutex_enter(&ksyms_lock);
	addsymtab(name, symstart, symsize, strstart, strsize, st, symstart,
		NULL, 0, NULL);
	mutex_exit(&ksyms_lock);
}

/*
 * Remove a symbol table from a loadable module.
 */
void
ksyms_modunload(const char *name)
{
	struct ksyms_symtab *st;

	mutex_enter(&ksyms_lock);
	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (st->sd_gone)
			continue;
		if (strcmp(name, st->sd_name) != 0)
			continue;
		st->sd_gone = true;
		if (!ksyms_isopen) {
			TAILQ_REMOVE(&ksyms_symtabs, st, sd_queue);
			ksyms_sizes_calc();
			kmem_free(st, sizeof(*st));
		}
		break;
	}
	mutex_exit(&ksyms_lock);
	KASSERT(st != NULL);
}

#ifdef DDB
/*
 * Keep sifting stuff here, to avoid export of ksyms internals.
 *
 * Systems is expected to be quiescent, so no locking done.
 */
int
ksyms_sift(char *mod, char *sym, int mode)
{
	struct ksyms_symtab *st;
	char *sb;
	int i, sz;

	if (!ksyms_initted)
		return ENOENT;

	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (st->sd_gone)
			continue;
		if (mod && strcmp(mod, st->sd_name))
			continue;
		sb = st->sd_strstart - st->sd_usroffset;

		sz = st->sd_symsize/sizeof(Elf_Sym);
		for (i = 0; i < sz; i++) {
			Elf_Sym *les = st->sd_symstart + i;
			char c;

			if (strstr(sb + les->st_name, sym) == NULL)
				continue;

			if (mode == 'F') {
				switch (ELF_ST_TYPE(les->st_info)) {
				case STT_OBJECT:
					c = '+';
					break;
				case STT_FUNC:
					c = '*';
					break;
				case STT_SECTION:
					c = '&';
					break;
				case STT_FILE:
					c = '/';
					break;
				default:
					c = ' ';
					break;
				}
				db_printf("%s%c ", sb + les->st_name, c);
			} else
				db_printf("%s ", sb + les->st_name);
		}
	}
	return ENOENT;
}
#endif /* DDB */

/*
 * In case we exposing the symbol table to the userland using the pseudo-
 * device /dev/ksyms, it is easier to provide all the tables as one.
 * However, it means we have to change all the st_name fields for the
 * symbols so they match the ELF image that the userland will read
 * through the device.
 *
 * The actual (correct) value of st_name is preserved through a global
 * offset stored in the symbol table structure.
 *
 * Call with ksyms_lock held.
 */
static void
ksyms_sizes_calc(void)
{
        struct ksyms_symtab *st;
	int i, delta;

        ksyms_symsz = ksyms_strsz = 0;
        TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		delta = ksyms_strsz - st->sd_usroffset;
		if (delta != 0) {
			for (i = 0; i < st->sd_symsize/sizeof(Elf_Sym); i++)
				st->sd_symstart[i].st_name += delta;
			st->sd_usroffset = ksyms_strsz;
		}
                ksyms_symsz += st->sd_symsize;
                ksyms_strsz += st->sd_strsize;
        }
}

static void
ksyms_hdr_init(void *hdraddr)
{

	/* Copy the loaded elf exec header */
	memcpy(&ksyms_hdr.kh_ehdr, hdraddr, sizeof(Elf_Ehdr));

	/* Set correct program/section header sizes, offsets and numbers */
	ksyms_hdr.kh_ehdr.e_phoff = offsetof(struct ksyms_hdr, kh_phdr[0]);
	ksyms_hdr.kh_ehdr.e_phentsize = sizeof(Elf_Phdr);
	ksyms_hdr.kh_ehdr.e_phnum = NPRGHDR;
	ksyms_hdr.kh_ehdr.e_shoff = offsetof(struct ksyms_hdr, kh_shdr[0]);
	ksyms_hdr.kh_ehdr.e_shentsize = sizeof(Elf_Shdr);
	ksyms_hdr.kh_ehdr.e_shnum = NSECHDR;
	ksyms_hdr.kh_ehdr.e_shstrndx = SHSTRTAB;

	/* Text/data - fake */
	ksyms_hdr.kh_phdr[0].p_type = PT_LOAD;
	ksyms_hdr.kh_phdr[0].p_memsz = (unsigned long)-1L;
	ksyms_hdr.kh_phdr[0].p_flags = PF_R | PF_X | PF_W;

	/* First section is null */

	/* Second section header; ".symtab" */
	ksyms_hdr.kh_shdr[SYMTAB].sh_name = 1; /* Section 3 offset */
	ksyms_hdr.kh_shdr[SYMTAB].sh_type = SHT_SYMTAB;
	ksyms_hdr.kh_shdr[SYMTAB].sh_offset = sizeof(struct ksyms_hdr);
/*	ksyms_hdr.kh_shdr[SYMTAB].sh_size = filled in at open */
	ksyms_hdr.kh_shdr[SYMTAB].sh_link = 2; /* Corresponding strtab */
	ksyms_hdr.kh_shdr[SYMTAB].sh_addralign = sizeof(long);
	ksyms_hdr.kh_shdr[SYMTAB].sh_entsize = sizeof(Elf_Sym);

	/* Third section header; ".strtab" */
	ksyms_hdr.kh_shdr[STRTAB].sh_name = 9; /* Section 3 offset */
	ksyms_hdr.kh_shdr[STRTAB].sh_type = SHT_STRTAB;
/*	ksyms_hdr.kh_shdr[STRTAB].sh_offset = filled in at open */
/*	ksyms_hdr.kh_shdr[STRTAB].sh_size = filled in at open */
	ksyms_hdr.kh_shdr[STRTAB].sh_addralign = sizeof(char);

	/* Fourth section, ".shstrtab" */
	ksyms_hdr.kh_shdr[SHSTRTAB].sh_name = 17; /* This section name offset */
	ksyms_hdr.kh_shdr[SHSTRTAB].sh_type = SHT_STRTAB;
	ksyms_hdr.kh_shdr[SHSTRTAB].sh_offset =
	    offsetof(struct ksyms_hdr, kh_strtab);
	ksyms_hdr.kh_shdr[SHSTRTAB].sh_size = SHSTRSIZ;
	ksyms_hdr.kh_shdr[SHSTRTAB].sh_addralign = sizeof(char);

	/* Fifth section, ".bss". All symbols reside here. */
	ksyms_hdr.kh_shdr[SHBSS].sh_name = 27; /* This section name offset */
	ksyms_hdr.kh_shdr[SHBSS].sh_type = SHT_NOBITS;
	ksyms_hdr.kh_shdr[SHBSS].sh_offset = 0;
	ksyms_hdr.kh_shdr[SHBSS].sh_size = (unsigned long)-1L;
	ksyms_hdr.kh_shdr[SHBSS].sh_addralign = PAGE_SIZE;
	ksyms_hdr.kh_shdr[SHBSS].sh_flags = SHF_ALLOC | SHF_EXECINSTR;

	/* Sixth section header; ".SUNW_ctf" */
	ksyms_hdr.kh_shdr[SHCTF].sh_name = 32; /* Section 6 offset */
	ksyms_hdr.kh_shdr[SHCTF].sh_type = SHT_PROGBITS;
/*	ksyms_hdr.kh_shdr[SHCTF].sh_offset = filled in at open */
/*	ksyms_hdr.kh_shdr[SHCTF].sh_size = filled in at open */
	ksyms_hdr.kh_shdr[SHCTF].sh_link = SYMTAB; /* Corresponding symtab */
	ksyms_hdr.kh_shdr[SHCTF].sh_addralign = sizeof(char);

	/* Set section names */
	strlcpy(&ksyms_hdr.kh_strtab[1], ".symtab",
	    sizeof(ksyms_hdr.kh_strtab) - 1);
	strlcpy(&ksyms_hdr.kh_strtab[9], ".strtab",
	    sizeof(ksyms_hdr.kh_strtab) - 9);
	strlcpy(&ksyms_hdr.kh_strtab[17], ".shstrtab",
	    sizeof(ksyms_hdr.kh_strtab) - 17);
	strlcpy(&ksyms_hdr.kh_strtab[27], ".bss",
	    sizeof(ksyms_hdr.kh_strtab) - 27);
	strlcpy(&ksyms_hdr.kh_strtab[32], ".SUNW_ctf",
	    sizeof(ksyms_hdr.kh_strtab) - 32);
}

static int
ksymsopen(dev_t dev, int oflags, int devtype, struct lwp *l)
{

	if (minor(dev) != 0 || !ksyms_initted)
		return ENXIO;

	/*
	 * Create a "snapshot" of the kernel symbol table.  Setting
	 * ksyms_isopen will prevent symbol tables from being freed.
	 */
	mutex_enter(&ksyms_lock);
	ksyms_hdr.kh_shdr[SYMTAB].sh_size = ksyms_symsz;
	ksyms_hdr.kh_shdr[SYMTAB].sh_info = ksyms_symsz / sizeof(Elf_Sym);
	ksyms_hdr.kh_shdr[STRTAB].sh_offset = ksyms_symsz +
	    ksyms_hdr.kh_shdr[SYMTAB].sh_offset;
	ksyms_hdr.kh_shdr[STRTAB].sh_size = ksyms_strsz;
	ksyms_hdr.kh_shdr[SHCTF].sh_offset = ksyms_strsz +
	    ksyms_hdr.kh_shdr[STRTAB].sh_offset;
	ksyms_hdr.kh_shdr[SHCTF].sh_size = ksyms_ctfsz;
	ksyms_isopen = true;
	mutex_exit(&ksyms_lock);

	return 0;
}

static int
ksymsclose(dev_t dev, int oflags, int devtype, struct lwp *l)
{
	struct ksyms_symtab *st, *next;
	bool resize;

	/* Discard refernces to symbol tables. */
	mutex_enter(&ksyms_lock);
	ksyms_isopen = false;
	resize = false;
	for (st = TAILQ_FIRST(&ksyms_symtabs); st != NULL; st = next) {
		next = TAILQ_NEXT(st, sd_queue);
		if (st->sd_gone) {
			TAILQ_REMOVE(&ksyms_symtabs, st, sd_queue);
			kmem_free(st, sizeof(*st));
			resize = true;
		}
	}
	if (resize)
		ksyms_sizes_calc();
	mutex_exit(&ksyms_lock);

	return 0;
}

static int
ksymsread(dev_t dev, struct uio *uio, int ioflag)
{
	struct ksyms_symtab *st, *cst;
	size_t filepos, inpos, off;
	int error;

	/*
	 * First: Copy out the ELF header.   XXX Lose if ksymsopen()
	 * occurs during read of the header.
	 */
	off = uio->uio_offset;
	if (off < sizeof(struct ksyms_hdr)) {
		error = uiomove((char *)&ksyms_hdr + off,
		    sizeof(struct ksyms_hdr) - off, uio);
		if (error != 0)
			return error;
	}

	/*
	 * Copy out the symbol table.
	 */
	filepos = sizeof(struct ksyms_hdr);
	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (uio->uio_resid == 0)
			return 0;
		if (uio->uio_offset <= st->sd_symsize + filepos) {
			inpos = uio->uio_offset - filepos;
			error = uiomove((char *)st->sd_symstart + inpos,
			   st->sd_symsize - inpos, uio);
			if (error != 0)
				return error;
		}
		filepos += st->sd_symsize;
	}

	/*
	 * Copy out the string table
	 */
	KASSERT(filepos == sizeof(struct ksyms_hdr) +
	    ksyms_hdr.kh_shdr[SYMTAB].sh_size);
	TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
		if (uio->uio_resid == 0)
			return 0;
		if (uio->uio_offset <= st->sd_strsize + filepos) {
			inpos = uio->uio_offset - filepos;
			error = uiomove((char *)st->sd_strstart + inpos,
			   st->sd_strsize - inpos, uio);
			if (error != 0)
				return error;
		}
		filepos += st->sd_strsize;
	}

	/*
	 * Copy out the CTF table.
	 */
	cst = TAILQ_FIRST(&ksyms_symtabs);
	if (cst->sd_ctfstart != NULL) {
		if (uio->uio_resid == 0)
			return 0;
		if (uio->uio_offset <= cst->sd_ctfsize + filepos) {
			inpos = uio->uio_offset - filepos;
			error = uiomove((char *)cst->sd_ctfstart + inpos,
			   cst->sd_ctfsize - inpos, uio);
			if (error != 0)
				return error;
		}
		filepos += cst->sd_ctfsize;
	}


	return 0;
}

static int
ksymswrite(dev_t dev, struct uio *uio, int ioflag)
{

	return EROFS;
}

static int
ksymsioctl(dev_t dev, u_long cmd, void *data, int fflag, struct lwp *l)
{
	struct ksyms_gsymbol *kg = (struct ksyms_gsymbol *)data;
	struct ksyms_symtab *st;
	Elf_Sym *sym = NULL, copy;
	unsigned long val;
	int error = 0;
	char *str = NULL;
	int len;

	/* Read ksyms_maxlen only once while not holding the lock. */
	len = ksyms_maxlen;

	if (cmd == KIOCGVALUE || cmd == KIOCGSYMBOL) {
		str = kmem_alloc(len, KM_SLEEP);
		if ((error = copyinstr(kg->kg_name, str, len, NULL)) != 0) {
			kmem_free(str, len);
			return error;
		}
	}

	switch (cmd) {
	case KIOCGVALUE:
		/*
		 * Use the in-kernel symbol lookup code for fast
		 * retreival of a value.
		 */
		error = ksyms_getval(NULL, str, &val, KSYMS_EXTERN);
		if (error == 0)
			error = copyout(&val, kg->kg_value, sizeof(long));
		kmem_free(str, len);
		break;

	case KIOCGSYMBOL:
		/*
		 * Use the in-kernel symbol lookup code for fast
		 * retreival of a symbol.
		 */
		mutex_enter(&ksyms_lock);
		TAILQ_FOREACH(st, &ksyms_symtabs, sd_queue) {
			if (st->sd_gone)
				continue;
			if ((sym = findsym(str, st, KSYMS_ANY)) == NULL)
				continue;
#ifdef notdef
			/* Skip if bad binding */
			if (ELF_ST_BIND(sym->st_info) != STB_GLOBAL) {
				sym = NULL;
				continue;
			}
#endif
			break;
		}
		if (sym != NULL) {
			memcpy(&copy, sym, sizeof(copy));
			mutex_exit(&ksyms_lock);
			error = copyout(&copy, kg->kg_sym, sizeof(Elf_Sym));
		} else {
			mutex_exit(&ksyms_lock);
			error = ENOENT;
		}
		kmem_free(str, len);
		break;

	case KIOCGSIZE:
		/*
		 * Get total size of symbol table.
		 */
		mutex_enter(&ksyms_lock);
		*(int *)data = ksyms_strsz + ksyms_symsz +
		    sizeof(struct ksyms_hdr);
		mutex_exit(&ksyms_lock);
		break;

	default:
		error = ENOTTY;
		break;
	}

	return error;
}

const struct cdevsw ksyms_cdevsw = {
	ksymsopen, ksymsclose, ksymsread, ksymswrite, ksymsioctl,
	nullstop, notty, nopoll, nommap, nullkqfilter, D_OTHER | D_MPSAFE
};
