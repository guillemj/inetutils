#!/usr/bin/perl

# Copyright (C) 2020, 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Inetutils.
#
# GNU Inetutils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# GNU Inetutils is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see `http://www.gnu.org/licenses/'.

use strict;
use warnings;

# Accepted format of entry line:
#
#     hex:hex::/num	ripe	# comments
#     hex:hex::/num	whois.arin.net
#
# where HEX are hexadecimal numbers of at most four significant digits,
# and NUM are decimal numbers in the range 1 to 32, inclusive.

my $prefix = qr/^([[:xdigit:]]{1,4}:[[:xdigit:]]{1,4})::\/(\d+)\s+([\w\.]+)$/;

while (<>) {
  # Sanitize input: strip comments after hash character,
  #   discard leading and ending space, drop empty lines.
  chomp;

  s/^\s*(.*)\s*$/$1/;
  s/\s*#.*$//;

  next if /^$/;

  # Check for acceptable data format.
  die "Prefix is not parseable: \"$_\".\n"
    unless (/$prefix/);

  my ($i1, $i2) = (0, 0);

  ($i1, $i2) = split /:/, $1;	# Prefix components, two double-octets.

  my $masklen = $2;	# Prefix length
  my $server = $3;	# Delegation target

  die "Illegal prefix length: $masklen used in \"$1/$masklen\".\n"
    if $masklen == 0 or $masklen > 32;

  print "{ " . ((oct("0x" . $i1) << 16) + oct("0x" . $i2)) . "UL, " .
    (0xffffffff^(0xffffffff >> $masklen)) . "UL, \"";

  if ($server =~ /\./) {
    print "$server";
  } else {
    print "whois.$server.net";
  }

  print "\" },\n";
}
