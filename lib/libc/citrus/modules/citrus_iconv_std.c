/*	$NetBSD: citrus_iconv_std.c,v 1.2 2003/06/26 12:09:58 tshiozak Exp $	*/

/*-
 * Copyright (c)2003 Citrus Project,
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
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: citrus_iconv_std.c,v 1.2 2003/06/26 12:09:58 tshiozak Exp $");
#endif /* LIBC_SCCS and not lint */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/endian.h>
#include <sys/queue.h>

#include "citrus_namespace.h"
#include "citrus_types.h"
#include "citrus_module.h"
#include "citrus_region.h"
#include "citrus_mmap.h"
#include "citrus_iconv.h"
#include "citrus_stdenc.h"
#include "citrus_hash.h"
#include "citrus_mapper.h"
#include "citrus_csmapper.h"
#include "citrus_memstream.h"
#include "citrus_iconv_std.h"
#include "citrus_esdb.h"

/* ---------------------------------------------------------------------- */

_CITRUS_ICONV_DECLS(iconv_std);
_CITRUS_ICONV_DEF_OPS(iconv_std);


/* ---------------------------------------------------------------------- */

int
_citrus_iconv_std_iconv_getops(struct _citrus_iconv_ops *ops, size_t lenops,
			       u_int32_t expected_version)
{
	if (expected_version<_CITRUS_ICONV_ABI_VERSION || lenops<sizeof(*ops))
		return (EINVAL);

	memcpy(ops, &_citrus_iconv_std_iconv_ops,
	       sizeof(_citrus_iconv_std_iconv_ops));

	return (0);
}

/* ---------------------------------------------------------------------- */

/*
 * convenience routines for stdenc.
 */
static __inline void
save_encoding_state(struct _citrus_iconv_std_encoding *se)
{
	if (se->se_ps)
		memcpy(se->se_pssaved, se->se_ps,
		       _stdenc_get_state_size(se->se_handle));
}

static __inline void
restore_encoding_state(struct _citrus_iconv_std_encoding *se)
{
	if (se->se_ps)
		memcpy(se->se_ps, se->se_pssaved,
		       _stdenc_get_state_size(se->se_handle));
}

static __inline void
init_encoding_state(struct _citrus_iconv_std_encoding *se)
{
	if (se->se_ps)
		_stdenc_init_state(se->se_handle, se->se_ps);
}

static __inline int
mbtocsx(struct _citrus_iconv_std_encoding *se,
	_csid_t *csid, _index_t *idx, const char **s, size_t n,
	size_t *nresult)
{
	return _stdenc_mbtocs(se->se_handle, csid, idx, s, n, se->se_ps,
			      nresult);
}

static __inline int
cstombx(struct _citrus_iconv_std_encoding *se,
	char *s, size_t n, _csid_t csid, _index_t idx, size_t *nresult)
{
	return _stdenc_cstomb(se->se_handle, s, n, csid, idx, se->se_ps,
			      nresult);
}

static __inline int
wctombx(struct _citrus_iconv_std_encoding *se,
	char *s, size_t n, _wc_t wc, size_t *nresult)
{
	return _stdenc_wctomb(se->se_handle, s, n, wc, se->se_ps, nresult);
}

static __inline int
put_state_resetx(struct _citrus_iconv_std_encoding *se,
		 char *s, size_t n, size_t *nresult)
{
	return _stdenc_put_state_reset(se->se_handle, s, n, se->se_ps, nresult);
}


/*
 * open/close an encoding.
 */
static __inline void
close_encoding(struct _citrus_iconv_std_encoding *se)
{
	free(se->se_ps); se->se_ps = NULL;
	free(se->se_pssaved); se->se_pssaved = NULL;
}

static __inline int
open_encoding(struct _citrus_iconv_std_encoding *se, struct _esdb *db)
{
	int ret;

	se->se_ps = se->se_pssaved = NULL;
	ret = _stdenc_open(&se->se_handle, db->db_encname,
			   db->db_variable, db->db_len_variable);
	if (ret)
		return ret;

	if (_stdenc_get_state_size(se->se_handle) == 0)
		return 0;

	se->se_ps = malloc(_stdenc_get_state_size(se->se_handle));
	if (se->se_ps == NULL) {
		ret = errno;
		goto err;
	}
	ret = _stdenc_init_state(se->se_handle, se->se_ps);
	if (ret)
		goto err;
	se->se_pssaved = malloc(_stdenc_get_state_size(se->se_handle));
	if (se->se_pssaved == NULL) {
		ret = errno;
		goto err;
	}
	ret = _stdenc_init_state(se->se_handle, se->se_pssaved);
	if (ret)
		goto err;
	return 0;

err:
	close_encoding(se);
	return ret;
}

static int
open_csmapper(struct _csmapper **rcm, const char *src, const char *dst,
	      unsigned long *rnorm)
{
	int ret;
	struct _csmapper *cm;

	ret = _csmapper_open(&cm, src, dst, 0, rnorm);
	if (ret)
		return ret;
	if (_csmapper_get_src_max(cm) != 1 || _csmapper_get_dst_max(cm) != 1 ||
	    _csmapper_get_state_size(cm) != 0) {
		_csmapper_close(cm);
		return EINVAL;
	}

	*rcm = cm;

	return 0;
}

