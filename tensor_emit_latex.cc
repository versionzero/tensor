
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
  
  for (i = 0, j = size-1; i < size; ++i) {
    fprintf(file, format, values[i], ((i != j) ? separator : eol));
  }
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

void
tensor_fwrite_compressed_latex(FILE *file, tensor_t const *tensor)
{
  uint                        l, m, n;
  int                         nnz, size;
  tensor_storage_compressed_t *storage;
  char const                  *name, *macro;
  
  debug("tensor_fwrite_compressed_latex(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  storage = STORAGE_COMPRESSED(tensor);
  l       = tensor->l;
  m       = tensor->m;
  n       = tensor->n;
  nnz     = tensor->nnz;
  size    = storage->size;
  name    = orientation_to_string(tensor->orientation);
  macro   = orientation_to_latex_macro(tensor->orientation);
  
  debug("tensor_fwrite_compressed_latex: l=%d, m=%d, n=%d, nnz=%d, orientation='%s', macro='%s', size=%d.\n", 
	l, m, n, nnz, name, macro, size);
  
  print_header(file, nnz);
  print_hline(file, size);
  fprintf(file, "$\\row_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%d%s", storage->RO, size, " & ", " \\\\\n");
  print_hline(file, nnz);
  fprintf(file, "$\\col_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%d%s", storage->CO, nnz,  " & ", " \\\\\n");
  print_hline(file, nnz);
  fprintf(file, "$\\tube_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%d%s", storage->KO, nnz,  " & ", " \\\\\n");
  print_hline(file, nnz);
  fprintf(file, "$\\val_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%g%s", tensor->values, nnz,  " & ", " \\\\\n");
  print_hline(file, nnz);
  print_footer(file);
}

void
tensor_fwrite_extended_compressed_latex(FILE *file, tensor_t const *tensor, strategy::type_t strategy)
{
  uint                      l, m, n;
  int                       nnz, size;
  tensor_storage_extended_t *storage;
  char const                *name, *macro;
  
  debug("tensor_fwrite_extended_compressed_latex(file=0x%x, tensor=0x%x)\n", file, tensor);
  
  storage = STORAGE_EXTENDED(tensor);
  l       = tensor->l;
  m       = tensor->m;
  n       = tensor->n;
  nnz     = tensor->nnz;
  size    = storage->size;
  name    = orientation_to_string(tensor->orientation);
  macro   = orientation_to_latex_macro(tensor->orientation);
  
  debug("tensor_fwrite_extended_compressed_latex: l=%d, m=%d, n=%d, nnz=%d, orientation='%s', size=%d.\n", 
	l, m, n, nnz, name, size);
  
  print_header(file, nnz);
  print_hline(file, size);
  fprintf(file, "$\\row_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%d%s", storage->RO, size, " & ", " \\\\\n");
  print_hline(file, nnz);
  fprintf(file, "$\\ct_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%d%s", storage->CK, nnz, " & ", " \\\\\n");
  print_hline(file, nnz);
  fprintf(file, "$\\val_{\\%s}$ & ", macro);
  for_each_fprintf(file, "%g%s", tensor->values, nnz, " & ", " \\\\\n");
  print_hline(file, nnz);
  print_footer(file);
}

void
tensor_emit_latex(FILE *file, tensor_t const *tensor)
{
  debug("tensor_emit_latex(file=0x%x, tensor=0x%x)\n", file, tensor);
  debug("tensor_emit_latex: strategy='%s'\n", strategy_to_string(tensor->strategy));
  
  switch (tensor->strategy) {
  case strategy::compressed:
    tensor_fwrite_compressed_latex(file, tensor);
    break;
  case strategy::slice:
  case strategy::ekmr:
  case strategy::zzekmr:
    tensor_fwrite_extended_compressed_latex(file, tensor, tensor->strategy);
    break;
  default:
    die("Emitting LaTeX source for storage strategy '%d' is not supported.\n", 
	strategy_to_string(tensor->strategy));
  }
}

