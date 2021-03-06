# File : Makefile.PL
use ExtUtils::MakeMaker;

open(STREAM, '../../mapscriptvars') or die('Unable to open mapscriptvars, have you built the MapServer yet?'); 

$home = <STREAM>;
chomp $home;

$define = <STREAM>;
chomp $define;

$inc = <STREAM>;
chomp $inc;
$inc =~ s/\-I\./\-I$home/g;

$libs = <STREAM>;
chomp $libs;
$libs =~ s/\-L\./\-L$home/g;

$static_libs = <STREAM>;
chomp $static_libs;
$static_libs =~ s/\-L\./\-L$home/g;

# Get MapServer version from mapscriptvars
$ms_version_line = <STREAM>;
if ($ms_version_line) {
	$ms_version_line =~ /MS_VERSION "(.+)"/;
	$ms_version = $1;
}
# Default is 4.3
else { $ms_version = '4.3'; }

print $inc."\n";
print $libs."\n";
print $static_libs."\n";
print $ms_version."\n";

my $swigInterfaceFile = "../mapscript.i";
my $swigWrapperFile   = "mapscript_wrap.c";

########################################################################

sub runSwigAutogeneration{
  print "Autogenerating wrappers from SWIG interface " . $swigInterfaceFile . "\n";
  my $command = "swig -perl5 -shadow -outdir . $define -o $swigWrapperFile $swigInterfaceFile";
  system($command);
  if ($?){
      print "An error occurred when generating the wrappers from the SWIG interface :\n$!\n";
      print "The command that was run was:\n\n$command\n\n";
      print "Perl mascript cannot be built.\n";
      exit;
  }
}

########################################################################

sub existsAndUpToDate{
  my ($sourceFile, $destFile) = @_;
  if (!(-e $destFile)) { return 0; }
  my $sourceMtime = (stat $sourceFile)[9];
  my $destMtime = (stat $destFile)[9];
  return $destMtime >= $sourceMtime;
}

########################################################################

runSwigAutogeneration() unless
  existsAndUpToDate($swigInterfaceFile, $swigWrapperFile);

WriteMakefile(
	      'NAME' 		=> 'mapscript',
	      'DEFINE' 		=> $define,
	      'INC' 		=> $inc,
	      'LIBS' 		=> [$libs,$static_libs],
	      'OBJECT' 		=> 'mapscript_wrap.o',
		  'VERSION' 	=> $ms_version
	      );

