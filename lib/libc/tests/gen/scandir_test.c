/*-
 * Copyright (c) 2025 Klara, Inc.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sys/stat.h>

#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>

#include <atf-c.h>

static void
scandir_prepare(const struct atf_tc *tc)
{
	ATF_REQUIRE_EQ(0, mkdir("dir", 0755));
	ATF_REQUIRE_EQ(0, mkdir("dir/dir", 0755));
	ATF_REQUIRE_EQ(0, close(creat("dir/file", 0644)));
	ATF_REQUIRE_EQ(0, symlink("file", "dir/link"));
	ATF_REQUIRE_EQ(0, mkdir("dir/skip", 0755));
}

static void
scandir_verify(const struct atf_tc *tc, int n, struct dirent **namelist)
{
	ATF_REQUIRE_EQ_MSG(5, n, "return value is %d", n);
	ATF_CHECK_STREQ("link", namelist[0]->d_name);
	ATF_CHECK_STREQ("file", namelist[1]->d_name);
	ATF_CHECK_STREQ("dir", namelist[2]->d_name);
	ATF_CHECK_STREQ("..", namelist[3]->d_name);
	ATF_CHECK_STREQ(".", namelist[4]->d_name);
}

static int
scandir_select(const struct dirent *ent)
{
	return (strcmp(ent->d_name, "skip") != 0);
}

static int
scandir_compare(const struct dirent **a, const struct dirent **b)
{
	return (strcmp((*b)->d_name, (*a)->d_name));
}

ATF_TC(scandir_test);
ATF_TC_HEAD(scandir_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test scandir()");
}
ATF_TC_BODY(scandir_test, tc)
{
	struct dirent **namelist = NULL;
	int i, ret;

	scandir_prepare(tc);
	ret = scandir("dir", &namelist, scandir_select, scandir_compare);
	scandir_verify(tc, ret, namelist);
	for (i = 0; i < ret; i++)
		free(namelist[i]);
	free(namelist);
}

ATF_TC(fdscandir_test);
ATF_TC_HEAD(fdscandir_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test fdscandir()");
}
ATF_TC_BODY(fdscandir_test, tc)
{
	struct dirent **namelist = NULL;
	int fd, i, ret;

	scandir_prepare(tc);
	ATF_REQUIRE((fd = open("dir", O_DIRECTORY | O_RDONLY)) >= 0);
	ret = fdscandir(fd, &namelist, scandir_select, scandir_compare);
	scandir_verify(tc, ret, namelist);
	for (i = 0; i < ret; i++)
		free(namelist[i]);
	free(namelist);
	ATF_REQUIRE_EQ(0, close(fd));
}

ATF_TC(scandirat_test);
ATF_TC_HEAD(scandirat_test, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test scandirat()");
}
ATF_TC_BODY(scandirat_test, tc)
{
	struct dirent **namelist = NULL;
	int fd, i, ret;

	scandir_prepare(tc);
	ATF_REQUIRE((fd = open("dir", O_DIRECTORY | O_SEARCH)) >= 0);
	ret = scandirat(fd, ".", &namelist, scandir_select, scandir_compare);
	scandir_verify(tc, ret, namelist);
	for (i = 0; i < ret; i++)
		free(namelist[i]);
	free(namelist);
	ATF_REQUIRE_EQ(0, close(fd));
}

static int
scandir_none(const struct dirent *ent __unused)
{
	return (0);
}

ATF_TC(scandir_none);
ATF_TC_HEAD(scandir_none, tc)
{
	atf_tc_set_md_var(tc, "descr",
	    "Test scandir() when no entries are selected");
}
ATF_TC_BODY(scandir_none, tc)
{
	struct dirent **namelist = NULL;

	ATF_REQUIRE_EQ(0, scandir(".", &namelist, scandir_none, alphasort));
	ATF_REQUIRE(namelist);
	free(namelist);
}

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, scandir_test);
	ATF_TP_ADD_TC(tp, fdscandir_test);
	ATF_TP_ADD_TC(tp, scandirat_test);
	ATF_TP_ADD_TC(tp, scandir_none);
	return (atf_no_error());
}
