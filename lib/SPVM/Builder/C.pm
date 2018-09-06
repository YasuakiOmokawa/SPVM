package SPVM::Builder::C;

use strict;
use warnings;
use Carp 'croak', 'confess';

use SPVM::Builder::Util;

use ExtUtils::CBuilder;
use File::Copy 'move';
use File::Path 'mkpath';
use DynaLoader;

use File::Basename 'dirname', 'basename';

sub new {
  my $class = shift;
  
  my $self = {@_};
  
  return bless $self, $class;
}

sub info {
  my $self = shift;
  
  return $self->{info};
}

sub category {
  my $self = shift;
  
  $self->{category};
}

sub build {
  my ($self, $opt) = @_;
  
  my $package_names = $self->info->get_package_names;
  for my $package_name (@$package_names) {
    
    my $category = $self->{category};
    my $sub_names;
    if ($category eq 'native') {
      $sub_names = $self->info->get_native_sub_names($package_name)
    }
    elsif ($category eq 'precompile') {
      $sub_names = $self->info->get_precompile_sub_names($package_name)
    }
    
    if (@$sub_names) {
      # Shared library is already installed in distribution directory
      my $shared_lib_path = $self->get_shared_lib_path_dist($package_name);

      # Try runtime compile if shared library is not found
      unless (-f $shared_lib_path) {
        $self->build_shared_lib_runtime($package_name, $sub_names);

        $shared_lib_path = $self->get_shared_lib_path_runtime($package_name);
      }
      $self->bind_subs($shared_lib_path, $package_name, $sub_names);
    }
  }
}

sub get_shared_lib_path_runtime {
  my ($self, $package_name) = @_;
  
  my $shared_lib_rel_file = SPVM::Builder::Util::convert_package_name_to_shared_lib_rel_file($package_name, $self->category);
  my $build_dir = $self->{build_dir};
  my $output_dir = "$build_dir/lib";
  my $shared_lib_path = "$output_dir/$shared_lib_rel_file";
  
  return $shared_lib_path;
}

sub create_cfunc_name {
  my ($self, $sub_abs_name) = @_;
  
  my $category = $self->category;
  my $prefix = 'SPVM_' . uc($category) . '_';
  
  # Precompile Subroutine names
  my $sub_abs_name_under_score = $sub_abs_name;
  $sub_abs_name_under_score =~ s/:/_/g;
  my $cfunc_name = "$prefix$sub_abs_name_under_score";
  
  return $cfunc_name;
}

sub bind_subs {
  my ($self, $shared_lib_path, $package_name, $sub_names) = @_;
  
  for my $sub_name (@$sub_names) {
    my $sub_abs_name = "${package_name}::$sub_name";

    my $cfunc_name = $self->create_cfunc_name($sub_abs_name);
    my $cfunc_address = SPVM::Builder::Util::get_shared_lib_func_address($shared_lib_path, $cfunc_name);
    
    unless ($cfunc_address) {
      my $cfunc_name = $self->create_cfunc_name($sub_abs_name);
      $cfunc_name =~ s/:/_/g;
      confess "Can't find function address of $sub_abs_name(). C function name must be $cfunc_name";
    }
    $self->bind_sub($sub_abs_name, $cfunc_address);
  }
}

sub build_shared_lib {
  my ($self, $package_name, $sub_names, $opt) = @_;
  
  # Compile source file and create object files
  my $object_files = $self->compile_objects($package_name, $sub_names, $opt);
  
  # Link object files and create shared library
  $self->link_shared_lib(
    $package_name,
    $sub_names,
    $object_files,
    $opt
  );
}

