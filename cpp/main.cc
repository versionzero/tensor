
#include "tensor.h"

int
main() {
  coordinate_tensor<double> *p = new coordinate_tensor<double>();
  delete p;
  return 0;
}
