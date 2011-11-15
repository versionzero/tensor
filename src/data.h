
#ifndef _DATA_H_
#define _DATA_H_

namespace data {
  namespace partition {
    typedef enum {
      unknown,
      fiber,
      slice
    } type_t;
  }
}

char const* data_partition_to_string(data::partition::type_t partition);
data::partition::type_t string_to_data_partition(char const *name);
void print_data_partitions(char const *format);
void print_data_partitions_with_descriptions(char const *format);

#endif /* _DATA_H_ */

