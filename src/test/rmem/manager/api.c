/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define __NEED_RMEM_CLIENT

#include <nanvix/servers/rmem.h>
#include <nanvix/ulib.h>
#include "../../test.h"

/**
 * @brief Dummy buffer.
 */
static char buffer[RMEM_BLOCK_SIZE];

/*============================================================================*
 * API Test: Alloc/Free                                                       *
 *============================================================================*/

/**
 * @brief API Test: Alloc/Free
 */
static void test_rmem_manager_alloc_free(void)
{
	rpage_t blknum;

	TEST_ASSERT((blknum = nanvix_rmem_alloc()) != RMEM_NULL);
	TEST_ASSERT(nanvix_rmem_free(blknum) == 0);
}

/*============================================================================*
 * API Test: Read Write                                                       *
 *============================================================================*/

/**
 * @brief API Test: Read Write
 */
static void test_rmem_manager_read_write(void)
{
	rpage_t blknum;

	TEST_ASSERT((blknum = nanvix_rmem_alloc()) != RMEM_NULL);

		umemset(buffer, 1, RMEM_BLOCK_SIZE);
		TEST_ASSERT(nanvix_rmem_write(blknum, buffer) == RMEM_BLOCK_SIZE);

		umemset(buffer, 0, RMEM_BLOCK_SIZE);
		TEST_ASSERT(nanvix_rmem_read(blknum, buffer) == RMEM_BLOCK_SIZE);

		/* Checksum. */
		for (unsigned long i = 0; i < RMEM_BLOCK_SIZE; i++)
			TEST_ASSERT(buffer[i] == 1);

	TEST_ASSERT(nanvix_rmem_free(blknum) == 0);
}

/*============================================================================*
 * API Test: Consistency                                                      *
 *============================================================================*/

/**
 * @brief API Test: Consistency
 */
static void test_rmem_manager_consistency(void)
{
	rpage_t blknum1;
	rpage_t blknum2;
	rpage_t blknum3;

	TEST_ASSERT((blknum1 = nanvix_rmem_alloc()) != RMEM_NULL);
	TEST_ASSERT((blknum2 = nanvix_rmem_alloc()) != RMEM_NULL);
	TEST_ASSERT((blknum3 = nanvix_rmem_alloc()) != RMEM_NULL);

		/* First round. */
		umemset(buffer, 1, RMEM_BLOCK_SIZE);
		TEST_ASSERT(nanvix_rmem_write(blknum1, buffer) == RMEM_BLOCK_SIZE);

		/* Second round. */
        umemset(buffer, 2, RMEM_BLOCK_SIZE);
        TEST_ASSERT(nanvix_rmem_write(blknum2, buffer) == RMEM_BLOCK_SIZE);

		/* Third round. */
        umemset(buffer, 3, RMEM_BLOCK_SIZE);
        TEST_ASSERT(nanvix_rmem_write(blknum3, buffer) == RMEM_BLOCK_SIZE);

		/* Checksum. */
		umemset(buffer, 9, RMEM_BLOCK_SIZE);
		TEST_ASSERT(nanvix_rmem_read(blknum1, buffer) == RMEM_BLOCK_SIZE);
		for (unsigned long i = 0; i < RMEM_BLOCK_SIZE; i++)
			TEST_ASSERT(buffer[i] == 1);
		umemset(buffer, 9, RMEM_BLOCK_SIZE);
		TEST_ASSERT(nanvix_rmem_read(blknum2, buffer) == RMEM_BLOCK_SIZE);
		for (unsigned long i = 0; i < RMEM_BLOCK_SIZE; i++)
			TEST_ASSERT(buffer[i] == 2);
		umemset(buffer, 9, RMEM_BLOCK_SIZE);
		TEST_ASSERT(nanvix_rmem_read(blknum3, buffer) == RMEM_BLOCK_SIZE);
		for (unsigned long i = 0; i < RMEM_BLOCK_SIZE; i++)
			TEST_ASSERT(buffer[i] == 3);

	TEST_ASSERT(nanvix_rmem_free(blknum1) == 0);
	TEST_ASSERT(nanvix_rmem_free(blknum2) == 0);
	TEST_ASSERT(nanvix_rmem_free(blknum3) == 0);
}

/*============================================================================*
 * Test Driver Table                                                          *
 *============================================================================*/

/**
 * @brief Unit tests.
 */
struct test tests_rmem_manager_api[] = {
	{ test_rmem_manager_alloc_free, "alloc/free" },
	{ test_rmem_manager_read_write, "read write" },
	{ test_rmem_manager_consistency, "consistency" },
	{ NULL,                          NULL        },
};
