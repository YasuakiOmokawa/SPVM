package SPVM::Build::Base;

# SPVM::Build::PPtUtil is used from Makefile.PL
# so this module must be wrote as pure per script, not contain XS and don't use any other SPVM modules.

use strict;
use warnings;
use Carp 'croak', 'confess';

use SPVM::Build::Util;
use SPVM::Build::SPVMInfo;

use ExtUtils::CBuilder;
use Config;
use File::Copy 'move';
use File::Path 'mkpath';
use DynaLoader;

use File::Basename 'dirname', 'basename';

sub new {
  my $class = shift;
  
  my $self = {@_};
  
  $self->{extra_compiler_flags} ||= SPVM::Build::Util::default_extra_compiler_flags();
  $self->{optimize} ||= SPVM::Build::Util::default_optimize();
  
  return bless $self, $class;
}

sub category {
  my $self = shift;
  
  $self->{category};
}

sub extra_compiler_flags {
  my $self = shift;
  
  return $self->{extra_compiler_flags};
}

sub optimize {
  my $self = shift;
  
  return $self->{optimize};
}

sub build {
  my $self = shift;
  
  my $packages = SPVM::Build::SPVMInfo::get_packages($self->{compiler});
  for my $package (@$packages) {
    my $package_name = $package->{name};
    
    next if $package_name eq "SPVM::CORE";
    
    my $subs = $self->get_subs_from_package_name($package_name);
    if (@$subs) {
      # Shared library is already installed in distribution directory
      my $shared_lib_path = $self->get_installed_shared_lib_path($package_name);

      # Try runtime compile if shared library is not found
      unless (-f $shared_lib_path) {
        $self->create_shared_lib_runtime($package_name);

        # Build directory
        my $shared_lib_rel_file = SPVM::Build::Util::convert_package_name_to_shared_lib_rel_file($package_name, $self->category);
        my $build_dir = $self->{build_dir};
        my $output_dir = "$build_dir/lib";
        $shared_lib_path = "$output_dir/$shared_lib_rel_file";
      }
      $self->bind_subs($shared_lib_path, $subs);
    }
  }
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
  my ($self, $shared_lib_path, $subs) = @_;
  
  for my $sub (@$subs) {
    my $sub_abs_name = $sub->{name};
    next if $sub_abs_name =~ /^SPVM::CORE::/;

    my $cfunc_name = $self->create_cfunc_name($sub_abs_name);
    my $cfunc_address = SPVM::Build::Util::get_shared_lib_func_address($shared_lib_path, $cfunc_name);
    
    unless ($cfunc_address) {
      my $cfunc_name = $self->create_cfunc_name($sub_abs_name);
      $cfunc_name =~ s/:/_/g;
      confess "Can't find function address of $sub_abs_name(). Native function name must be $cfunc_name";
    }
    $self->bind_sub($sub_abs_name, $cfunc_address);
  }
}

sub create_shared_lib {
  my ($self, %opt) = @_;
  
  # Config file
  my $config_file = $opt{config_file};
  
  # Package name
  my $package_name = $opt{package_name};
  
  # Build directory
  my $work_dir = $opt{work_dir};
  unless (defined $work_dir && -d $work_dir) {
    confess "Work directory must be specified for " . $self->category . " build";
  }
  
  # Output directory
  my $output_dir = $opt{output_dir};
  unless (defined $output_dir && -d $output_dir) {
    confess "Output directory must be specified for " . $self->category . " build";
  }
  
  my $sub_names = $opt{sub_names};
  
  # Quiet output
  my $quiet = defined $opt{quiet} ? $opt{quiet} : 0;
 
  my $input_dir = $opt{input_dir};
  my $package_path = SPVM::Build::Util::convert_package_name_to_path($package_name, $self->category);
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
  
  # Config
  my $config;
  if (defined $config_file && -f $config_file) {
    $config = do $config_file
      or confess "Can't parser $config_file: $!$@";
  }
  
  my $build_setting = $self->{build_setting};
  
  # CBuilder settings
  my $include_dirs = [@{$build_setting->get_include_dirs}];
  my $extra_linker_flags = [@{$build_setting->get_extra_linker_flags}];
  
  # Default include path
  my $env_header_include_dir = $INC{"SPVM/Build/Base.pm"};
  $env_header_include_dir =~ s/\/Build\/Base\.pm$//;
  push @$include_dirs, $env_header_include_dir;
  push @$include_dirs, $input_src_dir;
  
  # CBuilder config
  my $cbuilder_config = {};
  

  # OPTIMIZE
  $cbuilder_config->{optimize} ||= $self->optimize;
  
  # Compile source files
  my $cbuilder = ExtUtils::CBuilder->new(quiet => $quiet, config => $cbuilder_config);
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
      include_dirs => $include_dirs,
      extra_compiler_flags => $self->extra_compiler_flags
    );
    push @$object_files, $object_file;
  }
  
  my $dlext = $Config{dlext};
  
  my $cfunc_names = [];
  for my $sub_name (@$sub_names) {
    my $cfunc_name = "${package_name}::$sub_name";
    $cfunc_name =~ s/:/_/g;
    push @$cfunc_names, $cfunc_name;
  }
  
  # This is dummy to suppress boot strap function
  # This is bad hack
  unless (@$cfunc_names) {
    push @$cfunc_names, '';
  }
  
  my $tmp_shared_lib_file = $cbuilder->link(
    objects => $object_files,
    package_name => $package_name,
    dl_func_list => $cfunc_names,
    extra_linker_flags => $extra_linker_flags
  );

  # Create shared lib blib directory
  my $shared_lib_dir = "$output_dir/$package_path";
  mkpath $shared_lib_dir;
  
  # shared lib blib file
  my $shared_lib_rel_file = SPVM::Build::Util::convert_package_name_to_shared_lib_rel_file($package_name, $self->category);
  my $shared_lib_file = "$output_dir/$shared_lib_rel_file";
  
  # Move shared library file to blib directory
  move($tmp_shared_lib_file, $shared_lib_file)
    or die "Can't move $tmp_shared_lib_file to $shared_lib_file";
  
  return $shared_lib_file;
}

sub get_installed_shared_lib_path {
  my ($self, $package_name) = @_;
  
  my @package_name_parts = split(/::/, $package_name);
  my $module_load_path = SPVM::Build::SPVMInfo::get_package_load_path($self->{compiler}, $package_name);
  
  my $shared_lib_path = SPVM::Build::Util::convert_module_path_to_shared_lib_path($module_load_path, $self->category);
  
  return $shared_lib_path;
}

1;
