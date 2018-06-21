#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>


#include "spvm_compiler.h"
#include "spvm_dumper.h"
#include "spvm_list.h"
#include "spvm_hash.h"
#include "spvm_constant.h"
#include "spvm_field.h"
#include "spvm_sub.h"
#include "spvm_my.h"
#include "spvm_var.h"
#include "spvm_op.h"
#include "spvm_enumeration_value.h"
#include "spvm_type.h"
#include "spvm_enumeration.h"
#include "spvm_package.h"
#include "spvm_type.h"
#include "spvm_opcode.h"
#include "spvm_package_var.h"
#include "spvm_package_var_access.h"
#include "spvm_opcode_array.h"
#include "spvm_block.h"
#include "spvm_basic_type.h"

void SPVM_DUMPER_dump_ast(SPVM_COMPILER* compiler, SPVM_OP* op_base) {
  int32_t indent = 8;
  
  // Run OPs
  SPVM_OP* op_cur = op_base;
  _Bool finish = 0;
  while (op_cur) {
    // [START]Preorder traversal position
    {
      int32_t i;
      for (i = 0; i < indent; i++) {
        printf(" ");
      }
    }
    int32_t id = op_cur->id;
    printf("%s", SPVM_OP_C_ID_NAMES[id]);
    if (op_cur->id == SPVM_OP_C_ID_CONSTANT) {
      SPVM_CONSTANT* constant = op_cur->uv.constant;
      printf(" %s", SPVM_BASIC_TYPE_C_ID_NAMES[constant->type->basic_type->id]);
      if (constant->type->dimension == 0) {
        switch (constant->type->basic_type->id) {
          case SPVM_BASIC_TYPE_C_ID_BYTE:
            printf(" %" PRId8, constant->value.bval);
            break;
          case SPVM_BASIC_TYPE_C_ID_SHORT:
            printf(" %" PRId16, constant->value.sval);
            break;
          case SPVM_BASIC_TYPE_C_ID_INT:
            printf(" %" PRId32, constant->value.ival);
            break;
          case SPVM_BASIC_TYPE_C_ID_LONG:
            printf(" %" PRId64, constant->value.lval);
            break;
          case SPVM_BASIC_TYPE_C_ID_FLOAT:
            printf(" %f", constant->value.fval);
            break;
          case SPVM_BASIC_TYPE_C_ID_DOUBLE:
            printf(" %f", constant->value.dval);
            break;
        }
      }
      else if (constant->type->dimension == 1 && constant->type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
        printf(" \"%s\"", (char*)constant->value.oval);
        break;
      }
      printf(" (index %" PRId32 ")", constant->id);
    }
    else if (id == SPVM_OP_C_ID_MY) {
      SPVM_MY* my = op_cur->uv.my;
      printf(" \"%s\"", my->op_name->uv.name);
      printf(" (my->index:%d)", my->index);
    }
    else if (id == SPVM_OP_C_ID_PACKAGE_VAR) {
      SPVM_PACKAGE_VAR* package_var = op_cur->uv.package_var;
      printf(" \"%s\"", package_var->op_package_var_access->uv.package_var_access->op_name->uv.name);
      printf(" (id :%d)", package_var->id);
    }
    else if (id == SPVM_OP_C_ID_VAR) {
      SPVM_VAR* var = op_cur->uv.var;
      printf(" \"%s\"", var->op_name->uv.name);
      printf(" (my->index:%d)", var->op_my->uv.my->index);
    }
    else if (id == SPVM_OP_C_ID_PACKAGE_VAR_ACCESS) {
      SPVM_PACKAGE_VAR_ACCESS* package_var_access = op_cur->uv.package_var_access;
      printf(" \"%s\"", package_var_access->op_name->uv.name);
      printf(" (id :%d)", package_var_access->op_package_var->uv.package_var->id);
    }
    else if (id == SPVM_OP_C_ID_NAME) {
      printf(" \"%s\"", op_cur->uv.name);
    }
    else if (id == SPVM_OP_C_ID_TYPE) {
      if (op_cur->uv.type) {
        SPVM_TYPE_fprint_type_name(compiler, stdout, op_cur->uv.type->basic_type->id, op_cur->uv.type->dimension);
      }
      else {
        printf(" \"Unknown\"");
      }
    }
    else if (id == SPVM_OP_C_ID_BLOCK) {
      if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_IF) {
        printf(" IF");
      }
      else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_ELSE) {
        printf(" ELSE");
      }
      else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_LOOP_INIT) {
        printf(" LOOP_INIT");
      }
      else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_LOOP_STATEMENTS) {
        printf(" LOOP_STATEMENTS");
      }
      else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_SWITCH) {
        printf(" SWITCH");
      }
      else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_SUB) {
        printf(" SUB");
      }
      else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_EVAL) {
        printf(" EVAL");
      }
    }
    printf("\n");
    
    // [END]Preorder traversal position
    
    if (op_cur->first) {
      op_cur = op_cur->first;
      indent++;
    }
    else {
      while (1) {
        // [START]Postorder traversal position
        
        // [END]Postorder traversal position
        
        if (op_cur == op_base) {
          finish = 1;
          break;
        }
        
        // Next sibling
        if (op_cur->moresib) {
          op_cur = SPVM_OP_sibling(compiler, op_cur);
          break;
        }
        // Next is parent
        else {
          op_cur = op_cur->sibparent;
          indent--;
        }
      }
      if (finish) {
        break;
      }
    }
  }
}

