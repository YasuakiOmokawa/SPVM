#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include "spvm_compiler.h"
#include "spvm_opcode_builder.h"
#include "spvm_opcode.h"
#include "spvm_opcode_array.h"
#include "spvm_constant.h"
#include "spvm_op.h"
#include "spvm_type.h"
#include "spvm_type.h"
#include "spvm_list.h"
#include "spvm_sub.h"
#include "spvm_var.h"
#include "spvm_my.h"
#include "spvm_compiler_allocator.h"
#include "spvm_package.h"
#include "spvm_field_access.h"
#include "spvm_call_sub.h"
#include "spvm_hash.h"
#include "spvm_field.h"
#include "spvm_switch_info.h"
#include "spvm_type.h"
#include "spvm_limit.h"
#include "spvm_object.h"
#include "spvm_our.h"
#include "spvm_package_var.h"
#include "spvm_dumper.h"
#include "spvm_opcode.h"
#include "spvm_block.h"
#include "spvm_basic_type.h"

void SPVM_OPCODE_BUILDER_push_if_croak(
  SPVM_COMPILER* compiler,
  SPVM_OPCODE_ARRAY* opcode_array,
  SPVM_LIST* push_eval_opcode_index_stack,
  SPVM_LIST* if_croak_catch_goto_opcode_index_stack,
  SPVM_LIST* if_croak_return_goto_opcode_index_stack,
  int32_t sub_id,
  int32_t line
)
{
  if (push_eval_opcode_index_stack->length > 0) {
    SPVM_OPCODE opcode;
    memset(&opcode, 0, sizeof(SPVM_OPCODE));
    opcode.id = SPVM_OPCODE_C_ID_IF_CROAK_CATCH;
    opcode.operand1 = sub_id;
    opcode.operand2 = line;
    
    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
    
    int32_t* if_croak_catch_opcode_index_ptr = SPVM_COMPILER_ALLOCATOR_alloc_int(compiler);
    *if_croak_catch_opcode_index_ptr = opcode_array->length - 1;
    
    SPVM_LIST_push(if_croak_catch_goto_opcode_index_stack, if_croak_catch_opcode_index_ptr);
  }
  else {
    SPVM_OPCODE opcode;
    memset(&opcode, 0, sizeof(SPVM_OPCODE));
    opcode.id = SPVM_OPCODE_C_ID_IF_CROAK_RETURN;
    opcode.operand1 = sub_id;
    opcode.operand2 = line;
    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

    int32_t if_croak_return_opcode_index = opcode_array->length - 1;
    
    SPVM_LIST_push(if_croak_return_goto_opcode_index_stack, (void*)(intptr_t)if_croak_return_opcode_index);
  }
}

