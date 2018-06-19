#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include "spvm_csource_builder.h"
#include "spvm_string_buffer.h"
#include "spvm_sub.h"
#include "spvm_op.h"
#include "spvm_list.h"
#include "spvm_hash.h"
#include "spvm_type.h"
#include "spvm_object.h"
#include "spvm_runtime.h"
#include "spvm_type.h"
#include "spvm_my.h"
#include "spvm_opcode.h"
#include "spvm_opcode_array.h"
#include "spvm_runtime_api.h"

#include "spvm_package.h"
#include "spvm_sub.h"
#include "spvm_package.h"
#include "spvm_type.h"
#include "spvm_field.h"
#include "spvm_compiler.h"
#include "spvm_my.h"
#include "spvm_op.h"
#include "spvm_opcode_array.h"
#include "spvm_constant.h"
#include "spvm_basic_type.h"

void SPVM_CSOURCE_BUILDER_add_var(SPVM_STRING_BUFFER* string_buffer, int32_t index) {
  SPVM_STRING_BUFFER_add(string_buffer, "vars[");
  SPVM_STRING_BUFFER_add_int(string_buffer, index);
  SPVM_STRING_BUFFER_add(string_buffer, "]");
}

void SPVM_CSOURCE_BUILDER_add_operand(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t var_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "*(");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)type_name);
  SPVM_STRING_BUFFER_add(string_buffer, "*)&");
  SPVM_CSOURCE_BUILDER_add_var(string_buffer, var_index);
}

void SPVM_CSOURCE_BUILDER_add_bool(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = !!");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_eq(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " == ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
}

void SPVM_CSOURCE_BUILDER_add_ne(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " != ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
}

void SPVM_CSOURCE_BUILDER_add_gt(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " > ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
}

void SPVM_CSOURCE_BUILDER_add_ge(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " >= ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
}

void SPVM_CSOURCE_BUILDER_add_lt(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " < ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
}

void SPVM_CSOURCE_BUILDER_add_le(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " <= ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
}

void SPVM_CSOURCE_BUILDER_add_add(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " + ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_subtract(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " - ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_multiply(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " * ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_divide_integral(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, " == 0, 0)) { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"0 division\", 0));\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");

  SPVM_STRING_BUFFER_add(string_buffer, "  } else {\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " / ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
}

void SPVM_CSOURCE_BUILDER_add_divide_floating_point(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " / ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_remainder_integral(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, " == 0, 0)) {\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    void* exception = env->new_string(env, \"0 division\", 0);\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, exception);\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " % /* REMAINDER */ "); // % have undarstandable bug.
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
}

void SPVM_CSOURCE_BUILDER_add_left_shift(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " << (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  if (strcmp(type_name, "SPVM_VALUE_int") == 0) {
    SPVM_STRING_BUFFER_add(string_buffer, " & 0x1f))");
  }
  else if (strcmp(type_name, "SPVM_VALUE_long") == 0) {
    SPVM_STRING_BUFFER_add(string_buffer, " & 0x3f))");
  }
  else {
    assert(0);
  }
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_right_shift_unsigned(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ((");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)type_name);
  SPVM_STRING_BUFFER_add(string_buffer, ")((u");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)type_name);
  SPVM_STRING_BUFFER_add(string_buffer, ")");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " >> (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  if (strcmp(type_name, "int32_t") == 0) {
    SPVM_STRING_BUFFER_add(string_buffer, " & 0x1f))");
  }
  else if (strcmp(type_name, "int64_t") == 0) {
    SPVM_STRING_BUFFER_add(string_buffer, " & 0x3f))");
  }
  else {
    assert(0);
  }
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
}

void SPVM_CSOURCE_BUILDER_add_right_shift(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " >> (");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  if (strcmp(type_name, "SPVM_VALUE_int") == 0) {
    SPVM_STRING_BUFFER_add(string_buffer, " & 0x1f))");
  }
  else if (strcmp(type_name, "SPVM_VALUE_long") == 0) {
    SPVM_STRING_BUFFER_add(string_buffer, " & 0x3f))");
  }
  else {
    assert(0);
  }
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_bit_and(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " & ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_bit_or(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " | ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_bit_xor(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in1_index, int32_t in2_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in1_index);
  SPVM_STRING_BUFFER_add(string_buffer, " ^ ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in2_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_negate(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = -");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_plus(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_complement(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ~");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_inc(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t number) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " += ");
  SPVM_STRING_BUFFER_add_int(string_buffer, number);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_convert(SPVM_STRING_BUFFER* string_buffer, const char* out_type_name, const char* in_type_name, int32_t out_index, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, out_type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = (");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)out_type_name);
  SPVM_STRING_BUFFER_add(string_buffer, ")");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, in_type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_load_constant_0(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = 0");
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_array_load(SPVM_STRING_BUFFER* string_buffer, const char* element_type_name, int32_t out_index, int32_t array_index, int32_t index_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", array_index);
  SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "      env->set_exception(env, env->new_string(env, \"Array must not be undef\", 0)); \n");
  SPVM_STRING_BUFFER_add(string_buffer, "      exception_flag = 1;\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
  SPVM_STRING_BUFFER_add(string_buffer, "  else { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", index_index);
  SPVM_STRING_BUFFER_add(string_buffer, " < 0 || ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", index_index);
  SPVM_STRING_BUFFER_add(string_buffer, "  >= *(int32_t*)((intptr_t)");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", array_index);
  SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_elements_length_byte_offset), 0)) { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "        env->set_exception(env, env->new_string(env, \"Index is out of range\", 0)); \n");
  SPVM_STRING_BUFFER_add(string_buffer, "        exception_flag = 1;\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    else { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "      ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, element_type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = *(");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)element_type_name);
  SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", array_index);
  SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)element_type_name);
  SPVM_STRING_BUFFER_add(string_buffer, ") * ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", index_index);
  SPVM_STRING_BUFFER_add(string_buffer, "); \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
  SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
}

void SPVM_CSOURCE_BUILDER_add_array_store(SPVM_STRING_BUFFER* string_buffer, const char* element_type_name, int32_t array_index, int32_t index_index, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", array_index);
  SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Array must not be undef\", 0)); \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
  SPVM_STRING_BUFFER_add(string_buffer, "  else { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", index_index);
  SPVM_STRING_BUFFER_add(string_buffer, " < 0 || ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", index_index);
  SPVM_STRING_BUFFER_add(string_buffer, "  >= *(int32_t*)((intptr_t)");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", array_index);
  SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_elements_length_byte_offset), 0)) { \n");
  SPVM_STRING_BUFFER_add(string_buffer, "        env->set_exception(env, env->new_string(env, \"Index is out of range\", 0)); \n");
  SPVM_STRING_BUFFER_add(string_buffer, "        exception_flag = 1;\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
  SPVM_STRING_BUFFER_add(string_buffer, "    else { \n");
  
  SPVM_STRING_BUFFER_add(string_buffer, "      *(");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)element_type_name);
  SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", array_index);
  SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)element_type_name);
  SPVM_STRING_BUFFER_add(string_buffer, ") * ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", index_index);
  SPVM_STRING_BUFFER_add(string_buffer, ") = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, element_type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");

  SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
  SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
}

void SPVM_CSOURCE_BUILDER_add_move(SPVM_STRING_BUFFER* string_buffer, const char* type_name, int32_t out_index, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
}

void SPVM_CSOURCE_BUILDER_add_get_field(SPVM_STRING_BUFFER* string_buffer, const char* field_type_name, int32_t out_index, int32_t object_index, int32_t field_id) {
  SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", object_index);
  SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) {\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Object must be not undef.\", 0));\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, field_type_name, out_index);
  SPVM_STRING_BUFFER_add(string_buffer, " = *(");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)field_type_name);
  SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", object_index);
  SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(SPVM_VALUE) * ");
  SPVM_STRING_BUFFER_add_int(string_buffer, field_id);
  SPVM_STRING_BUFFER_add(string_buffer, ");\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
}

void SPVM_CSOURCE_BUILDER_add_set_field(SPVM_STRING_BUFFER* string_buffer, const char* field_type_name, int32_t object_index, int32_t field_byte_offset, int32_t in_index) {
  SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", object_index);
  SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) {\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Object must be not undef.\", 0));\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    *(");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)field_type_name);
  SPVM_STRING_BUFFER_add(string_buffer, "*)((intptr_t)");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", object_index);
  SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(SPVM_VALUE) * ");
  SPVM_STRING_BUFFER_add_int(string_buffer, field_byte_offset);
  SPVM_STRING_BUFFER_add(string_buffer, ") = ");
  SPVM_CSOURCE_BUILDER_add_operand(string_buffer, field_type_name, in_index);
  SPVM_STRING_BUFFER_add(string_buffer, ";\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
}

