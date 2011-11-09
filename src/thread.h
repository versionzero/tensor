/***********************************************************************
 * thread.h, based on:
 * pt.h -- pthreads utility macros
 *
 * Author: Mark Hays <hays@math.arizona.edu>
 */

#ifndef _THREAD_H_
#define _THREAD_H_

/* Linux defs:
 *   _REENTRANT to get thread-safe libs
 *   _POSIX_SOURCE to get POSIX semantics
 *   _P is a hack for LinuxThreads -- on my box,
 *      pthread.h includes sched.h. My sched.h
 *      (incorrectly) declares prototypes with
 *      _P instead of __P (which is what everything
 *      else uses... Maybe it's just me.
 */
#ifdef __linux__
#  define _REENTRANT
#  define _POSIX_SOURCE
#  define _P __P
#endif

#include <pthread.h>
#include <string.h>
#include "error.h"

typedef void *(*thread_function_t)(void *);
typedef void *thread_address_t;

#define THREAD_DIE(func,errcode)					\
  die("%s:%d: %s: %s.\n",__FILE__,__LINE__,func,strerror(errcode));

/*************************************************
 * low level wrappers that die on errors
 */
#define thread_create(t,start,arg) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_create(t, \
			       NULL, \
			       (thread_function_t) (start), \
			       (thread_address_t) (arg)))) { \
      THREAD_DIE("thread_create", errcode);		    \
    }							    \
}

#define thread_create_detached(start,arg) \
{ \
    pthread_t t; \
    int errcode; \
    \
    if ((errcode=pthread_create(&t, \
			       NULL, \
			       (thread_function_t) (start), \
			       (thread_address_t) (arg)))) { \
      THREAD_DIE("thread_create_detached", errcode);		    \
    }							    \
    if (pthread_detach(t)) {				    \
      THREAD_DIE("thread_create_detached", errcode);		    \
    }							    \
}

void thread_wait(pthread_t thread, thread_address_t exitcode);

#if 0
#define thread_wait(t,exitcode) \
{ \
    thread_address_t code; \
    int errcode; \
    \
    if ((errcode=pthread_join(*(t), \
		      (thread_address_t) ((exitcode)==NULL ? &code : (exitcode))))) { \
      THREAD_DIE("thread_wait", errcode);				\
    }									\
}
#endif

#define thread_exit(status) \
{ \
    pthread_exit(status); \
}

#define thread_mutex_init(m) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_mutex_init(m,NULL))) { \
      THREAD_DIE("thread_mutex_init", errcode);   \
    }					      \
}

#define thread_mutex_destroy(m) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_mutex_destroy(m))) { \
        THREAD_DIE("thread_mutex_destroy", errcode); \
    }					      \
}

#define thread_cond_init(c) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_cond_init(c,NULL))) { \
        THREAD_DIE("thread_cond_init", errcode); \
    }					      \
}

#define thread_cond_destroy(c) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_cond_destroy(c))) { \
        THREAD_DIE("thread_cond_destroy", errcode); \
    }					      \
}

#define thread_mutex_lock(m) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_mutex_lock(m))) { \
        THREAD_DIE("thread_mutex_lock", errcode); \
    }					       \
}

/* This one has to do some extra checking so it
 * isn't a macro...
 */
extern int thread_mutex_trylock(pthread_mutex_t *m, char *msg);

#define thread_mutex_unlock(m) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_mutex_unlock(m))) { \
        THREAD_DIE("thread_mutex_unlock", errcode); \
    }					      \
}

#define thread_cond_wait(c,m) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_cond_wait(c,m))) { \
        THREAD_DIE("thread_cond_wait", errcode); \
    }					      \
}

#define thread_cond_broadcast(c) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_cond_broadcast(c))) { \
        THREAD_DIE("thread_cond_broadcast", errcode); \
    }					      \
}

#define thread_cond_signal(c) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_cond_signal(c))) { \
      THREAD_DIE("thread_cond_signal", errcode);	\
    }						\
}

/*************************************************
 * N threads simultaneously doing the same thing
 */

typedef struct _thread_argument_t_ {
  int myid;
  int nthreads;
  pthread_t self;
  thread_address_t data;
} thread_argument_t;

#define thread_myid(th_arg)      ((th_arg)->myid)
#define thread_nthreads(th_arg)  ((th_arg)->nthreads)
#define thread_data(th_arg)      ((th_arg)->data)
#define thread_self(th_arg)      (&((th_arg)->self))
#define thread_thread(th_arg,id) (&(((th_arg)-((th_arg)->myid)+(id))->self))

#define thread_cancel(th_arg,id) \
{ \
    int errcode; \
    \
    if ((errcode=pthread_cancel(((th_arg)-((th_arg)->myid)+(id))->self))) {	\
        THREAD_DIE("thread_cancel", errcode); \
    }					       \
}

#define thread_cancel_all(th_arg) \
{ \
    int myid=(th_arg)->myid,nt=(th_arg)->nthreads,i,errcode; \
    thread_argument_t *base=(th_arg)-myid; \
    \
    for (i=0; i<nt; i++) { \
        if (i==id) continue; \
        if ((errcode=pthread_cancel(base[i].self))) { \
            THREAD_DIE("thread_cancel_all", errcode); \
	}					   \
    } \
}

extern void _thread_fork(int nthreads,
		     thread_function_t start,
		     thread_address_t arg,
		     thread_address_t *exitcodes);

#define thread_fork(nt,start,arg,codes) \
  _thread_fork(nt,(thread_function_t) start, \
	   (thread_address_t) arg,(thread_address_t *) codes)

/*************************************************
 * the gate struct (rendezvous point)
 */
typedef struct _thread_gate_t_ {
  int ngate;              
  int nthreads;           
  pthread_mutex_t mutex;  
  pthread_mutex_t block;  
  pthread_cond_t condvar; 
  pthread_cond_t last;    
} thread_gate_t;

extern void thread_gate_init(thread_gate_t *gate,int nthreads);
extern void thread_gate_destroy(thread_gate_t *gate);
extern void thread_gate_sync(thread_gate_t *gate);

/*************************************************
 * the pipeline struct (a single stage)
 */
typedef struct _thread_pipeline_t_ {
  thread_address_t gdata;
  thread_function_t setupproc;
  thread_function_t stageproc;
  int terminate;
  pthread_t slave;
  thread_gate_t gate1;
  thread_gate_t gate2;
} thread_pipeline_t;

extern void _thread_pipeline_init(thread_pipeline_t *p,
			      thread_address_t gdata,
			      thread_function_t setup,
			      thread_function_t stage);

#define thread_pipeline_init(p,gdata,setup,stage)  \
  _thread_pipeline_init(p,                         \
		    (thread_address_t) gdata,         \
		    (thread_function_t) setup, \
		    (thread_function_t) stage)

extern void thread_pipeline_destroy(thread_pipeline_t *p);
extern void thread_pipeline_execute(thread_pipeline_t *p);

/* names of C functions that can be called from
 * FORTRAN without the trailing underscore(s)
 */

#ifdef __linux__
   /* This works on my LinuxThreads + g77-0.5.21 */
#  define F77PIPELINEINIT pipeline_init__
#  define F77PIPELINEDONE pipeline_done__
#  define F77PIPELINEEXEC pipeline_execute__
#else
   /* This seems to work everywhere else */
#  define F77PIPELINEINIT pipeline_init_
#  define F77PIPELINEDONE pipeline_done_
#  define F77PIPELINEEXEC pipeline_execute_
#endif

#endif /* _THREAD_H_ */

/* EOF pt.h */

