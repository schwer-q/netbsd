//
// Automated Testing Framework (atf)
//
// Copyright (c) 2007, 2008, 2009, 2010 The NetBSD Foundation, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
// CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}

#include <cerrno>
#include <fstream>

#include "atf-c++/exceptions.hpp"
#include "atf-c++/fs.hpp"
#include "atf-c++/macros.hpp"
#include "atf-c++/user.hpp"

#include "fs.hpp"

// ------------------------------------------------------------------------
// Auxiliary functions.
// ------------------------------------------------------------------------

static
void
create_file(const char *name)
{
    std::ofstream os(name);
    os.close();
}

// ------------------------------------------------------------------------
// Test cases for the "temp_dir" class.
// ------------------------------------------------------------------------

ATF_TEST_CASE(temp_dir_raii);
ATF_TEST_CASE_HEAD(temp_dir_raii)
{
    set_md_var("descr", "Tests the RAII behavior of the temp_dir class");
    set_md_var("use.fs", "true");
}
ATF_TEST_CASE_BODY(temp_dir_raii)
{
    using atf::atf_run::temp_dir;

    atf::fs::path t1("non-existent");
    atf::fs::path t2("non-existent");

    {
        atf::fs::path tmpl("testdir.XXXXXX");
        temp_dir td1(tmpl);
        temp_dir td2(tmpl);
        t1 = td1.get_path();
        t2 = td2.get_path();
        ATF_CHECK(t1.str().find("XXXXXX") == std::string::npos);
        ATF_CHECK(t2.str().find("XXXXXX") == std::string::npos);
        ATF_CHECK(t1 != t2);
        ATF_CHECK(!atf::fs::exists(tmpl));
        ATF_CHECK( atf::fs::exists(t1));
        ATF_CHECK( atf::fs::exists(t2));

        atf::fs::file_info fi1(t1);
        ATF_CHECK( fi1.is_owner_readable());
        ATF_CHECK( fi1.is_owner_writable());
        ATF_CHECK( fi1.is_owner_executable());
        ATF_CHECK(!fi1.is_group_readable());
        ATF_CHECK(!fi1.is_group_writable());
        ATF_CHECK(!fi1.is_group_executable());
        ATF_CHECK(!fi1.is_other_readable());
        ATF_CHECK(!fi1.is_other_writable());
        ATF_CHECK(!fi1.is_other_executable());

        atf::fs::file_info fi2(t2);
        ATF_CHECK( fi2.is_owner_readable());
        ATF_CHECK( fi2.is_owner_writable());
        ATF_CHECK( fi2.is_owner_executable());
        ATF_CHECK(!fi2.is_group_readable());
        ATF_CHECK(!fi2.is_group_writable());
        ATF_CHECK(!fi2.is_group_executable());
        ATF_CHECK(!fi2.is_other_readable());
        ATF_CHECK(!fi2.is_other_writable());
        ATF_CHECK(!fi2.is_other_executable());
    }

    ATF_CHECK(t1.str() != "non-existent");
    ATF_CHECK(!atf::fs::exists(t1));
    ATF_CHECK(t2.str() != "non-existent");
    ATF_CHECK(!atf::fs::exists(t2));
}


// ------------------------------------------------------------------------
// Test cases for the free functions.
// ------------------------------------------------------------------------

ATF_TEST_CASE(cleanup);
ATF_TEST_CASE_HEAD(cleanup)
{
    set_md_var("descr", "Tests the cleanup function");
    set_md_var("use.fs", "true");
}
ATF_TEST_CASE_BODY(cleanup)
{
    using atf::atf_run::cleanup;

    ::mkdir("root", 0755);
    ::mkdir("root/dir", 0755);
    ::mkdir("root/dir/1", 0100);
    ::mkdir("root/dir/2", 0644);
    create_file("root/reg");

    atf::fs::path p("root");
    ATF_CHECK(atf::fs::exists(p));
    ATF_CHECK(atf::fs::exists(p / "dir"));
    ATF_CHECK(atf::fs::exists(p / "dir/1"));
    ATF_CHECK(atf::fs::exists(p / "dir/2"));
    ATF_CHECK(atf::fs::exists(p / "reg"));
    cleanup(p);
    ATF_CHECK(!atf::fs::exists(p));
}

ATF_TEST_CASE(cleanup_eacces_on_root);
ATF_TEST_CASE_HEAD(cleanup_eacces_on_root)
{
    set_md_var("descr", "Tests the cleanup function");
    set_md_var("use.fs", "true");
}
ATF_TEST_CASE_BODY(cleanup_eacces_on_root)
{
    using atf::atf_run::cleanup;

    ::mkdir("aux", 0755);
    ::mkdir("aux/root", 0755);
    ATF_CHECK(::chmod("aux", 0555) != -1);

    try {
        cleanup(atf::fs::path("aux/root"));
        ATF_CHECK(atf::user::is_root());
    } catch (const atf::system_error& e) {
        ATF_CHECK(!atf::user::is_root());
        ATF_CHECK_EQUAL(EACCES, e.code());
    }
}

