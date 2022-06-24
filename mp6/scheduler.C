/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  
  queueSize = 0;
  // assert(false);
  this->blockingDisk = NULL; // initializing blocking disk
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  if (Machine::interrupts_enabled()) {
    Machine::disable_interrupts();
  }
  if (blockingDisk != NULL && blockingDisk->is_ready() && blockingDisk->queueSize_disk != 0) {
    Thread *curr_disk_thread = blockingDisk->queue_disk->dequeue();
    blockingDisk->queueSize_disk = blockingDisk->queueSize_disk - 1;
    Thread::dispatch_to(curr_disk_thread);
  } // condition to yield when blockingDisk is ready
  else {
    assert(queueSize > 0);
  
    Thread *t = readyQueue.dequeue(); // removing the first-in thread from ready queue
    queueSize = queueSize - 1; // updating queue size
    if (!Machine::interrupts_enabled()) {
      Machine::enable_interrupts();
    }
    Thread::dispatch_to(t); // give cpu to the thread
  // assert(false);
  }
} 

void Scheduler::resume(Thread * _thread) {
  if (Machine::interrupts_enabled()) {
    Machine::disable_interrupts();
  }
  readyQueue.enqueue(_thread); // adding thread to ready queue
  queueSize = queueSize + 1; // update queue size
  if (!Machine::interrupts_enabled()) {
    Machine::enable_interrupts();
  }
  // assert(false);
}

void Scheduler::add(Thread * _thread) {
  if (Machine::interrupts_enabled()) {
    Machine::disable_interrupts();
  }
  Scheduler::resume(_thread); // add has same functionality as resume
  if (!Machine::interrupts_enabled()) {
    Machine::enable_interrupts();
  }
  // assert(false);
}

void Scheduler::terminate(Thread * _thread) {
  if (Machine::interrupts_enabled()) {
    Machine::disable_interrupts();
  }
  int pos = 0;
  while (pos < queueSize) {
    Thread *top = readyQueue.dequeue(); // retrieve the top thread by dequeuing
    if (top->ThreadId() == _thread->ThreadId()) {
      queueSize = queueSize - 1; // if the top thread is the thread to terminate, reduce size, i.e, terminate the thread by removing it from the ready queue
    }
    else {
      readyQueue.enqueue(top); // if top thread is not thread to terminate, enqueue back into the rready queue.
    }
    pos = pos + 1;
  }
  if (!Machine::interrupts_enabled()) {
    Machine::enable_interrupts();
  }
  // assert(false);
}

void Scheduler::add_disk(BlockingDisk * disk) { // add the blocking disk to scheduler
  blockingDisk = disk;
}
