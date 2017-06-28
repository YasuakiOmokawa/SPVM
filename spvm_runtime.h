#ifndef SPVM_RUNTIME_H
#define SPVM_RUNTIME_H

#include "spvm_base.h"

struct SPVM_runtime {
  
  // Runtime memory allocator
  SPVM_RUNTIME_ALLOCATOR* allocator;

  // Call stack
  SPVM_VALUE* call_stack;
  
  // Call stack capacity
  int32_t call_stack_capacity;
  
  // Operand stack base
  int32_t call_stack_base;
  
  // Operand stack top
  int32_t operand_stack_top;
  
  // Env
  SPVM_ENV* env;
  
  // Bytecodes
  uint8_t* bytecodes;
  
  // Constant pool
  int32_t* constant_pool;
  
  // Exception
  _Bool abort;
};

SPVM_RUNTIME* SPVM_RUNTIME_new();
void SPVM_RUNTIME_init(SPVM_RUNTIME* runtime);
void SPVM_RUNTIME_free(SPVM_RUNTIME* runtime);

void SPVM_RUNTIME_call_sub(SPVM_ENV* env, int32_t sub_constant_pool_index);

#endif
