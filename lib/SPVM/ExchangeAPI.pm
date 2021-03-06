package SPVM::ExchangeAPI;

use strict;
use warnings;

use Encode 'encode', 'decode';
use Carp 'confess';

sub array_to_string {
  my $bin = SPVM::ExchangeAPI::array_to_bin(@_);
  
  my $string = decode('UTF-8', $bin);
  
  return $string;
}

sub array_to_strings {
  my $elems = SPVM::ExchangeAPI::array_to_elems(@_);
  
  my $strs = [];
  
  for (my $i = 0; $i < @$elems; $i++) {
    my $elem = $elems->[$i];
    if (defined $elem) {
      $strs->[$i] = $elem->to_string;
    }
    else {
      $strs->[$i] = undef;
    }
  }
  
  return $strs;
}

sub new_byte_array_from_string {
  my ($env, $string) = @_;
  
  my $bin = encode('UTF-8', $string);
  
  return SPVM::ExchangeAPI::new_byte_array_from_bin($env, $bin);
}

sub new_string { SPVM::ExchangeAPI::new_byte_array_from_string(@_) }

sub new_string_from_bin { SPVM::ExchangeAPI::new_byte_array_from_bin(@_) }

sub new_object_array {
  my ($env, $type_name, $elems) = @_;
  
  my $basic_type_name;
  my $type_dimension = 0;
  if ($type_name =~ /^([a-zA-Z_0-9:]+)((\[\])+)$/) {
    $basic_type_name = $1;
    my $type_dimension_part = $2;
    
    while ($type_dimension_part =~ /\[/g) {
      $type_dimension++;
    }
  }
  
  unless ($type_dimension >= 1 && $type_dimension <= 255) {
    confess "Invalid type dimension(first argument of SPVM::ExchangeAPI::new_object_array)";
  }
  unless (defined $basic_type_name) {
    confess "Invalid basic_type name(first argument of SPVM::ExchangeAPI::new_object_array)";
  }
  
  unless (defined $elems) {
    return undef;
  }
  
  # Check second argument
  unless (ref $elems eq 'ARRAY') {
    confess "Second argument of SPVM::new_object_array must be array reference";
  }
  
  if ($type_dimension == 1) {
    SPVM::ExchangeAPI::_new_object_array($env, $basic_type_name, $elems);
  }
  else {
    SPVM::ExchangeAPI::_new_muldim_array($env, $basic_type_name, $type_dimension - 1, $elems);
  }
}

sub new_mulnum_array {
  my ($env, $type_name, $elems) = @_;
  
  my $basic_type_name;
  my $type_dimension = 0;
  if ($type_name =~ /^([a-zA-Z_0-9:]+)((\[\])+)$/) {
    $basic_type_name = $1;
    my $type_dimension_part = $2;
    
    while ($type_dimension_part =~ /\[/g) {
      $type_dimension++;
    }
  }
  
  unless ($type_dimension == 1) {
    confess "Invalid type dimension(first argument of SPVM::ExchangeAPI::new_mulnum_array)";
  }
  unless (defined $basic_type_name) {
    confess "Invalid basic_type name(first argument of SPVM::ExchangeAPI::new_mulnum_array)";
  }

  unless (defined $elems) {
    return undef;
  }
  
  # Check second argument
  unless (ref $elems eq 'ARRAY') {
    confess "Second argument of SPVM::new_mulnum_array must be array reference";
  }
  
  SPVM::ExchangeAPI::_new_mulnum_array($env, $basic_type_name, $elems);
}

sub new_mulnum_array_from_bin {
  my ($env, $type_name, $elems) = @_;
  
  my $basic_type_name;
  my $type_dimension = 0;
  if ($type_name =~ /^([a-zA-Z_0-9:]+)((\[\])+)$/) {
    $basic_type_name = $1;
    my $type_dimension_part = $2;
    
    while ($type_dimension_part =~ /\[/g) {
      $type_dimension++;
    }
  }
  
  unless ($type_dimension == 1) {
    confess "Invalid type dimension(first argument of SPVM::ExchangeAPI::new_mulnum_array_from_bin)";
  }
  unless (defined $basic_type_name) {
    confess "Invalid basic_type name(first argument of SPVM::ExchangeAPI::new_mulnum_array_from_bin)";
  }

  unless (defined $elems) {
    return undef;
  }
  
  SPVM::ExchangeAPI::_new_mulnum_array_from_bin($env, $basic_type_name, $elems);
}

sub set_exception {
  my ($env, $exception) = @_;
  
  $exception = encode('UTF-8', $exception);
  
  _set_exception($env, $exception);
}

sub get_exception {
  my ($env) = @_;
  
  my $exception = _get_exception($env);
  
  $exception = decode('UTF-8', $exception);
  
  return $exception;
}

# other functions is implemented in SPVM.xs

1;

=head1 NAME

SPVM::ExchangeAPI - SPVM Exchange API

=head1 Functions

=head2 call_sub

=head2 memory_blocks_count

=head2 new_byte_array

=head2 new_byte_array_from_bin

=head2 new_byte_array_from_string

=head2 new_double_array

=head2 new_double_array_from_bin

=head2 new_float_array

=head2 new_float_array_from_bin

=head2 new_int_array

=head2 new_int_array_from_bin

=head2 new_long_array

=head2 new_long_array_from_bin

=head2 new_object_array

=head2 new_short_array

=head2 new_short_array_from_bin

=head2 new_string

=head2 new_string_from_bin

=head2 new_mulnum_array

=head2 new_mulnum_array_from_bin

=head2 set_exception

=head2 array_to_bin

=head2 array_to_elems

=head2 array_to_string

=head2 array_to_strings