static void
close_dsts(struct _citrus_iconv_std_dst_list *dl)
{
	struct _citrus_iconv_std_dst *sd;

	while ((sd=TAILQ_FIRST(dl)) != NULL) {
		TAILQ_REMOVE(dl, sd, sd_entry);
		_csmapper_close(sd->sd_mapper);
		free(sd);
	}
}

static int
open_dsts(struct _citrus_iconv_std_dst_list *dl,
	  struct _esdb_charset *ec, struct _esdb *dbdst)
{
	int i, ret;
	struct _citrus_iconv_std_dst *sd, *sdtmp;
	unsigned long norm;

	sd = malloc(sizeof(*sd));
	if (sd == NULL)
		return errno;

	for (i=0; i<dbdst->db_num_charsets; i++) {
		ret = open_csmapper(&sd->sd_mapper,ec->ec_csname,
				    dbdst->db_charsets[i].ec_csname, &norm);
		if (ret == 0) {
			sd->sd_csid = dbdst->db_charsets[i].ec_csid;
			sd->sd_norm = norm;
			/* insert this mapper by sorted order. */
			TAILQ_FOREACH(sdtmp, dl, sd_entry) {
				if (sdtmp->sd_norm > norm) {
					TAILQ_INSERT_BEFORE(sdtmp, sd,
							    sd_entry);
					sd = NULL;
					break;
				}
			}
			if (sd)
				TAILQ_INSERT_TAIL(dl, sd, sd_entry);
			sd = malloc(sizeof(*sd));
			if (sd == NULL) {
				ret = errno;
				close_dsts(dl);
				return ret;
			}
		} else if (ret != ENOENT) {
			close_dsts(dl);
			free(sd);
			return ret;
		}
	}
	free(sd);
	return 0;
}

static void
close_srcs(struct _citrus_iconv_std_src_list *sl)
{
	struct _citrus_iconv_std_src *ss;

	while ((ss=TAILQ_FIRST(sl)) != NULL) {
		TAILQ_REMOVE(sl, ss, ss_entry);
		close_dsts(&ss->ss_dsts);
		free(ss);
	}
}

static int
open_srcs(struct _citrus_iconv_std_src_list *sl,
	  struct _esdb *dbsrc, struct _esdb *dbdst)
{
	int i, ret, count = 0;
	struct _citrus_iconv_std_src *ss;

	ss = malloc(sizeof(*ss));
	if (ss == NULL)
		return errno;

	TAILQ_INIT(&ss->ss_dsts);

	for (i=0; i<dbsrc->db_num_charsets; i++) {
		ret = open_dsts(&ss->ss_dsts, &dbsrc->db_charsets[i], dbdst);
		if (ret)
			goto err;
		if (!TAILQ_EMPTY(&ss->ss_dsts)) {
			ss->ss_csid = dbsrc->db_charsets[i].ec_csid;
			TAILQ_INSERT_TAIL(sl, ss, ss_entry);
			ss = malloc(sizeof(*ss));
			if (ss == NULL) {
				ret = errno;
				goto err;
			}
			count++;
			TAILQ_INIT(&ss->ss_dsts);
		}
	}
	free(ss);

	return count ? 0 : ENOENT;

err:
	free(ss);
	close_srcs(sl);
	return ret;
}

/* do convert a character */
#define E_NO_CORRESPONDING_CHAR ENOENT /* XXX */
static int
do_conv(struct _citrus_iconv_std *is, _csid_t *csid, _index_t *idx)
{
	_index_t tmpidx;
	int ret;
	struct _citrus_iconv_std_src *ss;
	struct _citrus_iconv_std_dst *sd;

	TAILQ_FOREACH(ss, &is->is_srcs, ss_entry) {
		if (ss->ss_csid == *csid) {
			TAILQ_FOREACH(sd, &ss->ss_dsts, sd_entry) {
				ret = _csmapper_convert(sd->sd_mapper,
							&tmpidx, *idx, NULL);
				switch (ret) {
				case _CITRUS_MAPPER_CONVERT_SUCCESS:
					*csid = sd->sd_csid;
					*idx = tmpidx;
					return 0;
				case _CITRUS_MAPPER_CONVERT_INVAL:
					break;
				case _CITRUS_MAPPER_CONVERT_SRC_MORE:
					/*FALLTHROUGH*/
				case _CITRUS_MAPPER_CONVERT_DST_MORE:
					/*FALLTHROUGH*/
				case _CITRUS_MAPPER_CONVERT_FATAL:
					return EINVAL;
				case _CITRUS_MAPPER_CONVERT_ILSEQ:
					return EILSEQ;
				}
			}
			break;
		}
	}

	return E_NO_CORRESPONDING_CHAR;
}
/* ---------------------------------------------------------------------- */