void SPVM_OPCODE_BUILDER_build_opcode_array(SPVM_COMPILER* compiler) {
  
  // Operation code
  SPVM_OPCODE_ARRAY* opcode_array = compiler->opcode_array;
  
  {
    int32_t sub_pos;
    for (sub_pos = 0; sub_pos < compiler->op_subs->length; sub_pos++) {
      SPVM_OP* op_sub = SPVM_LIST_fetch(compiler->op_subs, sub_pos);
      SPVM_SUB* sub = op_sub->uv.sub;
      
      // Check sub information
      assert(sub->id > -1);
      assert(sub->op_name);
      assert(sub->op_return_type);
      assert(sub->abs_name);
      assert(sub->file_name);
      
      if (sub->is_native) {
        continue;
      }
      
      sub->opcode_base = opcode_array->length;
      
      // Run OPs
      SPVM_OP* op_base = SPVM_OP_get_op_block_from_op_sub(compiler, op_sub);
      SPVM_OP* op_cur = op_base;
      _Bool finish = 0;
      
      // opcode index stack for if start
      SPVM_LIST* if_eq_or_if_ne_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // opcode index stack for if end
      SPVM_LIST* if_block_end_goto_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // opcode index stack for loop start
      SPVM_LIST* loop_first_goto_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // opcode index stack for last
      SPVM_LIST* last_goto_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // opcode index stack for next
      SPVM_LIST* next_goto_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // opcode index stack for eval start
      SPVM_LIST* push_eval_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // IF_CROAK_CATCH opcode index stack
      SPVM_LIST* if_croak_catch_goto_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);

      // IF_CROAK_RETURN opcode index stack
      SPVM_LIST* if_croak_return_goto_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);

      // RETURN goto opcode index stack
      SPVM_LIST* return_goto_opcode_index_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // Switch stack
      SPVM_LIST* switch_info_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // Block stack
      SPVM_LIST* op_block_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // Mortal variable stack
      SPVM_LIST* mortal_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      // Mortal variable base stack
      SPVM_LIST* mortal_base_stack = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
      
      int32_t mortal_stack_max = 0;
      
      while (op_cur) {
        // [START]Preorder traversal position
        switch (op_cur->id) {
          case SPVM_OP_C_ID_BLOCK: { // Preorder
            
            // Push block
            SPVM_LIST_push(op_block_stack, op_cur);
            
            if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_LOOP_STATEMENTS) {
              SPVM_OPCODE opcode;
              memset(&opcode, 0, sizeof(SPVM_OPCODE));
              // Add goto
              opcode.id = SPVM_OPCODE_C_ID_GOTO;
              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
              
              int32_t opcode_index = opcode_array->length - 1;
              
              SPVM_LIST_push(loop_first_goto_opcode_index_stack, (void*)(intptr_t)opcode_index);
            }
            else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_EVAL) {
              int32_t* opcode_index_ptr = SPVM_COMPILER_ALLOCATOR_alloc_int(compiler);
              *opcode_index_ptr = opcode_array->length;
              
              SPVM_LIST_push(push_eval_opcode_index_stack, opcode_index_ptr);
            }
            
            int32_t mortal_base = mortal_stack->length;
            SPVM_LIST_push(mortal_base_stack, (void*)(intptr_t)mortal_base);
          }
        }
        
        // [END]Preorder traversal position
        
        if (op_cur->first) {
          op_cur = op_cur->first;
        }
        else {
          while (1) {
            // [START]Postorder traversal position
            switch (op_cur->id) {
              case SPVM_OP_C_ID_ASSIGN: {
                SPVM_OP* op_assign_to = op_cur->last;
                SPVM_OP* op_assign_from = op_cur->first;
                
                if (op_assign_from->id == SPVM_OP_C_ID_SEQUENCE) {
                  op_assign_from = op_assign_from->last;
                }
                
                if (op_assign_to->id == SPVM_OP_C_ID_VAR) {
                  SPVM_TYPE* type_to = SPVM_OP_get_type(compiler, op_assign_to);
                  
                  // Skip if same variable assignment
                  if (op_assign_from->id == SPVM_OP_C_ID_VAR) {
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_from);
                    if (index_in == index_out) {
                      break;
                    }
                  }

                  if (0) {
                    
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_CONCAT) {
                    SPVM_TYPE* first_type = SPVM_OP_get_type(compiler, op_assign_from->first);
                    
                    assert(first_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING && first_type->dimension == 0);
                    
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    opcode.id = SPVM_OPCODE_C_ID_CONCAT;
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);

                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_UNDEF) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    opcode.id = SPVM_OPCODE_C_ID_MOVE_UNDEF;
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    opcode.operand0 = index_out;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_CALL_SUB) {
                    
                    SPVM_CALL_SUB* call_sub = op_assign_from->uv.call_sub;
                    const char* call_sub_abs_name = call_sub->sub->abs_name;
                    
                    SPVM_OP* op_sub_call_sub = SPVM_HASH_search(compiler->op_sub_symtable, call_sub_abs_name, strlen(call_sub_abs_name));
                    SPVM_SUB* sub_call_sub = op_sub_call_sub->uv.sub;

                    // Push args
                    int32_t first_arg_index = -1;
                    SPVM_OP* op_args = op_assign_from->last;
                    SPVM_OP* op_arg = op_args->first;
                    while ((op_arg = SPVM_OP_sibling(compiler, op_arg))) {
                      SPVM_OPCODE opcode;
                      memset(&opcode, 0, sizeof(SPVM_OPCODE));
                      
                      SPVM_TYPE* arg_type = SPVM_OP_get_type(compiler, op_arg);
                      
                      if (SPVM_TYPE_is_undef(compiler, arg_type)) {
                        assert(!sub_call_sub->op_package->uv.package->is_interface);
                        opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_UNDEF;
                        SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                      }
                      else {
                        if (arg_type->dimension == 0) {
                          switch (arg_type->basic_type->id) {
                            case SPVM_BASIC_TYPE_C_ID_BYTE:
                              opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_BYTE;
                              break;
                            case SPVM_BASIC_TYPE_C_ID_SHORT:
                              opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_SHORT;
                              break;
                            case SPVM_BASIC_TYPE_C_ID_INT:
                              opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_INT;
                              break;
                            case SPVM_BASIC_TYPE_C_ID_LONG:
                              opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_LONG;
                              break;
                            case SPVM_BASIC_TYPE_C_ID_FLOAT:
                              opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_FLOAT;
                              break;
                            case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                              opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_DOUBLE;
                              break;
                            default:
                              opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_OBJECT;
                          }
                        }
                        else {
                          opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_OBJECT;
                        }

                        int32_t index_arg = SPVM_OP_get_my_index(compiler, op_arg);
                        
                        opcode.operand0 = index_arg;
                        
                        if (first_arg_index == -1) {
                          first_arg_index = index_arg;
                        }
                        SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                      }
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);

                    // Call subroutine
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    opcode.operand0 = index_out;

                    if (sub_call_sub->op_package->uv.package->is_interface) {
                      opcode.id = SPVM_OPCODE_C_ID_CALL_INTERFACE_METHOD;
                      opcode.operand1 = sub_call_sub->id;
                      opcode.operand2 = first_arg_index;
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_CALL_SUB;
                      opcode.operand1 = sub_call_sub->id;
                    }

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_ARRAY_LENGTH) {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    opcode.id = SPVM_OPCODE_C_ID_ARRAY_LENGTH;
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_FIELD_ACCESS) {
                    
                    // $VAR = $VAR_OBJECT->{NAME}
                    SPVM_OP* op_field_access = op_assign_from;
                    SPVM_OP* op_term_object = op_field_access->first;
                    
                    // Call field
                    SPVM_FIELD_ACCESS* field_access = op_field_access->uv.field_access;
                    SPVM_FIELD* field = field_access->field;
                    
                    SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_field_access);
                    
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    if (type->dimension == 0) {
                      switch (type->basic_type->id) {
                        case SPVM_BASIC_TYPE_C_ID_BYTE:
                          opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_BYTE;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_SHORT:
                          opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_SHORT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_INT:
                          opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_INT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_LONG:
                          opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_LONG;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_FLOAT:
                          opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_FLOAT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                          opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_DOUBLE;
                          break;
                        default:
                          opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_GET_FIELD_OBJECT;
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_term_object = SPVM_OP_get_my_index(compiler, op_term_object);

                    opcode.operand0 = index_out;
                    opcode.operand1 = index_term_object;
                    opcode.operand2 = field->byte_offset;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_ARRAY_ACCESS) {
                    
                    // $VAR = $VAR_ARRAY->[$VAR_INDEX]
                    SPVM_OP* op_array_access = op_assign_from;
                    SPVM_OP* op_term_array = op_array_access->first;
                    SPVM_OP* op_term_index = op_array_access->last;
                    
                    SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_array_access);
                    
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    if (type->dimension == 0) {
                      switch (type->basic_type->id) {
                        case SPVM_BASIC_TYPE_C_ID_BYTE:
                          opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_BYTE;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_SHORT:
                          opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_SHORT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_INT:
                          opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_INT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_LONG:
                          opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_LONG;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_FLOAT:
                          opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_FLOAT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                          opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_DOUBLE;
                          break;
                        default:
                          opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_ARRAY_LOAD_OBJECT;
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_term_array = SPVM_OP_get_my_index(compiler, op_term_array);
                    int32_t index_term_index = SPVM_OP_get_my_index(compiler, op_term_index);

                    opcode.operand0 = index_out;
                    opcode.operand1 = index_term_array;
                    opcode.operand2 = index_term_index;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_PACKAGE_VAR) {
                    SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_assign_from);

                    // VAR = PACKAGE_VAR
                    SPVM_OUR* our = op_assign_from->uv.package_var->op_our->uv.our;
                    
                    int32_t package_var_id = our->id;
                    
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    if (type->dimension == 0) {
                      switch (type->basic_type->id) {
                        case SPVM_BASIC_TYPE_C_ID_BYTE:
                          opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_BYTE;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_SHORT:
                          opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_SHORT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_INT:
                          opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_INT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_LONG:
                          opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_LONG;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_FLOAT:
                          opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_FLOAT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                          opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_DOUBLE;
                          break;
                        default:
                          opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_LOAD_PACKAGE_VAR_OBJECT;
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = package_var_id;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_EXCEPTION_VAR) {
                    // VAR = EXCEPTION_VAR
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    opcode.id = SPVM_OPCODE_C_ID_LOAD_EXCEPTION_VAR;

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    
                    opcode.operand0 = index_out;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_ADD) {
                    
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_ADD_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_ADD_LONG;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_ADD_FLOAT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_ADD_DOUBLE;
                        break;
                      default:
                        assert(0);
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_SUBTRACT) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));

                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_SUBTRACT_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_SUBTRACT_LONG;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_SUBTRACT_FLOAT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_SUBTRACT_DOUBLE;
                        break;
                      default:
                        assert(0);
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_MULTIPLY) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));

                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_MULTIPLY_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_MULTIPLY_LONG;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_MULTIPLY_FLOAT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_MULTIPLY_DOUBLE;
                        break;
                      default:
                        assert(0);
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_DIVIDE) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_DIVIDE_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_DIVIDE_LONG;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_DIVIDE_FLOAT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_DIVIDE_DOUBLE;
                        break;
                      default:
                        assert(0);
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                    
                    // Check croak
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                      break;
                    }
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_REMAINDER) {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_REMAINDER_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_REMAINDER_LONG;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_REMAINDER_FLOAT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_REMAINDER_DOUBLE;
                        break;
                      default:
                        assert(0);
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    // Check croak
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                      break;
                    }
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_LEFT_SHIFT) {

                    

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_LEFT_SHIFT_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_LEFT_SHIFT_LONG;
                        break;
                      default:
                        assert(0);
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_RIGHT_SHIFT) {

                    

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_RIGHT_SHIFT_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_RIGHT_SHIFT_LONG;
                        break;
                      default:
                        assert(0);
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_RIGHT_SHIFT_UNSIGNED) {

                    

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_RIGHT_SHIFT_UNSIGNED_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_RIGHT_SHIFT_UNSIGNED_LONG;
                        break;
                      default:
                        assert(0);
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_BIT_AND) {

                    

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_BIT_AND_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_BIT_AND_LONG;
                        break;
                      default:
                        assert(0);
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_BIT_OR) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_BIT_OR_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_BIT_OR_LONG;
                        break;
                      default:
                        assert(0);
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_BIT_XOR) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_BIT_XOR_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_BIT_XOR_LONG;
                        break;
                      default:
                        assert(0);
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_assign_from->last);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in1;
                    opcode.operand2 = index_in2;

                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_COMPLEMENT) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));

                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_COMPLEMENT_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_COMPLEMENT_LONG;
                        break;
                      default:
                        assert(0);
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_PLUS) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                   
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_MOVE_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_MOVE_LONG;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_MOVE_FLOAT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_MOVE_DOUBLE;
                        break;
                      default:
                        assert(0);
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_NEGATE) {

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                   
                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_NEGATE_INT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_NEGATE_LONG;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_NEGATE_FLOAT;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_NEGATE_DOUBLE;
                        break;
                      default:
                        assert(0);
                    }
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from->first);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_CONSTANT) {
                    SPVM_CONSTANT* constant = op_assign_from->uv.constant;

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    opcode.operand0 = index_out;

                    assert(type_to->dimension == 0);
                    switch (type_to->basic_type->id) {
                      case SPVM_BASIC_TYPE_C_ID_BYTE:
                        opcode.id = SPVM_OPCODE_C_ID_LOAD_CONSTANT_BYTE;
                        *(SPVM_API_byte*)&opcode.operand1 = *(SPVM_API_byte*)&constant->value;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_SHORT:
                        opcode.id = SPVM_OPCODE_C_ID_LOAD_CONSTANT_SHORT;
                        *(SPVM_API_short*)&opcode.operand1 = *(SPVM_API_short*)&constant->value;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_INT:
                        opcode.id = SPVM_OPCODE_C_ID_LOAD_CONSTANT_INT;
                        *(SPVM_API_int*)&opcode.operand1 = *(SPVM_API_int*)&constant->value;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_LONG:
                        opcode.id = SPVM_OPCODE_C_ID_LOAD_CONSTANT_LONG;
                        *(SPVM_API_long*)&opcode.operand1 = *(SPVM_API_long*)&constant->value;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_FLOAT:
                        opcode.id = SPVM_OPCODE_C_ID_LOAD_CONSTANT_FLOAT;
                        *(SPVM_API_float*)&opcode.operand1 = *(SPVM_API_float*)&constant->value;
                        break;
                      case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                        opcode.id = SPVM_OPCODE_C_ID_LOAD_CONSTANT_DOUBLE;
                        *(SPVM_API_double*)&opcode.operand1 = *(SPVM_API_double*)&constant->value;
                        break;
                      default:
                        assert(0);
                    }
                    
                    assert(constant->id != -1);
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_CONVERT) {
                    
                    SPVM_OP* op_dist_term = op_assign_to;
                    SPVM_OP* op_src_term = op_assign_from->first;
                    SPVM_OP* op_dist_type = op_assign_from->last;
                    
                    SPVM_TYPE* src_type = SPVM_OP_get_type(compiler, op_src_term);
                    SPVM_TYPE* dist_type = SPVM_OP_get_type(compiler, op_dist_type);

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    if (src_type->dimension == 0 && src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                      assert(dist_type->dimension == 0);
                      if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_BYTE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_SHORT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_INT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_LONG;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_FLOAT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_DOUBLE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_TO_STRING;
                      }
                      else {
                        assert(0);
                      }
                    }
                    else if (src_type->dimension == 0 && src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                      assert(dist_type->dimension == 0);
                      if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_BYTE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_SHORT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_INT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_LONG;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_FLOAT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_DOUBLE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_TO_STRING;
                      }
                      else {
                        assert(0);
                      }
                    }
                    else if (src_type->dimension == 0 && src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                      assert(dist_type->dimension == 0);
                      if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_TO_BYTE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_TO_SHORT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_TO_INT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_TO_LONG;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_TO_FLOAT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_TO_DOUBLE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_TO_STRING;
                      }
                      else {
                        assert(0);
                      }
                    }
                    else if (src_type->dimension == 0 && src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                      assert(dist_type->dimension == 0);
                      if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_TO_BYTE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_TO_SHORT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_TO_INT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_TO_LONG;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_TO_FLOAT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_TO_DOUBLE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_TO_STRING;
                      }
                      else {
                        assert(0);
                      }
                    }
                    else if (src_type->dimension == 0 && src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                      assert(dist_type->dimension == 0);
                      if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_BYTE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_SHORT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_INT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_LONG;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_FLOAT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_DOUBLE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_TO_STRING;
                      }
                      else {
                        assert(0);
                      }
                    }
                    else if (src_type->dimension == 0 && src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                      assert(dist_type->dimension == 0);
                      if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_BYTE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_SHORT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_INT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_LONG;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_FLOAT;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_DOUBLE;
                      }
                      else if (dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_TO_STRING;
                      }
                      else {
                        assert(0);
                      }
                    }
                    else if (SPVM_TYPE_is_array_numeric(compiler, src_type)) {
                      assert(dist_type->dimension == 1 && dist_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_STRING);
                      
                      if (src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_BYTE_ARRAY_TO_STRING_ARRAY;
                      }
                      else if (src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_SHORT_ARRAY_TO_STRING_ARRAY;
                      }
                      else if (src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_INT_ARRAY_TO_STRING_ARRAY;
                      }
                      else if (src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_LONG_ARRAY_TO_STRING_ARRAY;
                      }
                      else if (src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_FLOAT_ARRAY_TO_STRING_ARRAY;
                      }
                      else if (src_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_CONVERT_DOUBLE_ARRAY_TO_STRING_ARRAY;
                      }
                      else {
                        assert(0);
                      }
                    }
                    // Check cast
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_CHECK_CAST;
                      opcode.operand2 = ((uint32_t)dist_type->basic_type->id + ((uint32_t)dist_type->dimension << 24));
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_dist_term);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_src_term);
                    
                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_NEW) {
                    

                    if (op_assign_from->first->id == SPVM_OP_C_ID_CONSTANT) {
                      SPVM_OPCODE opcode;
                      memset(&opcode, 0, sizeof(SPVM_OPCODE));
                      
                      opcode.id = SPVM_OPCODE_C_ID_NEW_STRING;

                      int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                      SPVM_CONSTANT* constant = op_assign_from->first->uv.constant;

                      opcode.operand0 = index_out;
                      opcode.operand1 = constant->id;

                      SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                    }
                    else if (op_assign_from->first->id == SPVM_OP_C_ID_TYPE) {
                      
                      SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_assign_from->first);
                      
                      if (type->dimension > 0) {
                        /*
                          NEW op_assign_from
                             TYPE op_assign_from->first
                               ELEMENT_TYPE op_assign_from->first->first
                               INDEX op_assign_from->first->last
                        */
                        
                        if (type->dimension == 1) {
                          switch (type->basic_type->id) {
                            case SPVM_BASIC_TYPE_C_ID_BYTE: {
                              SPVM_OPCODE opcode;
                              memset(&opcode, 0, sizeof(SPVM_OPCODE));
                              opcode.id = SPVM_OPCODE_C_ID_NEW_BYTE_ARRAY;

                              int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                              int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);

                              opcode.operand0 = index_out;
                              opcode.operand1 = index_index;

                              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                              
                              break;
                            }
                            case SPVM_BASIC_TYPE_C_ID_SHORT: {
                              SPVM_OPCODE opcode;
                              memset(&opcode, 0, sizeof(SPVM_OPCODE));
                              opcode.id = SPVM_OPCODE_C_ID_NEW_SHORT_ARRAY;

                              int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                              int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);
                              
                              opcode.operand0 = index_out;
                              opcode.operand1 = index_index;

                              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                              
                              break;
                            }
                            case SPVM_BASIC_TYPE_C_ID_INT: {
                              SPVM_OPCODE opcode;
                              memset(&opcode, 0, sizeof(SPVM_OPCODE));

                              opcode.id = SPVM_OPCODE_C_ID_NEW_INT_ARRAY;

                              int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                              int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);

                              opcode.operand0 = index_out;
                              opcode.operand1 = index_index;

                              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                              
                              break;
                            }
                            case SPVM_BASIC_TYPE_C_ID_LONG: {
                              SPVM_OPCODE opcode;
                              memset(&opcode, 0, sizeof(SPVM_OPCODE));

                              opcode.id = SPVM_OPCODE_C_ID_NEW_LONG_ARRAY;

                              int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                              int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);

                              opcode.operand0 = index_out;
                              opcode.operand1 = index_index;

                              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                              
                              break;
                            }
                            case SPVM_BASIC_TYPE_C_ID_FLOAT: {
                              SPVM_OPCODE opcode;
                              memset(&opcode, 0, sizeof(SPVM_OPCODE));

                              opcode.id = SPVM_OPCODE_C_ID_NEW_FLOAT_ARRAY;

                              int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                              int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);

                              opcode.operand0 = index_out;
                              opcode.operand1 = index_index;

                              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                              
                              break;
                            }
                            case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
                              SPVM_OPCODE opcode;
                              memset(&opcode, 0, sizeof(SPVM_OPCODE));

                              opcode.id = SPVM_OPCODE_C_ID_NEW_DOUBLE_ARRAY;
                              
                              int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                              int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);

                              opcode.operand0 = index_out;
                              opcode.operand1 = index_index;

                              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                              break;
                            }
                            default: {
                              SPVM_OPCODE opcode;
                              memset(&opcode, 0, sizeof(SPVM_OPCODE));

                              opcode.id = SPVM_OPCODE_C_ID_NEW_OBJECT_ARRAY;

                              int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                              SPVM_TYPE* type = op_assign_from->first->first->uv.type;
                              int32_t basic_type_id = type->basic_type->id;
                              int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);

                              opcode.operand0 = index_out;
                              opcode.operand1 = basic_type_id;
                              opcode.operand2 = index_index;

                              SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                            }
                          }
                        }
                        else {
                          SPVM_OPCODE opcode;
                          memset(&opcode, 0, sizeof(SPVM_OPCODE));

                          opcode.id = SPVM_OPCODE_C_ID_NEW_MULTI_ARRAY;

                          int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                          SPVM_TYPE* type = op_assign_from->first->first->uv.type;
                          int32_t index_index = SPVM_OP_get_my_index(compiler, op_assign_from->first->last);
                          
                          opcode.operand0 = index_out;
                          opcode.operand1 = ((uint32_t)type->basic_type->id + ((uint32_t)type->dimension << 24));
                          opcode.operand2 = index_index;

                          SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                        }
                      }
                      else {
                        SPVM_OPCODE opcode;
                        memset(&opcode, 0, sizeof(SPVM_OPCODE));

                        opcode.id = SPVM_OPCODE_C_ID_NEW_OBJECT;
                        
                        int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                        int32_t basic_type_id = op_assign_from->first->uv.type->basic_type->id;
                        
                        opcode.operand0 = index_out;
                        opcode.operand1 = basic_type_id;
                        SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                      }
                    }
                    else {
                      assert(0);
                    }
                  }
                  else if (op_assign_from->id == SPVM_OP_C_ID_VAR || op_assign_from->id == SPVM_OP_C_ID_ASSIGN) {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    if (type_to->dimension == 0) {
                      switch (type_to->basic_type->id) {
                        case SPVM_BASIC_TYPE_C_ID_BYTE:
                          opcode.id = SPVM_OPCODE_C_ID_MOVE_BYTE;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_SHORT:
                          opcode.id = SPVM_OPCODE_C_ID_MOVE_SHORT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_INT:
                          opcode.id = SPVM_OPCODE_C_ID_MOVE_INT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_LONG:
                          opcode.id = SPVM_OPCODE_C_ID_MOVE_LONG;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_FLOAT:
                          opcode.id = SPVM_OPCODE_C_ID_MOVE_FLOAT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                          opcode.id = SPVM_OPCODE_C_ID_MOVE_DOUBLE;
                          break;
                        default:
                          opcode.id = SPVM_OPCODE_C_ID_MOVE_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_MOVE_OBJECT;
                    }

                    int32_t index_out = SPVM_OP_get_my_index(compiler, op_assign_to);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from);

                    opcode.operand0 = index_out;
                    opcode.operand1 = index_in;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else {
                    break;
                  }
                }
                else if (op_assign_to->id == SPVM_OP_C_ID_PACKAGE_VAR) {
                  SPVM_OP* op_package_var = op_assign_to;
                  SPVM_PACKAGE_VAR* package_var = op_package_var->uv.package_var;

                  SPVM_TYPE* from_type = SPVM_OP_get_type(compiler, op_assign_from);
                  // PACKAGE_VAR = UNDEF
                  if (SPVM_TYPE_is_undef(compiler, from_type)) {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    int32_t package_var_id = package_var->op_our->uv.our->id;
                    opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_UNDEF;
                    opcode.operand0 = package_var_id;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  // PACKAGE_VAR = VAR
                  else {
                    SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_package_var);

                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    if (type->dimension == 0) {
                      if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_BYTE;
                      }
                      else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_SHORT;
                      }
                      else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_INT;
                      }
                      else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_LONG;
                      }
                      else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_FLOAT;
                      }
                      else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_DOUBLE;
                      }
                      else {
                        opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_STORE_PACKAGE_VAR_OBJECT;
                    }
                                      
                    int32_t package_var_id = package_var->op_our->uv.our->id;
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from);
                    
                    opcode.operand0 = package_var_id;
                    opcode.operand1 = index_in;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                }
                else if (op_assign_to->id == SPVM_OP_C_ID_EXCEPTION_VAR) {
                  
                  SPVM_TYPE* from_type = SPVM_OP_get_type(compiler, op_assign_from);
                  
                  if (SPVM_TYPE_is_undef(compiler, from_type)) {
                    // EXCEPTION_VAR = undef
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    opcode.id = SPVM_OPCODE_C_ID_STORE_EXCEPTION_VAR_UNDEF;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else {
                    // EXCEPTION_VAR = VAR
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    opcode.id = SPVM_OPCODE_C_ID_STORE_EXCEPTION_VAR;
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from);
                    opcode.operand0 = index_in;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                }
                else if (op_assign_to->id == SPVM_OP_C_ID_ARRAY_ACCESS) {
                  
                  // $VAR_ARRAY->[$VAR_INDEX] = $VAR_TERM
                  
                  SPVM_OP* op_array_access = op_assign_to;
                  SPVM_OP* op_term_array = op_array_access->first;
                  SPVM_OP* op_term_index = op_array_access->last;
                  
                  SPVM_TYPE* from_type = SPVM_OP_get_type(compiler, op_assign_from);

                  if (SPVM_TYPE_is_undef(compiler, from_type)) {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_UNDEF;
                    
                    int32_t index_term_array = SPVM_OP_get_my_index(compiler, op_term_array);
                    int32_t index_term_index = SPVM_OP_get_my_index(compiler, op_term_index);
                    
                    opcode.operand0 = index_term_array;
                    opcode.operand1 = index_term_index;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                  else {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    if (from_type->dimension == 0) {
                      if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_BYTE;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_SHORT;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_INT;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_LONG;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_FLOAT;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_DOUBLE;
                      }
                      else {
                        opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_ARRAY_STORE_OBJECT;
                    }
                    
                    int32_t index_term_array = SPVM_OP_get_my_index(compiler, op_term_array);
                    int32_t index_term_index = SPVM_OP_get_my_index(compiler, op_term_index);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from);
                    
                    opcode.operand0 = index_term_array;
                    opcode.operand1 = index_term_index;
                    opcode.operand2 = index_in;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                }
                else if (op_assign_to->id == SPVM_OP_C_ID_FIELD_ACCESS) {
                  
                  // $VAR_ARRAY->{NAME} = $VAR_TERM
                  
                  SPVM_OP* op_field_access = op_assign_to;
                  SPVM_OP* op_term_object = op_field_access->first;

                  // Call field
                  SPVM_FIELD_ACCESS* field_access = op_field_access->uv.field_access;
                  SPVM_FIELD* field = field_access->field;

                  SPVM_TYPE* from_type = SPVM_OP_get_type(compiler, op_assign_from);
                  
                  if (SPVM_TYPE_is_undef(compiler, from_type)) {
                    
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_UNDEF;
                    
                    int32_t index_term_object = SPVM_OP_get_my_index(compiler, op_term_object);
                    
                    opcode.operand0 = index_term_object;
                    opcode.operand1 = field->byte_offset;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                  else {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    if (from_type->dimension == 0) {
                      if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                        opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_BYTE;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                        opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_SHORT;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                        opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_INT;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                        opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_LONG;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                        opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_FLOAT;
                      }
                      else if (from_type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                        opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_DOUBLE;
                      }
                      else {
                        opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_SET_FIELD_OBJECT;
                    }
                    
                    int32_t index_term_object = SPVM_OP_get_my_index(compiler, op_term_object);
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_assign_from);
                    
                    opcode.operand0 = index_term_object;
                    opcode.operand1 = field->byte_offset;
                    opcode.operand2 = index_in;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                    SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                  }
                }
                else {
                  assert(0);
                }
                
                break;
              }
              case SPVM_OP_C_ID_SWITCH_CONDITION: {
                
                SPVM_SWITCH_INFO* switch_info = op_cur->uv.switch_info;

                // tableswitch
                if (switch_info->id == SPVM_SWITCH_INFO_C_ID_TABLE_SWITCH) {
                  SPVM_OPCODE opcode_switch_info;
                  memset(&opcode_switch_info, 0, sizeof(SPVM_OPCODE));

                  opcode_switch_info.id = SPVM_OPCODE_C_ID_TABLE_SWITCH;

                  int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->first);
                  
                  opcode_switch_info.operand0 = index_in;
                  
                  // Default
                  opcode_switch_info.operand1 = 0;

                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode_switch_info);

                  SPVM_OPCODE opcode_table_switch_range;
                  memset(&opcode_table_switch_range, 0, sizeof(SPVM_OPCODE));

                  opcode_table_switch_range.id = SPVM_OPCODE_C_ID_TABLE_SWITCH_RANGE;
                  
                  // Minimal
                  opcode_table_switch_range.operand0 = switch_info->min;
                  
                  // Max
                  opcode_table_switch_range.operand1 = switch_info->max;

                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode_table_switch_range);

                  // Switch bytecode index
                  int32_t switch_opcode_index = opcode_array->length - 2;
                  switch_info->opcode_index = switch_opcode_index;
                  SPVM_LIST_push(switch_info_stack, switch_info);
                  
                  // Jump offset length
                  int32_t jump_offset_length = switch_info->max - switch_info->min + 1;
                  
                  // Branches
                  {
                    int32_t i;
                    for (i = 0; i < jump_offset_length; i++) {
                      SPVM_OPCODE opcode_case;
                      memset(&opcode_case, 0, sizeof(SPVM_OPCODE));
                      opcode_case.id = SPVM_OPCODE_C_ID_CASE;
                      SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode_case);
                    }
                  }
                }
                // lookupswitch
                else if (switch_info->id == SPVM_SWITCH_INFO_C_ID_LOOKUP_SWITCH) {
                  SPVM_OPCODE opcode_switch_info;
                  memset(&opcode_switch_info, 0, sizeof(SPVM_OPCODE));

                  opcode_switch_info.id = SPVM_OPCODE_C_ID_LOOKUP_SWITCH;

                  int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->first);
                  opcode_switch_info.operand0 = index_in;
                  
                  // Default
                  opcode_switch_info.operand1 = 0;
                  
                  // Case count
                  int32_t case_length = switch_info->op_cases->length;
                  opcode_switch_info.operand2 = case_length;

                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode_switch_info);
                  
                  // Switch opcode index
                  int32_t switch_opcode_index = opcode_array->length - 1;
                  switch_info->opcode_index = switch_opcode_index;
                  SPVM_LIST_push(switch_info_stack, switch_info);
                  
                  // Match-Offsets
                  {
                    int32_t i;
                    for (i = 0; i < case_length; i++) {
                      SPVM_OPCODE opcode_case;
                      memset(&opcode_case, 0, sizeof(SPVM_OPCODE));
                      SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode_case);
                    }
                  }
                }
                
                break;
              }
              case SPVM_OP_C_ID_SWITCH: {
                
                // Pop switch information
                SPVM_SWITCH_INFO* switch_info = SPVM_LIST_pop(switch_info_stack);
                int32_t switch_opcode_index = switch_info->opcode_index;
                int32_t default_opcode_index = switch_info->default_opcode_index;
                SPVM_LIST* case_opcode_indexes = switch_info->case_opcode_indexes;
                
                // tableswitch
                if (switch_info->id == SPVM_SWITCH_INFO_C_ID_TABLE_SWITCH) {
                  // Default branch
                  int32_t default_branch;
                  if (!default_opcode_index) {
                    default_branch = opcode_array->length;
                  }
                  else {
                    default_branch = default_opcode_index;
                  }
                  ((SPVM_OPCODE*)opcode_array->values + switch_opcode_index)->operand1 = default_branch;
                  
                  // min
                  int32_t min = ((SPVM_OPCODE*)opcode_array->values + switch_opcode_index + 1)->operand0;
                  
                  // max
                  int32_t max = ((SPVM_OPCODE*)opcode_array->values + switch_opcode_index + 1)->operand1;
                  
                  int32_t length = (int32_t)(max - min + 1);
                  
                  int32_t case_pos = 0;
                  {
                    int32_t i;
                    for (i = 0; i < length; i++) {
                      SPVM_OP* op_case = SPVM_LIST_fetch(switch_info->op_cases, case_pos);
                      SPVM_OP* op_constant = op_case->first;
                      SPVM_OPCODE* opcode_case = (opcode_array->values + switch_opcode_index + 2 + i);

                      if (op_constant->uv.constant->value.int_value - min == i) {
                        // Branch
                        int32_t* case_opcode_index_ptr = SPVM_LIST_fetch(case_opcode_indexes, case_pos);
                        int32_t case_opcode_index = *case_opcode_index_ptr;
                        int32_t case_branch = case_opcode_index;
                        
                        opcode_case->operand1 = case_branch;
                        
                        case_pos++;
                      }
                      else {
                        // Default branch
                        opcode_case->operand1 = default_branch;
                      }
                    }
                  }
                }
                // lookupswitch
                else if (switch_info->id == SPVM_SWITCH_INFO_C_ID_LOOKUP_SWITCH) {
                  // Default branch
                  if (!default_opcode_index) {
                    default_opcode_index = opcode_array->length;
                  }
                  (opcode_array->values + switch_opcode_index)->operand1 = default_opcode_index;
                  
                  int32_t case_length = (int32_t) switch_info->op_cases->length;
                  
                  SPVM_LIST* ordered_op_cases = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
                  {
                    int32_t i;
                    for (i = 0; i < case_length; i++) {
                      SPVM_OP* op_case = SPVM_LIST_fetch(switch_info->op_cases, i);
                      SPVM_LIST_push(ordered_op_cases, op_case);
                    }
                  }
                  SPVM_LIST* ordered_case_opcode_indexes = SPVM_COMPILER_ALLOCATOR_alloc_array(compiler, 0);
                  {
                    int32_t i;
                    for (i = 0; i < case_length; i++) {
                      int32_t* case_opcode_index_ptr = SPVM_LIST_fetch(case_opcode_indexes, i);
                      SPVM_LIST_push(ordered_case_opcode_indexes, case_opcode_index_ptr);
                    }
                  }
                  
                  // sort by asc order
                  {
                    int32_t i;
                    for (i = 0; i < case_length; i++) {
                      int32_t j;
                      {
                        for (j = i + 1; j < case_length; j++) {
                          SPVM_OP* op_case_i = SPVM_LIST_fetch(ordered_op_cases, i);
                          SPVM_OP* op_case_j = SPVM_LIST_fetch(ordered_op_cases, j);
                          int32_t match_i = op_case_i->first->uv.constant->value.int_value;
                          int32_t match_j = op_case_j->first->uv.constant->value.int_value;
                          
                          int32_t* case_opcode_index_i = SPVM_LIST_fetch(ordered_case_opcode_indexes, i);
                          int32_t* case_opcode_index_j = SPVM_LIST_fetch(ordered_case_opcode_indexes, j);
                          
                          if (match_i > match_j) {
                            SPVM_LIST_store(ordered_op_cases, i, op_case_j);
                            SPVM_LIST_store(ordered_op_cases, j, op_case_i);
                            
                            SPVM_LIST_store(ordered_case_opcode_indexes, i, case_opcode_index_j);
                            SPVM_LIST_store(ordered_case_opcode_indexes, j, case_opcode_index_i);
                          }
                        }
                      }
                    }
                  }
                  
                  {
                    int32_t i;
                    for (i = 0; i < case_length; i++) {
                      SPVM_OP* op_case = SPVM_LIST_fetch(ordered_op_cases, i);
                      SPVM_OP* op_constant = op_case->first;
                      int32_t match = op_constant->uv.constant->value.int_value;

                      int32_t* case_opcode_index_ptr = SPVM_LIST_fetch(ordered_case_opcode_indexes, i);
                      int32_t case_opcode_index = *case_opcode_index_ptr;
                      
                      SPVM_OPCODE* opcode_case = (opcode_array->values + switch_opcode_index + 1 + i);
                      
                      opcode_case->id = SPVM_OPCODE_C_ID_CASE;
                      
                      // Match
                      opcode_case->operand0 = match;

                      // Branch
                      opcode_case->operand1 = case_opcode_index;
                    }
                  }
                }
                
                // Set last position
                while (last_goto_opcode_index_stack->length > 0) {
                  
                  int32_t* last_opcode_index_ptr = SPVM_LIST_pop(last_goto_opcode_index_stack);
                  int32_t last_opcode_index = *last_opcode_index_ptr;
                  
                  SPVM_OPCODE* opcode_goto = (opcode_array->values + last_opcode_index);
                  opcode_goto->operand0 = opcode_array->length;
                }
                
                break;
              }
              case SPVM_OP_C_ID_CASE: {
                if (switch_info_stack->length > 0) {
                  SPVM_SWITCH_INFO* switch_info = SPVM_LIST_fetch(switch_info_stack, switch_info_stack->length - 1);
                  int32_t* opcode_index_ptr = SPVM_COMPILER_ALLOCATOR_alloc_int(compiler);
                  *opcode_index_ptr = opcode_array->length;
                  
                  SPVM_LIST_push(switch_info->case_opcode_indexes, opcode_index_ptr);
                }
                break;
              }
              case SPVM_OP_C_ID_DEFAULT: {
                if (switch_info_stack->length > 0) {
                  SPVM_SWITCH_INFO* switch_info = SPVM_LIST_fetch(switch_info_stack, switch_info_stack->length - 1);
                  switch_info->default_opcode_index = opcode_array->length;
                }
                break;
              }
              case SPVM_OP_C_ID_FIELD_ACCESS: {
                
                // Weaken field
                if (op_cur->flag &= SPVM_OP_C_FLAG_FIELD_ACCESS_WEAKEN) {
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));

                  opcode.id = SPVM_OPCODE_C_ID_WEAKEN_FIELD_OBJECT;
                  SPVM_FIELD* field = op_cur->uv.field_access->field;
                  
                  SPVM_OP* op_term_object = op_cur->first;
                  int32_t index_term_object = SPVM_OP_get_my_index(compiler, op_term_object);
                    
                  opcode.operand0 = index_term_object;
                  opcode.operand1 = field->byte_offset;
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                  SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                }
                
                break;
              }
              case SPVM_OP_C_ID_CROAK: {
                
                // SET_CROAK_FLAG_TRUE
                {
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  
                  opcode.id = SPVM_OPCODE_C_ID_SET_CROAK_FLAG_TRUE;
                  
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                }
                
                SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                
                break;
              }
              case SPVM_OP_C_ID_LAST: {
                // GOTO end of loop init block
                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                opcode.id = SPVM_OPCODE_C_ID_GOTO;
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                int32_t* opcode_index_ptr = SPVM_COMPILER_ALLOCATOR_alloc_int(compiler);
                *opcode_index_ptr = opcode_array->length - 1;
                
                SPVM_LIST_push(last_goto_opcode_index_stack, opcode_index_ptr);
                
                break;
              }
              case SPVM_OP_C_ID_NEXT: {
                // GOTO increment statement
                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                opcode.id = SPVM_OPCODE_C_ID_GOTO;
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                int32_t* opcode_index_ptr = SPVM_COMPILER_ALLOCATOR_alloc_int(compiler);
                *opcode_index_ptr = opcode_array->length - 1;
                
                SPVM_LIST_push(next_goto_opcode_index_stack, opcode_index_ptr);
                
                break;
              }
              case SPVM_OP_C_ID_BLOCK: { // Postorder
                
                if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_IF) {
                  
                  {
                    // Prepare to jump to end of true block
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));

                    opcode.id = SPVM_OPCODE_C_ID_GOTO;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                    
                    int32_t* opcode_index_ptr = SPVM_COMPILER_ALLOCATOR_alloc_int(compiler);
                    *opcode_index_ptr = opcode_array->length - 1;
                    SPVM_LIST_push(if_block_end_goto_opcode_index_stack, opcode_index_ptr);
                  }

                  // Set if jump opcode index
                  int32_t* opcode_index_ptr = SPVM_LIST_pop(if_eq_or_if_ne_opcode_index_stack);
                  int32_t opcode_index = *opcode_index_ptr;
                  
                  // Set jump
                  SPVM_OPCODE* opcode_goto = (opcode_array->values + opcode_index);
                  opcode_goto->operand0 = opcode_array->length;
                }
                else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_ELSE) {
                  
                  assert(if_block_end_goto_opcode_index_stack->length > 0);
                  
                  int32_t* opcode_index_ptr = SPVM_LIST_pop(if_block_end_goto_opcode_index_stack);
                  int32_t opcode_index = *opcode_index_ptr;
                  
                  // Set jump
                  SPVM_OPCODE* opcode_goto = (opcode_array->values + opcode_index);
                  opcode_goto->operand0 = opcode_array->length;
                }
                else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_LOOP_INIT) {
                  // Set last position
                  while (last_goto_opcode_index_stack->length > 0) {
                    
                    int32_t* last_opcode_index_ptr = SPVM_LIST_pop(last_goto_opcode_index_stack);
                    int32_t last_opcode_index = *last_opcode_index_ptr;
                    
                    SPVM_OPCODE* opcode_last = (opcode_array->values + last_opcode_index);
                    opcode_last->operand0 = opcode_array->length;
                  }
                }
                else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_LOOP_STATEMENTS) {
                  // Set next position
                  while (next_goto_opcode_index_stack->length > 0) {
                    
                    int32_t* next_opcode_index_ptr = SPVM_LIST_pop(next_goto_opcode_index_stack);
                    int32_t next_opcode_index = *next_opcode_index_ptr;
                    
                    SPVM_OPCODE* opcode_next = (opcode_array->values + next_opcode_index);
                    opcode_next->operand0 = opcode_array->length;
                  }
                }
                else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_EVAL) {
                  // Set IF_CROAK_CATCH opcode index
                  while (if_croak_catch_goto_opcode_index_stack->length > 0) {
                    int32_t* if_croak_catch_opcode_index_ptr = SPVM_LIST_pop(if_croak_catch_goto_opcode_index_stack);
                    int32_t if_croak_catch_opcode_index = *if_croak_catch_opcode_index_ptr;
                    
                    SPVM_OPCODE* opcode_if_croak_catch = (opcode_array->values + if_croak_catch_opcode_index);
                    opcode_if_croak_catch->operand0 = opcode_array->length;
                  }
                  
                  SPVM_LIST_pop(push_eval_opcode_index_stack);
                }
                else if (op_cur->uv.block->id == SPVM_BLOCK_C_ID_SUB) {
                  while (return_goto_opcode_index_stack->length > 0) {
                    
                    int32_t return_opcode_index = (intptr_t)SPVM_LIST_pop(return_goto_opcode_index_stack);
                    
                    SPVM_OPCODE* opcode_return = (opcode_array->values + return_opcode_index);
                    opcode_return->operand1 = opcode_array->length;
                  }

                  // Set IF_CROAK_RETURN opcode index
                  while (if_croak_return_goto_opcode_index_stack->length > 0) {
                    int32_t if_croak_return_opcode_index = (intptr_t)SPVM_LIST_pop(if_croak_return_goto_opcode_index_stack);
                    
                    SPVM_OPCODE* opcode_if_croak_return = (opcode_array->values + if_croak_return_opcode_index);
                    opcode_if_croak_return->operand0 = opcode_array->length;
                  }
                }
                
                // Leave scope
                int32_t mortal_base = (intptr_t)SPVM_LIST_pop(mortal_base_stack);

                SPVM_OP* op_block_current = SPVM_LIST_fetch(op_block_stack, op_block_stack->length - 1);
                
                if (op_block_current->uv.block->need_leave_scope) {
                  while (mortal_stack->length > mortal_base) {
                    SPVM_LIST_pop(mortal_stack);
                  }
                  
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  opcode.id = SPVM_OPCODE_C_ID_LEAVE_SCOPE;
                  opcode.operand0 = mortal_base;
                  
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  
                }

                SPVM_LIST_pop(op_block_stack);

                // Parent block need LEAVE_SCOPE if child is needing LEAVE_SCOPE
                if (op_block_stack->length > 0) {
                  SPVM_OP* op_block_parent = SPVM_LIST_fetch(op_block_stack, op_block_stack->length - 1);
                  if (!op_block_parent->uv.block->need_leave_scope) {
                    if (op_block_current->uv.block->need_leave_scope) {
                      op_block_parent->uv.block->need_leave_scope = 1;
                    }
                  }
                }
                
                break;
              }
              case SPVM_OP_C_ID_LOOP_INCREMENT: {
                // Set loop first GOTO opcode
                int32_t loop_first_opcode_index = (intptr_t)SPVM_LIST_fetch(loop_first_goto_opcode_index_stack, loop_first_goto_opcode_index_stack->length - 1);
                
                SPVM_OPCODE* opcode_loop_first = (opcode_array->values + loop_first_opcode_index);
                opcode_loop_first->operand0 = opcode_array->length;
                
                break;
              }
              case SPVM_OP_C_ID_CONDITION:
              case SPVM_OP_C_ID_CONDITION_NOT:
              {
                
                int32_t opcode_index = opcode_array->length;

                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));

                if (op_cur->id == SPVM_OP_C_ID_CONDITION) {
                  opcode.id = SPVM_OPCODE_C_ID_IF_EQ_ZERO;
                }
                else {
                  opcode.id = SPVM_OPCODE_C_ID_IF_NE_ZERO;
                }
                
                if (op_cur->flag & SPVM_OP_C_FLAG_CONDITION_IF) {
                  int32_t* opcode_index_ptr = SPVM_COMPILER_ALLOCATOR_alloc_int(compiler);
                  *opcode_index_ptr = opcode_index;
                  
                  SPVM_LIST_push(if_eq_or_if_ne_opcode_index_stack, opcode_index_ptr);
                }
                else if (op_cur->flag & SPVM_OP_C_FLAG_CONDITION_LOOP) {
                  assert(loop_first_goto_opcode_index_stack->length > 0);
                  
                  int32_t loop_first_opcode_index = (intptr_t)SPVM_LIST_pop(loop_first_goto_opcode_index_stack);
                  
                  opcode.operand0 = loop_first_opcode_index + 1;
                }
                
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                break;
              }
              case SPVM_OP_C_ID_MY: {
                SPVM_MY* my = op_cur->uv.my;
                
                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur);
                
                if (SPVM_TYPE_is_object(compiler, type)) {
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  opcode.id = SPVM_OPCODE_C_ID_PUSH_MORTAL;
                  opcode.operand0 = my->index;
                  
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  
                  int32_t my_index = my->index;
                  SPVM_LIST_push(mortal_stack, (void*)(intptr_t)my_index);
                  
                  if (mortal_stack->length > mortal_stack_max) {
                    mortal_stack_max = mortal_stack->length;
                  }
                  
                  SPVM_OP* op_block_current = SPVM_LIST_fetch(op_block_stack, op_block_stack->length - 1);
                  op_block_current->uv.block->need_leave_scope = 1;
                }
                
                break;
              }
              case SPVM_OP_C_ID_INC:
              case SPVM_OP_C_ID_DEC:
              {
                SPVM_VAR* var = op_cur->first->uv.var;
                SPVM_MY* my = var->op_my->uv.my;
                
                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur);
                SPVM_OPCODE opcode;
                assert(type->dimension == 0);
                switch (type->basic_type->id) {
                  case SPVM_BASIC_TYPE_C_ID_BYTE:
                    opcode.id = SPVM_OPCODE_C_ID_INC_BYTE;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_SHORT:
                    opcode.id = SPVM_OPCODE_C_ID_INC_SHORT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_INT:
                    opcode.id = SPVM_OPCODE_C_ID_INC_INT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_LONG:
                    opcode.id = SPVM_OPCODE_C_ID_INC_LONG;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_FLOAT:
                    opcode.id = SPVM_OPCODE_C_ID_INC_FLOAT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                    opcode.id = SPVM_OPCODE_C_ID_INC_DOUBLE;
                    break;
                }
                opcode.operand0 = my->index;
                
                if (op_cur->id == SPVM_OP_C_ID_INC) {
                  opcode.operand1 = 1;
                }
                else if (op_cur->id == SPVM_OP_C_ID_DEC) {
                  opcode.operand1 = -1;
                }
                else {
                  assert(0);
                }
                
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                break;
              }
              case SPVM_OP_C_ID_GT: {
                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->first);

                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                
                assert(type->dimension == 0);
                switch (type->basic_type->id) {
                  case SPVM_BASIC_TYPE_C_ID_INT:
                    opcode.id = SPVM_OPCODE_C_ID_GT_INT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_LONG:
                    opcode.id = SPVM_OPCODE_C_ID_GT_LONG;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_FLOAT:
                    opcode.id = SPVM_OPCODE_C_ID_GT_FLOAT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                    opcode.id = SPVM_OPCODE_C_ID_GT_DOUBLE;
                    break;
                  default:
                    assert(0);
                }
                
                int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_cur->last);
                
                opcode.operand0 = index_in1;
                opcode.operand1 = index_in2;
                
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                break;
              }
              case SPVM_OP_C_ID_GE: {

                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->first);

                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                
                assert(type->dimension == 0);
                switch (type->basic_type->id) {
                  case SPVM_BASIC_TYPE_C_ID_INT:
                    opcode.id = SPVM_OPCODE_C_ID_GE_INT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_LONG:
                    opcode.id = SPVM_OPCODE_C_ID_GE_LONG;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_FLOAT:
                    opcode.id = SPVM_OPCODE_C_ID_GE_FLOAT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                    opcode.id = SPVM_OPCODE_C_ID_GE_DOUBLE;
                    break;
                  default:
                    assert(0);
                }
                
                int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_cur->last);
                
                opcode.operand0 = index_in1;
                opcode.operand1 = index_in2;
                
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

                break;
              }
              case  SPVM_OP_C_ID_LT: {
                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->first);
                
                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                
                assert(type->dimension == 0);
                switch (type->basic_type->id) {
                  case SPVM_BASIC_TYPE_C_ID_INT:
                    opcode.id = SPVM_OPCODE_C_ID_LT_INT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_LONG:
                    opcode.id = SPVM_OPCODE_C_ID_LT_LONG;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_FLOAT:
                    opcode.id = SPVM_OPCODE_C_ID_LT_FLOAT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                    opcode.id = SPVM_OPCODE_C_ID_LT_DOUBLE;
                    break;
                  default:
                    assert(0);
                }
                
                int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_cur->last);
                
                opcode.operand0 = index_in1;
                opcode.operand1 = index_in2;
                
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                break;
              }
              case SPVM_OP_C_ID_LE: {

                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->first);

                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                
                assert(type->dimension == 0);
                switch (type->basic_type->id) {
                  case SPVM_BASIC_TYPE_C_ID_INT:
                    opcode.id = SPVM_OPCODE_C_ID_LE_INT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_LONG:
                    opcode.id = SPVM_OPCODE_C_ID_LE_LONG;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_FLOAT:
                    opcode.id = SPVM_OPCODE_C_ID_LE_FLOAT;
                    break;
                  case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                    opcode.id = SPVM_OPCODE_C_ID_LE_DOUBLE;
                    break;
                  default:
                    assert(0);
                }
                
                int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_cur->last);
                
                opcode.operand0 = index_in1;
                opcode.operand1 = index_in2;

                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                break;
              }
              case  SPVM_OP_C_ID_EQ: {
                if (op_cur->first->id == SPVM_OP_C_ID_UNDEF || op_cur->last->id == SPVM_OP_C_ID_UNDEF) {
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));

                  opcode.id = SPVM_OPCODE_C_ID_IS_UNDEF;
                  if (op_cur->first->id == SPVM_OP_C_ID_UNDEF) {
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->last);
                    
                    opcode.operand0 = index_in;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else {
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->first);
                    
                    opcode.operand0 = index_in;
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                }
                else {
                  
                  SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->first);
                  
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  
                  if (type->dimension == 0) {
                    if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                      assert(0);
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                      assert(0);
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                      opcode.id = SPVM_OPCODE_C_ID_EQ_INT;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                      opcode.id = SPVM_OPCODE_C_ID_EQ_LONG;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                      opcode.id = SPVM_OPCODE_C_ID_EQ_FLOAT;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                      opcode.id = SPVM_OPCODE_C_ID_EQ_DOUBLE;
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_EQ_OBJECT;
                    }
                  }
                  else {
                    opcode.id = SPVM_OPCODE_C_ID_EQ_OBJECT;
                  }
                  
                  int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                  int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_cur->last);
                  
                  opcode.operand0 = index_in1;
                  opcode.operand1 = index_in2;
                  
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                }
                break;
              }
              case  SPVM_OP_C_ID_NE: {
                
                if (op_cur->first->id == SPVM_OP_C_ID_UNDEF || op_cur->last->id == SPVM_OP_C_ID_UNDEF) {
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));

                  opcode.id = SPVM_OPCODE_C_ID_IS_NOT_UNDEF;
                  if (op_cur->first->id == SPVM_OP_C_ID_UNDEF) {
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->last);
                    opcode.operand0 = index_in;
                  }
                  else {
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->first);
                    opcode.operand0 = index_in;
                  }
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                }
                else {
                  SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->first);

                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  
                  if (type->dimension == 0) {
                    if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                      assert(0);
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                      assert(0);
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                      opcode.id = SPVM_OPCODE_C_ID_NE_INT;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                      opcode.id = SPVM_OPCODE_C_ID_NE_LONG;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                      opcode.id = SPVM_OPCODE_C_ID_NE_FLOAT;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                      opcode.id = SPVM_OPCODE_C_ID_NE_DOUBLE;
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_NE_OBJECT;
                    }
                  }
                  else {
                    opcode.id = SPVM_OPCODE_C_ID_NE_OBJECT;
                  }
                  int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                  int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_cur->last);
                  
                  opcode.operand0 = index_in1;
                  opcode.operand1 = index_in2;

                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                }
                break;
              }
              case SPVM_OP_C_ID_ISA: {
                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                
                opcode.id = SPVM_OPCODE_C_ID_ISA;
                
                int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->last);
                
                opcode.operand0 = index_in1;
                opcode.operand1 = type->basic_type->id;
                opcode.operand2 = type->dimension;
                
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                break;
              }
              case SPVM_OP_C_ID_STRING_EQ:
              case SPVM_OP_C_ID_STRING_NE:
              case SPVM_OP_C_ID_STRING_GT:
              case SPVM_OP_C_ID_STRING_GE:
              case SPVM_OP_C_ID_STRING_LT:
              case SPVM_OP_C_ID_STRING_LE:
              {
                SPVM_OPCODE opcode;
                memset(&opcode, 0, sizeof(SPVM_OPCODE));
                
                switch (op_cur->id) {
                  case SPVM_OP_C_ID_STRING_EQ:
                    opcode.id = SPVM_OPCODE_C_ID_STRING_EQ;
                    break;
                  case SPVM_OP_C_ID_STRING_NE:
                    opcode.id = SPVM_OPCODE_C_ID_STRING_NE;
                    break;
                  case SPVM_OP_C_ID_STRING_GT:
                    opcode.id = SPVM_OPCODE_C_ID_STRING_GT;
                    break;
                  case SPVM_OP_C_ID_STRING_GE:
                    opcode.id = SPVM_OPCODE_C_ID_STRING_GE;
                    break;
                  case SPVM_OP_C_ID_STRING_LT:
                    opcode.id = SPVM_OPCODE_C_ID_STRING_LE;
                    break;
                  case SPVM_OP_C_ID_STRING_LE:
                    opcode.id = SPVM_OPCODE_C_ID_STRING_LE;
                    break;
                  default:
                    assert(0);
                }
                
                int32_t index_in1 = SPVM_OP_get_my_index(compiler, op_cur->first);
                int32_t index_in2 = SPVM_OP_get_my_index(compiler, op_cur->last);
                
                opcode.operand0 = index_in1;
                opcode.operand1 = index_in2;
                
                SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                
                break;
              }
              case  SPVM_OP_C_ID_BOOL: {
                SPVM_TYPE* type = SPVM_OP_get_type(compiler, op_cur->first);
                
                if (op_cur->first->id != SPVM_OP_C_ID_IF) {

                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  
                  if (type->dimension == 0) {
                    if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_BYTE) {
                      assert(0);
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_SHORT) {
                      assert(0);
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_INT) {
                      opcode.id = SPVM_OPCODE_C_ID_BOOL_INT;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_LONG) {
                      opcode.id = SPVM_OPCODE_C_ID_BOOL_LONG;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_FLOAT) {
                      opcode.id = SPVM_OPCODE_C_ID_BOOL_FLOAT;
                    }
                    else if (type->basic_type->id == SPVM_BASIC_TYPE_C_ID_DOUBLE) {
                      opcode.id = SPVM_OPCODE_C_ID_BOOL_DOUBLE;
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_BOOL_OBJECT;
                    }
                  }
                  else {
                    opcode.id = SPVM_OPCODE_C_ID_BOOL_OBJECT;
                  }

                  int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->first);
                  
                  opcode.operand0 = index_in;

                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                }
                
                break;
              }
              case SPVM_OP_C_ID_CALL_SUB: {
                
                SPVM_CALL_SUB* call_sub = op_cur->uv.call_sub;
                const char* call_sub_abs_name = call_sub->sub->abs_name;
                
                SPVM_OP* op_sub_call_sub = SPVM_HASH_search(compiler->op_sub_symtable, call_sub_abs_name, strlen(call_sub_abs_name));
                SPVM_SUB* sub_call_sub = op_sub_call_sub->uv.sub;
                
                if (sub_call_sub->op_return_type->uv.type->dimension == 0 && sub_call_sub->op_return_type->uv.type->basic_type->id == SPVM_BASIC_TYPE_C_ID_VOID) {
                  int32_t first_arg_index = -1;

                  // Push args
                  SPVM_OP* op_args = op_cur->last;
                  SPVM_OP* op_arg = op_args->first;
                  while ((op_arg = SPVM_OP_sibling(compiler, op_arg))) {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    SPVM_TYPE* arg_type = SPVM_OP_get_type(compiler, op_arg);
                    
                    if (SPVM_TYPE_is_undef(compiler, arg_type)) {
                      assert(!sub_call_sub->op_package->uv.package->is_interface);
                      opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_UNDEF;
                      SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                    }
                    else {
                      if (arg_type->dimension == 0) {
                        switch (arg_type->basic_type->id) {
                          case SPVM_BASIC_TYPE_C_ID_BYTE:
                            opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_BYTE;
                            break;
                          case SPVM_BASIC_TYPE_C_ID_SHORT:
                            opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_SHORT;
                            break;
                          case SPVM_BASIC_TYPE_C_ID_INT:
                            opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_INT;
                            break;
                          case SPVM_BASIC_TYPE_C_ID_LONG:
                            opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_LONG;
                            break;
                          case SPVM_BASIC_TYPE_C_ID_FLOAT:
                            opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_FLOAT;
                            break;
                          case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                            opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_DOUBLE;
                            break;
                          default:
                            opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_OBJECT;
                        }
                      }
                      else {
                        opcode.id = SPVM_OPCODE_C_ID_PUSH_ARG_OBJECT;
                      }

                      int32_t index_arg = SPVM_OP_get_my_index(compiler, op_arg);
                      opcode.operand0 = index_arg;
                      if (first_arg_index == -1) {
                        first_arg_index = index_arg;
                      }
                      SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                    }
                  }
                  

                  // CALL_SUB opcode
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  
                  if (sub_call_sub->op_package->uv.package->is_interface) {
                    opcode.id = SPVM_OPCODE_C_ID_CALL_INTERFACE_METHOD;
                    opcode.operand1 = sub_call_sub->id;
                    opcode.operand2 = first_arg_index;
                  }
                  else {
                    opcode.id = SPVM_OPCODE_C_ID_CALL_SUB;
                    opcode.operand1 = sub_call_sub->id;
                  }
                  
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  
                  SPVM_OPCODE_BUILDER_push_if_croak(compiler, opcode_array, push_eval_opcode_index_stack, if_croak_catch_goto_opcode_index_stack, if_croak_return_goto_opcode_index_stack, sub->id, op_cur->line);
                }
                
                break;
              }
              case SPVM_OP_C_ID_RETURN: {
                if (op_cur->first) {
                  SPVM_TYPE* return_type = SPVM_OP_get_type(compiler, op_cur->first);
                  
                  if (SPVM_TYPE_is_undef(compiler, return_type)) {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    opcode.id = SPVM_OPCODE_C_ID_RETURN_UNDEF;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                  else {
                    SPVM_OPCODE opcode;
                    memset(&opcode, 0, sizeof(SPVM_OPCODE));
                    
                    if (sub->op_return_type->uv.type->dimension == 0) {
                      switch (sub->op_return_type->uv.type->basic_type->id) {
                        case SPVM_BASIC_TYPE_C_ID_BYTE:
                          opcode.id = SPVM_OPCODE_C_ID_RETURN_BYTE;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_SHORT:
                          opcode.id = SPVM_OPCODE_C_ID_RETURN_SHORT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_INT:
                          opcode.id = SPVM_OPCODE_C_ID_RETURN_INT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_LONG:
                          opcode.id = SPVM_OPCODE_C_ID_RETURN_LONG;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_FLOAT:
                          opcode.id = SPVM_OPCODE_C_ID_RETURN_FLOAT;
                          break;
                        case SPVM_BASIC_TYPE_C_ID_DOUBLE:
                          opcode.id = SPVM_OPCODE_C_ID_RETURN_DOUBLE;
                          break;
                        default:
                          opcode.id = SPVM_OPCODE_C_ID_RETURN_OBJECT;
                      }
                    }
                    else {
                      opcode.id = SPVM_OPCODE_C_ID_RETURN_OBJECT;
                    }
                    
                    int32_t index_in = SPVM_OP_get_my_index(compiler, op_cur->first);
                    
                    opcode.operand0 = index_in;
                    
                    SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                  }
                }
                else {
                  SPVM_OPCODE opcode;
                  memset(&opcode, 0, sizeof(SPVM_OPCODE));
                  opcode.id = SPVM_OPCODE_C_ID_RETURN_VOID;
                  SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);
                }
                
                int32_t return_opcode_index = opcode_array->length - 1;
                
                SPVM_LIST_push(return_goto_opcode_index_stack, (void*)(intptr_t)return_opcode_index);
                
                break;
              }
            }
            
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
            }
          }
          if (finish) {
            break;
          }
        }
      }

      SPVM_OPCODE opcode;
      memset(&opcode, 0, sizeof(SPVM_OPCODE));
      opcode.id = SPVM_OPCODE_C_ID_END_SUB;
      SPVM_OPCODE_ARRAY_push_opcode(compiler, opcode_array, &opcode);

      sub->opcode_length = opcode_array->length - sub->opcode_base;
      sub->mortal_stack_max = mortal_stack_max;
    }
  }
}
