#ifndef SPVM_PACKAGE_H
#define SPVM_PACKAGE_H

#include "spvm_base.h"

enum {
  SPVM_PACKAGE_C_CATEGORY_CLASS,
  SPVM_PACKAGE_C_CATEGORY_INTERFACE,
  SPVM_PACKAGE_C_CATEGORY_STRUCT,
};

extern const char* const SPVM_PACKAGE_C_CATEGORY_NAMES[];

struct SPVM_package {
  SPVM_OP* op_name;
  SPVM_LIST* op_subs;
  SPVM_LIST* op_fields;
  SPVM_HASH* op_field_symtable;
  SPVM_LIST* object_field_ids;
  SPVM_HASH* op_our_symtable;
  SPVM_LIST* op_ours;
  SPVM_HASH* method_signature_symtable;
  SPVM_HASH* has_interface_cache_symtable;
  SPVM_OP* op_sub_destructor;
  const char* load_path;
  SPVM_OP* op_type;
  int32_t id;
  _Bool is_private;
  _Bool is_anon;
  int32_t category;
};

SPVM_PACKAGE* SPVM_PACKAGE_new(SPVM_COMPILER* compiler);

int32_t SPVM_PACKAGE_get_object_fields_length(SPVM_COMPILER* compiler, SPVM_PACKAGE* package);

#endif
