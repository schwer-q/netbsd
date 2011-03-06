# $NetBSD: t_quotalimit.sh,v 1.2 2011/03/06 17:08:40 bouyer Exp $ 
#
#  Copyright (c) 2011 Manuel Bouyer
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
#  THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
#  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
#  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
#  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

for e in le; do
  for v in 1; do
    for q in "user" "group"; do
      test_case_root limit_${e}_${v}_${q} limit_quota \
	 "hit hard limit quota with ${q} enabled" -b ${e} ${v} ${q}
      test_case_root limit_${e}_${v}_${q}_log limit_quota \
	 "hit hard limit quota with ${q} enabled, WAPBL" -bl ${e} ${v} ${q}
      test_case_root slimit_${e}_${v}_${q} limit_softquota \
	 "hit soft limit quota with ${q} enabled after grace time" \
	 -b ${e} ${v} ${q}
      test_case_root inolimit_${e}_${v}_${q} limit_iquota \
	 "hit hard limit ino quota with ${q} enabled" -b ${e} ${v} ${q}
      test_case_root inolimit_${e}_${v}_${q}_log limit_iquota \
	 "hit hard limit ino quota with ${q} enabled, WAPBL" -bl ${e} ${v} ${q}
      test_case_root sinolimit_${e}_${v}_${q} limit_softiquota \
	 "hit soft limit ino quota with ${q} enabled after grace time" \
	 -b ${e} ${v} ${q}
      test_case_root herit_defq_${e}_${v}_${q} inherit_defaultquota \
	 "new id herit from default for ${q} quota" -b ${e} ${v} ${q}
      test_case_root herit_defq_${e}_${v}_${q}_log inherit_defaultquota \
	 "new id herit from default for ${q} quota, WAPBL" -bl ${e} ${v} ${q}
      test_case_root herit_idefq_${e}_${v}_${q}_log inherit_defaultiquota \
	 "new id herit from default for ${q} ino quota, WAPBL" -bl ${e} ${v} ${q}
    done
  done
done

limit_quota()
{
	create_with_quotas_server $*
	local q=$4
	local expect
	local id=1

	case ${q} in
	user)
		expect=u
		fail=g
		;;
	group)
		expect=g
		fail=u
		;;
	both)
		expect="u g"
		fail=""
		;;
	*)
		atf_fail "wrong quota type"
		;;
	esac

	for q in ${expect} ; do
		atf_check -s exit:0 \
		   $(atf_get_srcdir)/rump_edquota -$q -s2k/4 -h3k/6 \
		   -t 2h/2h ${id}
	done
	atf_check -s exit:0 rump.halt

	#now start the server which does the limits tests
	atf_check -s exit:0 -o ignore \
-e match:'test 0: write up to hard limit returned 69: Disc quota exceeded' \
	    $(atf_get_srcdir)/h_quota2_tests -b 0 ${IMG} ${RUMP_SERVER}
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'/mnt   3072 B\*  2048 B   3072 B     2:0      2       4       6         ' \
		    $(atf_get_srcdir)/rump_quota -$q -h ${id}
		atf_check -s exit:0 \
		    -o match:'daemon    \+-        3        2        3    2:0         2       4       6' \
		    $(atf_get_srcdir)/rump_repquota -$q /mnt
	done
	rump_shutdown
}

limit_softquota()
{
	create_with_quotas_server $*
	local q=$4
	local expect
	local id=1

	case ${q} in
	user)
		expect=u
		fail=g
		;;
	group)
		expect=g
		fail=u
		;;
	both)
		expect="u g"
		fail=""
		;;
	*)
		atf_fail "wrong quota type"
		;;
	esac

	for q in ${expect} ; do
		atf_check -s exit:0 \
		   $(atf_get_srcdir)/rump_edquota -$q -s2k/4 -h3k/6 \
		   -t 1s/1d ${id}
	done
	atf_check -s exit:0 rump.halt

	#now start the server which does the limits tests
	atf_check -s exit:0 -o ignore \
-e match:'test 1: write beyond the soft limit after grace time returned 69: Disc quota exceeded' \
	    $(atf_get_srcdir)/h_quota2_tests -b 1 ${IMG} ${RUMP_SERVER}
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'/mnt   2560 B\*  2048 B   3072 B    none      2       4       6         ' \
		    $(atf_get_srcdir)/rump_quota -$q -h ${id}
		atf_check -s exit:0 \
		    -o match:'daemon    \+-        2        2        3   none         2       4       6' \
		    $(atf_get_srcdir)/rump_repquota -$q /mnt
	done
	rump_shutdown
}

limit_iquota()
{
	create_with_quotas_server $*
	local q=$4
	local expect
	local id=1

	case ${q} in
	user)
		expect=u
		fail=g
		;;
	group)
		expect=g
		fail=u
		;;
	both)
		expect="u g"
		fail=""
		;;
	*)
		atf_fail "wrong quota type"
		;;
	esac

	for q in ${expect} ; do
		atf_check -s exit:0 \
		   $(atf_get_srcdir)/rump_edquota -$q -s2m/4 -h3m/6 \
		   -t 2h/2h ${id}
	done
	atf_check -s exit:0 rump.halt

	#now start the server which does the limits tests
	atf_check -s exit:0 -o ignore \
