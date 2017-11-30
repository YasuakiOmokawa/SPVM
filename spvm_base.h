#ifndef SPVM_BASE_H
#define SPVM_BASE_H

#include <stdint.h>
#include <stdlib.h>

// spvm_opcode_array.h
struct SPVM_opcode_array;
typedef struct SPVM_opcode_array SPVM_OPCODE_ARRAY;

// spvm_opcode.h
struct SPVM_opcode;
typedef struct SPVM_opcode SPVM_OPCODE;

// spvm_undef.h
struct SPVM_undef;
typedef struct SPVM_undef SPVM_UNDEF;

// spvm_string_buffer.h
struct SPVM_string_buffer;
typedef struct SPVM_string_buffer SPVM_STRING_BUFFER;

// spvm_package_var.h
struct SPVM_package_var;
typedef struct SPVM_package_var SPVM_PACKAGE_VAR;

// spvm_field.h
struct SPVM_our;
typedef struct SPVM_our SPVM_OUR;

// spvm_call_sub.h
struct SPVM_call_sub;
typedef struct SPVM_call_sub SPVM_CALL_SUB;

// spvm_sub_check_info.h
struct SPVM_sub_check_info;
typedef struct SPVM_sub_check_info SPVM_SUB_CHECK_INFO;

// spvm_constant_pool_sub.h
struct SPVM_constant_pool_sub;
typedef struct SPVM_constant_pool_sub SPVM_CONSTANT_POOL_SUB;

// spvm_constant_pool_field.h
struct SPVM_constant_pool_field;
typedef struct SPVM_constant_pool_field SPVM_CONSTANT_POOL_FIELD;

// spvm_constant_pool_package.h
struct SPVM_constant_pool_package;
typedef struct SPVM_constant_pool_package SPVM_CONSTANT_POOL_PACKAGE;

// spvm_constant_pool_package.h
struct SPVM_constant_pool_type;
typedef struct SPVM_constant_pool_type SPVM_CONSTANT_POOL_TYPE;

// spvm_object.h
struct SPVM_object;
typedef struct SPVM_object SPVM_OBJECT;

// spvm_use.h
struct SPVM_use;
typedef struct SPVM_use SPVM_USE;

// spvm_value.h
union SPVM_value;
typedef union SPVM_value SPVM_VALUE;

// spvm_runtime.h
struct SPVM_runtime;
typedef struct SPVM_runtime SPVM_RUNTIME;

// spvm_runtime_allocator.h
struct SPVM_runtime_allocator;
typedef struct SPVM_runtime_allocator SPVM_RUNTIME_ALLOCATOR;

// spvm_compiler_allocator.h
struct SPVM_compiler_allocator;
typedef struct SPVM_compiler_allocator SPVM_COMPILER_ALLOCATOR;

// spvm_heap.h
struct SPVM_heap;
typedef struct SPVM_heap SPVM_HEAP;

// spvm_compiler.h
struct SPVM_compiler;
typedef struct SPVM_compiler SPVM_COMPILER;

// spvm_vm.h
struct SPVM_vm_stack;
typedef struct SPVM_vm_stack SPVM_VM_STACK;

// spvm_vm.h
struct SPVM_vm;
typedef struct SPVM_vm SPVM_VM;

// spvm_switch.h
struct SPVM_switch_info;
typedef struct SPVM_switch_info SPVM_SWITCH_INFO;

// spvm_compiler_allocator.h
struct SPVM_allocator;
typedef struct SPVM_allocator SPVM_ALLOCATOR;

// spvm_dumper.h
struct SPVM_dumper;
typedef struct SPVM_dumper SPVM_DUMPER;

// spvm_bytecode_array.h
struct SPVM_bytecode_array;
typedef struct SPVM_bytecode_array SPVM_BYTECODE_ARRAY;

// spvm_assign.h
struct SPVM_bytecode;
typedef struct SPVM_bytecode SPVM_BYTECODE;

// spvm_vmcode.h
struct SPVM_vmcodes;
typedef struct SPVM_vmcodes SPVM_VMCODES;

// spvm_constant_pool.h
struct SPVM_constant_pool;
typedef struct SPVM_constant_pool SPVM_CONSTANT_POOL;

// spvm_vmcode.h
struct SPVM_vmcode;
typedef struct SPVM_vmcode SPVM_VMCODE;

// spvm_assign.h
struct SPVM_assign;
typedef struct SPVM_assign SPVM_ASSIGN;

// spvm_package.h
struct SPVM_package;
typedef struct SPVM_package SPVM_PACKAGE;

// spvm_type.h
struct SPVM_call_field;
typedef struct SPVM_call_field SPVM_CALL_FIELD;

// spvm_type.h
struct SPVM_type;
typedef struct SPVM_type SPVM_TYPE;

// spvm_type_component_array.h
struct SPVM_type_part;
typedef struct SPVM_type_part SPVM_TYPE_PART;

// spvm_enum.h
struct SPVM_enumeration;
typedef struct SPVM_enumeration SPVM_ENUMERATION;

// spvm_enumeration_value.h
struct SPVM_enumeration_value;
typedef struct SPVM_enumeration_value SPVM_ENUMERATION_VALUE;

// spvm_memory_pool.h
struct SPVM_memory_pool;
typedef struct SPVM_memory_pool SPVM_MEMORY_POOL;

// spvm_var.h
struct SPVM_var;
typedef struct SPVM_var SPVM_VAR;

// spvm_my.h
struct SPVM_my;
typedef struct SPVM_my SPVM_MY;

// spvm_constant.h
struct SPVM_constant;
typedef struct SPVM_constant SPVM_CONSTANT;

// spvm_op.h
struct SPVM_op;
typedef struct SPVM_op SPVM_OP;

// spvm_field.h
struct SPVM_field;
typedef struct SPVM_field SPVM_FIELD;

// spvm_descriptor.h
struct SPVM_descriptor;
typedef struct SPVM_descriptor SPVM_DESCRIPTOR;

// spvm_sub.h
struct SPVM_sub;
typedef struct SPVM_sub SPVM_SUB;

// spvm_dynamic_array_element.h
struct SPVM_array_element;
typedef struct SPVM_array_element SPVM_DYNAMIC_ARRAY_ELEMENT;

// spvm_dynamic_array.h
struct SPVM_dynamic_array;
typedef struct SPVM_dynamic_array SPVM_DYNAMIC_ARRAY;

// spvm_hash_entry.h
struct SPVM_hash_entry;
typedef struct SPVM_hash_entry SPVM_HASH_ENTRY;

// spvm_hash.h
struct SPVM_hash;
typedef struct SPVM_hash SPVM_HASH;

union SPVM_yystype;
typedef union SPVM_yystype SPVM_YYSTYPE;

#endif
