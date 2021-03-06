package SPVM::BlessedObject::Array;

use base 'SPVM::BlessedObject';

use SPVM::ExchangeAPI;

use overload bool => sub {1}, '""' => sub { shift->to_string }, fallback => 1;

sub length {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::array_length($env, $self);
}

sub to_elems {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::array_to_elems($env, $self);
}

sub to_bin {
  my $self = shift;

  my $env = $self->{env};
  
  SPVM::ExchangeAPI::array_to_bin($env, $self);
}

sub to_string {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::array_to_string($env, $self);
}

sub to_strings {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::array_to_strings($env, $self);
}

sub set {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::array_set($env, $self, @_);
}

sub get {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::array_get($env, $self, @_);
}

1;

=head1 NAME

SPVM::BlessedObject::Array - Array based blessed object

=head2 DESCRIPTION

SPVM::BlessedObject::Array is array based blessed object.

This object contains SPVM array object.

=head1 SYNOPSYS

  # Get the value of a array element
  my $value = $spvm_nums->get(2);

  # Set the value of a array element
  $spvm_nums->set(2 => 5);
  
  # Convert SPVM array to Perl array reference
  my $nums = $spvm_nums->to_elems;

  # Convert SPVM array to Perl binary data
  my $binary = $spvm_nums->to_bin;
  
  # Convert SPVM array to perl text str(decoded str).
  my $str = $spvm_str->to_string;

  # Convert SPVM array to perl array reference which contains decoded strings.
  my $strs = $spvm_strs->to_strings;

=head1 METHODS

=head2 get

  my $value = $spvm_nums->get(2);

Get the value of a array element.

=head2 set

  $spvm_nums->set(2 => 5);

Set the value of a array element

=head2 to_elems

  my $nums = $spvm_nums->to_elems;

Convert SPVM array to Perl array reference.

=head2 to_bin

  my $binary = $spvm_nums->to_bin;

Convert SPVM array to binary data.

Binary data is unpacked by C<unpack> function.

An exmaple when array is int array:

  my @nums = unpack 'l*', $binary;

=head2 to_string

  my $str = $spvm_str->to_string;

Convert SPVM array to perl text str(decoded str).

=head2 to_strings

  my $strs = $spvm_strs->to_strings;

Convert SPVM array to perl array reference which contains decoded strings.
