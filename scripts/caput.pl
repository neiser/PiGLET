#!/usr/bin/perl
use warnings;
use strict;

use lib '/opt/epics/base/lib/perl';
use CA;
use Time::HiRes qw(usleep nanosleep);

my $pv = $ARGV[0] || '';

my $chan = CA->new($pv);
CA->pend_io(1);
unless(defined $chan && $chan->is_connected) {
  die "Cannot connect to PV $pv";
}

my $n = 0;
my $flag = 0;
while(1) {
  $flag = !$flag if $n % 1000 == 0;
  my $val = 0.2*sin(2*3.14*$n/100)-0.5+$flag+0.1*rand();
  printf("Putting %f\n", $val);
  $chan->put($val);
  CA->pend_io(1);
  $n++;
  usleep(10000);
}
