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
my $dry = 0;
my $csv_file;
my $pid;
my $max_pid = 2;
my @running;
my @estuff;

sub zap
{
	my $signame = shift;
	foreach $pid (@running)
	{
		print LOG "killing child $pid\n";
		kill 'INT', $pid;
	}
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
sub runit
{
	@estuff = ($_[0], $_[1]);
    	print LOG "Running cme_converter @estuff \n";

	FORK:
	{
  		if ($pid=fork)
		{
			return $pid;
 		}
  		elsif (defined $pid)
		{
    			exec "cme_converter",@estuff;
		    	# shouldn't reach this unless exec fails
    			print LOG "Couldn't exec\n";
    			exit 0;
    			# exit, NOT return.  We're the child process
  		}
  		elsif ($! == EAGAIN)
		{
     			sleep 3;
     			redo FORK;
  		}
  		else
		{
    			print LOG "Can't fork$!\n";
    			return 0;
  		}
 	}
}
sub dir_read
{
	#parse directory for directories and files

	my @file_list;
  	my $dir_prefix = $_[0];
#       (my $basename = $_[0]) =~ s!^.*/!!;
        my $counter = 1;

  	opendir(aDIR, $dir_prefix);
  	while($_ = readdir(aDIR))
  	{
        #filter any files you don't want
	    if(-d "$dir_prefix/$_" && $_ !~ /^\.\.?+$/ && $_ =~ /201/ )
            {
            	push(@file_list, "$dir_prefix/$_");
            }
        }
        closedir (aDIR);

        my $list_size = @file_list;
        foreach $_ (@file_list)
    	{
		print LOG "starting... ($counter of $list_size) cme_converter -d $_ \n";
                if($dry == 0)
                {
       			my $pid = &runit("-d",  $_);
			$counter++;
			push @running, $pid;

                  	if(@running >= $max_pid)
                  	{
                        	my $kid = -1;
                        	my @tmp;

                        	print LOG "waiting for one of @running to finish\n";

                        	do
                          	{
                                	while (@tmp) { pop(@tmp); }
                                  	foreach $pid (@running)
                                  	{
                                          	$kid = waitpid($pid, WNOHANG);
                                          	sleep 5;
                                          	if ($kid != -1)
                                          	{
                                                  	push(@tmp, $pid);
                                          	}
                                  	}
                          	} until (@tmp < $max_pid );
                          	@running = @tmp;
                          	print LOG "@running active processes \n";
                  	}
        	}
	}
}

	my $log_file = cwd()."/cme_script.log";
	my $ret = GetOptions("d"=>\$dry,
					"r=s"=>\$dir_root,
					"p:i"=>\$max_pid,
					"l:s"=>\$log_file);

	if($ret == 0 || !$dir_root) 		{die "Usage: cme_d.pl -r {root} [ -p{max pid} | -l{log_file} |-d ]\n";}
  	unless( -d $dir_root)	{die("root directory: ${dir_root} does not exist!, stopped\n");}

	open (LOG, ">>$log_file");
	LOG->autoflush(1);
 	print "cme_d.pl -r $dir_root -p $max_pid -l $log_file -d $dry\n";
	$SIG{INT} = \&zap;

 	if($dry == 1)
	{
		print LOG "dry run, printout only\n";
	}

 	&dir_read($dir_root);

	my $kid = -1;
	do{
		$kid = waitpid(-1, WNOHANG);
	} until $kid == -1;

  	print LOG "done.\n";
	close LOG;