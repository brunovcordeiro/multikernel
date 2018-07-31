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

#include <stdlib.h>
#include <stdio.h>

#include <mppaipc.h>

#include <nanvix/syscalls.h>
#include <nanvix/limits.h>
#include <nanvix/pm.h>

/**
 * @brief Asserts a logic expression.
 */
#define TEST_ASSERT(x) { if (!(x)) exit(EXIT_FAILURE); }

/*============================================================================*
 * Utilities                                                                  *
 *============================================================================*/

/**
 * @brief PID of slaves.
 */
static int pids[NANVIX_PROC_MAX];

/**
 * @brief Spawn slave processes.
 */
static void spawn_slaves(const char **args)
{
	for (int i = 0; i < NANVIX_PROC_MAX; i++)
		TEST_ASSERT((pids[i] = mppa_spawn(i, NULL, args[0], args, NULL)) != -1);
}

/**
 * @brief Wait for slaves to terminate.
 */
static void join_slaves(void)
{
	int status;

	for (int i = 0; i < NANVIX_PROC_MAX; i++)
	{
		TEST_ASSERT(mppa_waitpid(pids[i], &status, 0) != -1);
		TEST_ASSERT(status == EXIT_SUCCESS);
	}
}

/*============================================================================*
 * API Test: Create Unlink CC                                                 *
 *============================================================================*/

/**
* @brief API Test: Create Unlink CC
*/
static void test_ipc_barrier_create_unlink_cc(void)
{
	char masternode_str[4];
	char barrier_nclusters_str[4];
	char test_str[4];
	const char *args[] = {
		"/test/ipc-barrier-slave",
		masternode_str,
		barrier_nclusters_str,
		test_str,
		NULL
	};

	printf("[nanvix][test][api][ipc][barrier] Create Unlink CC\n");

	/* Build arguments. */
	sprintf(masternode_str, "%d", sys_get_node_num());
	sprintf(barrier_nclusters_str, "%d", NANVIX_PROC_MAX);
	sprintf(test_str, "%d", 0);

	spawn_slaves(args);
	join_slaves();
}

/*============================================================================*
 * API Test: Wait CC                                                          *
 *============================================================================*/

/**
* @brief API Test: Wait CC
*/
static void test_ipc_barrier_wait_cc(void)
{
	char masternode_str[4];
	char barrier_nclusters_str[4];
	char test_str[4];
	const char *args[] = {
		"/test/ipc-barrier-slave",
		masternode_str,
		barrier_nclusters_str,
		test_str,
		NULL
	};

	printf("[nanvix][test][api][ipc][barrier] Wait CC\n");

	/* Build arguments. */
	sprintf(masternode_str, "%d", sys_get_node_num());
	sprintf(barrier_nclusters_str, "%d", NANVIX_PROC_MAX);
	sprintf(test_str, "%d", 1);

	spawn_slaves(args);
	join_slaves();
}

/*============================================================================*
 * API Test: Wait 2 CC                                                        *
 *============================================================================*/

/**
* @brief API Test: Wait 2 CC
*/
static void test_ipc_barrier_wait2_cc(void)
{
	int nodenum;
	int barrier;
	int nodes[NANVIX_PROC_MAX + 1];
	char masternode_str[4];
	char barrier_nclusters_str[4];
	char test_str[4];
	const char *args[] = {
		"/test/ipc-barrier-slave",
		masternode_str,
		barrier_nclusters_str,
		test_str,
		NULL
	};

	printf("[nanvix][test][api][ipc][barrier] Wait 2 CC\n");

	nodenum = sys_get_node_num();

	/* Build nodes list. */
	nodes[0] = nodenum;
	for (int i = 0; i < NANVIX_PROC_MAX; i++)
		nodes[i + 1] = i;

	/* Build arguments. */
	sprintf(masternode_str, "%d", sys_get_node_num());
	sprintf(barrier_nclusters_str, "%d", NANVIX_PROC_MAX);
	sprintf(test_str, "%d", 2);

	TEST_ASSERT((barrier = barrier_create(nodes, NANVIX_PROC_MAX + 1)) >= 0);

	spawn_slaves(args);

	TEST_ASSERT(barrier_wait(barrier) == 0);

	join_slaves();

	TEST_ASSERT(barrier_unlink(barrier) == 0);
}

/*============================================================================*/

/**
 * @brief Automated test driver for Barriers.
 */
void test_ipc_barrier(void)
{
	test_ipc_barrier_create_unlink_cc();
	test_ipc_barrier_wait_cc();
	test_ipc_barrier_wait2_cc();
}
