#ifndef SPVM_PACKAGE_VAR_H
#define SPVM_PACKAGE_VAR_H

#include "spvm_base.h"

// Field information
struct SPVM_package_var {
  SPVM_OP* op_name;
  SPVM_OP* op_package;
  SPVM_OP* op_package_var_access;
  SPVM_OP* op_type;
  int32_t id;
  int32_t rel_id;
  const char* abs_name;
  const char* signature;
};

SPVM_PACKAGE_VAR* SPVM_PACKAGE_VAR_new(SPVM_COMPILER* compiler);

#endif
