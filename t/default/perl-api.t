use lib "t/lib";
use JITTestAuto;

use strict;
use warnings;
use utf8;
use Data::Dumper;
use File::Basename 'basename';
use FindBin;

use Test::More 'no_plan';

my $file = basename $0;

use FindBin;

use SPVM 'TestCase'; my $use_test_line = __LINE__;
use SPVM 'CORE'; my $use_core_line = __LINE__;

my $BYTE_MAX = 127;
my $BYTE_MIN = -128;
my $SHORT_MAX = 32767;
my $SHORT_MIN = -32768;
my $INT_MAX = 2147483647;
my $INT_MIN = -2147483648;
my $LONG_MAX = 9223372036854775807;
my $LONG_MIN = -9223372036854775808;
my $FLOAT_PRECICE = 16384.5;
my $DOUBLE_PRECICE = 65536.5;

{
  # SPVM::Build::ExtUtil tests
  my $spvm_build = SPVM::Build::ExtUtil->new;
  my $func_names = $spvm_build->get_native_func_names($ENV{SPVM_TEST_LIB_DIR}, 'SPVM::TestCase::Extension2');
  is($func_names->[0], 'SPVM__TestCase__Extension2__mul');
  is($func_names->[1], 'SPVM__TestCase__Extension2__one');
}

# Start objects count
my $start_objects_count = SPVM::get_objects_count();

# time
{
  cmp_ok(abs(time - SPVM::CORE->time()), '<', 2);
}

# my variable
{
  ok(SPVM::TestCase->my_var_initialized_zero());
  ok(SPVM::TestCase->my_var_initialized_zero());
}

{
  ok(SPVM::TestCase->new_near_small_base_object_max_byte_size_use_memory_pool());
}

=pod
is_deeply(
  \@SPVM::PACKAGE_INFOS,
  [
    {name => 'TestCase', file => $file, line => $use_test_line},
    {name => 'CORE', file => $file, line => $use_core_line}
  ]
);
=cut

