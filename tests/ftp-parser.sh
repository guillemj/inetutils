#!/bin/sh

# Copyright (C) 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021 Free
# Software Foundation, Inc.
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

# Exercise the internal command interpreter of `ftp',
# without any networking.  Intended for code coverage
# testing.

set -u

: ${EXEEXT:=}

. ./tools.sh

silence=
bucket=

# Executable under test.
#
FTP=${FTP:-../ftp/ftp$EXEEXT}

if test ! -x "$FTP"; then
    echo >&2 "Missing executable '$FTP'.  Skipping test."
    exit 77
fi

if test -z "${VERBOSE+set}"; then
    silence=:
    bucket='>/dev/null'
fi

if test -n "${VERBOSE:-}"; then
    set -x
    $FTP --version | $SED '1q'
fi

errno=0

# Check that a client is not connected.
#
reply=`echo 'cd /tmp' | $FTP | $GREP -cv 'Not connected\.'`
test $reply -eq 0 || { errno=1
  echo >&2 'Failed respond to missing connection.'; }

# Check that help is plentiful.
#
reply=`echo help | $FTP | $SED -n '$='`
test $reply -ge 18 || { errno=1
  echo >&2 'Unexpectedly short help listing.'; }

# Check change from passive mode to active mode.
#
reply=`echo passive | $FTP -p | $GREP -cv 'Passive mode off\.'`
test $reply -eq 0 || { errno=1
  echo >&2 'Failed while switching back to active mode.'; }

# Check address mode.
reply=`echo ipv4 | $FTP --ipv6 | $GREP -cv 'Selecting addresses: IPv4'`
test $reply -eq 0 || { errno=1
  echo >&2 'Failed to reset address family by command.'; }

# Step size for hash markers.
#
tell='hash 7M
hash 12k'
reply=`echo "$tell" | $FTP -v | $EGREP -c '12288|7340032'`

test $reply -eq 2 || { errno=1
  echo >&2 'Failed to parse step sizes for hash printing.'; }

# A set of mixed commands for the sake of code coverage.
#
tell='bell
case
hash
nmap
runique
sunique
epsv4
lcd /tmp
lpwd'
reply=`echo "$tell" | $FTP`

test `echo "$reply" | $SED -n '$='` \
     -eq `echo "$tell" | $SED -n '$='` \
|| { errno=1; echo >&2 'Some command in mixed list produced no response.'; }

# At least Darwin has been known to prepend a directory stem.
DIR_STEM=`echo "$reply" | $SED -n 's,Local directory now \([^ ]*\)/tmp$,\1,p'`

test -z "$DIR_STEM" \
|| $silence echo "This system prepends a directory stem: $DIR_STEM"

test `echo "$reply" | $GREP -c "Local directory is $DIR_STEM/tmp"` -eq 1 \
|| { errno=1; echo >&2 'Failed to set local directory.'; }

# Summary of work.
#
test $errno -ne 0 || $silence echo "Successful testing".

exit $errno
