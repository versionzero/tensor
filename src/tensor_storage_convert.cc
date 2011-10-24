
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "storage.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
convert_from_compressed_to_coordinate(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_compressed_to_coordinate(destination=0x%x, source=0x%x)\n", destination, source);
  
  tensor_storage_convert_from_compressed_to_coordinate(destination, source);
  
#if 0
  switch (destination->orientation) {
  case orientation::tube:
    tensor_storage_convert_from_compressed_to_coordinate(destination, source);
    break;
  case orientation::row:
  case orientation::column:
    tensor_storage_convert_from_compressed_to_coordinate(destination, source);
    break;
  case orientation::lateral:
  case orientation::horizontal:
  case orientation::frontal:
    tensor_storage_convert_from_compressed_slice_to_coordinate(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' (%d) is not currently supported.\n",
	orientation_to_string(destination->orientation), destination->orientation);
    break;
  }
#endif
}

void
convert_to_coordinate(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_coordinate(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::compressed:
  case strategy::slice:
    convert_from_compressed_to_coordinate(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
convert_from_coordinate_to_compressed(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_compressed(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    tensor_storage_convert_from_coordinate_to_compressed(destination, source);
    break;
  case orientation::lateral:
  case orientation::horizontal:
  case orientation::frontal:
    tensor_storage_convert_from_coordinate_to_compressed_slice(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_from_coordinate_to_gundersen(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_gundersen(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::tube:
    tensor_storage_convert_from_coordinate_to_gundersen(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_from_coordinate_to_ekmr(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_ekmr(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    tensor_storage_convert_from_coordinate_to_ekmr(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_from_coordinate_to_zzekmr(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_zzekmr(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    tensor_storage_convert_from_coordinate_to_zzekmr(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_to_compressed(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_compressed(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_compressed(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
convert_to_gundersen(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_gundersen(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_gundersen(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
convert_to_ekmr(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_ekmr(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_ekmr(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
convert_to_zzekmr(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_zzekmr(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_zzekmr(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

void
tensor_storage_convert(tensor_t *destination, tensor_t *source)
{
  debug("tensor_storage_convert(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->strategy) {
  case strategy::coordinate:
    convert_to_coordinate(destination, source);
    break;
  case strategy::compressed:
  case strategy::slice:
    convert_to_compressed(destination, source);
    break;
  case strategy::gundersen:
    convert_to_gundersen(destination, source);
    break;
  case strategy::ekmr:
    convert_to_ekmr(destination, source);
    break;
  case strategy::zzekmr:
    convert_to_zzekmr(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

