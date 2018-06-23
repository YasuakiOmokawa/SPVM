#ifndef SPVM_SWITCH_INFO_H
#define SPVM_SWITCH_INFO_H

#include "spvm_base.h"

enum {
  SPVM_SWITCH_INFO_C_ID_TABLE_SWITCH,
  SPVM_SWITCH_INFO_C_ID_LOOKUP_SWITCH,
};

// Parser information
struct SPVM_switch_info {
  SPVM_LIST* op_cases;
  SPVM_LIST* op_cases_ordered;
  SPVM_OP* op_default;
  int32_t id;
  int32_t opcode_rel_index;
  int32_t default_goto_opcode_rel_index;
  SPVM_LIST* case_goto_opcode_rel_indexes;
  int32_t rel_id;
};

SPVM_SWITCH_INFO* SPVM_SWITCH_INFO_new(SPVM_COMPILER* compiler);

#endif