void SPVM_DUMPER_dump_all(SPVM_COMPILER* compiler) {
  
  printf("\n[Basic types]\n");
  SPVM_DUMPER_dump_basic_types(compiler, compiler->basic_types);
  
  printf("\n[Packages]\n");
  SPVM_DUMPER_dump_packages(compiler, compiler->op_packages);
}

void SPVM_DUMPER_dump_constants(SPVM_COMPILER* compiler, SPVM_LIST* op_constants) {
  {
    int32_t i;
    for (i = 0; i < op_constants->length; i++) {
      SPVM_OP* op_constant = SPVM_LIST_fetch(op_constants, i);
      SPVM_CONSTANT* constant = op_constant->uv.constant;
      printf("    constant[%" PRId32 "]\n", i);
      SPVM_DUMPER_dump_constant(compiler, constant);
    }
  }
}

void SPVM_DUMPER_dump_packages(SPVM_COMPILER* compiler, SPVM_LIST* op_packages) {
  {
    int32_t i;
    for (i = 0; i < op_packages->length; i++) {
      printf("package[%" PRId32 "]\n", i);
      SPVM_OP* op_package = SPVM_LIST_fetch(op_packages, i);
      SPVM_PACKAGE* package = op_package->uv.package;
      
      if (package->op_name) {
        printf("  name => \"%s\"\n", package->op_name->uv.name);
      }
      else {
        printf("  name => \"ANON\"\n");
      }
      
      printf("  byte_size => %" PRId32 "\n", package->op_fields->length);
      
      // Field information
      printf("  fields\n");
      SPVM_LIST* op_fields = package->op_fields;
      {
        int32_t j;
        for (j = 0; j < op_fields->length; j++) {
          SPVM_OP* op_field = SPVM_LIST_fetch(op_fields, j);
          SPVM_FIELD* field = op_field->uv.field;
          printf("    field%" PRId32 "\n", j);
          SPVM_DUMPER_dump_field(compiler, field);
        }
      }
      {
        int32_t j;
        for (j = 0; j < package->op_subs->length; j++) {
          SPVM_OP* op_sub = SPVM_LIST_fetch(package->op_subs, j);
          SPVM_SUB* sub = op_sub->uv.sub;
          printf("  sub[%" PRId32 "]\n", j);
          SPVM_DUMPER_dump_sub(compiler, sub);
        }
      }
    }
  }
}

