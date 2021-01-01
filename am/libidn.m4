# libidn.m4 serial 2
dnl Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021
dnl Free Software Foundation, Inc.
dnl
dnl This file is part of GNU Inetutils.
dnl
dnl GNU Inetutils is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or (at
dnl your option) any later version.
dnl
dnl GNU Inetutils is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see `http://www.gnu.org/licenses/'.

dnl Written by Mats Erik Andersson.

dnl IU_CHECK_LIBIDN([PREFIX],[HEADERLOC])
dnl Search for libidn in standard location and in PREFIX,
dnl if the latter is set and is neither "yes", nor "no".
dnl
dnl First check: $with_idn != no

AC_DEFUN([IU_CHECK_LIBIDN],
[
if test x"$with_idn" != xno \
    && test "$ac_cv_header_locale_h" = yes \
    && test "$ac_cv_func_setlocale" = yes
then
  if test -n "$1" \
      && test x"$1" != xno \
      && test x"$1" != xyes
  then
    INCIDN=-I$1/include
    LIBIDN=-L$1/lib
  fi

  if test -n "$2" \
      && test x"$2" != xyes
  then
    INCIDN=-I$2
  fi

  AC_CHECK_LIB([idn2], [idn2_to_ascii_lz], [have_IDN2=yes], , [$LIBIDN])

  AC_CHECK_LIB([idn], [idna_to_ascii_lz], [have_IDN=yes], , [$LIBIDN])

  if test x"$have_IDN2" = xyes
  then
    LIBIDN="$LIBIDN -lidn2"
  else
    if test x"$have_IDN" = xyes
    then
      LIBIDN="$LIBIDN -lidn"
    else
      INCIDN= LIBIDN=
    fi
  fi

  # Some systems are known to install <idna.h> below
  # '/usr/include/idn'.  The caching performed by
  # AC_CHECK_HEADERS prevents detection of this using
  # repeated call of the macro.  Functional alternative?
  save_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $INCIDN"

  AC_CHECK_HEADERS([idna.h idn2.h])

  AC_MSG_CHECKING([if GNU libidn is available])
  if test "$ac_cv_lib_idn2_idn2_to_ascii_lz" = yes \
      && test "$ac_cv_header_idn2_h" = yes; then
    AC_DEFINE(HAVE_IDN2, 1, [Define to 1 for use of GNU Libidn2.])
    AC_MSG_RESULT($ac_cv_lib_idn2_idn2_to_ascii_lz)
  else
    if test "$ac_cv_lib_idn_idna_to_ascii_lz" = yes \
	&& test "$ac_cv_header_idna_h" = yes; then
      AC_DEFINE(HAVE_IDN, 1, [Define to 1 for use of GNU Libidn.])
      AC_MSG_RESULT($ac_cv_lib_idn_idna_to_ascii_lz)
    else
      AC_MSG_RESULT([no])
      INCIDN= LIBIDN=
    fi
  fi
  CPPFLAGS=$save_CPPFLAGS
fi
AC_SUBST([LIBIDN])
AC_SUBST([INCIDN])
])# IU_CHECK_LIBIDN
