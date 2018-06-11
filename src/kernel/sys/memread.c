/*
 * Copyright(C) 2011-2018 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/hal.h>
#include <nanvix/mm.h>
#include <nanvix/pm.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"

/**
 * @brief Reads from a remote memory.
 *
 * @param addr Remote address.
 * @param bug  Location where the data should be written to.
 * @param n    Number of bytes to read.
 */
void memread(uint64_t addr, void *buf, size_t n)
{
	int clusterid;           /* Cluster ID of the calling process. */
	struct rmem_message msg; /* Remote memory operation.           */

	clusterid = hal_get_cluster_id();

	meminit();

	/* Build operation header. */
	msg.source = clusterid;
	msg.op = RMEM_READ;
	msg.blknum = addr;
	msg.size = n;

	/* Send operation header. */
	mailbox_write(_mem_outbox, &msg);

	/* Send data. */
	portal_allow(_mem_inportal, IOCLUSTER1);
	portal_read(_mem_inportal, buf, n);
}

