/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"
#include "thread.H"

extern Scheduler * SYSTEM_SCHEDULER; // for scheduler
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
  queueSize_disk = 0;
  this->queue_disk = new ReadyQueue();
}

void BlockingDisk::wait_until_ready() {
  if(!BlockingDisk::is_ready()) {
    Thread * curr_thread = Thread::CurrentThread();
    this->queue_disk->enqueue(curr_thread);
    queueSize_disk = queueSize_disk + 1;
    SYSTEM_SCHEDULER->yield();
  }
}


bool BlockingDisk::is_ready() {
  return SimpleDisk::is_ready();
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  // SimpleDisk::read(_block_no, _buf);
  SimpleDisk::issue_operation(DISK_OPERATION::READ, _block_no);
  BlockingDisk::wait_until_ready();
  //similar to simple disk implementation
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = Machine::inportw(0x1F0);
    _buf[i*2]   = (unsigned char)tmpw;
    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
  }
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  // SimpleDisk::write(_block_no, _buf);
  SimpleDisk::issue_operation(DISK_OPERATION::WRITE, _block_no);
  BlockingDisk::wait_until_ready();
  // similar to simple disk implementation
  int i; 
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }
}
