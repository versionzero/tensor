
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
convert_from_coordinate_to_compressed_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_compressed_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    tensor_storage_convert_from_coordinate_to_compressed_inplace(destination, source);
    break;
  case orientation::lateral:
  case orientation::horizontal:
  case orientation::frontal:
    tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_from_coordinate_to_ekmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_ekmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    tensor_storage_convert_from_coordinate_to_ekmr_inplace(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_from_coordinate_to_zzekmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_zzekmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    tensor_storage_convert_from_coordinate_to_zzekmr_inplace(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_to_compressed_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_compressed_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_compressed_inplace(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
convert_to_ekmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_ekmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_ekmr_inplace(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
convert_to_zzekmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_zzekmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_zzekmr_inplace(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
tensor_storage_convert_inplace(tensor_t *destination, tensor_t *source)
{
  debug("tensor_storage_convert_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->strategy) {
  case strategy::compressed:
  case strategy::slice:
    convert_to_compressed_inplace(destination, source);
    break;
  case strategy::ekmr:
    convert_to_ekmr_inplace(destination, source);
    break;
  case strategy::zzekmr:
    convert_to_zzekmr_inplace(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

