#!/usr/bin/perl

use strict;
#use warnings;

# Extract the arguments from the html form
# rack, pattern

my $rack;
my $patt;
my @words;

if (length($ENV{'QUERY_STRING'}) > 0) {
  my $buffer = $ENV{'QUERY_STRING'};
  my @pairs = split(/&/, $buffer);
  my %in;
  foreach my $pair (@pairs) {
    (my $name, my $value) = split(/=/, $pair);
    $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
    $in{$name} = $value; 
  }
  $rack = $in{'rack'};
  $patt = $in{'pattern'};

  # Get a list of matching words using program word, max of 500 results

  @words = `./word '$rack' '$patt' | head -500`;
}

# Now output everything as JSON

printf "Content-type: application/json\n\n";

printf "{\"words\":[";

my $sep=0;
foreach my $word (@words) {
  chomp($word);
  if ($sep) { printf ","; }
  printf "\"$word\"";
  $sep=1;
}

printf "]}\n";
