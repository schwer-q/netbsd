#
# Automated Testing Framework (atf)
#
# Copyright (c) 2007, 2008 The NetBSD Foundation, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
# CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# -------------------------------------------------------------------------
# Helper tests for "t_atf_check".
# -------------------------------------------------------------------------

atf_test_case atf_check_info_ok
atf_check_info_ok_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_info_ok_body()
{
    atf_check 'true' 0 null null
}

atf_test_case atf_check_info_fail
atf_check_info_fail_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_info_fail_body()
{
    atf_check 'false' 1 null null
}

atf_test_case atf_check_expout_mismatch
atf_check_expout_mismatch_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_expout_mismatch_body()
{
    cat >expout <<SECONDEOF
foo
SECONDEOF
    atf_check 'echo bar' 0 expout null
}

atf_test_case atf_check_experr_mismatch
atf_check_experr_mismatch_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_experr_mismatch_body()
{
    cat >experr <<SECONDEOF
foo
SECONDEOF
    atf_check 'echo bar 1>&2' 0 null experr
}

atf_test_case atf_check_null_stdout
atf_check_null_stdout_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_null_stdout_body()
{
    atf_check 'echo "These are the contents"' 0 null null
}

atf_test_case atf_check_null_stderr
atf_check_null_stderr_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_null_stderr_body()
{
    atf_check 'echo "These are the contents" 1>&2' 0 null null
}

atf_test_case atf_check_change_cwd
atf_check_change_cwd_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_change_cwd_body()
{
    mkdir foo
    chmod 555 foo
    cd foo
    atf_check 'echo Hello' 0 stdout null
    cd -
    test -f stdout || atf_fail "Used incorrect work directory"
    echo Hello >bar
    cmp -s stdout bar || atf_fail "Used incorrect work directory"
}

atf_test_case atf_check_equal_ok
atf_check_equal_ok_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_equal_ok_body()
{
    atf_check_equal a a
}

atf_test_case atf_check_equal_fail
atf_check_equal_fail_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_equal_fail_body()
{
    atf_check_equal a b
}

atf_test_case atf_check_equal_eval_ok
atf_check_equal_eval_ok_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_equal_eval_ok_body()
{
    x=a
    y=a
    atf_check_equal '${x}' '${y}'
}

atf_test_case atf_check_equal_eval_fail
atf_check_equal_eval_fail_head()
{
    atf_set "descr" "Helper test case for the t_atf_check test program"
}
atf_check_equal_eval_fail_body()
{
    x=a
    y=b
    atf_check_equal '${x}' '${y}'
}

# -------------------------------------------------------------------------
# Helper tests for "t_config".
# -------------------------------------------------------------------------

atf_test_case config_get
config_get_head()
{
    atf_set "descr" "Helper test case for the t_config test program"
}
config_get_body()
{
    if atf_config_has ${TEST_VARIABLE}; then
        echo "${TEST_VARIABLE} = $(atf_config_get ${TEST_VARIABLE})"
    fi
}

atf_test_case config_has
config_has_head()
{
    atf_set "descr" "Helper test case for the t_config test program"
}
config_has_body()
{
    if atf_config_has ${TEST_VARIABLE}; then
        echo "${TEST_VARIABLE} found"
    else
        echo "${TEST_VARIABLE} not found"
    fi
}

# -------------------------------------------------------------------------
# Helper tests for "t_normalize".
# -------------------------------------------------------------------------

atf_test_case normalize
normalize_head()
{
    atf_set "descr" "Helper test case for the t_normalize test program"
    atf_set "a.b" "test value 1"
    atf_set "c-d" "test value 2"
}
normalize_body()
{
    echo "a.b: $(atf_get a.b)"
    echo "c-d: $(atf_get c-d)"
}

# -------------------------------------------------------------------------
# Helper tests for "t_tc".
# -------------------------------------------------------------------------

atf_test_case tc_pass_true
tc_pass_true_head()
{
    atf_set "descr" "Helper test case for the t_tc test program"
}
tc_pass_true_body()
{
    true
}

atf_test_case tc_pass_false
tc_pass_false_head()
{
    atf_set "descr" "Helper test case for the t_tc test program"
}
tc_pass_false_body()
{
    false
}

atf_test_case tc_fail
tc_fail_head()
{
    atf_set "descr" "Helper test case for the t_tc test program"
}
tc_fail_body()
{
    echo "An error" 1>&2
    exit 1
}

# -------------------------------------------------------------------------
# Helper tests for "t_tp".
# -------------------------------------------------------------------------

atf_test_case tp_srcdir
tp_srcdir_head()
{
    atf_set "descr" "Helper test case for the t_tp test program"
}
tp_srcdir_body()
{
    echo "Calling helper"
    helper_subr || atf_fail "Could not call helper subroutine"
}

# -------------------------------------------------------------------------
# Main.
# -------------------------------------------------------------------------

atf_init_test_cases()
{
    # Add helper tests for t_atf_check.
    atf_add_test_case atf_check_info_ok
    atf_add_test_case atf_check_info_fail
    atf_add_test_case atf_check_expout_mismatch
    atf_add_test_case atf_check_experr_mismatch
    atf_add_test_case atf_check_null_stdout
    atf_add_test_case atf_check_null_stderr
    atf_add_test_case atf_check_change_cwd
    atf_add_test_case atf_check_equal_ok
    atf_add_test_case atf_check_equal_fail
    atf_add_test_case atf_check_equal_eval_ok
    atf_add_test_case atf_check_equal_eval_fail

    # Add helper tests for t_config.
    atf_add_test_case config_get
    atf_add_test_case config_has

    # Add helper tests for t_normalize.
    atf_add_test_case normalize

    # Add helper tests for t_tc.
    atf_add_test_case tc_pass_true
    atf_add_test_case tc_pass_false
    atf_add_test_case tc_fail

    # Add helper tests for t_tp.
    [ -f $(atf_get_srcdir)/subrs ] && . $(atf_get_srcdir)/subrs
    atf_add_test_case tp_srcdir
}

# vim: syntax=sh:expandtab:shiftwidth=4:softtabstop=4
