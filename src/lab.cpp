#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "lab.h"

#define UNUSED(x) (void)x

struct node
{
    void *data;
    struct node *next;
};

struct queue
{
    int capacity;
    int size;
    bool shutdown;
    pthread_mutex_t lock;
    pthread_cond_t empty;
    pthread_cond_t full;
    struct node *head;
    struct node *tail;
};


LAB_EXPORT queue_t queue_init(int capacity)
{
    queue_t q = (queue_t) malloc(sizeof(struct queue));
    node *temp = (node *)malloc(sizeof(node));
    temp->next=NULL;
    q->capacity = capacity;
    q->size = 1;
    q->head = q->tail = temp;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->empty, NULL);
    pthread_cond_init(&q->full, NULL);
    return q;
}

/**
 * Frees all memory and related data signals all waiting threads.
 * @param q a queue to free
 */
LAB_EXPORT void queue_destroy(queue_t q)
{
    pthread_mutex_lock(&q->lock);
    while(q->size !=0){
        dequeue(q);
    }
    pthread_cond_broadcast(&q->empty);
    pthread_cond_broadcast(&q->full);
    pthread_mutex_unlock(&q->lock);
    
}

/**
 * Adds an element to the back of the queue
 * @param q the queue
 * @param block if true calling thread will block if q is full
 * @param data the data to add
 */
LAB_EXPORT void enqueue(queue_t q, void *data)
{
    node *temp = (node *)malloc(sizeof(node));
    temp->data = data;
    temp->next = NULL;
    //fprintf(stderr, "enqueue 1\n");
    pthread_mutex_lock(&q->lock);
    q->tail->next = temp;
    q->tail = temp;
    q->size = q->size+1;

    pthread_mutex_unlock(&q->lock);
    //fprintf(stderr, "enqueue 2");
    
}

/**
 * Removes the first element in the queue.
 * @param q the queue
 * @param block if true calling thread will block if q is empty
 */
LAB_EXPORT void *dequeue(queue_t q)
{
   // fprintf(stderr, "dequeue 1\n");
    pthread_mutex_lock(&q->lock);
    node *temp = q->head;
    node *newNode = temp->next;
    if(newNode == NULL){
        pthread_mutex_unlock(&q->lock);
      
    }
   // fprintf(stderr, "dequeue 2");
    q->head = newNode; 
    q->size = q->size-1;
    pthread_mutex_unlock(&q->lock);
    free(temp);

   
   // fprintf(stderr, "dequeue 3");
    return q;
}


LAB_EXPORT bool is_empty(queue_t q)
{
    pthread_mutex_lock(&q->lock);
    return q->size==0;
    pthread_mutex_unlock(&q->lock);
    
}


LAB_EXPORT void queue_shutdown(queue_t q)
{
    pthread_mutex_lock(&q->lock);
    q->shutdown=true;
    pthread_cond_broadcast(&q->empty);
    pthread_cond_broadcast(&q->full);
    pthread_mutex_unlock(&q->lock);
 
}