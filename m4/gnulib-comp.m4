# Copyright (C) 2004 Free Software Foundation, Inc.
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# This file represents the compiled summary of the specification in
# gnulib-cache.m4. It lists the computed macro invocations that need
# to be invoked from configure.ac.
# In projects using CVS, this file can be treated like other built files.


# This macro should be invoked from ./configure.ac, in the section
# "Checks for programs", right after AC_PROG_CC, and certainly before
# any checks for libraries, header files, types and library functions.
AC_DEFUN([gl_EARLY],
[
  AC_REQUIRE([AC_GNU_SOURCE])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
])

# This macro should be invoked from ./configure.ac, in the section
# "Check for header files, types and library functions".
AC_DEFUN([gl_INIT],
[
AM_CONDITIONAL([GL_COND_LIBTOOL], [false])
  gl_FUNC_ALLOCA
  gl_ALLOCSA
  gl_ERROR
  gl_EXITFAIL
  dnl gl_USE_SYSTEM_EXTENSIONS must be added quite early to configure.ac.
  gl_FUNC_FREE
  gl_FUNC_GETCWD
  gl_FUNC_GETDELIM
  gl_FUNC_GETLINE
  gl_GETOPT
  gl_FUNC_GETPASS
  dnl you must add AM_GNU_GETTEXT([external]) or similar to configure.ac.
  gl_PREREQ_GETUSERSHELL
  AC_FUNC_MALLOC
  gl_MBCHAR
  gl_MBITER
  gl_FUNC_MEMCHR
  gl_FUNC_MEMCMP
  gl_FUNC_MEMCPY
  gl_FUNC_MEMMOVE
  gl_FUNC_MEMPCPY
  gl_FUNC_MEMSET
  gl_MINMAX
  gl_OBSTACK
  gl_FUNC_POLL
  gl_READUTMP
  AC_FUNC_REALLOC
  gl_REGEX
  gl_C_RESTRICT
  gt_FUNC_SETENV
  gl_SIZE_MAX
  gl_FUNC_SNPRINTF
  AM_STDBOOL_H
  gl_STDIO_SAFER
  gl_STRCASE
  gl_FUNC_STRCHRNUL
  gl_FUNC_STRDUP
  gl_FUNC_STRERROR
  gl_FUNC_STRNDUP
  gl_FUNC_STRNLEN
  gl_SYSEXITS
  gl_UNISTD_SAFER
  gl_FUNC_GLIBC_UNLOCKED_IO
  gl_FUNC_VASNPRINTF
  gl_FUNC_VSNPRINTF
  gl_XALLOC
  gl_XGETCWD
  gl_XSIZE
])

