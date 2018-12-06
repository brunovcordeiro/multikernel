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

#include <stdio.h>

#define __NEED_HAL_BARRIER_
#include <nanvix/syscalls.h>

#include "test.h"

/**
 * @brief Test driver for Named Mailboxes.
 */
void test_nanvix_ipc_mailbox(void)
{

	/* Run API tests. */
	for (int i = 0; nanvix_ipc_mailbox_tests_api[i].test_fn != NULL; i++)
	{
		printf("[nanvix][test][api][ipc][mailbox] %s\n", nanvix_ipc_mailbox_tests_api[i].name);
		nanvix_ipc_mailbox_tests_api[i].test_fn();
	}

	/* Run fault injection tests. */
	for (int i = 0; nanvix_ipc_mailbox_tests_fault[i].test_fn != NULL; i++)
	{
		printf("[nanvix][test][fault][ipc][mailbox] %s\n", nanvix_ipc_mailbox_tests_fault[i].name);
		nanvix_ipc_mailbox_tests_fault[i].test_fn();
	}
}

