/*
  Copyright (C) 2015, 2016, 2017, 2018, 2019, 2020, 2021 Free Software
  Foundation, Inc.

  This file is part of GNU Inetutils.

  GNU Inetutils is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  GNU Inetutils is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see `http://www.gnu.org/licenses/'. */

/* Written by Mats Erik Andersson.  */

#ifndef IFCONFIG_SYSTEM_HURD_H
# define IFCONFIG_SYSTEM_HURD_H

# include "../printif.h"
# include "../options.h"


/* Option support.  */

struct system_ifconfig
{
  int valid;
};


/* Output format support.  */

# define SYSTEM_FORMAT_HANDLER \
  { "hurd", fh_nothing}, \
  { "hwaddr?", system_fh_hwaddr_query}, \
  { "hwaddr", system_fh_hwaddr}, \
  { "hwtype?", system_fh_hwtype_query}, \
  { "hwtype", system_fh_hwtype},

void system_fh_hwaddr_query (format_data_t form, int argc, char *argv[]);
void system_fh_hwaddr (format_data_t form, int argc, char *argv[]);
void system_fh_hwtype_query (format_data_t form, int argc, char *argv[]);
void system_fh_hwtype (format_data_t form, int argc, char *argv[]);

#endif /* !IFCONFIG_SYSTEM_HURD_H */
