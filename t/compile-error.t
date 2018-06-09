use strict;
use warnings;
use utf8;
use Data::Dumper;
use File::Basename 'basename';
use FindBin;

use SPVM::Build;

use Test::More 'no_plan';

my $file = 't/' . basename $0;

use FindBin;
use lib "$FindBin::Bin/default/lib";

my $ok;

{
  my $build = SPVM::Build->new;
  $build->use('TestCase::CompileError::InvalidType');
  my $success = $build->compile_spvm();
  ok($success == 0);
}

{
  my $build = SPVM::Build->new;
  $build->use('TestCase::CompileError::TypeCantBeDetectedUndef');
  my $success = $build->compile_spvm();
  ok($success == 0);
}

{
  my $build = SPVM::Build->new;
  $build->use('TestCase::CompileError::TypeCantBeDetectedUndefDefault');
  my $success = $build->compile_spvm();
  ok($success == 0);
}

{
  my $build = SPVM::Build->new;
  $build->use('TestCase::CompileError::AssignIncompatibleType::DifferentObject');
  my $success = $build->compile_spvm();
  ok($success == 0);
}

{
  my $build = SPVM::Build->new;
  $build->use('TestCase::CompileError::AssignIncompatibleType::ConstToNoConst');
  my $success = $build->compile_spvm();
  ok($success == 0);
}

{
  my $build = SPVM::Build->new;
  $build->use('TestCase::CompileError::Field::Private');
  my $success = $build->compile_spvm();
  ok($success == 0);
}

{
  my $build = SPVM::Build->new;
  $build->use('TestCase::CompileError::New::Private');
  my $success = $build->compile_spvm();
  ok($success == 0);
}
