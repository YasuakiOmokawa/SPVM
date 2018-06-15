#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>

#include "spvm_compiler.h"
#include "spvm_hash.h"
#include "spvm_list.h"
#include "spvm_util_allocator.h"
#include "spvm_runtime.h"
#include "spvm_op.h"
#include "spvm_sub.h"
#include "spvm_package.h"
#include "spvm_sub.h"
#include "spvm_my.h"
#include "spvm_type.h"
#include "spvm_field.h"
#include "spvm_object.h"
#include "spvm_native.h"
#include "spvm_opcode_builder.h"
#include "spvm_csource_builder.h"
#include "spvm_list.h"
#include "spvm_csource_builder.h"
#include "spvm_string_buffer.h"
#include "spvm_basic_type.h"
#include "spvm_use.h"

SPVM_ENV* SPVM_XS_UTIL_get_env() {
  
  SV* sv_env = get_sv("SPVM::ENV", 0);
  
  SPVM_ENV* env = INT2PTR(SPVM_ENV*, SvIV(SvRV(sv_env)));
  
  return env;
}

SV* SPVM_XS_UTIL_new_sv_object(SPVM_OBJECT* object, const char* package) {
  // Create object
  size_t iv_object = PTR2IV(object);
  SV* sviv_object = sv_2mortal(newSViv(iv_object));
  SV* sv_object = sv_2mortal(newRV_inc(sviv_object));
  HV* hv_class = gv_stashpv(package, 0);
  sv_bless(sv_object, hv_class);
  
  return sv_object;
}

SV* SPVM_XS_UTIL_create_sv_type_name(int32_t basic_type_id, int32_t dimension) {
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)env->get_runtime(env);
  SPVM_COMPILER* compiler = runtime->compiler;

  SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, basic_type_id);
  assert(basic_type);

  SV* sv_type_name = sv_2mortal(newSVpv(basic_type->name, 0));
  
  int32_t dim_index;
  for (dim_index = 0; dim_index < dimension; dim_index++) {
    sv_catpv(sv_type_name, "[]");
  }
  
  return sv_type_name;
}

SPVM_OBJECT* SPVM_XS_UTIL_get_object(SV* sv_object) {
  
  if (SvOK(sv_object)) {
    size_t iv_object = SvIV(SvRV(sv_object));
    SPVM_OBJECT* object = INT2PTR(SPVM_OBJECT*, iv_object);
    
    return object;
  }
  else {
    return NULL;
  }
}

MODULE = SPVM::Data		PACKAGE = SPVM::Data

SV*
DESTROY(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_object = ST(0);
  
  assert(SvOK(sv_object));
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // Get object
  void* object = SPVM_XS_UTIL_get_object(sv_object);
  
  assert(env->get_ref_count(env, object));
  
  // Decrement reference count
  env->dec_ref_count(env, object);
  
  XSRETURN(0);
}

MODULE = SPVM::Data::Array		PACKAGE = SPVM::Data::Array

SV*
set_elements(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_array = ST(0);
  SV* sv_values = ST(1);
  
  if (!(SvROK(sv_values) && sv_derived_from(sv_values, "ARRAY"))) {
    croak("Values must be array refenrece)");
  }
  
  AV* av_values = (AV*)SvRV(sv_values);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // Get object
  SPVM_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);

  int32_t length = env->get_array_length(env, array);

  // Check length
  if (av_len(av_values) + 1 != length) {
    croak("Elements length must be same as array length)");
  }
  
  int32_t basic_type_id = array->basic_type_id;
  int32_t dimension = array->dimension;
  
  if (dimension == 1) {
    switch (basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_BYTE: {
        int8_t* elements = env->get_byte_array_elements(env, array);
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV** sv_value_ptr = av_fetch(av_values, i, 0);
            SV* sv_value = sv_value_ptr ? *sv_value_ptr : &PL_sv_undef;
            elements[i] = (int8_t)SvIV(sv_value);
          }
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_SHORT: {
        int16_t* elements = env->get_short_array_elements(env, array);

        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV** sv_value_ptr = av_fetch(av_values, i, 0);
            SV* sv_value = sv_value_ptr ? *sv_value_ptr : &PL_sv_undef;
            elements[i] = (int16_t)SvIV(sv_value);
          }
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_INT: {
        int32_t* elements = env->get_int_array_elements(env, array);
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV** sv_value_ptr = av_fetch(av_values, i, 0);
            SV* sv_value = sv_value_ptr ? *sv_value_ptr : &PL_sv_undef;
            elements[i] = (int32_t)SvIV(sv_value);
          }
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_LONG: {
        int64_t* elements = env->get_long_array_elements(env, array);
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV** sv_value_ptr = av_fetch(av_values, i, 0);
            SV* sv_value = sv_value_ptr ? *sv_value_ptr : &PL_sv_undef;
            elements[i] = (int64_t)SvIV(sv_value);
          }
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_FLOAT: {
        float* elements = env->get_float_array_elements(env, array);
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV** sv_value_ptr = av_fetch(av_values, i, 0);
            SV* sv_value = sv_value_ptr ? *sv_value_ptr : &PL_sv_undef;
            elements[i] = (float)SvNV(sv_value);
          }
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
        double* elements = env->get_double_array_elements(env, array);
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV** sv_value_ptr = av_fetch(av_values, i, 0);
            SV* sv_value = sv_value_ptr ? *sv_value_ptr : &PL_sv_undef;
            elements[i] = (double)SvNV(sv_value);
          }
        }
        break;
      }
      default:
        croak("Invalid type");
    }
  }
  else if (dimension > 1) {
    croak("Invalid type");
  }
  
  XSRETURN(0);
}