sub compile_objects {
  my ($self, $package_name, $sub_names, $opt) = @_;

  # Build directory
  my $work_dir = $opt->{work_dir};
  unless (defined $work_dir && -d $work_dir) {
    confess "Work directory must be specified for " . $self->category . " build";
  }
  
  # Output directory
  my $output_dir = $opt->{output_dir};
  unless (defined $output_dir && -d $output_dir) {
    confess "Output directory must be specified for " . $self->category . " build";
  }

  # shared lib file
  my $shared_lib_rel_file = SPVM::Builder::Util::convert_package_name_to_shared_lib_rel_file($package_name, $self->category);
  my $shared_lib_file = "$output_dir/$shared_lib_rel_file";

  # Return if source code is chaced and exists shared lib file
  if ($opt->{is_cached} && -f $shared_lib_file) {
    return;
  }
  
  # Quiet output
  my $quiet = defined $opt->{quiet} ? $opt->{quiet} : 0;
 
  my $input_dir = $opt->{input_dir};
  my $package_path = SPVM::Builder::Util::convert_package_name_to_path($package_name, $self->category);
  my $input_src_dir = "$input_dir/$package_path";
  
  my $work_object_dir = "$work_dir/$package_path";
  mkpath $work_object_dir;
  
  # Correct source files
  my $src_files = [];
  my @valid_exts = ('c', 'C', 'cpp', 'i', 's', 'cxx', 'cc');
  for my $src_file (glob "$input_src_dir/*") {
    if (grep { $src_file =~ /\.$_$/ } @valid_exts) {
      push @$src_files, $src_file;
    }
  }
  
  # Config file
  my $package_base_name = $package_name;
  $package_base_name =~ s/^.+:://;
  my $input_config_dir = $input_src_dir;
  my $config_file = "$input_config_dir/$package_base_name.config";
  
  # Config
  my $build_config;
  if (-f $config_file) {
    $build_config = do $config_file
      or confess "Can't parser $config_file: $!$@";
  }
  else {
    $build_config = SPVM::Builder::Util::new_default_build_config;
  }
  
  # CBuilder configs
  my $ccflags = $build_config->get_ccflags;
  
  # Default include path
  $build_config->add_ccflags("-I$input_src_dir");

  # Use all of default %Config not to use %Config directory by ExtUtils::CBuilder
  # and overwrite user configs
  my $config = $build_config->to_hash;
  
  # Compile source files
  my $cbuilder = ExtUtils::CBuilder->new(quiet => $quiet, config => $config);
  my $object_files = [];
  for my $src_file (@$src_files) {
    # Object file
    my $object_file = "$work_object_dir/" . basename($src_file);
    $object_file =~ s/\.c$//;
    $object_file =~ s/\.C$//;
    $object_file =~ s/\.cpp$//;
    $object_file =~ s/\.i$//;
    $object_file =~ s/\.s$//;
    $object_file =~ s/\.cxx$//;
    $object_file =~ s/\.cc$//;
    $object_file .= '.o';
    
    # Compile source file
    $cbuilder->compile(
      source => $src_file,
      object_file => $object_file,
    );
    push @$object_files, $object_file;
  }
  
  return $object_files;
}

sub link_shared_lib {
  my ($self, $package_name, $sub_names, $object_files, $opt) = @_;

  # Build directory
  my $work_dir = $opt->{work_dir};
  unless (defined $work_dir && -d $work_dir) {
    confess "Work directory must be specified for " . $self->category . " build";
  }
  
  # Output directory
  my $output_dir = $opt->{output_dir};
  unless (defined $output_dir && -d $output_dir) {
    confess "Output directory must be specified for " . $self->category . " build";
  }

  # shared lib file
  my $shared_lib_rel_file = SPVM::Builder::Util::convert_package_name_to_shared_lib_rel_file($package_name, $self->category);
  my $shared_lib_file = "$output_dir/$shared_lib_rel_file";

  # Return if source code is chaced and exists shared lib file
  if ($opt->{is_cached} && -f $shared_lib_file) {
    return;
  }
  
  # Quiet output
  my $quiet = defined $opt->{quiet} ? $opt->{quiet} : 0;
 
  my $input_dir = $opt->{input_dir};
  my $package_path = SPVM::Builder::Util::convert_package_name_to_path($package_name, $self->category);
  my $input_src_dir = "$input_dir/$package_path";
  
  my $work_object_dir = "$work_dir/$package_path";
  mkpath $work_object_dir;
  
  # Config file
  my $package_base_name = $package_name;
  $package_base_name =~ s/^.+:://;
  my $input_config_dir = $input_src_dir;
  my $config_file = "$input_config_dir/$package_base_name.config";
  
  # Config
  my $build_config;
  if (-f $config_file) {
    $build_config = do $config_file
      or confess "Can't parser $config_file: $!$@";
  }
  else {
    $build_config = SPVM::Builder::Util::new_default_build_config;
  }
  
  # CBuilder configs
  my $lddlflags = $build_config->get_lddlflags;

  # Use all of default %Config not to use %Config directory by ExtUtils::CBuilder
  # and overwrite user configs
  my $config = $build_config->to_hash;
  
  my $cfunc_names = [];
  for my $sub_name (@$sub_names) {
    my $category = $self->category;
    my $category_uc = uc $category;
    my $cfunc_name = "SPVM_${category_uc}_${package_name}::$sub_name";
    $cfunc_name =~ s/:/_/g;
    push @$cfunc_names, $cfunc_name;
  }
  
  # This is dummy to suppress boot strap function
  # This is bad hack
  unless (@$cfunc_names) {
    push @$cfunc_names, '';
  }
  
  my $cbuilder = ExtUtils::CBuilder->new(quiet => $quiet, config => $config);
  my $tmp_shared_lib_file = $cbuilder->link(
    objects => $object_files,
    package_name => $package_name,
    dl_func_list => $cfunc_names,
  );
  

  # Create shared lib blib directory
  my $shared_lib_dir = "$output_dir/$package_path";
  mkpath $shared_lib_dir;
  
  # Move shared library file to blib directory
  move($tmp_shared_lib_file, $shared_lib_file)
    or die "Can't move $tmp_shared_lib_file to $shared_lib_file";
  
  return $shared_lib_file;
}

sub get_shared_lib_path_dist {
  my ($self, $package_name) = @_;
  
  my @package_name_parts = split(/::/, $package_name);
  my $module_load_path = $self->info->get_package_load_path($package_name);
  
  my $shared_lib_path = SPVM::Builder::Util::convert_module_path_to_shared_lib_path($module_load_path, $self->category);
  
  return $shared_lib_path;
}

1;