char* SPVM_CSOURCE_BUILDER_get_type_name(int32_t basic_type_id, int32_t dimension) {
  
  if (dimension == 0) {
    switch (basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_VOID:
        assert(0);
        break;
      case SPVM_BASIC_TYPE_C_ID_BYTE:
        return "SPVM_VALUE_byte";
        break;
      case SPVM_BASIC_TYPE_C_ID_SHORT:
        return "SPVM_VALUE_short";
        break;
      case SPVM_BASIC_TYPE_C_ID_INT:
        return "SPVM_VALUE_int";
        break;
      case SPVM_BASIC_TYPE_C_ID_LONG:
        return "SPVM_VALUE_long";
        break;
      case SPVM_BASIC_TYPE_C_ID_FLOAT:
        return "SPVM_VALUE_float";
        break;
      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
        return "SPVM_VALUE_double";
        break;
      default:
        return "void*";
    }
  }
  else {
    return "void*";
  }
}

void SPVM_CSOURCE_BUILDER_build_package_csource(SPVM_COMPILER* compiler, SPVM_STRING_BUFFER* string_buffer, const char* package_name) {
  SPVM_OP* op_package = SPVM_HASH_search(compiler->op_package_symtable, package_name, strlen(package_name));
  
  assert(op_package);
  
  SPVM_PACKAGE* package = op_package->uv.package;
  SPVM_LIST* op_subs = package->op_subs;
  
  // Head part - include and define
  SPVM_CSOURCE_BUILDER_build_head(compiler, string_buffer);
  
  // Subroutine decrations
  SPVM_STRING_BUFFER_add(string_buffer, "// Function Declarations\n");
  {
    int32_t sub_index;
    for (sub_index = 0; sub_index < op_subs->length; sub_index++) {
      SPVM_OP* op_sub = SPVM_LIST_fetch(op_subs, sub_index);
      SPVM_SUB* sub = op_sub->uv.sub;
      const char* sub_name = sub->op_name->uv.name;
      if (sub->have_precompile_desc) {
        SPVM_CSOURCE_BUILDER_build_sub_declaration(compiler, string_buffer, package_name, sub_name);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
      }
    }
  }
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  // Subroutine implementations
  SPVM_STRING_BUFFER_add(string_buffer, "// Function Implementations\n");
  {
    int32_t sub_index;
    for (sub_index = 0; sub_index < op_subs->length; sub_index++) {
      SPVM_OP* op_sub = SPVM_LIST_fetch(op_subs, sub_index);
      SPVM_SUB* sub = op_sub->uv.sub;
      const char* sub_name = sub->op_name->uv.name;
      if (sub->have_precompile_desc) {
        SPVM_CSOURCE_BUILDER_build_sub_implementation(compiler, string_buffer, package_name, sub_name);
      }
    }
  }
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
}

void SPVM_CSOURCE_BUILDER_build_head(SPVM_COMPILER* compiler, SPVM_STRING_BUFFER* string_buffer) {
  (void)compiler;
  
  // Include header
  SPVM_STRING_BUFFER_add(string_buffer, "#ifndef SPVM_CSOURCE_BUILDER_H\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_CSOURCE_BUILDER_H\n");
  
  // API header
  SPVM_STRING_BUFFER_add(string_buffer, "#include <spvm_native.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  
  // Inline macro function
  SPVM_STRING_BUFFER_add(string_buffer, "#include <stdlib.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <string.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <stdio.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <inttypes.h>\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#include <math.h>\n");

  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_RUNTIME_C_INLINE_GET_REF_COUNT(object) ((*(int32_t*)((intptr_t)object + (intptr_t)env->object_ref_count_byte_offset)))\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_RUNTIME_C_INLINE_INC_REF_COUNT_ONLY(object) ((*(int32_t*)((intptr_t)object + (intptr_t)env->object_ref_count_byte_offset))++)\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_RUNTIME_C_INLINE_INC_REF_COUNT(object)\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "do {\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  if (object != NULL) {\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_INC_REF_COUNT_ONLY(object);\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "} while (0)\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_RUNTIME_C_INLINE_DEC_REF_COUNT_ONLY(object) ((*(int32_t*)((intptr_t)object + (intptr_t)env->object_ref_count_byte_offset))--)\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_RUNTIME_C_INLINE_DEC_REF_COUNT(object)\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "do {\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  if (object != NULL) {\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    if (SPVM_RUNTIME_C_INLINE_GET_REF_COUNT(object) > 1) { SPVM_RUNTIME_C_INLINE_DEC_REF_COUNT_ONLY(object); }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    else { env->dec_ref_count(env, object); }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "} while (0)\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_RUNTIME_C_INLINE_ISWEAK(object) ((intptr_t)object & 1)\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#define SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(dist_ptr, source) \\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "do {\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  void* tmp_object = source;\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  if (tmp_object != NULL) {\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_INC_REF_COUNT_ONLY(tmp_object);\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  if (*(void**)(dist_ptr) != NULL) {\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    if (SPVM_RUNTIME_C_INLINE_ISWEAK(*(void**)(dist_ptr))) { env->unweaken(env, dist_ptr); }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    if (SPVM_RUNTIME_C_INLINE_GET_REF_COUNT(*(void**)(dist_ptr)) > 1) { SPVM_RUNTIME_C_INLINE_DEC_REF_COUNT_ONLY(*(void**)(dist_ptr)); }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "    else { env->dec_ref_count(env, *(void**)(dist_ptr)); }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  *(void**)(dist_ptr) = tmp_object;\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "} while (0)\\\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
  SPVM_STRING_BUFFER_add(string_buffer, "#endif\n");
}

void SPVM_CSOURCE_BUILDER_build_sub_declaration(SPVM_COMPILER* compiler, SPVM_STRING_BUFFER* string_buffer, const char* package_name, const char* sub_name) {
  SPVM_OP* op_package = SPVM_HASH_search(compiler->op_package_symtable, package_name, strlen(package_name));
  SPVM_PACKAGE* package = op_package->uv.package;
  SPVM_OP* op_sub = SPVM_HASH_search(package->op_sub_symtable, sub_name, strlen(sub_name));
  SPVM_SUB* sub = op_sub->uv.sub;

  assert(sub->have_precompile_desc);
  
  // Subroutine name
  const char* sub_abs_name = sub->abs_name;
  
  // Return type
  SPVM_STRING_BUFFER_add(string_buffer, "int32_t ");
        
  // Subroutine name. Replace : to _
  SPVM_STRING_BUFFER_add(string_buffer, "SPVM_PRECOMPILE_");
  SPVM_STRING_BUFFER_add(string_buffer, (char*)sub_abs_name);
  {
    int32_t index = string_buffer->length - strlen(sub_abs_name);
    
    while (index < string_buffer->length) {
      if (string_buffer->buffer[index] == ':') {
        string_buffer->buffer[index] = '_';
      }
      index++;
    }
  }
  
  // Arguments
  SPVM_STRING_BUFFER_add(string_buffer, "(SPVM_ENV* env, SPVM_VALUE* args)");
}

