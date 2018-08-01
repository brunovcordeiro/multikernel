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

#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include <mppaipc.h>
#include <mppa/osconfig.h>
#include <HAL/hal/core/mp.h>
#include <HAL/hal/core/diagnostic.h>

#include "../kernel.h"

/**
 * @brief Benchmark parameters.
 */
/**@{*/
static int nclusters = 0;         /**< Number of remotes processes.    */
static int niterations = 0;       /**< Number of benchmark parameters. */
static const char *kernel = NULL; /**< Benchmark kernel.               */
/**@}*/

/**
 * @brief Input rqueue.
 */
static int inbox;

/**
 * @brief Master sync.
 */
static int sync_master;

/**
 * @brief ID of slave processes.
 */
static int pids[NR_CCLUSTER];

/**
 * @brief Buffer.
 */
static char buffer[NR_CCLUSTER*MSG_SIZE];

/*============================================================================*
 * Utilities                                                                  *
 *============================================================================*/

/**
 * @brief Spawns remote processes.
 */
static void spawn_remotes(void)
{
	char niterations_str[3];
	const char *argv[] = {
		"/benchmark/mppa256-rqueue-slave",
		niterations_str,
		kernel,
		NULL
	};

	/* Spawn remotes. */
	sprintf(niterations_str, "%d", niterations);
	for (int i = 0; i < nclusters; i++)
		assert((pids[i] = mppa_spawn(i, NULL, argv[0], argv, NULL)) != -1);
}

/**
 * @brief Wait for remote processes.
 */
static void join_remotes(void)
{
	for (int i = 0; i < nclusters; i++)
		assert(mppa_waitpid(pids[i], NULL, 0) != -1);
}

/**
 * @brief Opens output mailboxes.
 *
 * @param outboxes Location to store mailbox IDs.
 */
static void open_outboxes(int *outboxes)
{
	char pathname[128];

	for (int i = 0; i < nclusters; i++)
	{
		sprintf(pathname, RQUEUE_SLAVE, i, 58 + i, 59 + i);
		assert((outboxes[i] = mppa_open(pathname, O_WRONLY)) != -1);
	}
}

/**
 * @brief Close output mailboxes.
 */
static void close_outboxes(int *outboxes)
{
	/* Open output mailboxes. */
	for (int i = 0; i < nclusters; i++)
		assert(mppa_close(outboxes[i]) != -1);
}

/*============================================================================*
 * Timer                                                                      *
 *============================================================================*/

/**
 * @brief Timer error.
 */
static uint64_t timer_error = 0;

/**
 * @brief Gets the current timer value.
 *
 * @returns The current timer value;
 */
static inline uint64_t timer_get(void)
{
	return (__k1_read_dsu_timestamp());
}

/**
 * @brief Computes the difference between two timer values.
 *
 * @param t1 Start time.
 * @param t2 End time.
 *
 * @returns The difference between the two timers (t2 - t1).
 */
static inline uint64_t timer_diff(uint64_t t1, uint64_t t2)
{
	return (((t2 - t1) <= timer_error) ? timer_error : t2 - t1 - timer_error);
}

/**
 * @brief Calibrates the timer.
 */
static void timer_init(void)
{
	uint64_t start, end;

	start = timer_get();
	end = timer_get();

	timer_error = (end - start);
}

/*============================================================================*
 * Broadcast Kernel                                                           *
 *============================================================================*/

/**
 * @brief Broadcast kernel.
 */
static void kernel_broadcast(void)
{
	uint64_t mask;
	int outboxes[nclusters];

	open_outboxes(outboxes);

	/* Wait for slaves. */
	assert(mppa_read(sync_master, &mask, sizeof(uint64_t)) != -1);

	/* Benchmark. */
	for (int k = 0; k <= (niterations + 1); k++)
	{
		double total;
		uint64_t t1, t2;

		/* Send data. */
		t1 = timer_get();
		for (int i = 0; i < nclusters; i++)
			assert(mppa_write(outboxes[i], buffer, MSG_SIZE) == MSG_SIZE);
		t2 = timer_get();

		total = timer_diff(t1, t2)/((double) MPPA256_FREQ);

		/* Warmup. */
		if (((k == 0) || (k == (niterations + 1))))
			continue;

		printf("nodeos;mailbox;%s;%d;%d;%lf;%lf\n",
			kernel,
			MSG_SIZE,
			nclusters,
			total/nclusters,
			(nclusters*MSG_SIZE)/total
		);
	}

	/* House keeping. */
	close_outboxes(outboxes);
}

