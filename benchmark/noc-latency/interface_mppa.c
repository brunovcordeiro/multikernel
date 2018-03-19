#include <mppa/osconfig.h>
#include <nanvix/arch/mppa.h>
#include "interface_mppa.h"

/**************************************
 * PORTAL COMMUNICATION
 **************************************/

portal_t *mppa_create_read_portal (char *path, void* buffer, unsigned long size, int trigger) {
  portal_t *ret = (portal_t*) malloc (sizeof(portal_t));
  ret->file_descriptor = mppa_open(path, O_RDONLY);
  assert(ret->file_descriptor != -1);
  
  mppa_aiocb_ctor(&ret->aiocb, ret->file_descriptor, buffer, size);
  
  if (trigger > -1) {
    mppa_aiocb_set_trigger(&ret->aiocb, trigger);
  }
  
  assert(mppa_aio_read(&ret->aiocb) == 0);
  
  return ret;
}

portal_t *mppa_create_write_portal (char *path, void* buffer, unsigned long size, int receiver_rank) {		
  portal_t *ret = (portal_t*) malloc (sizeof(portal_t));
  ret->file_descriptor = mppa_open(path, O_WRONLY);
  assert(ret->file_descriptor != -1);
  
  // Tell mppa_io_write to wait for resources when sending a asynchronous message
  assert(mppa_ioctl(ret->file_descriptor, MPPA_TX_WAIT_RESOURCE_ON) == 0);
  
  // Select the DMA interface according to the receiver's rank.
  // This is only possible on the IO-node!
  if (arch_get_cluster_id() == 128)
    assert(mppa_ioctl(ret->file_descriptor, MPPA_TX_SET_IFACE, receiver_rank % 4) == 0);
  
  // We need to initialize an aiocb for asynchronous writes.
  // It seems that the buffer and buffer size parameters are not important here,
  // because we're going to specify them with mppa_aiocb_set_pwrite()
  // before calling mppa_aio_write()
  assert(mppa_aiocb_ctor(&ret->aiocb, ret->file_descriptor, buffer, size) == &ret->aiocb);
  
  return ret;
}

void mppa_async_read_wait_portal(portal_t *portal) {
  int status;
  status = mppa_aio_rearm(&portal->aiocb);
  assert(status != -1);
}

void mppa_async_write_wait_portal(portal_t *portal) {
  int status;
  while(mppa_aio_error(&portal->aiocb) == EINPROGRESS);
  status = mppa_aio_return(&portal->aiocb);
  assert(status != -1);
}

void mppa_close_portal (portal_t *portal) {
  assert(mppa_close(portal->file_descriptor) != -1);
  free(portal);
}

void mppa_write_portal(portal_t *portal, void *buffer, int size, int offset) {
  assert(mppa_pwrite(portal->file_descriptor, buffer, size, offset) == size);
}

void mppa_async_write_portal(portal_t *portal, void *buffer, int size, int offset) {
  mppa_aiocb_set_pwrite(&portal->aiocb, buffer, size, offset);
  assert(mppa_aio_write(&portal->aiocb) == 0);
}

/**************************************
 * BARRIER
 **************************************/

barrier_t *mppa_create_master_barrier (char *path_master, char *path_slave, int clusters) {
  int status, i;
  int ranks[clusters];
  long long match;
  
  barrier_t *ret = (barrier_t*) malloc (sizeof (barrier_t));
  
  ret->sync_fd_master = mppa_open(path_master, O_RDONLY);
  assert(ret->sync_fd_master != -1);
  
  ret->sync_fd_slave = mppa_open(path_slave, O_WRONLY);
  assert(ret->sync_fd_slave != -1);

  // set all bits to 1 except the less significative "cluster" bits (those ones are set to 0).
  // when the IO receives messagens from the clusters, they will set their correspoding bit to 1.
  // the mppa_read() on the IO will return when match = 11111...1111
  match = (long long) - (1 << clusters);
  status = mppa_ioctl(ret->sync_fd_master, MPPA_RX_SET_MATCH, match);
  assert(status == 0);
  
  for (i = 0; i < clusters; i++)
    ranks[i] = i;
  
  // configure the sync connector to receive message from "ranks"
  status = mppa_ioctl(ret->sync_fd_slave, MPPA_TX_SET_RX_RANKS, clusters, ranks);
  assert(status == 0);
  
  ret->mode = BARRIER_MASTER;
  
  return ret;
}

barrier_t *mppa_create_slave_barrier (char *path_master, char *path_slave) {
  int status;
  
  barrier_t *ret = (barrier_t*) malloc (sizeof (barrier_t));
  assert(ret != NULL);
  
  ret->sync_fd_master = mppa_open(path_master, O_WRONLY);
  assert(ret->sync_fd_master != -1);
  
  ret->sync_fd_slave = mppa_open(path_slave, O_RDONLY);
  assert(ret->sync_fd_slave != -1);
  
  // set match to 0000...000.
  // the IO will send a massage containing 1111...11111, so it will allow mppa_read() to return
  uint64_t mask = 0;
  status = mppa_ioctl(ret->sync_fd_slave, MPPA_RX_SET_MATCH, mask);
  assert(status == 0);
  
  ret->mode = BARRIER_SLAVE;
  
  return ret;
}

void mppa_barrier_wait(barrier_t *barrier) {
  int status;
  long long dummy;
  
  if(barrier->mode == BARRIER_MASTER) {
    dummy = -1;
    long long match;
    
    // the IO waits for a message from each of the clusters involved in the barrier
    // each cluster will set its correspoding bit on the IO (variable match) to 1
    // when match = 11111...1111 the following mppa_read() returns
    status = mppa_read(barrier->sync_fd_master, &match, sizeof(match));
    assert(status == sizeof(match));
    
    // the IO sends a message (dummy) containing 1111...1111 to all slaves involved in the barrier
    // this will unblock their mppa_read()
    status = mppa_write(barrier->sync_fd_slave, &dummy, sizeof(long long));
    assert(status == sizeof(long long));
  }
  else {
    dummy = 0;
    long long mask;

    // the cluster sets its corresponding bit to 1
    mask = 0;
    mask |= 1 << arch_get_cluster_id();
    
    // the cluster sends the mask to the IO
    status = mppa_write(barrier->sync_fd_master, &mask, sizeof(mask));
    assert(status == sizeof(mask));
    
    // the cluster waits for a message containing 1111...111 from the IO to unblock
    status = mppa_read(barrier->sync_fd_slave, &dummy, sizeof(long long));
    assert(status == sizeof(long long));
  }
}

void mppa_close_barrier (barrier_t *barrier) {
  assert(mppa_close(barrier->sync_fd_master) != -1);
  assert(mppa_close(barrier->sync_fd_slave) != -1);
  free(barrier);
}