SV*
set_bin(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_array = ST(0);
  SV* sv_bin = ST(1);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // Get object
  SPVM_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = env->get_array_length(env, array);
  int32_t basic_type_id = array->basic_type_id;
  int32_t dimension = array->dimension;

  if (dimension == 1) {
    switch (basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_BYTE: {
        // Check range
        if ((int32_t)sv_len(sv_bin) != length) {
          croak("Data total byte size must be same as array length)");
        }

        int8_t* elements = env->get_byte_array_elements(env, array);
        if (length > 0) {
          memcpy(elements, SvPV_nolen(sv_bin), length);
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_SHORT: {
        if ((int32_t)sv_len(sv_bin) != length * 2) {
          croak("Data total byte size must be same as array length * 2)");
        }
        int16_t* elements = env->get_short_array_elements(env, array);
        if (length > 0) {
          memcpy(elements, SvPV_nolen(sv_bin), length * 2);
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_INT: {
        if ((int32_t)sv_len(sv_bin) != length * 4) {
          croak("Data total byte size must be same as array length * 4)");
        }
        int32_t* elements = env->get_int_array_elements(env, array);
        if (length > 0) {
          memcpy(elements, SvPV_nolen(sv_bin), length * 4);
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_LONG: {
        if ((int32_t)sv_len(sv_bin) != length * 8) {
          croak("Data total byte size must be same as array length * 8)");
        }
        int64_t* elements = env->get_long_array_elements(env, array);
        if (length > 0) {
          memcpy(elements, SvPV_nolen(sv_bin), length * 8);
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_FLOAT: {
        if ((int32_t)sv_len(sv_bin) != length * 4) {
          croak("Data total byte size must be same as array length * 4)");
        }
        float* elements = env->get_float_array_elements(env, array);
        if (length > 0) {
          memcpy(elements, SvPV_nolen(sv_bin), length * 4);
        }
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
        if ((int32_t)sv_len(sv_bin) != length * 8) {
          croak("Data total byte size must be same as array length * 8)");
        }
        double* elements = env->get_double_array_elements(env, array);
        if (length > 0) {
          memcpy(elements, SvPV_nolen(sv_bin), length * 8);
        }
        break;
      }
      default:
        croak("Invalid type");
    }
  }
  else if (dimension > 1) {
    croak("Invalid type");
  }
  
  XSRETURN(0);
}

SV*
set_element(...)
  PPCODE:
{
  (void)RETVAL;
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  SV* sv_array = ST(0);
  SV* sv_index = ST(1);
  SV* sv_value = ST(2);
  
  // Index
  int32_t index = (int32_t)SvIV(sv_index);

  // Array
  SPVM_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  // Length
  int32_t length = env->get_array_length(env, array);
  
  // Check range
  if (index < 0 || index > length - 1) {
    croak("Out of range)");
  }

  int32_t basic_type_id = array->basic_type_id;
  int32_t dimension = array->dimension;

  if (dimension == 1) {
    switch (basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_BYTE: {
        // Value
        int8_t value = (int8_t)SvIV(sv_value);
        
        // Set element
        int8_t* elements = env->get_byte_array_elements(env, array);
        
        elements[index] = value;
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_SHORT: {
        // Value
        int16_t value = (int16_t)SvIV(sv_value);
        
        // Set element
        int16_t* elements = env->get_short_array_elements(env, array);
        
        elements[index] = value;
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_INT: {
        // Value
        int32_t value = (int32_t)SvIV(sv_value);
        
        // Set element
        int32_t* elements = env->get_int_array_elements(env, array);
        
        elements[index] = value;
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_LONG: {
        // Value
        int64_t value = (int64_t)SvIV(sv_value);
        
        // Set element
        int64_t* elements = env->get_long_array_elements(env, array);
        
        elements[index] = value;
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_FLOAT: {
        // Value
        float value = (float)SvNV(sv_value);
        
        // Set element
        float* elements = env->get_float_array_elements(env, array);
        
        elements[index] = value;
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
        // Value
        double value = (double)SvNV(sv_value);
        
        // Set element
        double* elements = env->get_double_array_elements(env, array);
        
        elements[index] = value;
        break;
      }
      default: {
        // Get object
        SPVM_OBJECT* value = SPVM_XS_UTIL_get_object(sv_value);
        
        env->set_object_array_element(env, array, index, value);
      }
    }
  }
  else if (dimension > 1) {
    
    // Get object
    SPVM_OBJECT* value = SPVM_XS_UTIL_get_object(sv_value);
    
    env->set_object_array_element(env, array, index, value);
  }
  else {
    assert(0);
  }
  
  XSRETURN(0);
}

SV*
get_element(...)
  PPCODE:
{
  (void)RETVAL;
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)env->get_runtime(env);
  SPVM_COMPILER* compiler = runtime->compiler;
  
  SV* sv_array = ST(0);
  SV* sv_index = ST(1);
  
  // Index
  int32_t index = (int32_t)SvIV(sv_index);

  // Array
  SPVM_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  // Length
  int32_t length = env->get_array_length(env, array);
  
  // Check range
  if (index < 0 || index > length - 1) {
    croak("Out of range)");
  }

  int32_t basic_type_id = array->basic_type_id;
  int32_t dimension = array->dimension;

  SV* sv_value;
  _Bool is_object = 0;
  if (dimension == 1) {
    switch (basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_BYTE: {
        // Get element
        int8_t* elements = env->get_byte_array_elements(env, array);
        int8_t value = elements[index];
        sv_value = sv_2mortal(newSViv(value));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_SHORT: {
        // Get element
        int16_t* elements = env->get_short_array_elements(env, array);
        int16_t value = elements[index];
        sv_value = sv_2mortal(newSViv(value));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_INT: {
        // Get element
        int32_t* elements = env->get_int_array_elements(env, array);
        int32_t value = elements[index];
        sv_value = sv_2mortal(newSViv(value));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_LONG: {
        // Get element
        int64_t* elements = env->get_long_array_elements(env, array);
        int64_t value = elements[index];
        sv_value = sv_2mortal(newSViv(value));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_FLOAT: {
        // Get element
        float* elements = env->get_float_array_elements(env, array);
        float value = elements[index];
        sv_value = sv_2mortal(newSVnv(value));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
        // Get element
        double* elements = env->get_double_array_elements(env, array);
        double value = elements[index];
        sv_value = sv_2mortal(newSVnv(value));
        break;
      }
      default:
        is_object = 1;
    }
  }
  else if (dimension > 1) {
    is_object = 1;
  }
  
  if (is_object) {
    // Element dimension
    int32_t element_dimension = array->dimension - 1;
    
    // Element type id
    SPVM_BASIC_TYPE* basic_type = SPVM_LIST_fetch(compiler->basic_types, array->basic_type_id);

    // Index
    SPVM_OBJECT* value = env->get_object_array_element(env, array, index);
    if (value != NULL) {
      env->inc_ref_count(env, value);
    }
    
    if (element_dimension == 0) {
      SV* sv_basic_type_name = sv_2mortal(newSVpv(basic_type->name, 0));
      
      sv_value = SPVM_XS_UTIL_new_sv_object(value, SvPV_nolen(sv_basic_type_name));
    }
    else if (element_dimension > 0) {
      sv_value = SPVM_XS_UTIL_new_sv_object(value, "SPVM::Data::Array");
    }
  }
  
  XPUSHs(sv_value);
  XSRETURN(1);
}

SV*
to_elements(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_array = ST(0);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // Get object
  SPVM_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = env->get_array_length(env, array);

  int32_t basic_type_id = array->basic_type_id;
  int32_t dimension = array->dimension;
  
  SV* sv_values;
  if (dimension == 1) {
    switch (basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_BYTE: {
        int8_t* elements = env->get_byte_array_elements(env, array);
        AV* av_values = (AV*)sv_2mortal((SV*)newAV());
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV* sv_value = sv_2mortal(newSViv(elements[i]));
            av_push(av_values, SvREFCNT_inc(sv_value));
          }
        }
        sv_values = sv_2mortal(newRV_inc((SV*)av_values));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_SHORT: {
        int16_t* elements = env->get_short_array_elements(env, array);
        AV* av_values = (AV*)sv_2mortal((SV*)newAV());
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV* sv_value = sv_2mortal(newSViv(elements[i]));
            av_push(av_values, SvREFCNT_inc(sv_value));
          }
        }
        sv_values = sv_2mortal(newRV_inc((SV*)av_values));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_INT: {
        int32_t* elements = env->get_int_array_elements(env, array);
        AV* av_values = (AV*)sv_2mortal((SV*)newAV());
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV* sv_value = sv_2mortal(newSViv(elements[i]));
            av_push(av_values, SvREFCNT_inc(sv_value));
          }
        }
        sv_values = sv_2mortal(newRV_inc((SV*)av_values));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_LONG: {
        int64_t* elements = env->get_long_array_elements(env, array);
        AV* av_values = (AV*)sv_2mortal((SV*)newAV());
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV* sv_value = sv_2mortal(newSViv(elements[i]));
            av_push(av_values, SvREFCNT_inc(sv_value));
          }
        }
        sv_values = sv_2mortal(newRV_inc((SV*)av_values));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_FLOAT: {
        float* elements = env->get_float_array_elements(env, array);
        AV* av_values = (AV*)sv_2mortal((SV*)newAV());
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV* sv_value = sv_2mortal(newSVnv(elements[i]));
            av_push(av_values, SvREFCNT_inc(sv_value));
          }
        }
        sv_values = sv_2mortal(newRV_inc((SV*)av_values));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
        double* elements = env->get_double_array_elements(env, array);
        AV* av_values = (AV*)sv_2mortal((SV*)newAV());
        {
          int32_t i;
          for (i = 0; i < length; i++) {
            SV* sv_value = sv_2mortal(newSVnv(elements[i]));
            av_push(av_values, SvREFCNT_inc(sv_value));
          }
        }
        sv_values = sv_2mortal(newRV_inc((SV*)av_values));
        break;
      }
      default:
        croak("Invalid type");
    }
  }
  else if (dimension > 1) {
    croak("Invalid type");
  }
  
  XPUSHs(sv_values);
  XSRETURN(1);
}

SV*
to_bin(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_array = ST(0);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // Get object
  SPVM_OBJECT* array = SPVM_XS_UTIL_get_object(sv_array);
  
  int32_t length = env->get_array_length(env, array);

  int32_t basic_type_id = array->basic_type_id;
  int32_t dimension = array->dimension;
  
  SV* sv_bin;
  if (dimension == 1) {
    switch (basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_BYTE: {
        int8_t* elements = env->get_byte_array_elements(env, array);
        
        sv_bin = sv_2mortal(newSVpvn((char*)elements, length));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_SHORT: {
        int16_t* elements = env->get_short_array_elements(env, array);
        
        sv_bin = sv_2mortal(newSVpvn((char*)elements, length * 2));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_INT: {
        int32_t* elements = env->get_int_array_elements(env, array);
        
        sv_bin = sv_2mortal(newSVpvn((char*)elements, length * 4));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_LONG: {
        int64_t* elements = env->get_long_array_elements(env, array);
        
        sv_bin = sv_2mortal(newSVpvn((char*)elements, length * 8));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_FLOAT: {
        float* elements = env->get_float_array_elements(env, array);
        
        sv_bin = sv_2mortal(newSVpvn((char*)elements, length * 4));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
        double* elements = env->get_double_array_elements(env, array);
        
        sv_bin = sv_2mortal(newSVpvn((char*)elements, length * 8));
        break;
      }
      default:
        croak("Invalid type");
    }
  }
  else if (dimension > 1) {
    croak("Invalid type");
  }
  
  XPUSHs(sv_bin);
  XSRETURN(1);
}

MODULE = SPVM::Build::SPVMInfo		PACKAGE = SPVM::Build::SPVMInfo

SV*
get_subs(...)
  PPCODE:
{
  (void)RETVAL;

  SV* sv_compiler = ST(0);
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  SV* sv_package_name = ST(1);
  const char* package_name = SvPV_nolen(sv_package_name);

  SPVM_OP* op_package = SPVM_HASH_search(compiler->op_package_symtable, package_name, strlen(package_name));
  SPVM_PACKAGE* package = op_package->uv.package;
  
  AV* av_subs = (AV*)sv_2mortal((SV*)newAV());
  {
    int32_t sub_index;
    for (sub_index = 0; sub_index < package->op_subs->length; sub_index++) {
      
      SPVM_OP* op_sub = SPVM_LIST_fetch(package->op_subs, sub_index);
      SPVM_SUB* sub = op_sub->uv.sub;
      
      // Subroutine name
      const char* sub_abs_name = sub->abs_name;
      SV* sv_sub_abs_name = sv_2mortal(newSVpvn(sub_abs_name, strlen(sub_abs_name)));
      
      // Subroutine id
      int32_t sub_id = sub->id;
      SV* sv_sub_id = sv_2mortal(newSViv(sub_id));

      // Subroutine is_enum
      int32_t sub_is_enum = sub->is_enum;
      SV* sv_sub_is_enum = sv_2mortal(newSViv(sub_is_enum));

      // Subroutine have_native_desc
      int32_t sub_have_native_desc = sub->have_native_desc;
      SV* sv_sub_have_native_desc = sv_2mortal(newSViv(sub_have_native_desc));

      // Subroutine have_compile_desc
      int32_t sub_have_compile_desc = sub->have_compile_desc;
      SV* sv_sub_have_compile_desc = sv_2mortal(newSViv(sub_have_compile_desc));

      // Subroutine
      HV* hv_sub = (HV*)sv_2mortal((SV*)newHV());
      
      hv_store(hv_sub, "abs_name", strlen("abs_name"), SvREFCNT_inc(sv_sub_abs_name), 0);
      hv_store(hv_sub, "is_enum", strlen("is_enum"), SvREFCNT_inc(sv_sub_is_enum), 0);
      hv_store(hv_sub, "have_native_desc", strlen("have_native_desc"), SvREFCNT_inc(sv_sub_have_native_desc), 0);
      hv_store(hv_sub, "have_compile_desc", strlen("have_compile_desc"), SvREFCNT_inc(sv_sub_have_compile_desc), 0);
      
      SV* sv_sub = sv_2mortal(newRV_inc((SV*)hv_sub));
      av_push(av_subs, SvREFCNT_inc((SV*)sv_sub));
    }
  }
  
  SV* sv_subs = sv_2mortal(newRV_inc((SV*)av_subs));
  
  XPUSHs(sv_subs);
  XSRETURN(1);
}

SV*
get_package_names(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_compiler = ST(0);
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  AV* av_package_names = (AV*)sv_2mortal((SV*)newAV());
  
  {
    int32_t package_index;
    for (package_index = 0; package_index < compiler->op_packages->length; package_index++) {
      SPVM_OP* op_package = SPVM_LIST_fetch(compiler->op_packages, package_index);
      SPVM_PACKAGE* package = op_package->uv.package;
      
      // Package name
      const char* package_name = package->op_name->uv.name;
      SV* sv_package_name = sv_2mortal(newSVpvn(package_name, strlen(package_name)));
      
      av_push(av_package_names, SvREFCNT_inc((SV*)sv_package_name));
    }
  }
  
  SV* sv_package_names = sv_2mortal(newRV_inc((SV*)av_package_names));
  
  XPUSHs(sv_package_names);
  XSRETURN(1);
}

SV*
get_package_load_path(...)
  PPCODE:
{
  (void)RETVAL;

  SV* sv_compiler = ST(0);
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  SV* sv_package_name = ST(1);
  
  const char* package_name = SvPV_nolen(sv_package_name);
  
  // Subroutine information
  SPVM_OP* op_package = SPVM_HASH_search(compiler->op_package_symtable, package_name, strlen(package_name));;
  SPVM_PACKAGE* package = op_package->uv.package;
  
  const char* package_load_path = package->load_path;
  
  SV* sv_package_load_path = sv_2mortal(newSVpvn(package_load_path, strlen(package_load_path)));
  
  XPUSHs(sv_package_load_path);
  
  XSRETURN(1);
}

MODULE = SPVM::Build		PACKAGE = SPVM::Build

SV*
create_compiler(...)
  PPCODE:
{
  (void)RETVAL;

  // Create compiler
  SPVM_COMPILER* compiler = SPVM_COMPILER_new();
  
  // Set compiler
  size_t iv_compiler = PTR2IV(compiler);
  SV* sviv_compiler = sv_2mortal(newSViv(iv_compiler));
  SV* sv_compiler = sv_2mortal(newRV_inc(sviv_compiler));
  
  XPUSHs(sv_compiler);
  XSRETURN(1);
}

SV*
compile_spvm(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_self = ST(0);
  HV* hv_self = (HV*)SvRV(sv_self);

  SV** sv_compiler_ptr = hv_fetch(hv_self, "compiler", strlen("compiler"), 0);
  SV* sv_compiler = sv_compiler_ptr ? *sv_compiler_ptr : &PL_sv_undef;
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  SV** sv_package_infos_ptr = hv_fetch(hv_self, "package_infos", strlen("package_infos"), 0);
  SV* sv_package_infos = sv_package_infos_ptr ? *sv_package_infos_ptr : &PL_sv_undef;
  AV* av_package_infos = (AV*)SvRV(sv_package_infos);
  
  int32_t av_package_infos_length = (int32_t)av_len(av_package_infos) + 1;
  
  {
    int32_t i;
    for (i = 0; i < av_package_infos_length; i++) {
      SV** sv_package_info_ptr = av_fetch(av_package_infos, i, 0);
      SV* sv_package_info = sv_package_info_ptr ? *sv_package_info_ptr : &PL_sv_undef;
      HV* hv_package_info = (HV*)SvRV(sv_package_info);
      
      // Name
      SV** sv_name_ptr = hv_fetch(hv_package_info, "name", strlen("name"), 0);
      SV* sv_name = sv_name_ptr ? *sv_name_ptr : &PL_sv_undef;
      const char* name = SvPV_nolen(sv_name);
      
      // File
      SV** sv_file_ptr = hv_fetch(hv_package_info, "file", strlen("file"), 0);
      SV* sv_file = sv_file_ptr ? *sv_file_ptr : &PL_sv_undef;
      const char* file = SvPV_nolen(sv_file);
      
      // Line
      SV** sv_line_ptr = hv_fetch(hv_package_info, "line", strlen("line"), 0);
      SV* sv_line = sv_line_ptr ? *sv_line_ptr : &PL_sv_undef;
      int32_t line = (int32_t)SvIV(sv_line);
      
      // push package to compiler use stack
      SPVM_OP* op_name_package = SPVM_OP_new_op_name(compiler, name, file, line);
      SPVM_OP* op_type_package = SPVM_OP_build_basic_type(compiler, op_name_package);
      SPVM_OP* op_use_package = SPVM_OP_new_op(compiler, SPVM_OP_C_ID_USE, file, line);
      SPVM_OP_build_use(compiler, op_use_package, op_type_package);
      SPVM_LIST_push(compiler->op_use_stack, op_use_package);
    }
  }
  
  
  // Add include paths
  AV* av_include_paths = get_av("main::INC", 0);;
  int32_t av_include_paths_length = (int32_t)av_len(av_include_paths) + 1;
  {
    int32_t i;
    for (i = 0; i < av_include_paths_length; i++) {
      SV** sv_include_path_ptr = av_fetch(av_include_paths, i, 0);
      SV* sv_include_path = sv_include_path_ptr ? *sv_include_path_ptr : &PL_sv_undef;
      char* include_path = SvPV_nolen(sv_include_path);
      SPVM_LIST_push(compiler->module_include_pathes, include_path);
    }
  }

  // Compile SPVM
  SPVM_COMPILER_compile(compiler);
  SV* sv_compile_success;
  
  if (compiler->error_count > 0) {
    sv_compile_success = sv_2mortal(newSViv(0));
  }
  else {
    sv_compile_success = sv_2mortal(newSViv(1));
  }
  
  XPUSHs(sv_compile_success);
  
  XSRETURN(1);
}

SV*
build_opcode(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_self = ST(0);
  HV* hv_self = (HV*)SvRV(sv_self);

  SV** sv_compiler_ptr = hv_fetch(hv_self, "compiler", strlen("compiler"), 0);
  SV* sv_compiler = sv_compiler_ptr ? *sv_compiler_ptr : &PL_sv_undef;
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  // Build opcode
  SPVM_OPCODE_BUILDER_build_opcode_array(compiler);
  
  XSRETURN(0);
}

SV*
build_runtime(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_self = ST(0);
  HV* hv_self = (HV*)SvRV(sv_self);

  SV** sv_compiler_ptr = hv_fetch(hv_self, "compiler", strlen("compiler"), 0);
  SV* sv_compiler = sv_compiler_ptr ? *sv_compiler_ptr : &PL_sv_undef;
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  // Create run-time
  SPVM_RUNTIME* runtime = SPVM_RUNTIME_new(compiler);
  compiler->runtime = runtime;
  
  // Set ENV
  SPVM_ENV* env = runtime->env;
  size_t iv_env = PTR2IV(env);
  SV* sviv_env = sv_2mortal(newSViv(iv_env));
  SV* sv_env = sv_2mortal(newRV_inc(sviv_env));
  sv_setsv(get_sv("SPVM::ENV", 0), sv_env);
  
  XSRETURN(0);
}

SV*
free_compiler(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_self = ST(0);
  HV* hv_self = (HV*)SvRV(sv_self);

  SV** sv_compiler_ptr = hv_fetch(hv_self, "compiler", strlen("compiler"), 0);
  SV* sv_compiler = sv_compiler_ptr ? *sv_compiler_ptr : &PL_sv_undef;
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  // Free compiler
  SPVM_COMPILER_free(compiler);
  
  XSRETURN(0);
}

MODULE = SPVM::Build::CBuilder::Native		PACKAGE = SPVM::Build::CBuilder::Native

SV*
bind_sub(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_self = ST(0);
  HV* hv_self = (HV*)SvRV(sv_self);
  SV* sv_native_sub_name = ST(1);
  SV* sv_native_address = ST(2);
  
  SV** sv_compiler_ptr = hv_fetch(hv_self, "compiler", strlen("compiler"), 0);
  SV* sv_compiler = sv_compiler_ptr ? *sv_compiler_ptr : &PL_sv_undef;
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  // Native subroutine name
  const char* native_sub_name = SvPV_nolen(sv_native_sub_name);
  
  // Native address
  void* native_address = INT2PTR(void*, SvIV(sv_native_address));
  
  // Set native address to subroutine
  SPVM_OP* op_sub = SPVM_HASH_search(compiler->op_sub_symtable, native_sub_name, strlen(native_sub_name));
  SPVM_SUB* sub = op_sub->uv.sub;
  
  sub->native_address = native_address;
  
  XSRETURN(0);
}

MODULE = SPVM::Build::CBuilder::Precompile		PACKAGE = SPVM::Build::CBuilder::Precompile

SV*
build_package_csource(...)
  PPCODE:
{
  SV* sv_self = ST(0);
  HV* hv_self = (HV*)SvRV(sv_self);
  SV* sv_package_name = ST(1);
  const char* package_name = SvPV_nolen(sv_package_name);
  
  SV** sv_compiler_ptr = hv_fetch(hv_self, "compiler", strlen("compiler"), 0);
  SV* sv_compiler = sv_compiler_ptr ? *sv_compiler_ptr : &PL_sv_undef;
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  
  SPVM_OP* op_package = SPVM_HASH_search(compiler->op_package_symtable, package_name, strlen(package_name));
  int32_t package_id = op_package->uv.package->id;
  
  // String buffer for csource
  SPVM_STRING_BUFFER* string_buffer = SPVM_STRING_BUFFER_new(0);
  
  // Build package csource
  SPVM_CSOURCE_BUILDER_build_package_csource(compiler, string_buffer, package_id);
  
  SV* sv_package_csource = sv_2mortal(newSVpv(string_buffer->buffer, string_buffer->length));
  
  SPVM_STRING_BUFFER_free(string_buffer);
  
  XPUSHs(sv_package_csource);
  XSRETURN(1);
}

SV*
bind_sub(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_self = ST(0);
  HV* hv_self = (HV*)SvRV(sv_self);
  SV* sv_sub_abs_name = ST(1);
  SV* sv_sub_native_address = ST(2);
  
  const char* sub_abs_name = SvPV_nolen(sv_sub_abs_name);
  void* sub_precompile_address = INT2PTR(void*, SvIV(sv_sub_native_address));
  
  SV** sv_compiler_ptr = hv_fetch(hv_self, "compiler", strlen("compiler"), 0);
  SV* sv_compiler = sv_compiler_ptr ? *sv_compiler_ptr : &PL_sv_undef;
  SPVM_COMPILER* compiler = INT2PTR(SPVM_COMPILER*, SvIV(SvRV(sv_compiler)));
  
  SPVM_OP* op_sub = SPVM_HASH_search(compiler->op_sub_symtable, sub_abs_name, strlen(sub_abs_name));
  SPVM_SUB* sub = op_sub->uv.sub;
  
  sub->precompile_address = sub_precompile_address;
  sub->is_compiled = 1;
  
  XSRETURN(0);
}

MODULE = SPVM		PACKAGE = SPVM

SV*
get_objects_count(...)
  PPCODE:
{
  (void)RETVAL;
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  int32_t objects_count = env->get_objects_count(env);
  SV* sv_objects_count = sv_2mortal(newSViv(objects_count));
  
  XPUSHs(sv_objects_count);
  XSRETURN(1);
}

SV*
call_sub(...)
  PPCODE:
{
  (void)RETVAL;
  
  int32_t stack_arg_start = 0;
  
  SV* sv_sub_abs_name = ST(0);
  stack_arg_start++;
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)env->get_runtime(env);
  SPVM_COMPILER* compiler = runtime->compiler;

  const char* sub_abs_name = SvPV_nolen(sv_sub_abs_name);
  int32_t sub_id = env->get_sub_id(env, sub_abs_name);
  
  
  // Subroutine information
  SPVM_OP* op_sub = SPVM_LIST_fetch(compiler->op_subs, sub_id);
  SPVM_SUB* sub = op_sub->uv.sub;
  
  
  // Arguments
  {
    // If class method, first argument is ignored
    if (sub->call_type_id == SPVM_SUB_C_CALL_TYPE_ID_CLASS_METHOD) {
      stack_arg_start++;
    }
    
    int32_t arg_index;
    // Check argument count
    if (items - stack_arg_start != sub->op_args->length) {
      croak("Argument count is defferent");
    }
    
    for (arg_index = 0; arg_index < sub->op_args->length; arg_index++) {
      SV* sv_value = ST(arg_index + stack_arg_start);
      
      SPVM_OP* op_arg = SPVM_LIST_fetch(sub->op_args, arg_index);
      SPVM_TYPE* arg_type = op_arg->uv.my->op_type->uv.type;
      
      int32_t arg_basic_type_id = arg_type->basic_type->id;
      int32_t arg_type_dimension = arg_type->dimension;
      
      SPVM_VALUE* args = runtime->args;
      
      if (arg_type_dimension == 0 && arg_type->basic_type->id >= SPVM_BASIC_TYPE_C_ID_BYTE && arg_type->basic_type->id <= SPVM_BASIC_TYPE_C_ID_DOUBLE) {
        switch (arg_type->basic_type->id) {
          case SPVM_BASIC_TYPE_C_ID_BYTE : {
            int8_t value = (int8_t)SvIV(sv_value);
            args[arg_index].bval = value;
            break;
          }
          case  SPVM_BASIC_TYPE_C_ID_SHORT : {
            int16_t value = (int16_t)SvIV(sv_value);
            args[arg_index].sval = value;
            break;
          }
          case  SPVM_BASIC_TYPE_C_ID_INT : {
            int32_t value = (int32_t)SvIV(sv_value);
            args[arg_index].ival = value;
            break;
          }
          case  SPVM_BASIC_TYPE_C_ID_LONG : {
            int64_t value = (int64_t)SvIV(sv_value);
            args[arg_index].lval = value;
            break;
          }
          case  SPVM_BASIC_TYPE_C_ID_FLOAT : {
            float value = (float)SvNV(sv_value);
            args[arg_index].fval = value;
            break;
          }
          case  SPVM_BASIC_TYPE_C_ID_DOUBLE : {
            double value = (double)SvNV(sv_value);
            args[arg_index].dval = value;
            break;
          }
        }
      }
      else {
        if (!SvOK(sv_value)) {
          args[arg_index].oval = NULL;
        }
        else {
          if (sv_isobject(sv_value)) {
            SV* sv_basic_object = sv_value;
            if (sv_derived_from(sv_basic_object, "SPVM::Data")) {
              
              SPVM_OBJECT* basic_object = SPVM_XS_UTIL_get_object(sv_basic_object);
              
              if (!(basic_object->basic_type_id == arg_type->basic_type->id && basic_object->dimension == arg_type->dimension)) {
                SPVM_TYPE* basic_object_type = SPVM_LIST_fetch(compiler->basic_types, basic_object->basic_type_id);
                SV* sv_arg_type_name = SPVM_XS_UTIL_create_sv_type_name(arg_type->basic_type->id, arg_type->dimension);
                SV* sv_basic_object_type = SPVM_XS_UTIL_create_sv_type_name(basic_object_type->basic_type->id, basic_object_type->dimension);
                
                croak("Argument basic_object type need %s, but %s", SvPV_nolen(sv_arg_type_name), SvPV_nolen(sv_basic_object_type));
              }
              
              args[arg_index].oval = basic_object;
            }
            else {
              croak("Data must be derived from SPVM::Data");
            }
          }
          else {
            croak("Argument must be numeric value or SPVM::Data subclass");
          }
        }
      }
    }
  }
  
  // Return type id
  SPVM_TYPE* return_type = sub->op_return_type->uv.type;

  int32_t return_basic_type_id = return_type->basic_type->id;
  int32_t return_type_dimension = return_type->dimension;
  
  PUSHMARK(SP);
          
  // Return count
  SV* sv_return_value = NULL;
  SPVM_VALUE* args = runtime->args;
  if (return_type_dimension == 0 && return_basic_type_id <= SPVM_BASIC_TYPE_C_ID_DOUBLE) {
    switch (return_basic_type_id) {
      case SPVM_BASIC_TYPE_C_ID_VOID:  {
        env->call_sub(env, sub_id, args);
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_BYTE: {
        env->call_sub(env, sub_id, args);
        sv_return_value = sv_2mortal(newSViv(args[0].bval));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_SHORT: {
        env->call_sub(env, sub_id, args);
        sv_return_value = sv_2mortal(newSViv(args[0].sval));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_INT: {
        env->call_sub(env, sub_id, args);
        sv_return_value = sv_2mortal(newSViv(args[0].ival));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_LONG: {
        env->call_sub(env, sub_id, args);
        sv_return_value = sv_2mortal(newSViv(args[0].lval));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_FLOAT: {
        env->call_sub(env, sub_id, args);
        sv_return_value = sv_2mortal(newSVnv(args[0].fval));
        break;
      }
      case SPVM_BASIC_TYPE_C_ID_DOUBLE: {
        env->call_sub(env, sub_id, args);
        sv_return_value = sv_2mortal(newSVnv(args[0].dval));
        break;
      }
      default:
        assert(0);
    }
  }
  else {
    env->call_sub(env, sub_id, args);
    void* exception = env->get_exception(env);
    if (exception) {
      int32_t length = env->get_array_length(env, exception);
      int8_t* exception_bytes = env->get_byte_array_elements(env, exception);
      SV* sv_exception = sv_2mortal(newSVpvn((char*)exception_bytes, length));
      croak("%s", SvPV_nolen(sv_exception));
    }
    
    void* return_value = args[0].oval;
    sv_return_value = NULL;
    if (return_value != NULL) {
      env->inc_ref_count(env, return_value);
      
      if (return_type_dimension == 0) {
        SV* sv_return_type_name = SPVM_XS_UTIL_create_sv_type_name(return_type->basic_type->id, return_type->dimension);
        
        sv_return_value = SPVM_XS_UTIL_new_sv_object(return_value, SvPV_nolen(sv_return_type_name));
      }
      else if (return_type_dimension > 0) {
        sv_return_value = SPVM_XS_UTIL_new_sv_object(return_value, "SPVM::Data::Array");
      }
    }
    else {
      sv_return_value = &PL_sv_undef;
    }
  }
  SPAGAIN;
  ax = (SP - PL_stack_base) + 1;
  
  void* exception = env->get_exception(env);
  if (exception) {
    int32_t length = env->get_array_length(env, exception);
    int8_t* exception_bytes = env->get_byte_array_elements(env, exception);
    SV* sv_exception = sv_2mortal(newSVpvn((char*)exception_bytes, length));
    croak("%s", SvPV_nolen(sv_exception));
  }
  
  int32_t return_count;
  if (return_type_dimension == 0 && return_basic_type_id == SPVM_BASIC_TYPE_C_ID_VOID) {
    return_count = 0;
  }
  else {
    XPUSHs(sv_return_value);
    return_count = 1;
  }
  
  XSRETURN(return_count);
}

SV*
new_byte_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_length = ST(0);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // New array
  void* array =  env->new_byte_array(env, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_byte_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_byte_array);
  XSRETURN(1);
}

SV*
new_short_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_length = ST(0);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // New array
  void* array =  env->new_short_array(env, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_short_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_short_array);
  XSRETURN(1);
}

SV*
new_int_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_length = ST(0);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // New array
  void* array =  env->new_int_array(env, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_int_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_int_array);
  XSRETURN(1);
}

SV*
new_long_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_length = ST(0);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // New array
  void* array =  env->new_long_array(env, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_long_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_long_array);
  XSRETURN(1);
}

SV*
new_float_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_length = ST(0);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // New array
  void* array =  env->new_float_array(env, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_float_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_float_array);
  XSRETURN(1);
}

SV*
new_double_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_length = ST(0);
  
  int32_t length = (int32_t)SvIV(sv_length);
  
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  
  // New array
  void* array =  env->new_double_array(env, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_double_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_double_array);
  XSRETURN(1);
}

SV*
new_object_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_basic_type_name = ST(0);
  SV* sv_length = ST(1);
  
  // Environment
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)env->get_runtime(env);
  SPVM_COMPILER* compiler = runtime->compiler;

  int32_t length = (int32_t)SvIV(sv_length);

  // Element type id
  const char* basic_type_name = SvPV_nolen(sv_basic_type_name);
  
  SPVM_BASIC_TYPE* basic_type = SPVM_HASH_search(compiler->basic_type_symtable, basic_type_name, strlen(basic_type_name));
  assert(basic_type);
  
  // New array
  void* array = env->new_object_array(env, basic_type->id, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_array);
  XSRETURN(1);
}

SV*
new_multi_array_len(...)
  PPCODE:
{
  (void)RETVAL;
  
  SV* sv_basic_type_name = ST(0);
  SV* sv_element_dimension = ST(1);
  SV* sv_length = ST(2);
  
  // Environment
  SPVM_ENV* env = SPVM_XS_UTIL_get_env();
  SPVM_RUNTIME* runtime = (SPVM_RUNTIME*)env->get_runtime(env);
  SPVM_COMPILER* compiler = runtime->compiler;
  
  int32_t length = (int32_t)SvIV(sv_length);

  int32_t element_dimension = (int32_t)SvIV(sv_element_dimension);

  // Element type id
  const char* basic_type_name = SvPV_nolen(sv_basic_type_name);
  
  SPVM_BASIC_TYPE* basic_type = SPVM_HASH_search(compiler->basic_type_symtable, basic_type_name, strlen(basic_type_name));
  assert(basic_type);
  
  // New array
  void* array = env->new_multi_array(env, basic_type->id, element_dimension, length);
  
  // Increment reference count
  env->inc_ref_count(env, array);
  
  // New sv array
  SV* sv_array = SPVM_XS_UTIL_new_sv_object(array, "SPVM::Data::Array");
  
  XPUSHs(sv_array);
  XSRETURN(1);
}
