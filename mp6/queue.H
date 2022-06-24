//#include<cstddef>
#ifndef queue
#define queue

#include "thread.H"


class ReadyQueue {
    private:
        Thread *t_head; // head of thread 
        ReadyQueue *next; // next pointer
        
    public:
        ReadyQueue() {
            t_head = NULL;
            next = NULL;
        }
        ReadyQueue(Thread *thread) {
            t_head = thread;
            next = NULL;
        }
    
        void enqueue(Thread *thread){
            if (t_head == NULL){
                t_head = thread;
            }
            else {
                if (next == NULL){
                    next = new ReadyQueue(thread);
                }
                else {
                    next->enqueue(thread);
                }
            }
        }
        
        Thread *dequeue() {
            if (t_head == NULL) {
                //Console::puts("Empty queue!");
                return NULL;
                // assert(false); // empty queue
            }
            
            Thread *tmp = t_head;
            
            if (next == NULL) {
                t_head = NULL;
            }
            else {
                t_head = next->t_head; // update head of thread
                ReadyQueue *nxt = next; 
                next = next->next; // update next of queue
                delete nxt;
            }
            
            return tmp;
        }
};

#endif