-e match:'test 2: create file up to hard limit returned 69: Disc quota exceeded' \
	    $(atf_get_srcdir)/h_quota2_tests -b 2 ${IMG} ${RUMP_SERVER}
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'/mnt   3072 B   2048 K   3072 K              6 \*     4       6      2:0' \
		    $(atf_get_srcdir)/rump_quota -$q -h ${id}
		atf_check -s exit:0 \
		    -o match:'daemon    -\+        3     2048     3072                6       4       6    2:0' \
		    $(atf_get_srcdir)/rump_repquota -$q /mnt
	done
	rump_shutdown
}

limit_softiquota()
{
	create_with_quotas_server $*
	local q=$4
	local expect
	local id=1

	case ${q} in
	user)
		expect=u
		fail=g
		;;
	group)
		expect=g
		fail=u
		;;
	both)
		expect="u g"
		fail=""
		;;
	*)
		atf_fail "wrong quota type"
		;;
	esac

	for q in ${expect} ; do
		atf_check -s exit:0 \
		   $(atf_get_srcdir)/rump_edquota -$q -s2m/4 -h3m/6 \
		   -t 1d/1s ${id}
	done
	atf_check -s exit:0 rump.halt

	#now start the server which does the limits tests
	atf_check -s exit:0 -o ignore \
-e match:'test 3: create file beyond the soft limit after grace time returned 69: Disc quota exceeded' \
	    $(atf_get_srcdir)/h_quota2_tests -b 3 ${IMG} ${RUMP_SERVER}
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'/mnt   2560 B   2048 K   3072 K              5 \*     4       6     none' \
		    $(atf_get_srcdir)/rump_quota -$q -h ${id}
		atf_check -s exit:0 \
		    -o match:'daemon    -\+        2     2048     3072                5       4       6   none' \
		    $(atf_get_srcdir)/rump_repquota -$q /mnt
	done
	rump_shutdown
}

inherit_defaultquota()
{
	create_with_quotas_server $*
	local q=$4
	local expect
	local id=1

	case ${q} in
	user)
		expect=u
		fail=g
		;;
	group)
		expect=g
		fail=u
		;;
	both)
		expect="u g"
		fail=""
		;;
	*)
		atf_fail "wrong quota type"
		;;
	esac

	for q in ${expect} ; do
		atf_check -s exit:0 \
		   $(atf_get_srcdir)/rump_edquota -$q -s2k/4 -h3k/6 \
		   -t 2h/2h -d
	done
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'Disk quotas for .*id 1\): none' \
		    $(atf_get_srcdir)/rump_quota -$q -v ${id}
	done
	atf_check -s exit:0 rump.halt

	#now start the server which does the limits tests
	atf_check -s exit:0 -o ignore \
-e match:'test 0: write up to hard limit returned 69: Disc quota exceeded' \
	    $(atf_get_srcdir)/h_quota2_tests -b 0 ${IMG} ${RUMP_SERVER}
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'/mnt   3072 B\*  2048 B   3072 B     2:0      2       4       6         ' \
		    $(atf_get_srcdir)/rump_quota -$q -h ${id}
		atf_check -s exit:0 \
		    -o match:'daemon    \+-        3        2        3    2:0         2       4       6' \
		    $(atf_get_srcdir)/rump_repquota -$q /mnt
	done
	rump_shutdown
}

inherit_defaultiquota()
{
	create_with_quotas_server $*
	local q=$4
	local expect
	local id=1

	case ${q} in
	user)
		expect=u
		fail=g
		;;
	group)
		expect=g
		fail=u
		;;
	both)
		expect="u g"
		fail=""
		;;
	*)
		atf_fail "wrong quota type"
		;;
	esac

	for q in ${expect} ; do
		atf_check -s exit:0 \
		   $(atf_get_srcdir)/rump_edquota -$q -s2m/4 -h3m/6 \
		   -t 2h/2h -d
	done
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'Disk quotas for .*id 1\): none' \
		    $(atf_get_srcdir)/rump_quota -$q -v ${id}
	done
	atf_check -s exit:0 rump.halt

	#now start the server which does the limits tests
	atf_check -s exit:0 -o ignore \
-e match:'test 2: create file up to hard limit returned 69: Disc quota exceeded' \
	    $(atf_get_srcdir)/h_quota2_tests -b 2 ${IMG} ${RUMP_SERVER}
	for q in ${expect} ; do
		atf_check -s exit:0 \
		    -o match:'/mnt   3072 B   2048 K   3072 K              6 \*     4       6      2:0' \
		    $(atf_get_srcdir)/rump_quota -$q -h ${id}
		atf_check -s exit:0 \
		    -o match:'daemon    -\+        3     2048     3072                6       4       6    2:0' \
		    $(atf_get_srcdir)/rump_repquota -$q /mnt
	done
	rump_shutdown
}