void SPVM_CSOURCE_BUILDER_build_sub_implementation(SPVM_COMPILER* compiler, SPVM_STRING_BUFFER* string_buffer, const char* package_name, const char* sub_name) {
  SPVM_OP* op_package = SPVM_HASH_search(compiler->op_package_symtable, package_name, strlen(package_name));
  SPVM_PACKAGE* package = op_package->uv.package;
  SPVM_OP* op_sub = SPVM_HASH_search(package->op_sub_symtable, sub_name, strlen(sub_name));
  SPVM_SUB* sub = op_sub->uv.sub;

  // Subroutine return type
  SPVM_TYPE* sub_return_type = sub->op_return_type->uv.type;
  
  int32_t sub_return_basic_type_id = sub_return_type->basic_type->id;
  
  int32_t sub_return_type_dimension = sub_return_type->dimension;
  int32_t sub_return_type_is_object = SPVM_TYPE_is_object(compiler, sub_return_type);
  
  assert(sub->have_precompile_desc);
  
  SPVM_CSOURCE_BUILDER_build_sub_declaration(compiler, string_buffer, sub->op_package->uv.package->op_name->uv.name, sub->op_name->uv.name);
  
  // Block start
  SPVM_STRING_BUFFER_add(string_buffer, " {\n");
  
  // Variables
  if (sub->op_mys->length > 0) {
    SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_VALUE vars[");
    SPVM_STRING_BUFFER_add_int(string_buffer, sub->op_mys->length);
    SPVM_STRING_BUFFER_add(string_buffer, "];\n");
  }
  
  if (sub->mortal_stack_max > 0) {
    SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_VALUE mortal_stack[");
    SPVM_STRING_BUFFER_add_int(string_buffer, sub->mortal_stack_max);
    SPVM_STRING_BUFFER_add(string_buffer, "];\n");
    SPVM_STRING_BUFFER_add(string_buffer, "  int32_t mortal_stack_top = -1;\n");
  }
  
  // Call subroutine argument stack top
  SPVM_STRING_BUFFER_add(string_buffer, "  int32_t call_sub_arg_stack_top = -1;\n");

  // Condition flag
  SPVM_STRING_BUFFER_add(string_buffer, "  register int32_t condition_flag;\n");
  
  // tmp string
  SPVM_STRING_BUFFER_add(string_buffer, "  char tmp_string[30];\n");

  // Exception
  SPVM_STRING_BUFFER_add(string_buffer, "  int32_t exception_flag = 0;\n");

  // Initialize variables
  {
    int32_t my_index;
    for (my_index = sub->op_args->length; my_index < sub->op_mys->length; my_index++) {
      SPVM_OP* op_my = SPVM_LIST_fetch(sub->op_mys, my_index);
      SPVM_TYPE* my_type = op_my->uv.my->op_type->uv.type;
      
      if (SPVM_TYPE_is_object(compiler, my_type)) {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", my_index);
        SPVM_STRING_BUFFER_add(string_buffer, " = NULL;\n");
      }
      else {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, SPVM_CSOURCE_BUILDER_get_type_name(my_type->basic_type->id, my_type->dimension), my_index);
        SPVM_STRING_BUFFER_add(string_buffer, " = 0;\n");
      }
    }
    SPVM_STRING_BUFFER_add(string_buffer, "\n");
  }

  // Copy arguments to variables
  {
    int32_t arg_index;
    for (arg_index = 0; arg_index < sub->op_args->length; arg_index++) {
      
      SPVM_OP* op_arg = SPVM_LIST_fetch(sub->op_args, arg_index);
      SPVM_TYPE* arg_type = op_arg->uv.my->op_type->uv.type;
      const char* arg_type_name = SPVM_CSOURCE_BUILDER_get_type_name(arg_type->basic_type->id, arg_type->dimension);

      // Assign argument
      SPVM_STRING_BUFFER_add(string_buffer, "  ");
      SPVM_CSOURCE_BUILDER_add_operand(string_buffer, arg_type_name, arg_index);
      SPVM_STRING_BUFFER_add(string_buffer, " = ");
      SPVM_STRING_BUFFER_add(string_buffer, "*(");
      SPVM_STRING_BUFFER_add(string_buffer, SPVM_CSOURCE_BUILDER_get_type_name(arg_type->basic_type->id, arg_type->dimension));
      SPVM_STRING_BUFFER_add(string_buffer, "*)&args[");
      SPVM_STRING_BUFFER_add_int(string_buffer, arg_index);
      SPVM_STRING_BUFFER_add(string_buffer, "]");
      
      SPVM_STRING_BUFFER_add(string_buffer, ";\n");
    }
  }
  
  // If arg is object, increment reference count
  {
    int32_t arg_index;
    for (arg_index = 0; arg_index < sub->op_args->length; arg_index++) {
      SPVM_OP* op_arg = SPVM_LIST_fetch(sub->op_args, arg_index);
      SPVM_TYPE* arg_type = op_arg->uv.my->op_type->uv.type;
      
      if (SPVM_TYPE_is_object(compiler, arg_type)) {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", arg_index);
        SPVM_STRING_BUFFER_add(string_buffer, " != NULL) { SPVM_RUNTIME_C_INLINE_INC_REF_COUNT_ONLY(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", arg_index);
        SPVM_STRING_BUFFER_add(string_buffer, "); }\n");
      }
    }
    SPVM_STRING_BUFFER_add(string_buffer, "\n");
  }

  SPVM_OPCODE* opcodes = compiler->opcode_array->values;
  int32_t sub_opcode_base = sub->opcode_base;
  int32_t opcode_length = sub->opcode_length;
  int32_t opcode_index = 0;
  
  SPVM_OPCODE* opcode = NULL;
  
  while (opcode_index < opcode_length) {

    // Line label
    SPVM_STRING_BUFFER_add(string_buffer, "L");
    SPVM_STRING_BUFFER_add_int(string_buffer, opcode_index);
    SPVM_STRING_BUFFER_add(string_buffer, ": ");
    
    opcode = &(opcodes[sub_opcode_base + opcode_index]);

    SPVM_STRING_BUFFER_add(string_buffer, "// ");
    SPVM_STRING_BUFFER_add(string_buffer, (char*)SPVM_OPCODE_C_ID_NAMES[opcode->id]);
    SPVM_STRING_BUFFER_add(string_buffer, "\n");

    
    switch (opcode->id) {
      case SPVM_OPCODE_C_ID_BOOL_INT:
        SPVM_CSOURCE_BUILDER_add_bool(string_buffer, "SPVM_VALUE_int", opcode->operand0);
        break;
      case SPVM_OPCODE_C_ID_BOOL_LONG:
        SPVM_CSOURCE_BUILDER_add_bool(string_buffer, "SPVM_VALUE_long", opcode->operand0);
        break;
      case SPVM_OPCODE_C_ID_BOOL_FLOAT:
        SPVM_CSOURCE_BUILDER_add_bool(string_buffer, "float", opcode->operand0);
        break;
      case SPVM_OPCODE_C_ID_BOOL_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_bool(string_buffer, "double", opcode->operand0);
        break;
      case SPVM_OPCODE_C_ID_BOOL_OBJECT:
        SPVM_CSOURCE_BUILDER_add_bool(string_buffer, "void*", opcode->operand0);
        break;
      case SPVM_OPCODE_C_ID_IS_UNDEF:
        SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " == NULL);\n");
        break;
      case SPVM_OPCODE_C_ID_IS_NOT_UNDEF:
        SPVM_STRING_BUFFER_add(string_buffer, "  condition_flag = (");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " != NULL);\n");
        break;
      case SPVM_OPCODE_C_ID_EQ_INT:
        SPVM_CSOURCE_BUILDER_add_eq(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_EQ_LONG:
        SPVM_CSOURCE_BUILDER_add_eq(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_EQ_FLOAT:
        SPVM_CSOURCE_BUILDER_add_eq(string_buffer, "float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_EQ_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_eq(string_buffer, "double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_EQ_OBJECT:
        SPVM_CSOURCE_BUILDER_add_eq(string_buffer, "void*", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NE_INT:
        SPVM_CSOURCE_BUILDER_add_ne(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NE_LONG:
        SPVM_CSOURCE_BUILDER_add_ne(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NE_FLOAT:
        SPVM_CSOURCE_BUILDER_add_ne(string_buffer, "float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NE_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_ne(string_buffer, "double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NE_OBJECT:
        SPVM_CSOURCE_BUILDER_add_ne(string_buffer, "void*", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GT_INT:
        SPVM_CSOURCE_BUILDER_add_gt(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GT_LONG:
        SPVM_CSOURCE_BUILDER_add_gt(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GT_FLOAT:
        SPVM_CSOURCE_BUILDER_add_gt(string_buffer, "float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GT_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_gt(string_buffer, "double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GE_INT:
        SPVM_CSOURCE_BUILDER_add_ge(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GE_LONG:
        SPVM_CSOURCE_BUILDER_add_ge(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GE_FLOAT:
        SPVM_CSOURCE_BUILDER_add_ge(string_buffer, "float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_GE_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_ge(string_buffer, "double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LT_INT:
        SPVM_CSOURCE_BUILDER_add_lt(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LT_LONG:
        SPVM_CSOURCE_BUILDER_add_lt(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LT_FLOAT:
        SPVM_CSOURCE_BUILDER_add_lt(string_buffer, "float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LT_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_lt(string_buffer, "double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LE_INT:
        SPVM_CSOURCE_BUILDER_add_le(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LE_LONG:
        SPVM_CSOURCE_BUILDER_add_le(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LE_FLOAT:
        SPVM_CSOURCE_BUILDER_add_le(string_buffer, "float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_LE_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_le(string_buffer, "double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_ISA:
      {
        int32_t basic_type_id = opcode->operand1;
        int32_t dimension = opcode->operand2;
        
        SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);
        
        SPVM_STRING_BUFFER_add(string_buffer, "  {");
        SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t basic_type_id = -1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (basic_type_id == -1) { basic_type_id = env->get_basic_type_id(env, \"");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)basic_type->name);
        SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t dimension = ");
        SPVM_STRING_BUFFER_add_int(string_buffer, dimension);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    void* object = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    condition_flag = (*(int32_t*)(object + (intptr_t)env->object_basic_type_id_byte_offset) == basic_type_id && *(int32_t*)(object + (intptr_t)env->object_dimension_byte_offset) == dimension);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_STRING_EQ:
      case SPVM_OPCODE_C_ID_STRING_NE:
      case SPVM_OPCODE_C_ID_STRING_GT:
      case SPVM_OPCODE_C_ID_STRING_GE:
      case SPVM_OPCODE_C_ID_STRING_LT:
      case SPVM_OPCODE_C_ID_STRING_LE:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  {");
        SPVM_STRING_BUFFER_add(string_buffer, "    void* object1 = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    void* object2 = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");

        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t length1 = *(SPVM_VALUE_int*)((intptr_t)object1 + (intptr_t)env->object_elements_length_byte_offset);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t lenght2 = *(SPVM_VALUE_int*)((intptr_t)object2 + (intptr_t)env->object_elements_length_byte_offset);\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_VALUE_byte* bytes1 = (SPVM_VALUE_byte*)((intptr_t)object1 + (intptr_t)env->object_header_byte_size);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_VALUE_byte* bytes2 = (SPVM_VALUE_byte*)((intptr_t)object2 + (intptr_t)env->object_header_byte_size);\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t short_string_length = length1 < lenght2 ? length1 : lenght2;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t retval = memcmp(bytes1, bytes2, short_string_length);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t cmp;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (retval) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      cmp = retval < 0 ? -1 : 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    } else if (length1 == lenght2) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      cmp = 0;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    } else {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      cmp = length1 < lenght2 ? -1 : 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
        
        switch (opcode->id) {
          case SPVM_OPCODE_C_ID_STRING_EQ:
            SPVM_STRING_BUFFER_add(string_buffer, "    condition_flag = (cmp == 0);\n");
            break;
          case SPVM_OPCODE_C_ID_STRING_NE:
            SPVM_STRING_BUFFER_add(string_buffer, "    condition_flag = (cmp != 0);\n");
            break;
          case SPVM_OPCODE_C_ID_STRING_GT:
            SPVM_STRING_BUFFER_add(string_buffer, "    condition_flag = (cmp == 1);\n");
            break;
          case SPVM_OPCODE_C_ID_STRING_GE:
            SPVM_STRING_BUFFER_add(string_buffer, "    condition_flag = (cmp >= 0);\n");
            break;
          case SPVM_OPCODE_C_ID_STRING_LT:
            SPVM_STRING_BUFFER_add(string_buffer, "    condition_flag = (cmp == -1);\n");
            break;
          case SPVM_OPCODE_C_ID_STRING_LE:
            SPVM_STRING_BUFFER_add(string_buffer, "    condition_flag = (cmp <= 0);\n");
            break;
        }
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");

        break;
      }
      case SPVM_OPCODE_C_ID_ADD_INT:
        SPVM_CSOURCE_BUILDER_add_add(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ADD_LONG:
        SPVM_CSOURCE_BUILDER_add_add(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ADD_FLOAT:
        SPVM_CSOURCE_BUILDER_add_add(string_buffer, "float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ADD_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_add(string_buffer, "double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_SUBTRACT_INT:
        SPVM_CSOURCE_BUILDER_add_subtract(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_SUBTRACT_LONG:
        SPVM_CSOURCE_BUILDER_add_subtract(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_SUBTRACT_FLOAT:
        SPVM_CSOURCE_BUILDER_add_subtract(string_buffer, "float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_SUBTRACT_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_subtract(string_buffer, "double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_MULTIPLY_INT:
        SPVM_CSOURCE_BUILDER_add_multiply(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_MULTIPLY_LONG:
        SPVM_CSOURCE_BUILDER_add_multiply(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_MULTIPLY_FLOAT:
        SPVM_CSOURCE_BUILDER_add_multiply(string_buffer, "float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_MULTIPLY_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_multiply(string_buffer, "double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_DIVIDE_INT:
        SPVM_CSOURCE_BUILDER_add_divide_integral(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_DIVIDE_LONG: 
        SPVM_CSOURCE_BUILDER_add_divide_integral(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_DIVIDE_FLOAT:
        SPVM_CSOURCE_BUILDER_add_divide_floating_point(string_buffer, "float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_DIVIDE_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_divide_floating_point(string_buffer, "double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_REMAINDER_INT:
        SPVM_CSOURCE_BUILDER_add_remainder_integral(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_REMAINDER_LONG: 
        SPVM_CSOURCE_BUILDER_add_remainder_integral(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_REMAINDER_FLOAT:
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "float", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = (float)fmod((double)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "float", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ", (double)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "float", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, ");\n");
        break;
      case SPVM_OPCODE_C_ID_REMAINDER_DOUBLE:
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "double", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = fmod(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "double", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "double", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, ");\n");
        break;
      case SPVM_OPCODE_C_ID_LEFT_SHIFT_INT:
        SPVM_CSOURCE_BUILDER_add_left_shift(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_LEFT_SHIFT_LONG:
        SPVM_CSOURCE_BUILDER_add_left_shift(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_RIGHT_SHIFT_INT:
        SPVM_CSOURCE_BUILDER_add_right_shift(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_RIGHT_SHIFT_LONG:
        SPVM_CSOURCE_BUILDER_add_right_shift(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_RIGHT_SHIFT_UNSIGNED_INT:
        SPVM_CSOURCE_BUILDER_add_right_shift_unsigned(string_buffer, "int32_t", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_RIGHT_SHIFT_UNSIGNED_LONG:
        SPVM_CSOURCE_BUILDER_add_right_shift_unsigned(string_buffer, "int64_t", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_BIT_AND_INT:
        SPVM_CSOURCE_BUILDER_add_bit_and(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_BIT_AND_LONG:
        SPVM_CSOURCE_BUILDER_add_bit_and(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_BIT_OR_INT:
        SPVM_CSOURCE_BUILDER_add_bit_or(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_BIT_OR_LONG:
        SPVM_CSOURCE_BUILDER_add_bit_or(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_BIT_XOR_INT:
        SPVM_CSOURCE_BUILDER_add_bit_xor(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_BIT_XOR_LONG:
        SPVM_CSOURCE_BUILDER_add_bit_xor(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_NEGATE_INT:
        SPVM_CSOURCE_BUILDER_add_negate(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NEGATE_LONG:
        SPVM_CSOURCE_BUILDER_add_negate(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NEGATE_FLOAT:
        SPVM_CSOURCE_BUILDER_add_negate(string_buffer, "float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_NEGATE_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_negate(string_buffer, "double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_COMPLEMENT_INT:
        SPVM_CSOURCE_BUILDER_add_complement(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_COMPLEMENT_LONG:
        SPVM_CSOURCE_BUILDER_add_complement(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_INC_BYTE:
        SPVM_CSOURCE_BUILDER_add_inc(string_buffer, "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_INC_SHORT:
        SPVM_CSOURCE_BUILDER_add_inc(string_buffer, "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_INC_INT:
        SPVM_CSOURCE_BUILDER_add_inc(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_INC_LONG:
        SPVM_CSOURCE_BUILDER_add_inc(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_INC_FLOAT:
        SPVM_CSOURCE_BUILDER_add_inc(string_buffer, "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_INC_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_inc(string_buffer, "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_BYTE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_byte", "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_BYTE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_byte", "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_INT_TO_BYTE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_byte", "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_BYTE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_byte", "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_BYTE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_byte", "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_BYTE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_byte", "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_SHORT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_short", "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_SHORT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_short", "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_INT_TO_SHORT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_short", "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_SHORT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_short", "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_SHORT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_short", "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_SHORT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_short", "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_INT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_int", "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_INT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_int", "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_INT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_int", "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_INT_TO_INT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_int", "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_INT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_int", "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_INT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_int", "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_LONG:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_long", "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_LONG:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_long", "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_INT_TO_LONG:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_long", "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_LONG:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_long", "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_LONG:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_long", "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_LONG:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_long", "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_FLOAT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_float", "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_FLOAT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_float", "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_INT_TO_FLOAT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_float", "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_FLOAT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_float", "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_FLOAT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_float", "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_FLOAT:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_float", "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_double", "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_double", "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_INT_TO_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_double", "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_double", "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_double", "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_convert(string_buffer, "SPVM_VALUE_double", "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;

      case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_STRING:
      case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_STRING:
      case SPVM_OPCODE_C_ID_CONVERT_INT_TO_STRING:
      case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_STRING:
      case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_STRING:
      case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_STRING:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  {\n");
        switch (opcode->id) {
          case SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_STRING:
            SPVM_STRING_BUFFER_add(string_buffer, "    sprintf(tmp_string, \"%\" PRId8, ");
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_byte", opcode->operand1);
            SPVM_STRING_BUFFER_add(string_buffer, ");\n");
            break;
          case SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_STRING:
            SPVM_STRING_BUFFER_add(string_buffer, "    sprintf(tmp_string, \"%\" PRId16, ");
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_short", opcode->operand1);
            SPVM_STRING_BUFFER_add(string_buffer, ");\n");
            break;
          case SPVM_OPCODE_C_ID_CONVERT_INT_TO_STRING:
            SPVM_STRING_BUFFER_add(string_buffer, "    sprintf(tmp_string, \"%\" PRId32, ");
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
            SPVM_STRING_BUFFER_add(string_buffer, ");\n");
            break;
          case SPVM_OPCODE_C_ID_CONVERT_LONG_TO_STRING:
            SPVM_STRING_BUFFER_add(string_buffer, "    sprintf(tmp_string, \"%\" PRId64, ");
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_long", opcode->operand1);
            SPVM_STRING_BUFFER_add(string_buffer, ");\n");
            break;
          case SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_STRING:
            SPVM_STRING_BUFFER_add(string_buffer, "    sprintf(tmp_string, \"%g\", ");
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_float", opcode->operand1);
            SPVM_STRING_BUFFER_add(string_buffer, ");\n");
            
            break;
          case SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_STRING:
            SPVM_STRING_BUFFER_add(string_buffer, "    sprintf(tmp_string, \"%g\", ");
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_double", opcode->operand1);
            SPVM_STRING_BUFFER_add(string_buffer, ");\n");
            break;
        }
        
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t string_length = strlen(tmp_string);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    void* string = env->new_string(env, tmp_string, string_length);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", string);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");

        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_CONSTANT_BYTE: {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_byte", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = ");
        SPVM_STRING_BUFFER_add_byte(string_buffer, *(SPVM_VALUE_byte*)&opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_CONSTANT_SHORT: {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_short", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = ");
        SPVM_STRING_BUFFER_add_short(string_buffer, *(SPVM_VALUE_short*)&opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_CONSTANT_INT: {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = ");
        SPVM_STRING_BUFFER_add_int(string_buffer, *(SPVM_VALUE_int*)&opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_CONSTANT_LONG: {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_long", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = ");
        SPVM_STRING_BUFFER_add_long(string_buffer, *(SPVM_VALUE_long*)&opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "ULL;\n");
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_CONSTANT_FLOAT: {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_float", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = ");
        SPVM_STRING_BUFFER_add_float(string_buffer, *(SPVM_VALUE_float*)&opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "f;\n");
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_CONSTANT_DOUBLE: {
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_double", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = ");
        SPVM_STRING_BUFFER_add_double(string_buffer, *(SPVM_VALUE_double*)&opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_ARRAY_LOAD_BYTE:
        SPVM_CSOURCE_BUILDER_add_array_load(string_buffer, "SPVM_VALUE_byte", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_LOAD_SHORT:
        SPVM_CSOURCE_BUILDER_add_array_load(string_buffer, "SPVM_VALUE_short", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_LOAD_INT:
        SPVM_CSOURCE_BUILDER_add_array_load(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_LOAD_LONG:
        SPVM_CSOURCE_BUILDER_add_array_load(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_LOAD_FLOAT:
        SPVM_CSOURCE_BUILDER_add_array_load(string_buffer, "SPVM_VALUE_float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_LOAD_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_array_load(string_buffer, "SPVM_VALUE_double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_LOAD_OBJECT:
        SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "      env->set_exception(env, env->new_string(env, \"Array must not be undef\", 0)); \n");
        SPVM_STRING_BUFFER_add(string_buffer, "      exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
        SPVM_STRING_BUFFER_add(string_buffer, "  else { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (__builtin_expect(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, " < 0 || ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, "  >= *(int32_t*)((intptr_t)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_elements_length_byte_offset), 0)) { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "        env->set_exception(env, env->new_string(env, \"Index is out of range\", 0)); \n");
        SPVM_STRING_BUFFER_add(string_buffer, "        exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    else { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "      SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", *(void**)((intptr_t)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(void*) * ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, ")); \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
        SPVM_STRING_BUFFER_add(string_buffer, "  } \n");

        break;
      case SPVM_OPCODE_C_ID_ARRAY_STORE_BYTE:
        SPVM_CSOURCE_BUILDER_add_array_store(string_buffer, "SPVM_VALUE_byte", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_STORE_SHORT:
        SPVM_CSOURCE_BUILDER_add_array_store(string_buffer, "SPVM_VALUE_short", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_STORE_INT:
        SPVM_CSOURCE_BUILDER_add_array_store(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_STORE_LONG:
        SPVM_CSOURCE_BUILDER_add_array_store(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_STORE_FLOAT:
        SPVM_CSOURCE_BUILDER_add_array_store(string_buffer, "SPVM_VALUE_float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_STORE_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_array_store(string_buffer, "SPVM_VALUE_double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      case SPVM_OPCODE_C_ID_ARRAY_STORE_OBJECT:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  {");
        SPVM_STRING_BUFFER_add(string_buffer, "    void* array = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t index = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (__builtin_expect(array == NULL, 0)) { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "      env->set_exception(env, env->new_string(env, \"Array must not be undef\", 0)); \n");
        SPVM_STRING_BUFFER_add(string_buffer, "      exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    else { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "      if (__builtin_expect(index < 0 || index >= *(int32_t*)((intptr_t)array + (intptr_t)env->object_elements_length_byte_offset), 0)) { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "          env->set_exception(env, env->new_string(env, \"Index is out of range\", 0)); \n");
        SPVM_STRING_BUFFER_add(string_buffer, "          exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      } \n");
        SPVM_STRING_BUFFER_add(string_buffer, "      else {\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "        SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(\n");
        SPVM_STRING_BUFFER_add(string_buffer, "          &*(void**)((intptr_t)array + (intptr_t)env->object_header_byte_size + sizeof(void*) * index),\n");
        SPVM_STRING_BUFFER_add(string_buffer, "        ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, "\n");
        SPVM_STRING_BUFFER_add(string_buffer, "        );\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }");
        
        break;
      }
      case SPVM_OPCODE_C_ID_ARRAY_STORE_UNDEF:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Array must not be undef\", 0)); \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  } \n");
        SPVM_STRING_BUFFER_add(string_buffer, "  else { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (__builtin_expect(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " < 0 || ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "  >= *(int32_t*)((intptr_t)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_elements_length_byte_offset), 0)) { \n");
        SPVM_STRING_BUFFER_add(string_buffer, "        env->set_exception(env, env->new_string(env, \"Index is out of range\", 0)); \n");
        SPVM_STRING_BUFFER_add(string_buffer, "        exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    } \n");
        SPVM_STRING_BUFFER_add(string_buffer, "    else {\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "      SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(\n");
        SPVM_STRING_BUFFER_add(string_buffer, "        &*(void**)((intptr_t)array + (intptr_t)env->object_header_byte_size + sizeof(SPVM_OBJECT*) * \n");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "),\n");
        SPVM_STRING_BUFFER_add(string_buffer, "        NULL");
        SPVM_STRING_BUFFER_add(string_buffer, "      );\n");

        SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_MOVE_BYTE:
        SPVM_CSOURCE_BUILDER_add_move(string_buffer, "SPVM_VALUE_byte", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_MOVE_SHORT:
        SPVM_CSOURCE_BUILDER_add_move(string_buffer, "SPVM_VALUE_short", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_MOVE_INT:
        SPVM_CSOURCE_BUILDER_add_move(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_MOVE_LONG:
        SPVM_CSOURCE_BUILDER_add_move(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_MOVE_FLOAT:
        SPVM_CSOURCE_BUILDER_add_move(string_buffer, "SPVM_VALUE_float", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_MOVE_DOUBLE:
        SPVM_CSOURCE_BUILDER_add_move(string_buffer, "SPVM_VALUE_double", opcode->operand0, opcode->operand1);
        break;
      case SPVM_OPCODE_C_ID_MOVE_OBJECT:
          SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
          SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
          SPVM_STRING_BUFFER_add(string_buffer, ", ");
          SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
          SPVM_STRING_BUFFER_add(string_buffer, ");\n");
        break;
      case SPVM_OPCODE_C_ID_MOVE_UNDEF:
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", NULL);\n");
        break;
      case SPVM_OPCODE_C_ID_PUSH_MORTAL: {
        SPVM_STRING_BUFFER_add(string_buffer, "  mortal_stack_top++;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  *(int32_t*)&mortal_stack[mortal_stack_top] = ");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }

      case SPVM_OPCODE_C_ID_LEAVE_SCOPE: {
        if (sub->mortal_stack_max > 0) {
          SPVM_STRING_BUFFER_add(string_buffer, "  {\n");
          SPVM_STRING_BUFFER_add(string_buffer, "    int32_t mortal_stack_current_base = ");
          SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
          SPVM_STRING_BUFFER_add(string_buffer, ";\n");
          SPVM_STRING_BUFFER_add(string_buffer, "    {\n");
          SPVM_STRING_BUFFER_add(string_buffer, "      int32_t object_var_index_index;\n");
          SPVM_STRING_BUFFER_add(string_buffer, "      for (object_var_index_index = mortal_stack_current_base; object_var_index_index <= mortal_stack_top; object_var_index_index++) {\n");
          SPVM_STRING_BUFFER_add(string_buffer, "        int32_t var_index = *(int32_t*)&mortal_stack[object_var_index_index];\n");
          SPVM_STRING_BUFFER_add(string_buffer, "        if (*(void**)&vars[var_index] != NULL) {\n");
          SPVM_STRING_BUFFER_add(string_buffer, "          if (SPVM_RUNTIME_C_INLINE_GET_REF_COUNT(*(void**)&vars[var_index]) > 1) { SPVM_RUNTIME_C_INLINE_DEC_REF_COUNT_ONLY(*(void**)&vars[var_index]); }\n");
          SPVM_STRING_BUFFER_add(string_buffer, "          else { env->dec_ref_count(env, *(void**)&vars[var_index]); }\n");
          SPVM_STRING_BUFFER_add(string_buffer, "          *(void**)&vars[var_index] = NULL;\n");
          SPVM_STRING_BUFFER_add(string_buffer, "        }\n");
          SPVM_STRING_BUFFER_add(string_buffer, "      }\n");
          SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
          SPVM_STRING_BUFFER_add(string_buffer, "    mortal_stack_top = mortal_stack_current_base - 1;\n");
          SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        }
        break;
      }
      case SPVM_OPCODE_C_ID_NEW_OBJECT: {
        int32_t basic_type_id = opcode->operand1;
        SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);
        SPVM_STRING_BUFFER_add(string_buffer, "  {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t basic_type_id = -1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (basic_type_id == -1) { basic_type_id = env->get_basic_type_id(env, \"");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)basic_type->name);
        SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    void* object = env->new_object(env, basic_type_id);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", object);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");

        break;
      }
      case SPVM_OPCODE_C_ID_NEW_BYTE_ARRAY:
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add(string_buffer, "env->new_byte_array(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        break;
      case SPVM_OPCODE_C_ID_NEW_SHORT_ARRAY:
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add(string_buffer, "env->new_short_array(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        break;
      case SPVM_OPCODE_C_ID_NEW_INT_ARRAY:
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add(string_buffer, "env->new_int_array(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        break;
      case SPVM_OPCODE_C_ID_NEW_LONG_ARRAY:
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add(string_buffer, "env->new_long_array(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        break;
      case SPVM_OPCODE_C_ID_NEW_FLOAT_ARRAY:
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add(string_buffer, "env->new_float_array(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        break;
      case SPVM_OPCODE_C_ID_NEW_DOUBLE_ARRAY:
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add(string_buffer, "env->new_double_array(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        break;
      case SPVM_OPCODE_C_ID_NEW_OBJECT_ARRAY: {
        int32_t basic_type_id = opcode->operand1;
        SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);

        SPVM_STRING_BUFFER_add(string_buffer, "  {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t basic_type_id = -1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (basic_type_id == -1) { basic_type_id = env->get_basic_type_id(env, \"");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)basic_type->name);
        SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", env->new_object_array(env, basic_type_id, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        break;
      }
      case SPVM_OPCODE_C_ID_NEW_MULTI_ARRAY: {
        int32_t basic_type_id = (uint32_t)opcode->operand1 & 0xFFFFFF;
        int32_t element_dimension  = (uint32_t)opcode->operand1 >> 24;
        
        SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);
        
        SPVM_STRING_BUFFER_add(string_buffer, "  {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t basic_type_id = -1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (basic_type_id == -1) { basic_type_id = env->get_basic_type_id(env, \"");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)basic_type->name);
        SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t element_dimension = ");
        SPVM_STRING_BUFFER_add_int(string_buffer, element_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", env->new_multi_array(env, basic_type_id, element_dimension, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_NEW_STRING: {
        int32_t constant_id = opcode->operand1;
        
        SPVM_OP* op_constant = SPVM_LIST_fetch(compiler->op_constants, constant_id);
        SPVM_CONSTANT* constant = op_constant->uv.constant;

        const char* name = constant->value.oval;
        int32_t length = constant->string_length;
        
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add(string_buffer, "env->new_string(env, \"");
        
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SPVM_STRING_BUFFER_add_hex_char(string_buffer, name[i]);
          }
        }
        
        SPVM_STRING_BUFFER_add(string_buffer, "\", ");
        SPVM_STRING_BUFFER_add_int(string_buffer, length);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        break;
      }
      case SPVM_OPCODE_C_ID_ARRAY_LENGTH: {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " == NULL) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Can't get array length of undef value.\", 0));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = *(int32_t*)((intptr_t)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_elements_length_byte_offset);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        break;
      }
      case SPVM_OPCODE_C_ID_GET_FIELD_BYTE: {
        SPVM_CSOURCE_BUILDER_add_get_field(string_buffer, "SPVM_VALUE_byte", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_GET_FIELD_SHORT: {
        SPVM_CSOURCE_BUILDER_add_get_field(string_buffer, "SPVM_VALUE_short", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_GET_FIELD_INT: {
        SPVM_CSOURCE_BUILDER_add_get_field(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_GET_FIELD_LONG: {
        SPVM_CSOURCE_BUILDER_add_get_field(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_GET_FIELD_FLOAT: {
        SPVM_CSOURCE_BUILDER_add_get_field(string_buffer, "SPVM_VALUE_float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_GET_FIELD_DOUBLE: {
        SPVM_CSOURCE_BUILDER_add_get_field(string_buffer, "SPVM_VALUE_double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_GET_FIELD_OBJECT: {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Object must be not undef.\", 0));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", *(void**)((intptr_t)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(SPVM_VALUE) * ");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, "));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_BYTE: {
        SPVM_CSOURCE_BUILDER_add_set_field(string_buffer, "SPVM_VALUE_byte", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_SHORT: {
        SPVM_CSOURCE_BUILDER_add_set_field(string_buffer, "SPVM_VALUE_short", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_INT: {
        SPVM_CSOURCE_BUILDER_add_set_field(string_buffer, "SPVM_VALUE_int", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_LONG: {
        SPVM_CSOURCE_BUILDER_add_set_field(string_buffer, "SPVM_VALUE_long", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_FLOAT: {
        SPVM_CSOURCE_BUILDER_add_set_field(string_buffer, "SPVM_VALUE_float", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_DOUBLE: {
        SPVM_CSOURCE_BUILDER_add_set_field(string_buffer, "SPVM_VALUE_double", opcode->operand0, opcode->operand1, opcode->operand2);
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_OBJECT:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Object must be not undef.\", 0));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      (void**)((intptr_t)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(SPVM_VALUE) * ");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "),\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, "    );\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_SET_FIELD_UNDEF:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (__builtin_expect(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " == NULL, 0)) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->new_string(env, \"Object must be not undef.\", 0));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  else {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      (void**)((intptr_t)");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " + (intptr_t)env->object_header_byte_size + sizeof(SPVM_VALUE) * ");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "),\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      NULL");
        SPVM_STRING_BUFFER_add(string_buffer, "    );\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_WEAKEN_FIELD_OBJECT: {
        SPVM_STRING_BUFFER_add(string_buffer, "  env->weaken_object_field(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ");\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  if (env->get_exception(env)) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        break;
      }
      case SPVM_OPCODE_C_ID_CONCAT:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    void* concat_string = env->concat(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ", ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, ");\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "    if (concat_string == NULL) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    else {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    &");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", concat_string);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_GOTO: {
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_IF_EQ_ZERO: {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (condition_flag == 0) { goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, "; }\n");
        break;
      }
      case SPVM_OPCODE_C_ID_IF_NE_ZERO: {
        SPVM_STRING_BUFFER_add(string_buffer, "  if (condition_flag) { goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, "; }\n");
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_EXCEPTION_VAR: {
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", env->get_exception(env));\n");
        break;
      }
      case SPVM_OPCODE_C_ID_STORE_EXCEPTION_VAR: {
        SPVM_STRING_BUFFER_add(string_buffer, "  env->set_exception(env, ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ");\n");
        break;
      }
      case SPVM_OPCODE_C_ID_STORE_EXCEPTION_VAR_UNDEF: {
        SPVM_STRING_BUFFER_add(string_buffer, "  env->set_exception(env, NULL);\n");
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_BYTE:
      case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_SHORT:
      case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_INT:
      case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_LONG:
      case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_FLOAT:
      case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_DOUBLE:
      {
        char* package_var_type = NULL;
        switch (opcode->id) {
          case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_BYTE:
            package_var_type = "SPVM_VALUE_byte";
            break;
          case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_SHORT:
            package_var_type = "SPVM_VALUE_short";
            break;
          case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_INT:
            package_var_type = "SPVM_VALUE_int";
            break;
          case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_LONG:
            package_var_type = "SPVM_VALUE_long";
            break;
          case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_FLOAT:
            package_var_type = "float";
            break;
          case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_DOUBLE:
            package_var_type = "double";
            break;
        }
        
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, package_var_type, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, " = *(");
        SPVM_STRING_BUFFER_add(string_buffer, package_var_type);
        SPVM_STRING_BUFFER_add(string_buffer, "*)");
        SPVM_STRING_BUFFER_add(string_buffer, "&(*(SPVM_VALUE**)(env->get_runtime(env) + ");
        SPVM_STRING_BUFFER_add_int(string_buffer, offsetof(SPVM_RUNTIME, package_vars));
        SPVM_STRING_BUFFER_add(string_buffer, "))[");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "]");
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_OBJECT: {
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", *(void**)");
        SPVM_STRING_BUFFER_add(string_buffer, "&(*(SPVM_VALUE**)(env->get_runtime(env) + ");
        SPVM_STRING_BUFFER_add_int(string_buffer, offsetof(SPVM_RUNTIME, package_vars));
        SPVM_STRING_BUFFER_add(string_buffer, "))[");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, "])");
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_BYTE:
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_SHORT:
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_INT:
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_LONG:
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_FLOAT:
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_DOUBLE:
      {
        char* package_var_type = NULL;
        switch (opcode->id) {
          case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_BYTE:
            package_var_type = "SPVM_VALUE_byte";
            break;
          case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_SHORT:
            package_var_type = "SPVM_VALUE_short";
            break;
          case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_INT:
            package_var_type = "SPVM_VALUE_int";
            break;
          case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_LONG:
            package_var_type = "SPVM_VALUE_long";
            break;
          case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_FLOAT:
            package_var_type = "float";
            break;
          case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_DOUBLE:
            package_var_type = "double";
            break;
        }
        
        SPVM_STRING_BUFFER_add(string_buffer, "  *(");
        SPVM_STRING_BUFFER_add(string_buffer, package_var_type);
        SPVM_STRING_BUFFER_add(string_buffer, "*)");
        SPVM_STRING_BUFFER_add(string_buffer, "&(*(SPVM_VALUE**)(env->get_runtime(env) + ");
        SPVM_STRING_BUFFER_add_int(string_buffer, offsetof(SPVM_RUNTIME, package_vars));
        SPVM_STRING_BUFFER_add(string_buffer, "))[");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, "]");
        SPVM_STRING_BUFFER_add(string_buffer, " = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, package_var_type, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_OBJECT: {
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN((void**)&(*(SPVM_VALUE**)(env->get_runtime(env) + ");
        SPVM_STRING_BUFFER_add_int(string_buffer, offsetof(SPVM_RUNTIME, package_vars));        SPVM_STRING_BUFFER_add(string_buffer, "))[\n");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, "],\n");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ");");
        
        break;
      }
      case SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_UNDEF: {
        SPVM_STRING_BUFFER_add(string_buffer, "  SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN((void**)&(*(SPVM_VALUE**)(env->get_runtime(env) + ");
        SPVM_STRING_BUFFER_add_int(string_buffer, offsetof(SPVM_RUNTIME, package_vars));        SPVM_STRING_BUFFER_add(string_buffer, "))[\n");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, "], NULL);\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_PUSH_ARG_BYTE:
      case SPVM_OPCODE_C_ID_PUSH_ARG_SHORT:
      case SPVM_OPCODE_C_ID_PUSH_ARG_INT:
      case SPVM_OPCODE_C_ID_PUSH_ARG_LONG:
      case SPVM_OPCODE_C_ID_PUSH_ARG_FLOAT:
      case SPVM_OPCODE_C_ID_PUSH_ARG_DOUBLE:
      case SPVM_OPCODE_C_ID_PUSH_ARG_OBJECT:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  call_sub_arg_stack_top++;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  ");
        switch (opcode->id) {
          case SPVM_OPCODE_C_ID_PUSH_ARG_BYTE:
            SPVM_STRING_BUFFER_add(string_buffer, "*(SPVM_VALUE_byte*)&");
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_SHORT:
            SPVM_STRING_BUFFER_add(string_buffer, "*(SPVM_VALUE_short*)&");
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_INT:
            SPVM_STRING_BUFFER_add(string_buffer, "*(SPVM_VALUE_int*)&");
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_LONG:
            SPVM_STRING_BUFFER_add(string_buffer, "*(SPVM_VALUE_long*)&");
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_FLOAT:
            SPVM_STRING_BUFFER_add(string_buffer, "*(SPVM_VALUE_float*)&");
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_DOUBLE:
            SPVM_STRING_BUFFER_add(string_buffer, "*(SPVM_VALUE_double*)&");
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_OBJECT:
            SPVM_STRING_BUFFER_add(string_buffer, "*(void**)&");
            break;
        }
        SPVM_STRING_BUFFER_add(string_buffer, "args[call_sub_arg_stack_top] = ");
        switch (opcode->id) {
          case SPVM_OPCODE_C_ID_PUSH_ARG_BYTE:
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_byte", opcode->operand0);
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_SHORT:
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_short", opcode->operand0);
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_INT:
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand0);
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_LONG:
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_long", opcode->operand0);
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_FLOAT:
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_float", opcode->operand0);
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_DOUBLE:
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_double", opcode->operand0);
            break;
          case SPVM_OPCODE_C_ID_PUSH_ARG_OBJECT:
            SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
            break;
        }
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_PUSH_ARG_UNDEF:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  call_sub_arg_stack_top++;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "*(void**)&args[call_sub_arg_stack_top] = NULL;\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_CHECK_CAST: {
        int32_t cast_basic_type_id = (uint32_t)opcode->operand2 & 0xFFFFFF;
        int32_t cast_type_dimension  = (uint32_t)opcode->operand2 >> 24;

        SPVM_BASIC_TYPE* cast_basic_type = SPVM_LIST_fetch(compiler->basic_types, cast_basic_type_id);
        
        SPVM_STRING_BUFFER_add(string_buffer, "  {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t cast_basic_type_id = -1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (cast_basic_type_id == -1) { cast_basic_type_id = env->get_basic_type_id(env, \"");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)cast_basic_type->name);
        SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t cast_type_dimension = ");
        SPVM_STRING_BUFFER_add_int(string_buffer, cast_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");

        SPVM_STRING_BUFFER_add(string_buffer, "    void* object = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t can_assign = env->check_cast(env, cast_basic_type_id, cast_type_dimension, object);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (can_assign) { SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ", object); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    else {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      void* exception = env->new_string(env, \"Can't cast uncompatible type.\", 0);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      env->set_exception(env, exception);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "      exception_flag = 1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_CALL_SUB:
      case SPVM_OPCODE_C_ID_CALL_INTERFACE_METHOD:
      {
        int32_t decl_sub_id = opcode->operand1;
        SPVM_OP* op_sub_decl = SPVM_LIST_fetch(compiler->op_subs, decl_sub_id);
        SPVM_SUB* decl_sub = op_sub_decl->uv.sub;
        
        // Declare subroutine return type
        SPVM_TYPE* decl_sub_return_type = decl_sub->op_return_type->uv.type;
        int32_t decl_sub_return_type_is_object = SPVM_TYPE_is_object(compiler, decl_sub_return_type);
        
        // Declare subroutine return type id
        int32_t decl_sub_return_basic_type_id = decl_sub_return_type->basic_type->id;
        int32_t decl_sub_return_type_dimension = decl_sub_return_type->dimension;
        
        // Declare subroutine argument length
        int32_t decl_sub_args_length = decl_sub->op_args->length;

        SPVM_STRING_BUFFER_add(string_buffer, "  // ");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)decl_sub->abs_name);
        SPVM_STRING_BUFFER_add(string_buffer, "\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  {\n");

        // Call subroutine id
        if (opcode->id == SPVM_OPCODE_C_ID_CALL_SUB) {
          SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t call_sub_id = -1;\n");
          SPVM_STRING_BUFFER_add(string_buffer, "    if (call_sub_id == -1) { call_sub_id = env->get_sub_id(env, \"");
          SPVM_STRING_BUFFER_add(string_buffer, (char*)decl_sub->abs_name);
          SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        }
        else if (opcode->id == SPVM_OPCODE_C_ID_CALL_INTERFACE_METHOD) {
          SPVM_STRING_BUFFER_add(string_buffer, "    void* object = *(void**)&vars[");
          SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
          SPVM_STRING_BUFFER_add(string_buffer, "];\n");
          
          SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t decl_sub_id = -1;\n");
          SPVM_STRING_BUFFER_add(string_buffer, "    if (decl_sub_id == -1) { decl_sub_id = env->get_sub_id(env, \"");
          SPVM_STRING_BUFFER_add(string_buffer, (char*)decl_sub->abs_name);
          SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
          SPVM_STRING_BUFFER_add(string_buffer, "    int32_t call_sub_id = env->get_sub_id_interface_method(env, object, decl_sub_id);\n");
        }
        else {
          assert(0);
        }
        
        // Subroutine inline expantion in same package
        if (decl_sub->op_package->uv.package->id == sub->op_package->uv.package->id && decl_sub->have_precompile_desc) {
          SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = SPVM_PRECOMPILE_");
          SPVM_STRING_BUFFER_add(string_buffer, (char*)decl_sub->abs_name);
          {
            int32_t index = string_buffer->length - strlen(decl_sub->abs_name);
            
            while (index < string_buffer->length) {
              if (string_buffer->buffer[index] == ':') {
                string_buffer->buffer[index] = '_';
              }
              index++;
            }
          }
          SPVM_STRING_BUFFER_add(string_buffer, "(env, args);\n");
        }
        // Call subroutine
        else {
          SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = env->call_sub(env, call_sub_id, args);\n");
        }
        
        // Call subroutine
        SPVM_STRING_BUFFER_add(string_buffer, "    if (!exception_flag) {");
        if (decl_sub_return_type_is_object) {
          SPVM_STRING_BUFFER_add(string_buffer, " SPVM_RUNTIME_C_INLINE_OBJECT_ASSIGN(&");
          SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "void*", opcode->operand0);
          SPVM_STRING_BUFFER_add(string_buffer, ", args[0].oval);");
        }
        else if ((decl_sub_return_type_dimension == 0 && decl_sub_return_basic_type_id != SPVM_BASIC_TYPE_C_ID_VOID)) {
          SPVM_CSOURCE_BUILDER_add_var(string_buffer, opcode->operand0);
          SPVM_STRING_BUFFER_add(string_buffer, " = args[0]; ");
        }
        SPVM_STRING_BUFFER_add(string_buffer, "}\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "    call_sub_arg_stack_top -= ");
        SPVM_STRING_BUFFER_add_int(string_buffer, decl_sub_args_length);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_IF_CROAK_CATCH: {
        int32_t sub_id = opcode->operand1;
        SPVM_OP* op_sub = SPVM_LIST_fetch(compiler->op_subs, sub_id);
        SPVM_SUB* sub = op_sub->uv.sub;
        
        SPVM_STRING_BUFFER_add(string_buffer, "  if (exception_flag) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t sub_id = -1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (sub_id == -1) { sub_id = env->get_sub_id(env, \"");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)sub->abs_name);
        SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t line = ");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    exception_flag = 0;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->create_exception_stack_trace(env, env->get_exception(env), sub_id, line));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer,  opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_IF_CROAK_RETURN: {
        int32_t sub_id = opcode->operand1;
        SPVM_OP* op_sub = SPVM_LIST_fetch(compiler->op_subs, sub_id);
        SPVM_SUB* sub = op_sub->uv.sub;
        
        SPVM_STRING_BUFFER_add(string_buffer, "  if (exception_flag) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    static int32_t sub_id = -1;\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    if (sub_id == -1) { sub_id = env->get_sub_id(env, \"");
        SPVM_STRING_BUFFER_add(string_buffer, (char*)sub->abs_name);
        SPVM_STRING_BUFFER_add(string_buffer, "\"); }\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    int32_t line = ");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand2);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, env->create_exception_stack_trace(env, env->get_exception(env), sub_id, line));\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        break;
      }
      case SPVM_OPCODE_C_ID_SET_CROAK_FLAG_TRUE: {
        SPVM_STRING_BUFFER_add(string_buffer, "  exception_flag = 1;\n");
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_VOID:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_BYTE:
      {
        const char* return_type_name = SPVM_CSOURCE_BUILDER_get_type_name(sub_return_basic_type_id, sub_return_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, "  *(SPVM_VALUE_byte*)&args[0] = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, return_type_name, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_SHORT:
      {
        const char* return_type_name = SPVM_CSOURCE_BUILDER_get_type_name(sub_return_basic_type_id, sub_return_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, "  *(SPVM_VALUE_short*)&args[0] = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, return_type_name, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_INT:
      {
        const char* return_type_name = SPVM_CSOURCE_BUILDER_get_type_name(sub_return_basic_type_id, sub_return_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, "  *(SPVM_VALUE_int*)&args[0] = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, return_type_name, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_LONG:
      {
        const char* return_type_name = SPVM_CSOURCE_BUILDER_get_type_name(sub_return_basic_type_id, sub_return_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, "  *(SPVM_VALUE_long*)&args[0] = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, return_type_name, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_FLOAT:
      {
        const char* return_type_name = SPVM_CSOURCE_BUILDER_get_type_name(sub_return_basic_type_id, sub_return_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, "  *(SPVM_VALUE_float*)&args[0] = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, return_type_name, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_DOUBLE:
      {
        const char* return_type_name = SPVM_CSOURCE_BUILDER_get_type_name(sub_return_basic_type_id, sub_return_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, "  *(SPVM_VALUE_double*)&args[0] = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, return_type_name, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_OBJECT:
      {
        const char* return_type_name = SPVM_CSOURCE_BUILDER_get_type_name(sub_return_basic_type_id, sub_return_type_dimension);
        SPVM_STRING_BUFFER_add(string_buffer, "  *(void**)&args[0] = ");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, return_type_name, opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        // Increment ref count of return value not to release by decrement
        SPVM_STRING_BUFFER_add(string_buffer, "  if (*(void**)&args[0] != NULL) {\n");
        SPVM_STRING_BUFFER_add(string_buffer, "    SPVM_RUNTIME_C_INLINE_INC_REF_COUNT_ONLY(*(void**)&args[0]);\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_RETURN_UNDEF:
      {
        SPVM_STRING_BUFFER_add(string_buffer, "  *(void**)&args[0] = NULL;\n");
        
        SPVM_STRING_BUFFER_add(string_buffer, "  goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, opcode->operand1);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        
        break;
      }
      case SPVM_OPCODE_C_ID_TABLE_SWITCH: {
        // 1  default
        // 5  npare
        // 9  match1 offset1 // min
        // 17 match2 offset2
        // 25 match3 offset3 // max
        
        // default offset
        int32_t default_branch = opcode->operand1;
        
        int32_t min = (opcode + 1)->operand0;
        
        int32_t max = (opcode + 1)->operand1;
        
        // case count
        int32_t length = max - min + 1;
        
        SPVM_STRING_BUFFER_add(string_buffer, "  switch(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ") {\n");
        {
          int32_t case_index;
          for (case_index = 0; case_index < length; case_index++) {
            int32_t branch = (opcode + 2 + case_index)->operand1;
            
            SPVM_STRING_BUFFER_add(string_buffer, "    case ");
            SPVM_STRING_BUFFER_add_int(string_buffer, min + case_index);
            SPVM_STRING_BUFFER_add(string_buffer, ": goto L");
            SPVM_STRING_BUFFER_add_int(string_buffer, branch);
            SPVM_STRING_BUFFER_add(string_buffer, ";\n");
          }
        }
        SPVM_STRING_BUFFER_add(string_buffer, "    default: goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, default_branch);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");

        opcode_index += (2 + length);
        continue;
      }
      case SPVM_OPCODE_C_ID_LOOKUP_SWITCH: {
        // 1  default
        // 5  npare
        // 9  match1 offset1 // min
        // 17 match2 offset2
        // 25 match3 offset3 // max
        
        // default offset
        int32_t default_branch = opcode->operand1;
        
        // case count
        int32_t case_count = opcode->operand2;
        
        SPVM_STRING_BUFFER_add(string_buffer, "  switch(");
        SPVM_CSOURCE_BUILDER_add_operand(string_buffer, "SPVM_VALUE_int", opcode->operand0);
        SPVM_STRING_BUFFER_add(string_buffer, ") {\n");
        {
          int32_t case_index;
          for (case_index = 0; case_index < case_count; case_index++) {
            int32_t match = (opcode + 1 + case_index)->operand0;
            int32_t branch = (opcode + 1 + case_index)->operand1;
            
            SPVM_STRING_BUFFER_add(string_buffer, "    case ");
            SPVM_STRING_BUFFER_add_int(string_buffer, match);
            SPVM_STRING_BUFFER_add(string_buffer, ": goto L");
            SPVM_STRING_BUFFER_add_int(string_buffer, branch);
            SPVM_STRING_BUFFER_add(string_buffer, ";\n");
          }
        }
        SPVM_STRING_BUFFER_add(string_buffer, "    default: goto L");
        SPVM_STRING_BUFFER_add_int(string_buffer, default_branch);
        SPVM_STRING_BUFFER_add(string_buffer, ";\n");
        SPVM_STRING_BUFFER_add(string_buffer, "  }\n");

        opcode_index += (1 + case_count);
        continue;
      }
    }
    opcode_index++;
  }

  // No exception
  SPVM_STRING_BUFFER_add(string_buffer, "  if (!exception_flag) {\n");
  if (sub_return_type_is_object) {
    SPVM_STRING_BUFFER_add(string_buffer, "    if (args[0].oval != NULL) { SPVM_RUNTIME_C_INLINE_DEC_REF_COUNT_ONLY(args[0].oval); }\n");
  }
  SPVM_STRING_BUFFER_add(string_buffer, "    env->set_exception(env, NULL);\n");
  SPVM_STRING_BUFFER_add(string_buffer, "  }\n");
  
  SPVM_STRING_BUFFER_add(string_buffer, "  return exception_flag;\n");
  
  // Close subroutine
  SPVM_STRING_BUFFER_add(string_buffer, "}\n");
  SPVM_STRING_BUFFER_add(string_buffer, "\n");
}
