use strict;
use warnings;
use Config;
use File::Copy 'move';
use File::Path 'mkpath';

use Getopt::Long 'GetOptions';

use FindBin;
use lib "$FindBin::Bin/lib";
use SPVM::Build;

my $object_dir;
GetOptions('object_dir=s' => \$object_dir);

# Module name
my $module_name = shift;

# Build shared library
my $shared_lib_file = SPVM::Build::build_shared_lib(
  module_name => $module_name,
  module_dir => 'lib',
  object_dir => $object_dir
);

# Create shared lib directory
my $shared_lib_dir = $module_name;
$shared_lib_dir =~ s/::/\//g;
$shared_lib_dir .= '.native';
$shared_lib_dir = "blib/arch/auto/$shared_lib_dir";
mkpath $shared_lib_dir;

# blib shared lib file
my $module_base_name = $module_name;
$module_base_name =~ s/^.+:://;
my $dlext = $Config{dlext};
my $blib_shared_lib_file = "$shared_lib_dir/${module_base_name}.$dlext";

# Move shared library file to blib directory
move($shared_lib_file, $blib_shared_lib_file)
  or die "Can't move $shared_lib_file to $blib_shared_lib_file";