void SPVM_DUMPER_dump_basic_types(SPVM_COMPILER* compiler, SPVM_LIST* basic_types) {
  (void)compiler;
  
  {
    int32_t i;
    for (i = 0; i < basic_types->length; i++) {
      printf("basic_type[%" PRId32 "]\n", i);
      SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(basic_types, i);
      printf("    name => %s", basic_type->name);
    }
  }
}

void SPVM_DUMPER_dump_opcode_array(SPVM_COMPILER* compiler, SPVM_OPCODE_ARRAY* opcode_array, int32_t start_pos, int32_t length) {
  (void)compiler;
  
  int32_t end_pos = start_pos + length - 1;
  
  {
    int32_t i;
    for (i = start_pos; i <= end_pos; i++) {
      
      SPVM_OPCODE opcode = opcode_array->values[i];
      printf("        [%" PRId32 "] %-20s", i, SPVM_OPCODE_C_ID_NAMES[opcode.id]);
      
      // Operand
      switch (opcode.id) {
        case SPVM_OPCODE_C_ID_TABLE_SWITCH: {
          printf(" ");
          
          printf(" %d %d %d\n", opcode.operand0, opcode.operand1, opcode.operand2);
          
          i++;

          SPVM_OPCODE opcode_table_switch_range = opcode_array->values[i];
          
          int32_t min = opcode_table_switch_range.operand0;

          int32_t max = opcode_table_switch_range.operand0;
          
          int32_t length = max - min + 1;

          printf("        [%" PRId32 "] %s %d %d %d\n",
            i, SPVM_OPCODE_C_ID_NAMES[opcode_table_switch_range.id], opcode_table_switch_range.operand0, opcode_table_switch_range.operand1, opcode_table_switch_range.operand2);
          
          // Branch
          {
            int32_t j;
            for (j = 0; j < length; j++) {
              i++;
              SPVM_OPCODE opcode = opcode_array->values[i];
              printf("        [%" PRId32 "] %s %d %d %d\n",
                i, SPVM_OPCODE_C_ID_NAMES[opcode.id], opcode.operand0, opcode.operand1, opcode.operand2);
            }
          }
          
          break;
        }
        case SPVM_OPCODE_C_ID_LOOKUP_SWITCH: {
          printf(" ");
          
          printf(" %d %d %d\n",
            opcode.operand0, opcode.operand1, opcode.operand2);
          
          int32_t case_length = opcode.operand2;
          
          // Match - offset
          {
            int32_t j;
            for (j = 0; j < case_length; j++) {
              i++;
              SPVM_OPCODE opcode = opcode_array->values[i];
              printf("        [%" PRId32 "] %s %d %d %d\n",
                i, SPVM_OPCODE_C_ID_NAMES[opcode.id], opcode.operand0, opcode.operand1, opcode.operand2);
            }
          }
          
          break;
        }
        default :
        // Have seven operands
        {
          printf(" %d %d %d\n",
            opcode.operand0, opcode.operand1, opcode.operand2);
          break;
        }
      }
    }
  }
}

void SPVM_DUMPER_dump_constant(SPVM_COMPILER* compiler, SPVM_CONSTANT* constant) {
  (void)compiler;
  
  if (constant->type->dimension == 0) {
    switch(constant->type->basic_type->id) {
      case SPVM_BASIC_TYPE_C_ID_BYTE:
        printf("      int %" PRId8 "\n", constant->value.bval);
        break;
      case SPVM_BASIC_TYPE_C_ID_SHORT:
        printf("      int %" PRId16 "\n", constant->value.sval);
        break;
      case SPVM_BASIC_TYPE_C_ID_INT:
        printf("      int %" PRId32 "\n", constant->value.ival);
        break;
      case SPVM_BASIC_TYPE_C_ID_LONG:
        printf("      long %" PRId64 "\n", constant->value.lval);
        break;
      case SPVM_BASIC_TYPE_C_ID_FLOAT:
        printf("      float %f\n", constant->value.fval);
        break;
      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
        printf("      double %f\n", constant->value.dval);
        break;
    }
  }
  else if (constant->type->dimension == 1) {
    if (constant->type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
      printf("      String \"%s\"\n", (char*)constant->value.oval);
    }
  }
  printf("      address => %" PRId32 "\n", constant->id);
}

