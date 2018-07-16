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

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include <nanvix/pm.h>
#include <nanvix/syscalls.h>

extern int name_init(void); 

/**
 * @brief Global runtime lock.
 */
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 *
 * @brief Is the runtime initialized ?
 */
static int initialized[HAL_NR_NOC_IONODES] = { 0, };

/**
 * @brief Locks the runtime.
 */
static void runtime_lock(void)
{
	pthread_mutex_lock(&lock);
}

/**
 * @brief Unlocks the runtime.
 */
static void runtime_unlock(void)
{
	pthread_mutex_unlock(&lock);
}

/**
 * @brief Initializes the runtime.
 */
int runtime_setup(void)
{
	int index;

	runtime_lock();

		index = sys_get_core_id();

		/* Runtime was already initialized. */
		if (initialized[index])
			goto error;

		 /* Create underlying input mailbox. */		
		if (initialize_inbox(index) != 0)
			goto error;

		initialized[index] = 1;

		name_init();

	runtime_unlock();

	return (0);

error:
	runtime_unlock();
	printf("[nanvix][libs] failled to setup runtime\n");
	return (-EAGAIN);
}

/**
 * @brief Cleans runtime.
 */
int runtime_cleanup(void)
{
	int index;

	runtime_lock();

		index = sys_get_core_id();

		/* Runtime was not initialized. */
		if (!initialized[index])
			goto error;

		/* Destroy underlying input mailbox. */
		if (destroy_inbox(index) != 0)
			goto error;

		initialized[index] = 0;

	runtime_unlock();

	return (0);

error:
	runtime_unlock();
	printf("[nanvix][libs] failled to clean runtime\n");
	return (-EAGAIN);
}
