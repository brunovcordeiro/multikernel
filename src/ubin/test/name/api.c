/*
 * MIT License
 *
 * Copyright (c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.  THE SOFTWARE IS PROVIDED
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define __NEED_NAME_CLIENT

#include <nanvix/runtime/name.h>
#include <nanvix/sys/noc.h>
#include <nanvix/limits.h>
#include <nanvix/ulib.h>
#include "../test.h"

/*============================================================================*
 * API Test: Link Unlink                                                      *
 *============================================================================*/

/**
 * @brief API Test: Link Unlink
 */
static void test_name_link_unlink(void)
{
	int nodenum;
	char pathname[NANVIX_PROC_NAME_MAX];

	nodenum = knode_get_num();

	ustrcpy(pathname, "cool-name");
	TEST_ASSERT(name_link(nodenum, pathname) == 0);
	TEST_ASSERT(name_unlink(pathname) == 0);
}

/*============================================================================*
 * API Test: Lookup                                                           *
 *============================================================================*/

/**
 * @brief API Test: Lookup
 */
static void test_name_lookup(void)
{
	int nodenum;
	char pathname[NANVIX_PROC_NAME_MAX];

	nodenum = knode_get_num();

	ustrcpy(pathname, "cool-name");
	TEST_ASSERT(name_link(nodenum, pathname) == 0);
	TEST_ASSERT(name_lookup(pathname) == nodenum);
	TEST_ASSERT(name_unlink(pathname) == 0);
}

/*============================================================================*
 * API Test Driver Table                                                      *
 *============================================================================*/

/**
 * @brief Unit tests.
 */
struct test tests_name_api[] = {
	{ test_name_link_unlink, "link unlink"    },
	{ test_name_lookup,      "lookup"         },
	{ NULL,                   NULL            }
};
