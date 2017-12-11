#ifndef SPVM_STRING_BUFFER_H
#define SPVM_STRING_BUFFER_H

#include "spvm_base.h"

struct SPVM_string_buffer {
  char* buffer;
  int32_t capacity;
  int32_t length;
};

SPVM_STRING_BUFFER* SPVM_STRING_BUFFER_new(int32_t capacity);
char* SPVM_STRING_BUFFER_get_buffer(SPVM_STRING_BUFFER* string_buffer);
void SPVM_STRING_BUFFER_add(SPVM_STRING_BUFFER* string_buffer, char* string);
void SPVM_STRING_BUFFER_add_int(SPVM_STRING_BUFFER* string_buffer, int32_t value);
void SPVM_STRING_BUFFER_add_address(SPVM_STRING_BUFFER* string_buffer, void* value);
void SPVM_STRING_BUFFER_free(SPVM_STRING_BUFFER* string_buffer);

#endif
