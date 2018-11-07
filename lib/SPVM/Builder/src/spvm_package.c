#include "spvm_package.h"

#include "spvm_compiler_allocator.h"
#include "spvm_compiler.h"
#include "spvm_field.h"
#include "spvm_op.h"
#include "spvm_list.h"
#include "spvm_type.h"
#include "spvm_limit.h"
#include "spvm_hash.h"
#include "spvm_constant_pool.h"

const char* const SPVM_PACKAGE_C_CATEGORY_NAMES[] = {
  "class",
  "interface",
  "pointer",
  "value_t",
};

SPVM_PACKAGE* SPVM_PACKAGE_new(SPVM_COMPILER* compiler) {
  SPVM_PACKAGE* package = SPVM_COMPILER_ALLOCATOR_safe_malloc_zero(compiler, sizeof(SPVM_PACKAGE));
  
  // Fields
  package->fields = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->field_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  
  // Package variables
  package->package_vars = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->package_var_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);

  package->subs = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->sub_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->package_vars = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->package_var_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->object_field_indexes = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  
  package->info_package_var_ids = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->info_package_var_id_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->info_sub_ids = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->info_sub_id_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->info_field_ids = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->info_field_id_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->info_types = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->info_type_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->info_switch_infos = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->info_double_constants = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->info_double_constant_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->info_string_constants = SPVM_COMPILER_ALLOCATOR_alloc_list(compiler, 0);
  package->info_string_constant_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  
  package->constant_pool = SPVM_COMPILER_ALLOCATOR_alloc_constant_pool(compiler, 0);
  
  package->constant_pool_32bit_value_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  package->constant_pool_64bit_value_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, 0);
  
  return package;
}
