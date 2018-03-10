#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "spvm_compiler.h"
#include "spvm_type.h"
#include "spvm_package.h"
#include "spvm_type.h"
#include "spvm_op.h"
#include "spvm_memory_pool.h"
#include "spvm_hash.h"
#include "spvm_list.h"
#include "spvm_util_allocator.h"
#include "spvm_compiler_allocator.h"
#include "spvm_yacc_util.h"
#include "spvm_list.h"
#include "spvm_opcode_array.h"
#include "spvm_sub.h"
#include "spvm_constant_pool.h"
#include "spvm_runtime.h"
#include "spvm_runtime_api.h"
#include "spvm_sub.h"
#include "spvm_field.h"
#include "spvm_api.h"
#include "spvm_opcode.h"

SPVM_RUNTIME* SPVM_COMPILER_new_runtime(SPVM_COMPILER* compiler) {
  
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_API_new_runtime();
  
  runtime->compiler = compiler;
  
  // Set global runtime
  SPVM_RUNTIME_API_set_runtime(runtime->api, runtime);
  
  // Initialize Package Variables
  runtime->package_vars = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(sizeof(SPVM_API_VALUE) * (compiler->package_var_length + 1));
  
  runtime->type_code_to_id_base = compiler->type_code_to_id_base;
  
  return runtime;
}

SPVM_COMPILER* SPVM_COMPILER_new() {
  SPVM_COMPILER* compiler = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(sizeof(SPVM_COMPILER));

  // Allocator
  compiler->allocator = SPVM_COMPILER_ALLOCATOR_new(compiler);
  
  // Parser information
  compiler->op_sub_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, compiler->allocator, 0);
  compiler->op_packages = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, compiler->allocator, 0);
  compiler->op_package_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, compiler->allocator, 0);
  compiler->op_our_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, compiler->allocator, 0);
  compiler->op_types = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, compiler->allocator, 0);
  compiler->op_use_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, compiler->allocator, 0);
  compiler->op_use_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, compiler->allocator, 0);
  compiler->include_pathes = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, compiler->allocator, 0);
  compiler->bufptr = "";
  compiler->types = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, compiler->allocator, 0);
  compiler->type_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, compiler->allocator, 0);
  compiler->op_constants = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, compiler->allocator, 0);
  compiler->op_subs = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, compiler->allocator, 0);
  compiler->string_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, compiler->allocator, 0);

  compiler->package_load_path_symtable = SPVM_COMPILER_ALLOCATOR_alloc_hash(compiler, compiler->allocator, 0);

  compiler->enum_default_type_code = SPVM_TYPE_C_CODE_INT;

  // Constant pool
  compiler->constant_pool = SPVM_CONSTANT_POOL_new(compiler);
  
  // Bytecodes
  compiler->opcode_array = SPVM_OPCODE_ARRAY_new(compiler);
  
  // Add core types
  {
    int32_t type_code;
    for (type_code = 0; type_code < SPVM_TYPE_C_CORE_LENGTH; type_code++) {
      // Type
      SPVM_TYPE* type = SPVM_TYPE_new(compiler);
      const char* name = SPVM_TYPE_C_CODE_NAMES[type_code];
      type->name = name;
      type->code = type_code;
      if (type_code >= SPVM_TYPE_C_CODE_BYTE_ARRAY && type_code <= SPVM_TYPE_C_CODE_DOUBLE_ARRAY) {
        type->dimension++;
        type->base_type = SPVM_LIST_fetch(compiler->types, type_code - SPVM_TYPE_C_ARRAY_SHIFT);
      }
      else {
        type->base_type = type;
      }
      SPVM_LIST_push(compiler->types, type);
      SPVM_HASH_insert(compiler->type_symtable, name, strlen(name), type);
    }
  }
  
  return compiler;
}

int32_t SPVM_COMPILER_compile(SPVM_COMPILER* compiler) {
  
  const char* entyr_point_package_name = compiler->entry_point_package_name;
  
  if (entyr_point_package_name) {
    // Create use op for entry point package
    SPVM_OP* op_use_entry_point = SPVM_OP_new_op_use_from_package_name(compiler, entyr_point_package_name, "main", 1);
    SPVM_LIST_push(compiler->op_use_stack, op_use_entry_point);
    SPVM_HASH_insert(compiler->op_use_symtable, entyr_point_package_name, strlen(entyr_point_package_name), op_use_entry_point);
    
    // Entry point
    int32_t entyr_point_package_name_length = (int32_t)strlen(entyr_point_package_name);
    int32_t entry_point_sub_name_length =  (int32_t)(entyr_point_package_name_length + 6);
    char* entry_point_sub_name = SPVM_UTIL_ALLOCATOR_safe_malloc_zero(entry_point_sub_name_length + 1);
    strncpy(entry_point_sub_name, entyr_point_package_name, entyr_point_package_name_length);
    strncpy(entry_point_sub_name + entyr_point_package_name_length, "::main", 6);
    entry_point_sub_name[entry_point_sub_name_length] = '\0';
    compiler->entry_point_sub_name = entry_point_sub_name;
  }
  
  // use std module
  SPVM_OP* op_use_std = SPVM_OP_new_op_use_from_package_name(compiler, "CORE", "CORE", 0);
  SPVM_LIST_push(compiler->op_use_stack, op_use_std);
  SPVM_HASH_insert(compiler->op_use_symtable, "CORE", strlen("CORE"), op_use_std);
  
  // use String module
  SPVM_OP* op_use_string = SPVM_OP_new_op_use_from_package_name(compiler, "String", "CORE", 0);
  SPVM_LIST_push(compiler->op_use_stack, op_use_string);
  SPVM_HASH_insert(compiler->op_use_symtable, "String", strlen("String"), op_use_string);
  
  /* call SPVM_yyparse */
  int32_t parse_success = SPVM_yyparse(compiler);
  
  return parse_success;
}

void SPVM_COMPILER_free(SPVM_COMPILER* compiler) {
  
  // Free allocator
  SPVM_COMPILER_ALLOCATOR_free(compiler, compiler->allocator);

  // Free constant pool
  SPVM_CONSTANT_POOL_free(compiler, compiler->constant_pool);
  
  // Free opcode array
  SPVM_OPCODE_ARRAY_free(compiler, compiler->opcode_array);
  
  free(compiler);
}