# SPVM new_object_array_len
{

  # element object array
  {
    my $object_array = SPVM::new_object_array_len("TestCase", 3);
    my $object1 = SPVM::TestCase->new();
    
    $object1->set_x_int(1);
    $object_array->set_element(0, $object1);
    my $object2 = SPVM::TestCase->new();
    $object2->set_x_int(2);
    $object_array->set_element(1, $object2);
    ok(SPVM::TestCase->spvm_new_object_array_len_element_object_array($object_array));
    
    my $object1_get = $object_array->get_element(0);
    my $object2_get = $object_array->get_element(1);
    
    is_deeply($object1_get->get_x_int, 1);
    is_deeply($object2_get->get_x_int, 2);
  }

  # element byte array
  {
    my $object_array = SPVM::new_multi_array_len("byte", 1, 3);
    
    my $object1 = SPVM::new_byte_array([1, 2, 3]);
    $object_array->set_element(0, $object1);
    my $object2 = SPVM::new_byte_array([4, 5, 6]);
    $object_array->set_element(1, $object2);
    ok(SPVM::TestCase->spvm_new_object_array_len_element_byte_array($object_array));
    
    my $object1_get = $object_array->get_element(0);
    my $object2_get = $object_array->get_element(1);
    
    is_deeply($object1_get->to_elements, [1, 2, 3]);
    is_deeply($object2_get->to_elements, [4, 5, 6]);
  }

  # element short array
  {
    my $object_array = SPVM::new_multi_array_len("short", 1, 3);
    my $object1 = SPVM::new_short_array([1, 2, 3]);
    $object_array->set_element(0, $object1);
    my $object2 = SPVM::new_short_array([4, 5, 6]);
    $object_array->set_element(1, $object2);
    ok(SPVM::TestCase->spvm_new_object_array_len_element_short_array($object_array));
    
    my $object1_get = $object_array->get_element(0);
    my $object2_get = $object_array->get_element(1);
    
    is_deeply($object1_get->to_elements, [1, 2, 3]);
    is_deeply($object2_get->to_elements, [4, 5, 6]);
  }

  # element int array
  {
    my $object_array = SPVM::new_multi_array_len("int", 1, 3);
    my $object1 = SPVM::new_int_array([1, 2, 3]);
    $object_array->set_element(0, $object1);
    my $object2 = SPVM::new_int_array([4, 5, 6]);
    $object_array->set_element(1, $object2);
    ok(SPVM::TestCase->spvm_new_object_array_len_element_int_array($object_array));
    
    my $object1_get = $object_array->get_element(0);
    my $object2_get = $object_array->get_element(1);
    
    is_deeply($object1_get->to_elements, [1, 2, 3]);
    is_deeply($object2_get->to_elements, [4, 5, 6]);
  }

  # element long array
  {
    my $object_array = SPVM::new_multi_array_len("long", 1, 3);
    my $object1 = SPVM::new_long_array([1, 2, 3]);
    $object_array->set_element(0, $object1);
    my $object2 = SPVM::new_long_array([4, 5, 6]);
    $object_array->set_element(1, $object2);
    ok(SPVM::TestCase->spvm_new_object_array_len_element_long_array($object_array));
    
    my $object1_get = $object_array->get_element(0);
    my $object2_get = $object_array->get_element(1);
    
    is_deeply($object1_get->to_elements, [1, 2, 3]);
    is_deeply($object2_get->to_elements, [4, 5, 6]);
  }

  # element float array
  {
    my $object_array = SPVM::new_multi_array_len("float", 1, 3);
    my $object1 = SPVM::new_float_array([1, 2, 3]);
    $object_array->set_element(0, $object1);
    my $object2 = SPVM::new_float_array([4, 5, 6]);
    $object_array->set_element(1, $object2);
    ok(SPVM::TestCase->spvm_new_object_array_len_element_float_array($object_array));
    
    my $object1_get = $object_array->get_element(0);
    my $object2_get = $object_array->get_element(1);
    
    is_deeply($object1_get->to_elements, [1, 2, 3]);
    is_deeply($object2_get->to_elements, [4, 5, 6]);
  }

  # element double array
  {
    my $object_array = SPVM::new_multi_array_len("double", 1, 3);
    my $object1 = SPVM::new_double_array([1, 2, 3]);
    $object_array->set_element(0, $object1);
    my $object2 = SPVM::new_double_array([4, 5, 6]);
    $object_array->set_element(1, $object2);
    ok(SPVM::TestCase->spvm_new_object_array_len_element_double_array($object_array));
    
    my $object1_get = $object_array->get_element(0);
    my $object2_get = $object_array->get_element(1);
    
    is_deeply($object1_get->to_elements, [1, 2, 3]);
    is_deeply($object2_get->to_elements, [4, 5, 6]);
  }
}

# get and set
{
  {
    my $sp_values = SPVM::new_byte_array([0, 0]);
    $sp_values->set_element(1, $BYTE_MAX);
    ok(SPVM::TestCase->spvm_set_and_get_byte($sp_values));
    my $value = $sp_values->get_element(1);
    is($value, $BYTE_MAX);
  }
  {
    my $sp_values = SPVM::new_short_array([0, 0]);
    $sp_values->set_element(1, $SHORT_MAX);
    ok(SPVM::TestCase->spvm_set_and_get_short($sp_values));
    my $value = $sp_values->get_element(1);
    is($value, $SHORT_MAX);
  }
  {
    my $sp_values = SPVM::new_int_array([0, 0]);
    $sp_values->set_element(1, $INT_MAX);
    ok(SPVM::TestCase->spvm_set_and_get_int($sp_values));
    my $value = $sp_values->get_element(1);
    is($value, $INT_MAX);
  }
  {
    my $sp_values = SPVM::new_long_array([0, 0]);
    $sp_values->set_element(1, $LONG_MAX);
    ok(SPVM::TestCase->spvm_set_and_get_long($sp_values));
    my $value = $sp_values->get_element(1);
    is($value, $LONG_MAX);
  }
  {
    my $sp_values = SPVM::new_float_array([0, 0]);
    $sp_values->set_element(1, $FLOAT_PRECICE);
    ok(SPVM::TestCase->spvm_set_and_get_float($sp_values));
    my $value = $sp_values->get_element(1);
    is($value, $FLOAT_PRECICE);
  }
  {
    my $sp_values = SPVM::new_double_array([0, 0]);
    $sp_values->set_element(1, $DOUBLE_PRECICE);
    ok(SPVM::TestCase->spvm_set_and_get_double($sp_values));
    my $value = $sp_values->get_element(1);
    is($value, $DOUBLE_PRECICE);
  }
}

