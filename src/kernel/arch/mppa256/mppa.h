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

#ifndef _MPPA256_H_
#define _MPPA256_H_

	#ifndef _KALRAY_MPPA256
		#error "bad target"
	#endif

	#include <inttypes.h>
	#include <HAL/hal/core/timer.h>
	#include <HAL/hal/core/diagnostic.h>
#ifdef _KALRAY_MPPA_256_HIGH_LEVEL
	#include <mppaipc.h>
	#include <pthread.h>
#endif
#ifdef _KALRAY_MPPA_256_LOW_LEVEL
	#include <mppa_power.h>
	#include <mppa_rpc.h>
	#include <mppa_async.h>
	#include <utask.h>
#endif

/*=======================================================================*
 * Core Interface                                                        *
 *=======================================================================*/

	/**
	 * @brief Number of compute clusters.
	 */
	#define NR_CCLUSTER 16

	/**
	 * @brief Number of IO clusters.
	 */
	#define NR_IOCLUSTER 2

	/**
	 * @brief Number of Cores in an IO CLUSTER.
	 */
	#define NR_IOCLUSTER_CORES 4

	/* Cluster IDs. */
	#define CCLUSTER0    0 /**< Compute cluster  0. */
	#define CCLUSTER1    1 /**< Compute cluster  1. */
	#define CCLUSTER2    2 /**< Compute cluster  2. */
	#define CCLUSTER3    3 /**< Compute cluster  3. */
	#define CCLUSTER4    4 /**< Compute cluster  4. */
	#define CCLUSTER5    5 /**< Compute cluster  5. */
	#define CCLUSTER6    6 /**< Compute cluster  6. */
	#define CCLUSTER7    7 /**< Compute cluster  7. */
	#define CCLUSTER8    8 /**< Compute cluster  8. */
	#define CCLUSTER9    9 /**< Compute cluster  9. */
	#define CCLUSTER10  10 /**< Compute cluster 10. */
	#define CCLUSTER11  11 /**< Compute cluster 11. */
	#define CCLUSTER12  12 /**< Compute cluster 12. */
	#define CCLUSTER13  13 /**< Compute cluster 13. */
	#define CCLUSTER14  14 /**< Compute cluster 14. */
	#define CCLUSTER15  15 /**< Compute cluster 15. */
	#define IOCLUSTER0 128 /**< IO cluster 0.       */
	#define IOCLUSTER1 192 /**< IO cluster 1.       */

	/* Forward definitions. */
	extern int hal_get_cluster_id(void);
	extern int k1_is_ccluster(int);
	extern int k1_is_iocluster(int);
	extern int k1_is_ionode(int);
	extern int k1_is_cnode(int);
	extern long k1_get_ccluster_freq(void);

	/* Forward definitions. */
	extern pthread_t __threads[4];
	extern pthread_mutex_t core_lock;

/*=======================================================================*
 * NOC                                                                   *
 *=======================================================================*/

	/**
	 * @brief Number DMAs per compute cluster.
	 */
	#define NR_CCLUSTER_DMA 1

	/**
	 * @brief Number of DMAs per compute cluster.
	 */
	#define NR_IOCLUSTER_DMA 4

	/* Forward definitions. */
	extern int noctag_mailbox(int);
	extern int noctag_sync(int);
	extern int noctag_portal(int);
	extern int noc_get_node_num(int);
	extern void noc_get_remotes(char *, int);
	extern void noc_get_names(char *, const int *, int);
	extern int noc_get_dma(int);
	extern int noc_is_ionode(int);
	extern int noc_is_ionode0(int);
	extern int noc_is_ionode1(int);
	extern int noc_is_cnode(int);

/*=======================================================================*
 *                                                                       *
 *=======================================================================*/

	/* Forward defnitions. */
	extern uint64_t k1_timer_get(void);
	extern uint64_t k1_timer_diff(uint64_t, uint64_t);
	extern void k1_timer_init(void);

#endif /* _MPPA256_ */