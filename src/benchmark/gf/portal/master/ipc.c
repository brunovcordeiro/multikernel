/*
 * Copyright(C) 2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 */

#include <nanvix/pm.h>
#include <assert.h>
#include <stdio.h>
#include "master.h"

/* Interprocess communication. */
int infd;                            /* Input channels.  */
int outfd[NR_CCLUSTER];              /* Output channels. */
static mppa_pid_t pids[NR_CCLUSTER]; /* Processes IDs.   */

/*
 * Sends data.
 */
void data_send(int fd, void *data, size_t n)
{
	portal_write(fd, data, n);
}

/*
 * Receives data.
 */
void data_receive(int fd, int remote, void *data, size_t n)
{
	portal_allow(fd, remote);
	portal_read(fd, data, n);
}

/*
 * Spwans slave processes.
 */
void spawn_slaves(void)
{
    char arg0[4];   /* Argument 0. */
    char *args[2];  /* Arguments.  */

    /* Spawn slaves. */
    args[1] = NULL;
    for (int i = 0; i < nclusters; i++)
    {
        sprintf(arg0, "%d", i);
        args[0] = arg0;
        pids[i] = mppa_spawn(i, NULL, "gf-portal-slave", (const char **)args, NULL);
        assert(pids[i] != -1);
    }
}

/*
 * Joins slave processes.
 */
void join_slaves(void)
{
    for (int i = 0; i < nclusters; i++)
        mppa_waitpid(pids[i], NULL, 0);
}

/*
 * Open NoC connectors.
 */
void open_noc_connectors(void)
{
    char path[35];

    infd = portal_create("/io0");

    /* Open channels. */
    for (int i = 0; i < nclusters; i++)
    {
        sprintf(path,"/cpu%d", i);
        outfd[i] = portal_open(path);
    }
}

/*
 * Close NoC connectors.
 */
void close_noc_connectors(void)
{
    portal_unlink(infd);
    /* Close channels. */
    for (int i = 0; i < nclusters; i++)
        portal_close(outfd[i]);
}
