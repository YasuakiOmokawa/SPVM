#ifndef SPVM_OBJECT_H
#define SPVM_OBJECT_H

#include "spvm_base.h"
#include "spvm_native.h"

enum {
  SPVM_OBJECT_C_CATEGORY_OBJECT,
  SPVM_OBJECT_C_CATEGORY_NUMERIC_ARRAY,
  SPVM_OBJECT_C_CATEGORY_OBJECT_ARRAY,
  SPVM_OBJECT_C_CATEGORY_ADDRESS_ARRAY,
  SPVM_OBJECT_C_CATEGORY_CALL_STACK,
};

// SPVM_OBJECT
struct SPVM_VALUE_object {
  SPVM_OBJECT* weaken_back_refs;
  int32_t ref_count;
  int32_t basic_type_id;
  int32_t units_length;
  int32_t weaken_back_refs_length;
  int16_t unit_byte_size;
  int16_t dimension;
  unsigned has_destructor : 1;
  unsigned in_destroy : 1;
  unsigned category : 3;
};

#endif
