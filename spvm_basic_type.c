#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "spvm_compiler.h"
#include "spvm_basic_type.h"
#include "spvm_compiler_allocator.h"

const char* const SPVM_BASIC_TYPE_C_CATEGORY_NAMES[] = {
  "unknown",
  "void",
  "undef",
  "numeric",
  "any_object",
  "package",
};

const char* const SPVM_BASIC_TYPE_C_ID_NAMES[] = {
  "unknown",
  "void",
  "undef",
  "byte",
  "short",
  "int",
  "long",
  "float",
  "double",
  "Object",
  "String",
};

SPVM_BASIC_TYPE* SPVM_BASIC_TYPE_new(SPVM_COMPILER* compiler) {
  SPVM_BASIC_TYPE* basic_type = SPVM_COMPILER_ALLOCATOR_alloc_memory_pool(compiler, compiler->allocator, sizeof(SPVM_BASIC_TYPE));
  
  return basic_type;
}
