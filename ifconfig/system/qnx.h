/*
  Copyright (C) 2001-2021 Free Software Foundation, Inc.

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

/* Written by Marcus Brinkmann.  */

#ifndef IFCONFIG_SYSTEM_QNX_H
# define IFCONFIG_SYSTEM_QNX_H

# include "../printif.h"
# include "../options.h"
# include <sys/sockio.h>



/* Option support.  */

struct system_ifconfig
{
  int valid;
};


/* Output format support.  */

# define SYSTEM_FORMAT_HANDLER \
  {"qnx", fh_nothing},

#endif
