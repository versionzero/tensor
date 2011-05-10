
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

uint        master_threshold;
extern uint worker_threshold;
char        batch_system_type[10];
extern bool verbose;
static char outputs[8][256];
static char intermediates[8][256];
static char script[256];
static char redirect[256];
static uint unique_id = 0;
static uint block;

#define NUM_BLOCKS 8

void
makeflow_init()
{
  uint i;
  char cwd[256];

  if (verbose) {
    strcpy(redirect, "1>&2");
  } else {
    strcpy(redirect, "1>/dev/null");
  }
  
  for (i = 1; i < 8; ++i) {    
    strcpy(outputs[i], "");
    strcpy(intermediates[i], "");
  }
  
  sprintf(script, "%s/strassen.makeflow", getcwd(cwd, 256));
}

void
makeflow_cleanup()
{
  uint i;
  char command[256];

  for (i = 1; i < 8; ++i) {    
    sprintf(command, "/bin/rm -f %s %s", intermediates[i], redirect);
    system(command);    
  }

  sprintf(command, "/bin/rm -f %s* %s", script, redirect);
  system(command);

  system("/bin/rm -f condor.sh condor.submit");
}

void
makeflow_status()
{
  uint       i;
  char       command[256], extra[256];
  char const extension[][20] = { "condorlog", "makeflowlog" };
  
  for (i = 0; i < 2; ++i) {
    sprintf(extra, "%s.%s %s", script, extension[i], redirect);
    sprintf(command, "echo %s; cat %s", extra, extra);
    system(command);
  }
}

void
makeflow_run(matrix_t *p[NUM_BLOCKS])
{
  uint i;
  char command[256];
  
  sprintf(command, "makeflow -T %s %s %s", 
	  batch_system_type, script, redirect);
  system(command);
  
  for (i = 1; i < 8; ++i) {    
    p[i] = matrix_read(outputs[i]);
  }
  
  makeflow_status();
  makeflow_cleanup();
}

void
makeflow_target_fwrite(FILE           *f,
		       matrix_t const *m1,
		       matrix_t const *m2)
{
  char filenames[3][256];
  char executable[256], cwd[256];
  
  sprintf(filenames[1],   "file%d.mtx",  unique_id++);
  sprintf(filenames[2],   "file%d.mtx",  unique_id++);
  sprintf(outputs[block], "file%d.mtx",  unique_id++);
  sprintf(executable,     "%s/multiply", getcwd(cwd, 256));
  
  matrix_write(filenames[1], m1);
  matrix_write(filenames[2], m2);

  fprintf(f, "%s: %s %s\n", outputs[block], filenames[1], filenames[2]);
  fprintf(f, "\t%s %s -o3 -t%d %s %s %s\n\n", 
	  executable, (verbose ? "-v" : ""), worker_threshold, 
	  filenames[1], filenames[2], outputs[block]);
  
  sprintf(intermediates[block], "%s %s %s", 
	  filenames[1], filenames[2], outputs[block]);
}

void
makeflow_target_write(matrix_t const *m1,
		      matrix_t const *m2)
{
  FILE *f;
  
  f = fopen_or_die(script, "a+");
  makeflow_target_fwrite(f, m1, m2);
  fclose(f);
}

