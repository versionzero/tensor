
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
    storage_convert_from_coordinate_to_compressed_inplace(destination, source);
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
    storage_convert_from_coordinate_to_ekmr_inplace(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_from_coordinate_to_pkmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_pkmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    storage_convert_from_coordinate_to_pkmr_inplace(destination, source);
    break;
  default:
    die("Conversion to orientation '%s' is not currently supported.\n",
	orientation_to_string(destination->orientation));
    break;
  }
}

void
convert_from_coordinate_to_zzpkmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_from_coordinate_to_zzpkmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
    storage_convert_from_coordinate_to_zzpkmr_inplace(destination, source);
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
	strategy_to_string(strategy::compressed));
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
	strategy_to_string(strategy::compressed));
    break;
  }
}

void
convert_to_pkmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_pkmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_pkmr_inplace(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(strategy::compressed));
    break;
  }
}

void
convert_to_zzpkmr_inplace(tensor_t *destination, tensor_t *source)
{
  debug("convert_to_zzpkmr_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::coordinate:
    convert_from_coordinate_to_zzpkmr_inplace(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(strategy::compressed));
    break;
  }
}

void
storage_convert_inplace(tensor_t *destination, tensor_t *source)
{
  debug("storage_convert_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (destination->strategy) {
  case strategy::compressed:
    convert_to_compressed_inplace(destination, source);
    break;
  case strategy::ekmr:
    convert_to_ekmr_inplace(destination, source);
    break;
  case strategy::pkmr:
    convert_to_pkmr_inplace(destination, source);
    break;
  case strategy::zzpkmr:
    convert_to_zzpkmr_inplace(destination, source);
    break;
  default:
    die("Conversion from '%s' strategy to '%s' is not currently supported.\n",
	strategy_to_string(source->strategy), 
	strategy_to_string(destination->strategy));
    break;
  }
}