# This macro records the list of files which have been installed by
# gnulib-tool and may be removed by future gnulib-tool invocations.
AC_DEFUN([gl_FILE_LIST], [
  build-aux/config.rpath
  lib/alloca.c
  lib/alloca_.h
  lib/allocsa.c
  lib/allocsa.h
  lib/allocsa.valgrind
  lib/asnprintf.c
  lib/dup-safer.c
  lib/error.c
  lib/error.h
  lib/exit.h
  lib/exitfail.c
  lib/exitfail.h
  lib/fd-safer.c
  lib/fopen-safer.c
  lib/free.c
  lib/getcwd.c
  lib/getcwd.h
  lib/getdelim.c
  lib/getdelim.h
  lib/getline.c
  lib/getline.h
  lib/getopt.c
  lib/getopt1.c
  lib/getopt_.h
  lib/getopt_int.h
  lib/getpass.c
  lib/getpass.h
  lib/gettext.h
  lib/getusershell.c
  lib/malloc.c
  lib/mbchar.c
  lib/mbchar.h
  lib/mbuiter.h
  lib/memchr.c
  lib/memcmp.c
  lib/memcpy.c
  lib/memmove.c
  lib/mempcpy.c
  lib/mempcpy.h
  lib/memset.c
  lib/minmax.h
  lib/obstack.c
  lib/obstack.h
  lib/pipe-safer.c
  lib/poll.c
  lib/poll_.h
  lib/printf-args.c
  lib/printf-args.h
  lib/printf-parse.c
  lib/printf-parse.h
  lib/readutmp.c
  lib/readutmp.h
  lib/realloc.c
  lib/regcomp.c
  lib/regex.c
  lib/regex.h
  lib/regex_internal.c
  lib/regex_internal.h
  lib/regexec.c
  lib/setenv.c
  lib/setenv.h
  lib/size_max.h
  lib/snprintf.c
  lib/snprintf.h
  lib/stdbool_.h
  lib/stdio--.h
  lib/stdio-safer.h
  lib/strcase.h
  lib/strcasecmp.c
  lib/strchrnul.c
  lib/strchrnul.h
  lib/strdup.c
  lib/strdup.h
  lib/strerror.c
  lib/strncasecmp.c
  lib/strndup.c
  lib/strndup.h
  lib/strnlen.c
  lib/strnlen.h
  lib/strnlen1.c
  lib/strnlen1.h
  lib/sysexit_.h
  lib/unistd--.h
  lib/unistd-safer.h
  lib/unlocked-io.h
  lib/unsetenv.c
  lib/vasnprintf.c
  lib/vasnprintf.h
  lib/vsnprintf.c
  lib/vsnprintf.h
  lib/xalloc-die.c
  lib/xalloc.h
  lib/xgetcwd.c
  lib/xgetcwd.h
  lib/xmalloc.c
  lib/xsize.h
  m4/alloca.m4
  m4/allocsa.m4
  m4/codeset.m4
  m4/d-ino.m4
  m4/eealloc.m4
  m4/eoverflow.m4
  m4/error.m4
  m4/exitfail.m4
  m4/extensions.m4
  m4/free.m4
  m4/getcwd-path-max.m4
  m4/getcwd.m4
  m4/getdelim.m4
  m4/getline.m4
  m4/getopt.m4
  m4/getpass.m4
  m4/gettext.m4
  m4/getusershell.m4
  m4/glibc2.m4
  m4/glibc21.m4
  m4/iconv.m4
  m4/intdiv0.m4
  m4/intmax.m4
  m4/intmax_t.m4
  m4/inttypes-pri.m4
  m4/inttypes.m4
  m4/inttypes_h.m4
  m4/isc-posix.m4
  m4/lcmessage.m4
  m4/lib-ld.m4
  m4/lib-link.m4
  m4/lib-prefix.m4
  m4/longdouble.m4
  m4/longlong.m4
  m4/mbchar.m4
  m4/mbiter.m4
  m4/mbrtowc.m4
  m4/memchr.m4
  m4/memcmp.m4
  m4/memcpy.m4
  m4/memmove.m4
  m4/mempcpy.m4
  m4/memset.m4
  m4/minmax.m4
  m4/nls.m4
  m4/obstack.m4
  m4/onceonly_2_57.m4
  m4/po.m4
  m4/poll.m4
  m4/printf-posix.m4
  m4/progtest.m4
  m4/readutmp.m4
  m4/regex.m4
  m4/restrict.m4
  m4/setenv.m4
  m4/signed.m4
  m4/size_max.m4
  m4/snprintf.m4
  m4/stdbool.m4
  m4/stdint_h.m4
  m4/stdio-safer.m4
  m4/strcase.m4
  m4/strchrnul.m4
  m4/strdup.m4
  m4/strerror.m4
  m4/strerror_r.m4
  m4/strndup.m4
  m4/strnlen.m4
  m4/sysexits.m4
  m4/uintmax_t.m4
  m4/ulonglong.m4
  m4/unistd-safer.m4
  m4/unlocked-io.m4
  m4/vasnprintf.m4
  m4/vsnprintf.m4
  m4/wchar_t.m4
  m4/wint_t.m4
  m4/xalloc.m4
  m4/xgetcwd.m4
  m4/xsize.m4
])
