
#include "error.h"
#include "file.h"
#include "math.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

template<class T>
void
for_each_fprintf(FILE *file, char const *format, T values, uint size, char const *separator, char const *eol)
{
  uint i, j;
  
  j = size - 1;
  
  for (i = 0; i < j; ++i) {
    fprintf(file, format, values[i], separator);
  }
  fprintf(file, format, values[j], eol);
}

void
print_header(FILE *file, int size)
{
  int i, j;
  
  fprintf(file, "\\begin{tabular}{r|*{%d}{c|}}\n", size);
  fprintf(file, "\\multicolumn{1}{r}{$k$} &\n");
  for (i = 0, j = size-1; i < size; ++i) {
    fprintf(file, "\\multicolumn{1}{r}{%d}%s", i, ((i != j) ? " &\n" : " \\\\\n"));
  }
}

void
print_hline(FILE *file, int size)
{
  fprintf(file, "\\cline{2-%d}\n", size+1);
}

void
print_footer(FILE *file)
{
  fprintf(file, "\\end{tabular}\n");
}

#if 0
void
tensor_fwrite_extended_compressed_latex(FILE *file, tensor_t const *tensor, strategy::type_t strategy)
{
  uint                      l, m, n;
  int                       nnz;
  tensor_storage_extended_t *storage;
  char const                *name, *macro;
  
  debug("tensor_fwrite_extended_compressed_latex(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  storage = STORAGE_EXTENDED(tensor);
  l       = tensor->l;
  m       = tensor->m;
  n       = tensor->n;
  nnz     = tensor->nnz;
  name    = orientation_to_string(tensor->orientation);
  macro   = orientation_to_latex_macro(tensor->orientation);
  
  debug("tensor_fwrite_extended_compressed_latex: l=%d, m=%d, n=%d, nnz=%d, orientation='%s'.\n", 
	l, m, n, nnz, name);
  
  print_header(file, nnz);
  print_hline(file, storage->rn);
  fprintf(file, "$\\row_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%d%s", storage->RO, storage->rn, " & ", " \\\\\n");
  print_hline(file, storage->ckn);
  fprintf(file, "$\\ct_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%d%s", storage->CK, storage->ckn, " & ", " \\\\\n");
  print_hline(file, nnz);
  fprintf(file, "$\\val_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%g%s", tensor->values, nnz, " & ", " \\\\\n");
  print_hline(file, nnz);
  print_footer(file);
}
#endif

void
tensor_emit_latex(FILE *file, tensor_t const *tensor)
{
  debug("tensor_emit_latex(file=0x%x, tensor=0x%x)\n", file, tensor);
  debug("tensor_emit_latex: strategy='%s'\n", strategy_to_string(tensor->strategy));
}

