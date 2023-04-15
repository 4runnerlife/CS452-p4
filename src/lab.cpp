#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "lab.h"

#define UNUSED(x) (void)x

/**
 * opaque type definition for a queue
 */
typedef struct queue* queue_t;

/**
 * Initialize a new queue
 * @param capacity the maximum capacity of the queue
 * @return A fully initialized queue
 */
LAB_EXPORT queue_t queue_init(int capacity){
    UNUSED(capacity);
    return 0;

}

/**
 * Frees all memory and related data signals all waiting threads.
 * @param q a queue to free
 */
LAB_EXPORT void queue_destroy(queue_t q){
    UNUSED(q);
}

/**
 * Adds an element to the back of the queue 
 * @param q the queue
 * @param block if true calling thread will block if q is full
 * @param data the data to add
 */
LAB_EXPORT void enqueue(queue_t q, void *data){
    UNUSED(q);
    UNUSED(data);
}


/**
 * Removes the first element in the queue.
 * @param q the queue 
 * @param block if true calling thread will block if q is empty
 */
LAB_EXPORT void * dequeue(queue_t q){
    UNUSED(q);
    return 0;
}

/**
 * Returns true is the queue is empty
 * @param q the queue
 */
LAB_EXPORT bool is_empty(queue_t q){
    UNUSED(q);
    return true;
}

/**
 * Set the shutdown flag in the queue so all threads can 
 * complete and exit properly
 */
LAB_EXPORT void queue_shutdown(queue_t q){
    UNUSED(q);
}