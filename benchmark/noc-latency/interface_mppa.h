#ifndef __INTERFACE_MPPA_H
#define __INTERFACE_MPPA_H

#include <mppaipc.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * GLOBAL CONSTANTS
 */

#define BARRIER_SYNC_MASTER "/mppa/sync/128:101"
#define BARRIER_SYNC_SLAVE "/mppa/sync/[0..15]:102"

#define TRUE 1
#define FALSE 0

#define IO_NODE_RANK 128
#define MAX_CLUSTERS 16
#define MAX_THREADS_PER_CLUSTER 16
#define MPPA_FREQUENCY 400

/*
 * INTERNAL STRUCTURES
 */

typedef enum {
  BARRIER_MASTER,
  BARRIER_SLAVE
} barrier_mode_t;

typedef struct {
  int file_descriptor;
  mppa_aiocb_t aiocb;
} portal_t;

typedef struct {
  int sync_fd_master;
  int sync_fd_slave;
  barrier_mode_t mode;
  int nb_clusters;
} barrier_t;

/*
 * FUNCTIONS
 */

void set_path_name(char *path, char *template_path, int rx, int tag);

portal_t *mppa_create_read_portal (char *path, void* buffer, unsigned long buffer_size, int trigger);
portal_t *mppa_create_write_portal (char *path, void* buffer, unsigned long buffer_size, int receiver_rank);
void mppa_write_portal (portal_t *portal, void *buffer, int buffer_size, int offset);
void mppa_async_write_portal (portal_t *portal, void *buffer, int buffer_size, int offset);
void mppa_async_write_wait_portal(portal_t *portal);
void mppa_async_read_wait_portal(portal_t *portal);
void mppa_close_portal (portal_t *portal);

barrier_t *mppa_create_master_barrier (char *path_master, char *path_slave, int clusters);
barrier_t *mppa_create_slave_barrier (char *path_master, char *path_slave);
void mppa_barrier_wait (barrier_t *barrier);
void mppa_close_barrier (barrier_t *barrier);

#endif // __INTERFACE_MPPA_H
