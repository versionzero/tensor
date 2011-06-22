
#include "compatible.h"
#include "error.h"
#include "tensor.h"
#include "operation.h"
#include "arithmetic.h"
#include <stdio.h>
#include <stdlib.h>

typedef double (*operation_t)(double, double);

void
tensor_operation_inplace(tensor_t       *t1, 
			 tensor_t const *t2, 
			 operation_t    op,
			 char const     *name)
{
  //uint i, j, k;

  debug("tensor_operation_inplace(t1=0x%x, t2=0x%x, op=%d, name='%s')\n", 
	t1, t2, op, name);
  
  compatible(t1, t2);

#if 0
  for (k = 0; k < t1->l; ++k) {
    for (i = 0; i < t1->m; ++i) {
      for (j = 0; j < t1->n; ++j) {
	t1->data[k][i][j] = op(t1->data[k][i][j], t2->data[k][i][j]);
      }
    }
  }
#endif
}

tensor_t*
tensor_operation(tensor_t const *t1, 
		 tensor_t const *t2,
		 operation_t    op,
		 char const     *name)
{
  tensor_t *tr;
  
  debug("tensor_operation(t1=0x%x, t2=0x%x, op=%d, name='%s')\n", 
	t1, t2, op, name);
  
  compatible(t1, t2);
  tr = tensor_malloc_from_template(t1);
#if 0
  tensor_convert_inplace(tr, t1);
  tensor_operation_inplace(tr, t2, op, name);
#endif
  
  return tr;
}

#define MTX_OP(name)							\
  void tensor_##name##_inplace(tensor_t *t1, tensor_t const *t2)	\
  { tensor_operation_inplace(t1, t2, name, #name);	}		\
  tensor_t* tensor_##name(tensor_t const *t1, tensor_t const *t2)	\
  { return tensor_operation(t1, t2, name, #name); }

MTX_OP(add)
MTX_OP(subtract)

void
tensor_multiply_inplace(tensor_t *tr, tensor_t const *t1, tensor_t const *t2)
{
  //uint i, j, k;
  
  debug("tensor_multiply_inplace(tr=0x%x, t1=0x%x, t2=0x%x)\n", tr, t1, t2);
  
  compatible(t1, t2);
  compatible(tr, t1);
  
#if 0  
  for (i = 0; i < t1->m; ++i) {
    for (j = 0; j < t1->n; ++j) {
      tr->data[i][j] = 0.0;
      for (k = 0; k < t1->n; ++k) {
	tr->data[i][j] += t1->data[i][k] * t2->data[k][j];
      }
    }
  }
#endif
}

tensor_t*
tensor_multiply(tensor_t const *t1, tensor_t const *t2)
{
  tensor_t *tr;
  
  debug("tensor_multiply_inplace(t1=0x%x, t2=0x%x)\n", t1, t2);
  
  compatible(t1, t2);
  tr = tensor_malloc_from_template(t1);
  tensor_multiply_inplace(tr, t1, t2);
  
  return tr;
}
