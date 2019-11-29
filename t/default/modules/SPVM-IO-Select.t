use lib "t/lib";
use TestAuto;

use strict;
use warnings;

use Test::More 'no_plan';

use SPVM 'TestCase::Lib::SPVM::IO::Select';

# Start objects count
my $start_memory_blocks_count = SPVM::get_memory_blocks_count();

# Select
{
  ok(TestCase::Lib::SPVM::IO::Select->add);
  ok(TestCase::Lib::SPVM::IO::Select->remove);
  ok(TestCase::Lib::SPVM::IO::Select->exists);
  if ($^O ne 'MSWin32') {
    ok(TestCase::Lib::SPVM::IO::Select->can_read);
    ok(TestCase::Lib::SPVM::IO::Select->can_write);
    ok(TestCase::Lib::SPVM::IO::Select->has_exception);
  }
}

# All object is freed
my $end_memory_blocks_count = SPVM::get_memory_blocks_count();
is($end_memory_blocks_count, $start_memory_blocks_count);