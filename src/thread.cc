/***********************************************************************
 * pt.c -- thread utility routines
 *
 * Author: Mark Hays <hays@math.arizona.edu>
 */

#include "thread.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>	/* for EBUSY */

static char const *map_thread_partition_to_string[] = { 
  "unknown",
  "tube",
  "slice"
};

char const*
thread_partition_to_string(thread::partition::type_t partition)
{
  return map_thread_partition_to_string[partition];
}

thread::partition::type_t
string_to_thread_partition(char const *name)
{
  uint i;
  
  for (i = 0; i < COUNT_OF(map_thread_partition_to_string); ++i) {
    if (0 == strcmp(name, map_thread_partition_to_string[i])) {
      return (thread::partition::type_t) i;
    }
  }
  
  return thread::partition::unknown;
}

/*************************************************
 * attempt to lock a mutex
 */
int thread_mutex_trylock(pthread_mutex_t *m)
{
  int res;

  /* returns EBUSY if mutex is already locked,
   * and EINVAL if the ptr is bad (on RedHat5.0)
   *
   * might this return EAGAIN on some systems??
   * i can't find any docs on this one's retval!
   *
   */
  if ((res=pthread_mutex_trylock(m)) != EBUSY) {
    THREAD_DIE("thread_mutex_trylock",res);
  }
  return(res ? 1 : 0);
}

void 
thread_wait(pthread_t *thread, thread_address_t exitcode)
{
  thread_address_t code, p;
  int errcode; 
  
  p = (thread_address_t) ((exitcode)==NULL ? &code : exitcode);
  if ((errcode=pthread_join(*thread, &p))) { 
    THREAD_DIE("thread_wait", errcode);
  }
}

/*************************************************
 * run nthreads threads in the routine start
 */
void _thread_fork(int nthreads,
		  thread_function_t start,
		  thread_address_t arg,
		  thread_address_t *exitcodes,
		  int setaffinity)
{
  int i;
  thread_argument_t *args;
  thread_address_t *address;
  
  if (nthreads<1) {
    die("thread_mutex_trylock: nthreads<1\n");
  }
  if ((args=(thread_argument_t *) malloc(nthreads*sizeof(thread_argument_t)))==NULL) {
    die("thread_fork: malloc failed!\n");
  }
  for (i=0; i<nthreads; i++) {
    args[i].nthreads=nthreads; args[i].myid=i; args[i].data=arg;
  }
  for (i=0; i<nthreads; i++) {
    thread_create(&args[i].self,start,args+i);
  }
  for (i=0; i<nthreads; i++) {
    address = (exitcodes==NULL?NULL:exitcodes+i);
    thread_wait(&args[i].self,address);
  }
  free(args);
}

#if 0
void _thread_fork(int nthreads,
		  thread_function_t start,
		  thread_address_t arg,
		  thread_address_t *exitcodes,
		  int setaffinity)
{
  int i;
  thread_argument_t *args;
  thread_address_t *address;
  pthread_attr_t attr, *pattr;
#ifdef __linux__
  cpu_set_t mask;
#endif
  
  if (nthreads<1) {
    die("thread_mutex_trylock: nthreads<1\n");
  }
  if ((args=(thread_argument_t *) malloc(nthreads*sizeof(thread_argument_t)))==NULL) {
    die("thread_fork: malloc failed!\n");
  }
  for (i=0; i<nthreads; i++) {
    args[i].nthreads=nthreads; args[i].myid=i; args[i].data=arg;
  }
  pthread_attr_init(&attr);
  for (i=0; i<nthreads; i++) {
    pattr = NULL;
#if 0
    /* for this to work correctly, we need to detect the number of
       CPUs */
#ifdef __linux__
    if (setaffinity) {
      CPU_ZERO(&mask);
      CPU_SET(i%thread_count,&mask);
      pthread_attr_setaffinity_np(&attr,sizeof(mask),&mask);
      pattr = &attr;
#endif
    }
    thread_create_with_attr(&args[i].self,pattr,start,args+i);
#endif
  }
  pthread_attr_destroy(&attr);
  for (i=0; i<nthreads; i++) {
    address = (exitcodes==NULL?NULL:exitcodes+i);
    thread_wait(&args[i].self,address);
  }
  free(args);
}
#endif

/*************************************************
 * initialize a gate
 */
