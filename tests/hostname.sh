#!/bin/sh

# Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
# 2020, 2021 Free Software Foundation, Inc.
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

# Prerequisites:
#
#  * Shell: SVR3 Bourne shell, or newer.
#
#  * id(1), mktemp(1), uname(1).

. ./tools.sh

hostname=${hostname:-../src/hostname$EXEEXT}

if [ $VERBOSE ]; then
    set -x
    $hostname --version
fi

errno=0

posttest () {
    test -n "$NAMEFILE" && test -r "$NAMEFILE" && rm "$NAMEFILE"
}

our_hostname=`$hostname` || errno=$?
test $errno -eq 0 || echo "Failed to get hostname." >&2
test $errno -eq 0 || exit $errno

sys_hostname=`hostname` || errno=$?
if test $errno -ne 0; then
    echo "System hostname failed (rc $errno out $sys_hostname)." >&2
    errno=0
    sys_hostname=`uname -n` || errno=$?
    test $errno -eq 0 || echo "Failed uname (rc $errno out $sys_hostname)." >&2
    test $errno -eq 0 || exit $errno
fi

if test "$our_hostname" != "$sys_hostname"; then
    echo "Hostname mismatch $our_hostname != $sys_hostname"
    exit 1
fi

if test `func_id_uid` != 0; then
    echo "hostname: skipping tests to set host name"
else
    $need_mktemp || exit_no_mktemp

    # Only run this if hostname succeeded...
    if test $errno -eq 0; then
	$hostname `$hostname` || errno=$?
	test $errno -eq 0 || echo "Failed to set hostname." >&2
	test $errno -eq 0 || exit $errno

	NAMEFILE=`$MKTEMP tmp.XXXXXXXXXX` || errno=$?
	test $errno -eq 0 || echo >&2 'Cannot create test file.'
	test $errno -eq 0 || exit $errno

	trap posttest 0 1 2 3 15

	SAVEDNAME=`$hostname` || errno=$?
	echo $SAVEDNAME > $NAMEFILE

	if test $errno -eq 0 && test -s $NAMEFILE; then
	    $hostname -F $NAMEFILE || errno=$?
	    test $errno -eq 0 || echo >&2 'Failed to set hostname.'

	    # Attempt to rescue name using the first method.
	    test $errno -eq 0 || $hostname "$SAVEDNAME" || errno=$?
	fi
    fi
fi

exit $errno