matrix_t*
makeflow_distribute_matrix_multiply_strassen(matrix_t const *m1,
					     matrix_t const *m2,
					     uint           n)
{
  uint     i, j;
  uint     m, u, v;
  matrix_t *c, *a[3][3], *b[3][3], *p[NUM_BLOCKS], *t1, *t2;
    
  c = NULL;
  
  if (n <= master_threshold) {
    
    makeflow_target_write(m1, m2);
    
  } else {
    
    if (NULL == (c = matrix_new(m1->m, m1->n))) {
      error(D_ERROR|D_FATAL, "Failed to allocate output matrix.\n");
    }
    
    m = n/2;
    u = 0;
    v = m;
    
    a[1][1] = matrix_partition(m1, u, u, m, m);
    a[1][2] = matrix_partition(m1, u, v, m, n);
    a[2][1] = matrix_partition(m1, v, u, n, m);
    a[2][2] = matrix_partition(m1, v, v, n, n);
    
    b[1][1] = matrix_partition(m2, u, u, m, m);
    b[1][2] = matrix_partition(m2, u, v, m, n);
    b[2][1] = matrix_partition(m2, v, u, n, m);
    b[2][2] = matrix_partition(m2, v, v, n, n);
    
    t1      = matrix_new(m, m);
    t2      = matrix_new(m, m);
    
    /* P_1  = (A_11 + A_22) * (B_11 + B_22); */
    block = 1;
    matrix_copy_inplace(t1, a[1][1]);
    matrix_add_inplace (t1, a[2][2]);
    matrix_copy_inplace(t2, b[1][1]);
    matrix_add_inplace (t2, b[2][2]);
    p[block] = makeflow_distribute_matrix_multiply_strassen(t1, t2, m);
    
    /* P_2  = (A_21 + A_22) * (B_11); */
    block = 2;
    matrix_copy_inplace(t1, a[2][1]);
    matrix_add_inplace (t1, a[2][2]);
    p[block] = makeflow_distribute_matrix_multiply_strassen(t1, b[1][1], m);

    /* P_3  = (A_11)        * (B_12 - B_22); */
    block = 3;
    matrix_copy_inplace    (t2, b[1][2]);
    matrix_subtract_inplace(t2, b[2][2]);
    p[block] = makeflow_distribute_matrix_multiply_strassen(a[1][1], t2, m);
    
    /* P_4  = (A_22)        * (B_21 - B_11); */
    block = 4;
    matrix_copy_inplace    (t2, b[2][1]);
    matrix_subtract_inplace(t2, b[1][1]);
    p[block] = makeflow_distribute_matrix_multiply_strassen(a[2][2], t2, m);
    
    /* P_5  = (A_11 + A_12) * (B_22); */
    block = 5;
    matrix_copy_inplace(t1, a[1][1]);
    matrix_add_inplace (t1, a[1][2]);
    p[block] = makeflow_distribute_matrix_multiply_strassen(t1, b[2][2], m);
    
    /* P_6  = (A_21 - A_11) * (B_11 + B_12); */
    block = 6;
    matrix_copy_inplace    (t1, a[2][1]);
    matrix_subtract_inplace(t1, a[1][1]);
    matrix_copy_inplace    (t2, b[1][1]);
    matrix_add_inplace     (t2, b[1][2]);
    p[block] = makeflow_distribute_matrix_multiply_strassen(t1, t2, m);
    
    /* P_7  = (A_12 - A_22) * (B_21 + B_22); */
    block = 7;
    matrix_copy_inplace    (t1, a[1][2]);
    matrix_subtract_inplace(t1, a[2][2]);
    matrix_copy_inplace    (t2, b[2][1]);
    matrix_add_inplace     (t2, b[2][2]);
    p[block] = makeflow_distribute_matrix_multiply_strassen(t1, t2, m);
    
    /* wait for child nodes to finish processing; then read the
       resulting matricies from the output files we are tracking. */
    if (n/2 <= master_threshold) {
      makeflow_run(p);
    }
    
    /* C_11 = P_1 + P_4 - P_5 + P_7; */
    matrix_copy_inplace    (a[1][1], p[1]);
    matrix_add_inplace     (a[1][1], p[4]);
    matrix_subtract_inplace(a[1][1], p[5]);
    matrix_add_inplace     (a[1][1], p[7]);
    
    /* C_12 = P_3 + P_5; */
    matrix_copy_inplace(a[1][2], p[3]);
    matrix_add_inplace (a[1][2], p[5]);
    
    /* C_21 = P_2 + P_4; */
    matrix_copy_inplace(a[2][1], p[2]);
    matrix_add_inplace (a[2][1], p[4]);
    
    /* C_22 = P_1 + P_3 - P_2 + P_6; */
    matrix_copy_inplace    (a[2][2], p[1]);
    matrix_add_inplace     (a[2][2], p[3]);
    matrix_subtract_inplace(a[2][2], p[2]);
    matrix_add_inplace     (a[2][2], p[6]);
    
    /* for all i, j, merge c[i][j] into c */
    matrix_copy_inplace_with_offset(c, 0, 0, a[1][1]);
    matrix_copy_inplace_with_offset(c, 0, m, a[1][2]);
    matrix_copy_inplace_with_offset(c, m, 0, a[2][1]);
    matrix_copy_inplace_with_offset(c, m, m, a[2][2]);
    
    matrix_delete(t1);
    matrix_delete(t2);
    for (i = 1; i < NUM_BLOCKS; ++i) {
      matrix_delete(p[i]);
    }
    for (i = 1; i <= 2; ++i) {
      for (j = 1; j <= 2; ++j) {
	matrix_delete(a[i][j]);
	matrix_delete(b[i][j]);
      }
    }
    
  }
  
  return c;
}

matrix_t*
matrix_distributed_multiply_strassen(matrix_t const *m1, matrix_t const *m2)
{
  matrix_supported(m1);
  matrix_compatible(m1, m2);  

  makeflow_init();
  makeflow_cleanup();

  return makeflow_distribute_matrix_multiply_strassen(m1, m2, m1->n);
}
