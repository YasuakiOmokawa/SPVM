use lib "t/lib";
use TestAuto;

use strict;
use warnings;

use Test::More 'no_plan';

use SPVM 'TestCase::Example';

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

# Positive infinity(unix like system : inf, Windows : 1.#INF)
my $POSITIVE_INFINITY = 9**9**9;

my $NaN = 9**9**9 / 9**9**9;

my $nan_re = qr/(nan|ind)/i;

# Start objects count
my $start_objects_count = SPVM::get_objects_count();

# Call subroutine
{
  ok(TestCase::Example->sin());
  ok(TestCase::Example->cos());
  ok(TestCase::Example->tan());
}

# float
{
  ok(TestCase::Example->float_pass_positive_infinity($POSITIVE_INFINITY));
  ok(TestCase::Example->float_pass_nan($NaN));
  
  ok(TestCase::Example->isinff());
  ok(TestCase::Example->isfinitef());
  ok(TestCase::Example->isnanf());
  
  is(SPVM::Example->INFINITYF(), $POSITIVE_INFINITY);
  
  like(SPVM::Example->NANF(), $nan_re);
}

# SPVM::Double
{
  ok(TestCase::Example->double_pass_positive_infinity($POSITIVE_INFINITY));
  ok(TestCase::Example->double_pass_nan($NaN));
  
  ok(TestCase::Example->isinf());
  ok(TestCase::Example->isfinite());
  ok(TestCase::Example->isnan());
  
  is(SPVM::Example->INFINITY(), $POSITIVE_INFINITY);
  
  like(SPVM::Example->NAN(), $nan_re);
}

{
  ok(TestCase::Example->byte_constant());
  ok(TestCase::Example->short_constant());
  ok(TestCase::Example->int_constant());
}

{
  # Check not Inf or NaN in Perl value
  like(SPVM::Example->FLT_MAX(), qr/[0-9]/);
  like(SPVM::Example->FLT_MIN(), qr/[0-9]/);
}

# All object is freed
my $end_objects_count = SPVM::get_objects_count();
is($end_objects_count, $start_objects_count);
