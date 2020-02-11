#!/usr/bin/perl -w

use strict; #use warning, and strict
use File::Copy;
use FileHandle;
use IO::Handle;
use Cwd;
use POSIX "sys_wait_h";
use Errno qw(EAGAIN);
use Getopt::Long;

my $dir_root; #dir to start in
my $zip_root; #dir for zip files
my $dry = 0;

sub zap
{
	my $signame = shift;
        die "Somebody sent me a SIG$signame";
}
sub goforit
{
	my $args = shift;
	my $rc = 0xffff & system $args;
	printf LOG "system (%s) returned %#04x: ", "$args", $rc;
	if ($rc == 0){
		print LOG "ran with normal exit\n";
	}
	elsif($rc == 0xff00){
		print LOG "command failed: $!\n";
	}
	elsif(($rc & 0xff) == 0){
		$rc >>= 8;
		print LOG "ran with non-zero exit status $rc\n";
	}
	else{
		print LOG "ran with ";
		if($rc & 0x80) {
			$rc &= ~0x80;
			print LOG "coredump from ";
		}
		die "signal $rc ...exiting...\n";
	}
}
	my $num = @ARGV;
	print "running ($num) cme_sub.pl @ARGV\n";
	my $zip_file = 0;
	my $log_file = cwd()."/cme_sub_$$.log";
	my $ret = GetOptions("d:i"=>\$dry,
					"r=s"=>\$dir_root,
                                        "t=s"=>\$zip_root,
					"z:s"=>\$zip_file,
					"l:s"=>\$log_file);


   	print "cme_sub.pl -r $dir_root -t $zip_root -z $zip_file -l $log_file -d $dry\n";

	if($ret == 0 || !$dir_root || !$zip_root) {die "Usage: cme_sub.pl -r {dir_root} -t (zip_root) [-l{log_file} |-d ]\n";}
  	unless( -d $dir_root)	{die("root directory: ${dir_root} does not exist!, stopped\n");}

 	open (LOG, ">>$log_file");
	LOG->autoflush(1);

	$SIG{INT} = \&zap;

  	if($dry == 1){print LOG ("dry run, printout only\n");	}

	if(!$dry)
      	{
		if($zip_file)
		{
			print LOG "starting... nohup 7za e -o$dir_root -y $zip_file\n";
			goforit("7z e -o$dir_root -y $zip_file");
			print LOG "done... nohup 7z e -o$dir_root -y $zip_file\n";
		}
		print LOG  "starting... nohup /usr/data/bin/opra/opra_feed -config $dir_root/opra_feed.cfg\n" ;
		#goforit("/usr/data/bin/opra/opra_feed -config $dir_root/opra_feed.cfg") ;
		print LOG "done... /usr/data/bin/opra/opra_feed -config $dir_root/opra_feed.cfg to finish\n";
	}
	else
	{
		#print LOG "run 7za e -o$dir_root -y $zip_file to finish\n";
		print LOG "7z e -o$zip_root -y $dir_root/$zip_file);\n";
	}
	unlink <$dir_root/*.dat>;
	print LOG "rm <$dir_root/*.dat>\n";
	close LOG;