package SPVM::Array::Byte;

use base 'SPVM::Array';

use Encode 'decode';

sub get_string {
  my $self = shift;
  
  my $string = $self->get_string_bytes;
  
  $string = decode('UTF-8', $string);
  
  return $string;
}

1;
