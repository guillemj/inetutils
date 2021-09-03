#
# Copyright (C) 2009-2021 Free Software Foundation, Inc.
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

manual_title = GNU Networking Utilities

bootstrap-tools = gnulib,autoconf,automake,bison,m4,makeinfo,help2man,make,gzip,tar

old_NEWS_hash = 9e58290a292044bd6de634f52f6ed940

translation_project_ =

_makefile_at_at_check_exceptions = ' && !/PATHDEFS_MAKE/'

VC_LIST_ALWAYS_EXCLUDE_REGEX = ^doc/fdl-1.3.texi$$
update-copyright-env = \
  UPDATE_COPYRIGHT_FORCE=1 \
  UPDATE_COPYRIGHT_USE_INTERVALS=2 \
  UPDATE_COPYRIGHT_MAX_LINE_LENGTH=79

local-checks-to-skip = \
	sc_cast_of_x_alloc_return_value \
	sc_copyright_check \
	sc_program_name \
	sc_prohibit_always_true_header_tests \
	sc_prohibit_assert_without_use \
	sc_prohibit_doubled_word \
	sc_prohibit_error_without_use \
	sc_prohibit_have_config_h \
	sc_prohibit_magic_number_exit \
	sc_prohibit_strncpy \
	sc_prohibit_undesirable_word_seq \
	sc_prohibit_xalloc_without_use \
	sc_error_message_period \
	sc_error_message_uppercase \
	sc_indent \
	sc_m4_quote_check \
	sc_prohibit_atoi_atof \
	sc_prohibit_stat_st_blocks \
	sc_prohibit_strcmp \
	sc_unmarked_diagnostics \
	sc_bindtextdomain \
	sc_assignment_in_if \
	sc_prohibit_gnu_make_extensions

exclude_file_name_regexp--sc_prohibit_have_config_h = \
	^libinetutils/libinetutils.h$$

exclude_file_name_regexp--sc_obsolete_symbols = \
	^tests/identify.c$$

exclude_file_name_regexp--sc_trailing_blank = \
	^gl/top/README-release.diff$$

sc_unsigned_char:
	@prohibit=u''_char \
	halt='don'\''t use u''_char; instead use unsigned char'	\
	  $(_sc_search_regexp)

sc_unsigned_long:
	@prohibit=u''_long \
	halt='don'\''t use u''_long; instead use unsigned long'	\
	  $(_sc_search_regexp)

sc_unsigned_short:
	@prohibit=u''_short \
	halt='don'\''t use u''_char; instead use unsigned short' \
	  $(_sc_search_regexp)

sc_assignment_in_if:
	prohibit='\<if *\(.* = ' halt="don't use assignments in if statements"	\
	  $(_sc_search_regexp)