void SPVM_DUMPER_dump_sub(SPVM_COMPILER* compiler, SPVM_SUB* sub) {
  (void)compiler;
  
  if (sub) {
    
    printf("      abs_name => \"%s\"\n", sub->abs_name);
    printf("      name => \"%s\"\n", sub->op_name->uv.name);
    printf("      return_type => ");
    SPVM_TYPE_fprint_type_name(compiler, stdout, sub->op_return_type->uv.type->basic_type->id, sub->op_return_type->uv.type->dimension);
    printf("\n");
    printf("      is_enum => %d\n", sub->is_enum);
    printf("      have_native_desc => %d\n", sub->have_native_desc);
    
    printf("      args\n");
    SPVM_LIST* op_args = sub->op_args;
    {
      int32_t i;
      for (i = 0; i < op_args->length; i++) {
        SPVM_OP* op_arg = SPVM_LIST_fetch(sub->op_args, i);
        SPVM_MY* my = op_arg->uv.my;
        printf("        [%" PRId32 "] ", i);
        SPVM_DUMPER_dump_my(compiler, my);
      }
    }
    
    if (!sub->have_native_desc) {
      printf("      mys\n");
      SPVM_LIST* op_mys = sub->op_mys;
      {
        int32_t i;
        for (i = 0; i < op_mys->length; i++) {
          SPVM_OP* op_my = SPVM_LIST_fetch(sub->op_mys, i);
          SPVM_MY* my = op_my->uv.my;
          printf("        [%" PRId32 "] ", i);
          SPVM_DUMPER_dump_my(compiler, my);
        }
      }
      
      printf("      call_sub_arg_stack_max => %" PRId32 "\n", sub->call_sub_arg_stack_max);
      
      printf("      AST\n");
      SPVM_DUMPER_dump_ast(compiler, sub->op_block);
      printf("\n");
      
      printf("      opcode_array\n");
      SPVM_DUMPER_dump_opcode_array(compiler, compiler->opcode_array, sub->opcode_base, sub->opcode_length);
    }
  }
  else {
    printf("      None\n");
  }
}

void SPVM_DUMPER_dump_field(SPVM_COMPILER* compiler, SPVM_FIELD* field) {
  (void)compiler;
  
  if (field) {
    printf("      name => \"%s\"\n", field->op_name->uv.name);
    
    printf("      index => \"%" PRId32 "\"\n", field->rel_id);
    
    SPVM_TYPE* type = field->op_type->uv.type;
    printf("      type => ");
    SPVM_TYPE_fprint_type_name(compiler, stdout, type->basic_type->id, type->dimension);
    printf("\n");
    printf("      byte_size => \"%" PRId32 "\"\n", SPVM_FIELD_get_byte_size(compiler, field));
    
    printf("      index => \"%" PRId32 "\"\n", field->rel_id);
  }
  else {
    printf("        None\n");
  }
}


void SPVM_DUMPER_dump_enumeration_value(SPVM_COMPILER* compiler, SPVM_ENUMERATION_VALUE* enumeration_value) {
  (void)compiler;
  
  if (enumeration_value) {
    printf("      name => \"%s\"\n", enumeration_value->op_name->uv.name);
    // TODO add types
    printf("      value => %" PRId32 "\n", enumeration_value->op_constant->uv.constant->value.ival);
  }
  else {
    printf("      None\n");
  }
}

void SPVM_DUMPER_dump_my(SPVM_COMPILER* compiler, SPVM_MY* my) {
  (void)compiler;

  if (my) {
    printf("name => %s, type => ", my->op_name->uv.name);
    SPVM_TYPE* type = my->op_type->uv.type;
    SPVM_TYPE_fprint_type_name(compiler, stdout, type->basic_type->id, type->dimension);
    printf("\n");
  }
  else {
    printf("(Unexpected)\n");
  }
}