ATF_TEST_CASE(cleanup_eacces_on_subdir);
ATF_TEST_CASE_HEAD(cleanup_eacces_on_subdir)
{
    set_md_var("descr", "Tests the cleanup function");
    set_md_var("use.fs", "true");
}
ATF_TEST_CASE_BODY(cleanup_eacces_on_subdir)
{
    using atf::atf_run::cleanup;

    ::mkdir("root", 0755);
    ::mkdir("root/1", 0755);
    ::mkdir("root/1/2", 0755);
    ::mkdir("root/1/2/3", 0755);
    ATF_CHECK(::chmod("root/1/2", 0555) != -1);
    ATF_CHECK(::chmod("root/1", 0555) != -1);

    const atf::fs::path p("root");
    cleanup(p);
    ATF_CHECK(!atf::fs::exists(p));
}

ATF_TEST_CASE(change_directory);
ATF_TEST_CASE_HEAD(change_directory)
{
    set_md_var("descr", "Tests the change_directory function");
    set_md_var("use.fs", "true");
}
ATF_TEST_CASE_BODY(change_directory)
{
    using atf::atf_run::change_directory;
    using atf::atf_run::get_current_dir;

    ::mkdir("files", 0755);
    ::mkdir("files/dir", 0755);
    create_file("files/reg");

    const atf::fs::path old = get_current_dir();

    ATF_CHECK_THROW(atf::system_error,
                    change_directory(atf::fs::path("files/reg")));
    ATF_CHECK(get_current_dir() == old);

    atf::fs::path old2 = change_directory(atf::fs::path("files"));
    ATF_CHECK(old2 == old);
    atf::fs::path old3 = change_directory(atf::fs::path("dir"));
    ATF_CHECK(old3 == old2 / "files");
    atf::fs::path old4 = change_directory(atf::fs::path("../.."));
    ATF_CHECK(old4 == old3 / "dir");
    ATF_CHECK(get_current_dir() == old);
}

ATF_TEST_CASE(get_current_dir);
ATF_TEST_CASE_HEAD(get_current_dir)
{
    set_md_var("descr", "Tests the get_current_dir function");
    set_md_var("use.fs", "true");
}
ATF_TEST_CASE_BODY(get_current_dir)
{
    using atf::atf_run::change_directory;
    using atf::atf_run::get_current_dir;

    ::mkdir("files", 0755);
    ::mkdir("files/dir", 0755);
    create_file("files/reg");

    atf::fs::path curdir = get_current_dir();
    change_directory(atf::fs::path("."));
    ATF_CHECK(get_current_dir() == curdir);
    change_directory(atf::fs::path("files"));
    ATF_CHECK(get_current_dir() == curdir / "files");
    change_directory(atf::fs::path("dir"));
    ATF_CHECK(get_current_dir() == curdir / "files/dir");
    change_directory(atf::fs::path(".."));
    ATF_CHECK(get_current_dir() == curdir / "files");
    change_directory(atf::fs::path(".."));
    ATF_CHECK(get_current_dir() == curdir);
}

ATF_TEST_CASE(set_immutable);
ATF_TEST_CASE_HEAD(set_immutable)
{
    set_md_var("descr", "Tests the set_immutable function");
    set_md_var("use.fs", "true");
}
ATF_TEST_CASE_BODY(set_immutable)
{
    using atf::atf_run::set_immutable;

    if (::mkdir("dir", 0755) == -1)
        ATF_FAIL("Failed to create test directory");

    if (!set_immutable(atf::fs::path("dir"), true))
        ATF_SKIP("Don't know how to set the immutable flag");

    if (::mkdir("dir/other", 0755) != -1)
        ATF_FAIL("Immutable flag was not correctly set");

    if (!set_immutable(atf::fs::path("dir"), false))
        ATF_SKIP("Don't know how to unset the immutable flag");

    if (::mkdir("dir/other", 0755) == -1)
        ATF_FAIL("Immutable flag was not correctly unset");
}

// ------------------------------------------------------------------------
// Main.
// ------------------------------------------------------------------------

ATF_INIT_TEST_CASES(tcs)
{
    // Add the tests for the "temp_dir" class.
    ATF_ADD_TEST_CASE(tcs, temp_dir_raii);

    // Add the tests for the free functions.
    ATF_ADD_TEST_CASE(tcs, cleanup);
    ATF_ADD_TEST_CASE(tcs, cleanup_eacces_on_root);
    ATF_ADD_TEST_CASE(tcs, cleanup_eacces_on_subdir);
    ATF_ADD_TEST_CASE(tcs, change_directory);
    ATF_ADD_TEST_CASE(tcs, get_current_dir);
    ATF_ADD_TEST_CASE(tcs, set_immutable);
}