# SPVM Functions
{
  # to_elements
  {
    {
      my $sp_values = SPVM::new_byte_array([1, $BYTE_MAX, $BYTE_MIN]);
      my $values = $sp_values->to_elements;
      is_deeply($values, [1, $BYTE_MAX, $BYTE_MIN]);
    }
    {
      my $sp_values = SPVM::new_short_array([1, $SHORT_MAX, $SHORT_MIN]);
      my $values = $sp_values->to_elements;
      is_deeply($values, [1, $SHORT_MAX, $SHORT_MIN]);
    }
    {
      my $sp_values = SPVM::new_int_array([1, $INT_MAX, $INT_MIN]);
      my $values = $sp_values->to_elements;
      is_deeply($values, [1, $INT_MAX, $INT_MIN]);
    }
    {
      my $sp_values = SPVM::new_long_array([1, $LONG_MAX, $LONG_MIN]);
      my $values = $sp_values->to_elements;
      is_deeply($values, [1, $LONG_MAX, $LONG_MIN]);
    }
  }

  # to_bin 0 length
  {
    {
      my $sp_values = SPVM::new_byte_array([]);
      my $bin = $sp_values->to_bin;
      is($bin, "");
    }
    {
      my $sp_values = SPVM::new_short_array([]);
      my $bin = $sp_values->to_bin;
      is($bin, "");
    }
    {
      my $sp_values = SPVM::new_int_array([]);
      my $bin = $sp_values->to_bin;
      is($bin, "");
    }
    {
      my $sp_values = SPVM::new_long_array([]);
      my $bin = $sp_values->to_bin;
      is($bin, "");
    }
    {
      my $sp_values = SPVM::new_float_array([]);
      my $bin = $sp_values->to_bin;
      is($bin, "");
    }
    {
      my $sp_values = SPVM::new_double_array([]);
      my $bin = $sp_values->to_bin;
      is($bin, "");
    }
  }
    
  # to_bin
  {
    {
      my $sp_values = SPVM::new_byte_array([1, 2, $BYTE_MAX]);
      my $bin = $sp_values->to_bin;
      
      my @values = unpack('c3', $bin);
      is_deeply(\@values, [1, 2, $BYTE_MAX]);
    }
    {
      my $sp_values = SPVM::new_short_array([1, 2, $SHORT_MAX]);
      my $bin = $sp_values->to_bin;
      
      my @values = unpack('s3', $bin);
      is_deeply(\@values, [1, 2, $SHORT_MAX]);
    }
    {
      my $sp_values = SPVM::new_int_array([1, 2, $INT_MAX]);
      my $bin = $sp_values->to_bin;
      
      my @values = unpack('l3', $bin);
      is_deeply(\@values, [1, 2, $INT_MAX]);
    }
    {
      my $sp_values = SPVM::new_long_array([1, 2, $LONG_MAX]);
      my $bin = $sp_values->to_bin;
      
      my @values = unpack('q3', $bin);
      is_deeply(\@values, [1, 2, $LONG_MAX]);
    }
    {
      my $sp_values = SPVM::new_float_array([1, 2, $FLOAT_PRECICE]);
      my $bin = $sp_values->to_bin;
      
      my @values = unpack('f3', $bin);
      is_deeply(\@values, [1, 2, $FLOAT_PRECICE]);
    }
    {
      my $sp_values = SPVM::new_double_array([1, 2, $DOUBLE_PRECICE]);
      my $bin = $sp_values->to_bin;
      
      my @values = unpack('d3', $bin);
      is_deeply(\@values, [1, 2, $DOUBLE_PRECICE]);
    }
  }

  # new_xxx_array_string
  {
    {
      my $sp_values = SPVM::new_byte_array_string("あ");
      ok(SPVM::TestCase->spvm_new_byte_array_string($sp_values));
    }
  }
  
  # new_xxx_array_bin
  {
    {
      my $sp_values = SPVM::new_byte_array_bin("abc");
      ok(SPVM::TestCase->spvm_new_byte_array_bin($sp_values));
    }
    {
      my $bin = pack('c3', 97, 98, $BYTE_MAX);
      
      my $sp_values = SPVM::new_byte_array_bin($bin);
      ok(SPVM::TestCase->spvm_new_byte_array_bin_pack($sp_values));
    }
    {
      my $bin = pack('c3', 97, 98, $BYTE_MAX);
      
      my $sp_values = SPVM::new_byte_array_bin($bin);
      ok(SPVM::TestCase->spvm_new_byte_array_bin_pack($sp_values));
    }
    {
      my $bin = pack('s3', 97, 98, $SHORT_MAX);
      
      my $sp_values = SPVM::new_short_array_bin($bin);
      ok(SPVM::TestCase->spvm_new_short_array_bin_pack($sp_values));
    }
    {
      my $bin = pack('l3', 97, 98, $INT_MAX);
      
      my $sp_values = SPVM::new_int_array_bin($bin);
      ok(SPVM::TestCase->spvm_new_int_array_bin_pack($sp_values));
    }
    {
      my $bin = pack('q3', 97, 98, $LONG_MAX);
      
      my $sp_values = SPVM::new_long_array_bin($bin);
      ok(SPVM::TestCase->spvm_new_long_array_bin_pack($sp_values));
    }
    {
      my $bin = pack('f3', 97, 98, $FLOAT_PRECICE);
      
      my $sp_values = SPVM::new_float_array_bin($bin);
      ok(SPVM::TestCase->spvm_new_float_array_bin_pack($sp_values));
    }
    {
      my $bin = pack('d3', 97, 98, $DOUBLE_PRECICE);
      
      my $sp_values = SPVM::new_double_array_bin($bin);
      ok(SPVM::TestCase->spvm_new_double_array_bin_pack($sp_values));
    }
  }
}