/*============================================================================*
 * Gather Kernel                                                              *
 *============================================================================*/

/**
 * @brief Gather kernel.
 */
static void kernel_gather(void)
{
	/* Benchmark. */
	for (int k = 0; k <= (niterations + 1); k++)
	{
		double total;
		uint64_t t1, t2;

		/* Read data. */
		t1 = timer_get();
		for (int i = 0; i < nclusters; i++)
			assert(mppa_read(inbox, buffer, MSG_SIZE) == MSG_SIZE);
		t2 = timer_get();

		total = timer_diff(t1, t2)/((double) MPPA256_FREQ);

		/* Warmup. */
		if (((k == 0) || (k == (niterations + 1))))
			continue;

		printf("nodeos;mailbox;%s;%d;%d;%lf;%lf\n",
			kernel,
			MSG_SIZE,
			nclusters,
			total/nclusters,
			(nclusters*MSG_SIZE)/total
		);
	}
}

/*============================================================================*
 * Ping-Pong Kernel                                                           *
 *============================================================================*/

/**
 * @brief Ping-Pong kernel. 
 */
static void kernel_pingpong(void)
{
	uint64_t mask;
	int outboxes[nclusters];

	open_outboxes(outboxes);

	/* Wait for slaves. */
	assert(mppa_read(sync_master, &mask, sizeof(uint64_t)) != -1);

	/* Benchmark. */
	for (int k = 0; k <= (niterations + 1); k++)
	{
		double total;
		uint64_t t1, t2;

		/* Send data. */
		t1 = timer_get();
		for (int i = 0; i < nclusters; i++)
		{
			assert(mppa_write(outboxes[i], buffer, MSG_SIZE) == MSG_SIZE);
			assert(mppa_read(inbox, buffer, MSG_SIZE) == MSG_SIZE);
		}
		t2 = timer_get();

		total = timer_diff(t1, t2)/((double) MPPA256_FREQ);

		/* Warmup. */
		if (((k == 0) || (k == (niterations + 1))))
			continue;

		printf("nodeos;mailbox;%s;%d;%d;%lf;%lf\n",
			kernel,
			MSG_SIZE,
			nclusters,
			total/nclusters,
			2*(nclusters*MSG_SIZE)/total
		);
	}

	/* House keeping. */
	close_outboxes(outboxes);
}

/*============================================================================*
 * MPPA-256 Rqueue Microbenchmark Driver                                      *
 *============================================================================*/

/**
 * @brief Rqueue microbenchmark.
 */
static void benchmark(void)
{
	uint64_t mask;
	
	mask = ~((1 << nclusters) - 1);

	/* Initialization. */
	assert((inbox = mppa_open(RQUEUE_MASTER, O_RDONLY)) != -1);
	assert((sync_master = mppa_open(SYNC_MASTER, O_RDONLY)) != -1);
	assert(mppa_ioctl(sync_master, MPPA_RX_SET_MATCH, mask) != -1);
	spawn_remotes();

	timer_init();

	/* Run kernel. */
	if (!strcmp(kernel, "broadcast"))
		kernel_broadcast();
	else if (!strcmp(kernel, "gather"))
		kernel_gather();
	else if (!strcmp(kernel, "pingpong"))
		kernel_pingpong();
	
	/* House keeping. */
	join_remotes();
	assert(mppa_close(sync_master) != -1);
	assert(mppa_close(inbox) != -1);
}

/**
 * @brief Rqueue Microbenchmark Driver
 */
int main(int argc, const char **argv)
{
	assert(argc == 4);

	/* Retrieve kernel parameters. */
	nclusters = atoi(argv[1]);
	niterations = atoi(argv[2]);
	kernel = argv[3];

	/* Parameter checking. */
	assert((nclusters > 0) && (nclusters <= NR_CCLUSTER));
	assert(niterations > 0);

	benchmark();

	return (EXIT_SUCCESS);
}
