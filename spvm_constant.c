#include <string.h>


#include "spvm_compiler.h"
#include "spvm_constant.h"
#include "spvm_compiler_allocator.h"
#include "spvm_hash.h"
#include "spvm_type.h"

SPVM_CONSTANT* SPVM_CONSTANT_new(SPVM_COMPILER* compiler) {
  SPVM_CONSTANT* constant = SPVM_COMPILER_ALLOCATOR_alloc_memory_pool(compiler, sizeof(SPVM_CONSTANT));
  
  return constant;
}

SPVM_CONSTANT* SPVM_CONSTANT_create_int_1(SPVM_COMPILER* compiler) {
  
  SPVM_CONSTANT* constant = SPVM_CONSTANT_new(compiler);
  constant->value.int_value = 1;
  constant->type = SPVM_TYPE_get_int_type(compiler);
  
  return constant;
}