static int
/*ARGSUSED*/
_citrus_iconv_std_iconv_init(struct _citrus_iconv *ci,
			     const char * __restrict curdir,
			     const char * __restrict src,
			     const char * __restrict dst,
			     const void * __restrict var, size_t lenvar)
{
	int ret;
	struct _citrus_iconv_std *is;
	struct _citrus_esdb esdbsrc, esdbdst;

	is = malloc(sizeof(*is));
	if (is==NULL) {
		ret = errno;
		goto err0;
	}
	ret = _citrus_esdb_open(&esdbsrc, src);
	if (ret)
		goto err1;
	ret = _citrus_esdb_open(&esdbdst, dst);
	if (ret)
		goto err2;
	ret = open_encoding(&is->is_src_encoding, &esdbsrc);
	if (ret)
		goto err3;
	ret = open_encoding(&is->is_dst_encoding, &esdbdst);
	if (ret)
		goto err4;
	is->is_use_invalid = esdbdst.db_use_invalid;
	is->is_invalid = esdbdst.db_invalid;

	TAILQ_INIT(&is->is_srcs);
	ret = open_srcs(&is->is_srcs, &esdbsrc, &esdbdst);
	if (ret)
		goto err5;

	_esdb_close(&esdbsrc);
	_esdb_close(&esdbdst);
	ci->ci_closure = is;

	return 0;

err5:
	close_encoding(&is->is_dst_encoding);
err4:
	close_encoding(&is->is_src_encoding);
err3:
	_esdb_close(&esdbdst);
err2:
	_esdb_close(&esdbsrc);
err1:
	free(is);
err0:
	return ret;
}

static void
/*ARGSUSED*/
_citrus_iconv_std_iconv_uninit(struct _citrus_iconv *ci)
{
	struct _citrus_iconv_std *is;

	if (ci->ci_closure == NULL)
		return;

	is = ci->ci_closure;
	close_encoding(&is->is_src_encoding);
	close_encoding(&is->is_dst_encoding);
	close_srcs(&is->is_srcs);
	free(is);
}

static int
/*ARGSUSED*/
_citrus_iconv_std_iconv_convert(struct _citrus_iconv * __restrict ci,
				const char * __restrict * __restrict in,
				size_t * __restrict inbytes,
				char * __restrict * __restrict out,
				size_t * __restrict outbytes, u_int32_t flags,
				size_t * __restrict invalids)
{
	struct _citrus_iconv_std *is = ci->ci_closure;
	_index_t idx;
	_csid_t csid;
	int ret;
	size_t szrin, szrout;
	size_t inval;
	const char *tmpin;

	inval = 0;
	if (in==NULL || *in==NULL) {
		/* special cases */
		if (out!=NULL && *out!=NULL) {
			/* init output state */
			save_encoding_state(&is->is_src_encoding);
			save_encoding_state(&is->is_dst_encoding);
			szrout = 0;

			ret = put_state_resetx(&is->is_dst_encoding,
					       *out, *outbytes,
					       &szrout);
			if (ret)
				goto err;

			if (szrout == (size_t)-2) {
				/* too small to store the character */
				ret = EINVAL;
				goto err;
			}
			*out += szrout;
			*outbytes -= szrout;
		}
		*invalids = 0;
		init_encoding_state(&is->is_src_encoding);
		return 0;
	}

	/* normal case */
	for (;;) {
		/* save the encoding states for the error recovery */
		save_encoding_state(&is->is_src_encoding);
		save_encoding_state(&is->is_dst_encoding);

		/* mb -> csid/index */
		tmpin = *in;
		szrin = szrout = 0;
		ret = mbtocsx(&is->is_src_encoding, &csid, &idx,
			     &tmpin, *inbytes, &szrin);
		if (ret)
			goto err;

		if (szrin == (size_t)-2) {
			/* incompleted character */
			ret = EINVAL;
			goto err;
		}
		/* convert the character */
		ret = do_conv(is, &csid, &idx);
		if (ret) {
			if (ret == E_NO_CORRESPONDING_CHAR) {
				inval ++;
				szrout = 0;
				if ((flags&_CITRUS_ICONV_F_HIDE_INVALID)==0 &&
				    is->is_use_invalid) {
					ret = wctombx(&is->is_dst_encoding,
						      *out, *outbytes,
						      is->is_invalid,
						      &szrout);
					if (ret)
						goto err;
				}
				goto next;
			} else {
				goto err;
			}
		}
		/* csid/index -> mb */
		ret = cstombx(&is->is_dst_encoding,
			      *out, *outbytes, csid, idx, &szrout);
		if (ret)
			goto err;
next:
		_DIAGASSERT(*inbytes>=szrin && *outbytes>=szrout);
		*inbytes -= tmpin-*in; /* szrin is insufficient on \0. */
		*in = tmpin;
		*outbytes -= szrout;
		*out += szrout;
		if (*inbytes==0)
			break;
		if (*outbytes == 0) {
			ret = E2BIG;
			goto err_norestore;
		}
	}
	*invalids = inval;

	return 0;

err:
	restore_encoding_state(&is->is_src_encoding);
	restore_encoding_state(&is->is_dst_encoding);
err_norestore:
	*invalids = inval;

	return ret;
}