void thread_gate_init(thread_gate_t *gate,int nthreads)
{
  gate->ngate=0; gate->nthreads=nthreads;
  thread_mutex_init(  &gate->mutex);
  thread_mutex_init(  &gate->block);
  thread_cond_init (&gate->condvar);
  thread_cond_init (   &gate->last);
}

/*************************************************
 * destroy a gate variable
 */
void thread_gate_destroy(thread_gate_t *gate)
{
  gate->ngate=gate->nthreads=0;
  thread_mutex_destroy(  &gate->mutex);
  thread_mutex_destroy(  &gate->block);
  thread_cond_destroy (&gate->condvar);
  thread_cond_destroy (   &gate->last);
}

/*************************************************
 * enter the gate
 */
void thread_gate_sync(thread_gate_t *gate)
{
  if (gate->nthreads<2) return;           /* trivial case            */
  thread_mutex_lock(&gate->block);             /* lock the block -- new
					      threads sleep here  */
  thread_mutex_lock(&gate->mutex);             /* lock the mutex          */
  if (++(gate->ngate) < gate->nthreads) { /* are we the last one in? */
    thread_mutex_unlock(&gate->block);         /* no, unlock block and    */
    thread_cond_wait(&gate->condvar,          /*   go to sleep           */
		 &gate->mutex);
  } else {                                /* yes, we're last         */
    thread_cond_broadcast(&gate->condvar);     /* wake everyone up and    */
    thread_cond_wait(&gate->last,&gate->mutex); /* go to sleep til they're
					       all awake... then  */
    thread_mutex_unlock(&gate->block);         /* release the block       */
  }
  if (--(gate->ngate)==1) {               /* next to last one out?   */
    thread_cond_broadcast(&gate->last);        /* yes, wake up last one   */
  }
  thread_mutex_unlock(&gate->mutex);           /* release the mutex       */
}

/*************************************************
 * Pipeline stage: the idea:
 *
 *   main thread   I/O thread
 *           \      /      \
 *            \    /        \
 *            gate1         |
 *           /     \        |
 *          /       \       |
 *       setup       |     work
 *          \       /       |
 *           \     /        |
 *            gate2         |
 *           /    \         /
 *          /      \_______/
 *         |
 *   main continues
 */

/*************************************************
 * couple of convenient macros
 */
#define GATE1(pipeline) thread_gate_sync(&((pipeline)->gate1))
#define GATE2(pipeline) thread_gate_sync(&((pipeline)->gate2))
#define STAGE(pipeline) (*((pipeline)->stageproc))((pipeline)->gdata)
#define SETUP(pipeline) \
  { thread_function_t fp; \
    \
    if ((fp=(pipeline)->setupproc)!=NULL) (*fp)(pipeline->gdata); \
  }

/*************************************************
 * slave thread executes this
 */
static void _thread_pipeline_slave_code(thread_pipeline_t *pipeline)
{
  while (1) {
    GATE1(pipeline);
    if (pipeline->terminate) break;
    GATE2(pipeline);
    STAGE(pipeline);
  }
  thread_exit(NULL);
}

/*************************************************
 * init the info struct and start up the slave
 */
void _thread_pipeline_init(thread_pipeline_t *pipeline,
		       thread_address_t gdata,
		       thread_function_t setupproc,
		       thread_function_t stageproc)
{
  thread_gate_init(&(pipeline->gate1),2);
  thread_gate_init(&(pipeline->gate2),2);
  pipeline->terminate=0;
  pipeline->gdata=gdata;
  pipeline->setupproc=setupproc;
  pipeline->stageproc=stageproc;
  thread_create(&(pipeline->slave),_thread_pipeline_slave_code,pipeline);
}

/*************************************************
 * kill the slave, free resources
 */
void thread_pipeline_destroy(thread_pipeline_t *pipeline)
{
  pipeline->terminate=1;
  GATE1(pipeline);
  thread_wait(&(pipeline->slave),NULL);
  thread_gate_destroy(&(pipeline->gate1));
  thread_gate_destroy(&(pipeline->gate2));
  pipeline->gdata=NULL;
  pipeline->setupproc=NULL;
  pipeline->stageproc=NULL;
}

/*************************************************
 * run the pipeline stage
 */
void thread_pipeline_execute(thread_pipeline_t *pipeline)
{
  GATE1(pipeline);
  SETUP(pipeline);
  GATE2(pipeline);
}

/* EOF pt.c */