# SPVM::Object::Array
{
  my $sp_values = SPVM::new_int_array_len(3);
  $sp_values->set_elements([1, 2, 3]);
}

# byte
{
  my $total = SPVM::TestCase->sum_byte(8, 3);
  is($total, 11);
}

# short
{
  my $total = SPVM::TestCase->sum_short(8, 3);
  is($total, 11);
}

# int
{
  my $total = SPVM::TestCase->sum_int(8, 3);
  is($total, 11);
}

# long
{
  my $total = SPVM::TestCase->sum_long(8, 3);
  is($total, 11);
}
{
  my $total = SPVM::TestCase->sum_long(9223372036854775806, 1);
  is($total, 9223372036854775807);
}

# float
{
  my $total = SPVM::TestCase->sum_float(0.25, 0.25);
  cmp_ok($total, '==', 0.5);
}

# double
{
  my $total = SPVM::TestCase->sum_double(0.25, 0.25);
  cmp_ok($total, '==', 0.5);
}

# .
{
  {
    is("ab", SPVM::TestCase->concat_special_assign()->to_bin);
    is("ab", SPVM::TestCase->concat()->to_bin);
  }
}

# String
{
  {
    my $values = SPVM::TestCase->string_empty();
    is($values->to_bin, "");
  }
  
  {
    my $values = SPVM::TestCase->string_utf8();
    is($values->to_string, "あいうえお");
  }
}

# All object is freed
my $end_objects_count = SPVM::get_objects_count();
is($end_objects_count, $start_objects_count);

