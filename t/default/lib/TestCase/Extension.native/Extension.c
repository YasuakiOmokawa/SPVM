#include <stdlib.h>
#include <string.h>

#include <spvm_native.h>

int32_t SPVM_NATIVE_TestCase__Extension__native_use_strlen(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* string = stack[0].oval;
  
  int8_t* bytes = env->get_byte_array_elements(env, string);
  
  int32_t length = (int32_t)strlen((char*)bytes);
  
  stack[0].ival = length;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_env_get_byte_field(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* test_case = stack[0].oval;
  
  int32_t field_index = env->get_field_index(env, test_case, "(byte)x_byte");
  if (field_index < 0) {
    return SPVM_EXCEPTION;
  }
  
  int8_t value = env->get_byte_field(env, test_case, field_index);
  
  stack[0].bval = value;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_env_get_short_field(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* test_case = stack[0].oval;
  
  int32_t field_index = env->get_field_index(env, test_case, "(short)x_short");
  if (field_index < 0) {
    return SPVM_EXCEPTION;
  }
  
  int16_t value = env->get_short_field(env, test_case, field_index);
  
  stack[0].sval = value;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_env_get_int_field(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* test_case = stack[0].oval;
  
  int32_t field_index = env->get_field_index(env, test_case, "(int)x_int");
  if (field_index < 0) {
    return SPVM_EXCEPTION;
  }
  
  int32_t value = env->get_int_field(env, test_case, field_index);
  
  stack[0].ival = value;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_env_get_long_field(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* test_case = stack[0].oval;
  
  int32_t field_index = env->get_field_index(env, test_case, "(long)x_long");
  if (field_index < 0) {
    return SPVM_EXCEPTION;
  }
  
  int64_t value = env->get_long_field(env, test_case, field_index);
  
  stack[0].ival = value;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_env_get_float_field(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* test_case = stack[0].oval;
  
  int32_t field_index = env->get_field_index(env, test_case, "(float)x_float");
  if (field_index < 0) {
    return SPVM_EXCEPTION;
  }
  
  float value = env->get_float_field(env, test_case, field_index);
  
  stack[0].fval = value;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_env_get_double_field(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* test_case = stack[0].oval;
  
  int32_t field_index = env->get_field_index(env, test_case, "(double)x_double");
  if (field_index < 0) {
    return SPVM_EXCEPTION;
  }
  
  double value = env->get_double_field(env, test_case, field_index);
  
  stack[0].ival = value;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_env_get_object_field(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* test_case = stack[0].oval;
  
  int32_t field_index = env->get_field_index(env, test_case, "(TestCase::Minimal)minimal");
  if (field_index < 0) {
    return SPVM_EXCEPTION;
  }
  
  void* value = env->get_object_field(env, test_case, field_index);
  
  stack[0].oval = value;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__sum(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  int32_t total = stack[0].ival + stack[1].ival;
  
  stack[0].ival = total;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__add_int_array(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* obj_nums1 = stack[0].oval;
  void* obj_nums2 = stack[1].oval;
  
  int32_t length = env->get_array_length(env, obj_nums1);
  
  int32_t* nums1 = env->get_int_array_elements(env, obj_nums1);
  int32_t* nums2 = env->get_int_array_elements(env, obj_nums2);
  
  void* obj_nums3 = env->new_int_array_raw(env, length);
  int32_t* nums3 = env->get_int_array_elements(env, obj_nums3);
  
  {
    int32_t i;
    for (i = 0; i < length; i++) {
      nums3[i] = nums1[i] + nums2[i];
    }
  }
  
  stack[0].oval = obj_nums3;
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__call_void_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  
  return SPVM_EXCEPTION;
}

int32_t SPVM_NATIVE_TestCase__Extension__call_byte_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  
  return SPVM_EXCEPTION;
}

int32_t SPVM_NATIVE_TestCase__Extension__call_short_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;

  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  
  return SPVM_EXCEPTION;
}

int32_t SPVM_NATIVE_TestCase__Extension__call_int_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  
  return SPVM_EXCEPTION;
}

int32_t SPVM_NATIVE_TestCase__Extension__call_long_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  
  return SPVM_EXCEPTION;
}

float SPVM_NATIVE_TestCase__Extension__call_float_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  return SPVM_EXCEPTION;
}

int32_t SPVM_NATIVE_TestCase__Extension__call_double_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  
  return SPVM_EXCEPTION;
}

int32_t SPVM_NATIVE_TestCase__Extension__call_object_sub_exception_native(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  void* exception = env->new_string_raw(env, "Exception", 0);
  env->set_exception(env, exception);
  
  return SPVM_EXCEPTION;
}

int32_t SPVM_NATIVE_TestCase__Extension__mortal_api(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  // Check if object count is zero in extension.t
  int32_t ref_count = 0;
  
  int32_t length = 10;
  // 1
  {
    void* sp_values = env->new_byte_array(env, length);
    ref_count += env->get_ref_count(env, sp_values);
  }
  // 2
  {
    void* sp_values = env->new_short_array(env, length);
    ref_count += env->get_ref_count(env, sp_values);
  }
  // 3
  {
    void* sp_values = env->new_int_array(env, length);
    ref_count += env->get_ref_count(env, sp_values);
  }
  // 4
  {
    void* sp_values = env->new_long_array(env, length);
    ref_count += env->get_ref_count(env, sp_values);
  }
  // 5
  {
    void* sp_values = env->new_float_array(env, length);
    ref_count += env->get_ref_count(env, sp_values);
  }
  // 6
  {
    void* sp_values = env->new_long_array(env, length);
    ref_count += env->get_ref_count(env, sp_values);
  }
  // 7
  {
    void* sp_values = env->new_string(env, "foo", 0);
    ref_count += env->get_ref_count(env, sp_values);
  }
  // 8
  {
    int32_t basic_type_id = env->get_basic_type_id(env, "TestCase::Minimal");
    if (basic_type_id < 0) {
      return SPVM_EXCEPTION;
    }
    void* sp_object = env->new_object(env, basic_type_id);
    ref_count += env->get_ref_count(env, sp_object);
  }
  // 9
  {
    int32_t basic_type_id = env->get_basic_type_id(env, "TestCase::Minimal");
    if (basic_type_id < 0) {
      return SPVM_EXCEPTION;
    }
    void* sp_objects = env->new_object_array(env, basic_type_id, 3);
    ref_count += env->get_ref_count(env, sp_objects);
  }
  // 10
  {
    int32_t basic_type_id = env->get_basic_type_id(env, "TestCase::Minimal");
    if (basic_type_id < 0) {
      return SPVM_EXCEPTION;
    }
    void* sp_objects = env->new_object_array(env, basic_type_id, 3);
    ref_count += env->get_ref_count(env, sp_objects);
  }
  // 11
  {
    int32_t basic_type_id = env->get_basic_type_id(env, "TestCase::Pointer");
    if (basic_type_id < 0) {
      return SPVM_EXCEPTION;
    }
    void* sp_objects = env->new_pointer(env, basic_type_id, NULL);
    ref_count += env->get_ref_count(env, sp_objects);
  }
  
  if (ref_count == 11) {
    stack[0].ival = 1;
  }
  else {
    stack[0].ival = 0;
  }
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__enter_scope_leave_scope(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  int32_t length = 10;
  int32_t start_objects_count = env->get_objects_count(env);
  env->new_int_array(env, length);
  env->new_int_array(env, length);
  int32_t before_enter_objects_count = env->get_objects_count(env);
  int32_t before_leave_objects_count;
  {
    int32_t scope_id = env->enter_scope(env);

    env->new_int_array(env, length);
    env->new_int_array(env, length);
    env->new_int_array(env, length);
    
    before_leave_objects_count = env->get_objects_count(env);
    env->leave_scope(env, scope_id);
  }
  
  int32_t after_leave_objects_counts = env->get_objects_count(env);
  
  stack[0].ival = 0;
  if ((before_enter_objects_count - start_objects_count) == 2) {
    if (before_enter_objects_count == after_leave_objects_counts) {
      if ((before_leave_objects_count - before_enter_objects_count) == 3) {
        stack[0].ival = 1;
      }
    }
  }
  
  return SPVM_SUCCESS;
}

int32_t SPVM_NATIVE_TestCase__Extension__native_call_sub(SPVM_ENV* env, SPVM_VALUE* stack) {
  (void)env;
  (void)stack;
  
  int32_t sub_id = env->get_sub_id(env, "TestCase::Extension", "(int)get_my_value(int)");
  if (sub_id < 0) {
    return SPVM_EXCEPTION;
  }
  int32_t output;
  {
    stack[0].ival = 5;
    int32_t exception_flag = env->call_sub(env, sub_id, stack);
    if (exception_flag) {
      return SPVM_EXCEPTION;
    }
    output = stack[0].ival;
  }
  
  stack[0].ival = 0;
  
  if (output == 5) {
    stack[0].ival = 1;
  }
  
  return SPVM_SUCCESS;
}
