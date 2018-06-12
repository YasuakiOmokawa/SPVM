package SPVM::Build::Util;

use strict;
use warnings;
use Carp 'croak';
use Config;
use File::Basename 'dirname', 'basename';

use SPVM::Build::Config;

# SPVM::Build::tUtil is used from Makefile.PL
# so this module must be wrote as pure per script, not contain XS and don't use any other SPVM modules except for SPVM::Build::Config.

sub get_shared_lib_func_address {
  my ($shared_lib_file, $shared_lib_func_name) = @_;
  
  my $native_address;
  
  if ($shared_lib_file) {
    my $dll_libref = DynaLoader::dl_load_file($shared_lib_file);
    if ($dll_libref) {
      $native_address = DynaLoader::dl_find_symbol($dll_libref, $shared_lib_func_name);
    }
    else {
      return;
    }
  }
  else {
    return;
  }
  
  return $native_address;
}

sub convert_module_path_to_shared_lib_path {
  my ($module_path, $category) = @_;
  
  my $module_dir = dirname $module_path;
  my $base_name = basename $module_path;
  $base_name =~ s/\.[^.]+$//;
  
  my $shared_lib_path .= "$module_dir/$base_name.$category/$base_name.$Config{dlext}";
  
  return $shared_lib_path;
}

sub remove_package_part_from_path {
  my ($path, $package_name) = @_;
  
  $path =~ s/\.spvm$//;
  my $package_path = $package_name;
  $package_path =~ s/::/\//g;
  $path =~ s/$package_path$//;
  
  return $path;
}

sub create_make_rule_native {
  my $package_name = shift;
  
  create_package_make_rule($package_name, 'native');
}

sub create_make_rule_precompile {
  my $package_name = shift;
  
  create_package_make_rule($package_name, 'precompile');
}

sub create_package_make_rule {
  my ($package_name, $category) = @_;
  
  my $make_rule;
  
  # dynamic section
  $make_rule
  = "dynamic :: ";

  my $package_name_under_score = $package_name;
  $package_name_under_score =~ s/:/_/g;
  
  $make_rule
    .= "shared_lib_$package_name_under_score ";
  $make_rule .= "\n\n";
  
  my $module_base_name = $package_name;
  $module_base_name =~ s/^.+:://;
  
  my $src_dir = $package_name;
  $src_dir =~ s/::/\//g;
  $src_dir = "lib/$src_dir." . $category;
  
  # Dependency
  my @deps = grep { $_ ne '.' && $_ ne '..' } glob "$src_dir/*";
  
  # Shared library file
  my $shared_lib_rel_file = convert_package_name_to_shared_lib_rel_file($package_name, $category);
  my $shared_lib_file = "blib/lib/$shared_lib_rel_file";
  
  # Get source files
  my $module_category = $category;
  $module_category = ucfirst $module_category;
  $make_rule
    .= "shared_lib_$package_name_under_score :: $shared_lib_file\n\n";
  $make_rule
    .= "$shared_lib_file :: @deps\n\n";
  $make_rule
    .= "\tperl -Mblib -MSPVM::Build -e \"SPVM::Build->new(build_dir => 'spvm_build')->create_shared_lib_${category}_dist('$package_name')\"\n\n";
  
  return $make_rule;
}

sub convert_package_name_to_path {
  my ($package_name, $category) = @_;
  
  my $module_base_name = $package_name;
  $module_base_name =~ s/^.+:://;
  
  my $shared_lib_rel_dir = $package_name;
  $shared_lib_rel_dir =~ s/::/\//g;
  $shared_lib_rel_dir = "$shared_lib_rel_dir.$category";
  
  return $shared_lib_rel_dir;
}

sub convert_package_name_to_shared_lib_rel_file {
  my ($package_name, $category) = @_;
  
  my $dlext = $Config{dlext};
  
  my $module_base_name = $package_name;
  $module_base_name =~ s/^.+:://;
  
  my $package_path = convert_package_name_to_path($package_name, $category);
  my $shared_lib_rel_file = "$package_path/$module_base_name.$dlext";
  
  return $shared_lib_rel_file;
}

sub convert_package_name_to_shared_lib_dir {
  my ($lib_dir, $package_name, $category) = @_;
  
  # Shared library file
  my $shared_lib_rel_dir = convert_package_name_to_path($package_name, $category);
  my $shared_lib_dir = "$lib_dir/$shared_lib_rel_dir";
  
  return $shared_lib_dir;
}

sub default_extra_compiler_flags {
  my $default_extra_compiler_flags = '-std=c99';
  
  # I want to print warnings, but if gcc version is different, can't suppress no needed warning message.
  # so I dicide not to print warning in release version
  if ($ENV{SPVM_TEST_ENABLE_WARNINGS}) {
    $default_extra_compiler_flags .= " -Wall -Wextra -Wno-unused-label -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-variable";
  }
  
  return $default_extra_compiler_flags;
}

sub default_optimize {
  my $default_optimize = '-O3';
  
  return $default_optimize;
}

sub default_build_config {
  my $build_config = SPVM::Build::Config->new;
  
  $build_config->replace_extra_compiler_flags('-std=c99');

  $build_config->replace_extra_linker_flags('');

  # I want to print warnings, but if gcc version is different, can't suppress no needed warning message.
  # so I dicide not to print warning in release version
  if ($ENV{SPVM_TEST_ENABLE_WARNINGS}) {
    $build_config->add_extra_compiler_flags(" -Wall -Wextra -Wno-unused-label -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-variable");
  }

  $build_config->optimize('-O3');
}

1;

