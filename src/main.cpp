
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <lab.h>

#define UNUSED(x) (void)x
#define MAX_C 8           /* Maximum number of consumer threads */
#define MAX_P 8           /* Maximum number of producer threads */
#define MAX_SLEEP 1000000 /* maximum time a thread can sleep in nanoseconds*/

/*Track the total items produced and consumed*/
static struct
{
     unsigned int num;
     pthread_mutex_t lock;
} numproduced = {0, PTHREAD_MUTEX_INITIALIZER},
  numconsumed = {0, PTHREAD_MUTEX_INITIALIZER};

/*Shared queue that producers and consumers will access*/
static queue_t pc_queue;

/**
 * Produces items at a random interval. Exits once it has produced
 * the correct number of items.
 */
static void *producer(void *args)
{

     int num = *((int *)args);
     pthread_t tid = pthread_self();
     unsigned int seedp = 0;
     struct timespec s = {0, 0};
     int *itm = NULL;

     //fprintf(stderr, "Producer thread: %ld - producing %d items\n", tid, num);
     for (int i = 0; i < num; i++)
     {
          /*simulate producing the item*/
          s.tv_nsec = (rand_r(&seedp) % MAX_SLEEP);
          nanosleep(&s, NULL);
          itm = (int*)malloc(sizeof(int));
          *itm = i;
          //Put the item into the queue
          enqueue(pc_queue, itm);

          //Update counters for testing purposes
          pthread_mutex_lock(&numproduced.lock);
          numproduced.num++;
          pthread_mutex_unlock(&numproduced.lock);
     }
     //fprintf(stderr, "Producer thread: %ld - Done producing!\n", tid);
     pthread_exit(NULL);
}

/**
 * Consumes items.
 */
static void *consumer(void *args)
{
     UNUSED(args);
     pthread_t tid = pthread_self();
     unsigned int seedp = 0;
     struct timespec s = {0, 0};
     int *itm = NULL;
     //fprintf(stderr, "Consumer thread: %ld\n", tid);

     while (true)
     {
          /*simulate producing the item*/
          s.tv_nsec = (rand_r(&seedp) % MAX_SLEEP);
          nanosleep(&s, NULL);

          itm = (int *)dequeue(pc_queue);
          if (itm)
          {
               free(itm);
               itm = NULL;
               //Update counters for testing purposes
               pthread_mutex_lock(&numconsumed.lock);
               numconsumed.num++;
               pthread_mutex_unlock(&numconsumed.lock);
          }
          else
          {
               //If the queue is implemented correctly we should not
               //get a NULL item. This is for testing purposes only
               //in production we may allow nulls depending on the 
               //context
               fprintf(stderr, "Got a null item exiting consumer!\n");
               break;
          }
          
     }
     //fprintf(stderr, "Consumer Thread: %ld - Done consuming!\n", tid);
     pthread_exit(NULL);
}

static void usage(char *n)
{
     fprintf(stderr, "Usage: %s [-c num consumer] [-p num producer] [-i num items] [-s queue size]\n", n);
     exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
     int nump = 1;       /*total number of producers*/
     int numc = 1;       /*total number of consumers*/
     int numitems = 10;  /*total number of items to produce per thread*/
     int queue_size = 5; /*The default size of the queue*/
     int c;

     pthread_t producers[MAX_P];
     pthread_t consumers[MAX_C];

     while ((c = getopt(argc, argv, "c:p:i:s:h")) != -1)
          switch (c)
          {
          case 'c':
               numc = atoi(optarg);
               break;
          case 'p':
               nump = atoi(optarg);
               ;
               break;
          case 'i':
               numitems = atoi(optarg);
               break;
          case 's':
               queue_size = atoi(optarg);
               break;
          case 'h':
               usage(argv[0]);
               break;
          default: /* ? */
               usage(argv[0]);
          }
     if (numc > MAX_C)
          numc = MAX_C;
     if (nump > MAX_P)
          nump = MAX_P;

     //Initialize the queue for usage
     pc_queue = queue_init(queue_size);

     fprintf(stderr, "Creating %d producers each producing %d items for a total of %d\n", nump, numitems, nump * numitems);
     /*Create the producer threads*/
     for (int i = 0; i < nump; i++)
     {
          pthread_create(&producers[i], NULL, producer, (void *)&numitems);
     }

     fprintf(stderr, "Creating %d consumer threads\n", numc);
     /*Create the consumer threads*/
     for (int i = 0; i < numc; i++)
     {
          pthread_create(&consumers[i], NULL, consumer, (void *)NULL);
     }

     /*Wait for all the the producer threads to finish*/
     for (int i = 0; i < nump; i++)
     {
          pthread_join(producers[i], NULL);
     }

     //Once all the producers are finished we set a flag so the consumer thread can finish up
     //Once shutdown is called your queue should drain all remaining items and be read for
     //destruction!
     queue_shutdown(pc_queue);

     /*Wait for all the the consumer threads to finish*/
     for (int i = 0; i < numc; i++)
     {
          pthread_join(consumers[i], NULL);
     }



     if (numproduced.num != numconsumed.num)
     {
          fprintf(stderr, "ERROR! produced != consumed\n");
     }
     fprintf(stderr, "Queue is empty:%s\n", is_empty(pc_queue) ? "true" : "false");
     fprintf(stderr, "Total produced:%d\n", numproduced.num);
     fprintf(stderr, "Total consumed:%d\n", numconsumed.num);

     //Free up all the stuff we allocated
     queue_destroy(pc_queue);

     return 0;
}
