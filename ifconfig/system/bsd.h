/*
  Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
  2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021
  Free Software Foundation, Inc.

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

#ifndef IFCONFIG_SYSTEM_BSD_H
# define IFCONFIG_SYSTEM_BSD_H
# include "../printif.h"
# include "../options.h"
# include <sys/sockio.h>

/* BSD variant chooser.  */
# if defined __DragonFly__
#  define IU_BSD_TYPE "dragonfly"
# elif defined __FreeBSD__
#  define IU_BSD_TYPE "freebsd"
# elif defined __FreeBSD_kernel__
#  define IU_BSD_TYPE "GNU/kFreeBSD"
# elif defined __NetBSD__
#  define IU_BSD_TYPE "netbsd"
# elif defined __OpenBSD__
#  define IU_BSD_TYPE "openbsd"
#  define ETHERNAME "lladdr"
# else /* Exotic BSD versions.  */
#  define IU_BSD_TYPE "unknownbsd"
# endif

# ifndef ETHERNAME
#  define ETHERNAME "ether"
# endif


/* Option support.  */

# undef SYSTEM_SHORT_OPTIONS
# undef SYSTEM_LONG_OPTIONS

struct system_ifconfig
{
  int valid;
};


/* Output format support.  */

# define _IU_CAT2(a, b) a ## b
# define _IU_DCL(name, fld)				\
  {#name, _IU_CAT2(system_fh_, fld) }
# define _IU_EXTRN(fld)				\
  extern void _IU_CAT2(system_fh_, fld) (format_data_t, int, char *[]);	\

# define SYSTEM_FORMAT_HANDLER	\
  {"bsd", fh_nothing},		\
  {IU_BSD_TYPE, fh_nothing},	\
  {"brdaddr?", system_fh_brdaddr_query}, \
  {"brdaddr", system_fh_brdaddr}, \
  {"hwaddr?", system_fh_hwaddr_query}, \
  {"hwaddr", system_fh_hwaddr}, \
  {"hwtype?", system_fh_hwtype_query}, \
  {"hwtype", system_fh_hwtype},	\
  {"media?", system_fh_media_query}, \
  {"media", system_fh_media},	\
  {"netmask?", system_fh_netmask_query}, \
  {"netmask", system_fh_netmask},	\
  {"status?", system_fh_status_query}, \
  {"status", system_fh_status}, \
  {"tunnel?", system_fh_tunnel_query}, \
  {"tundst", system_fh_tundst}, \
  {"tunsrc", system_fh_tunsrc}, \
  {"ifstat?", system_fh_ifstat_query}, \
  _IU_DCL (collisions, collisions), \
  _IU_DCL (rxbytes, rx_bytes), \
  _IU_DCL (rxdropped, rx_dropped), \
  _IU_DCL (rxerrors, rx_errors), \
  {"rxfifoerr", system_fh_missing_stat}, \
  {"rxframeerr", system_fh_missing_stat}, \
  _IU_DCL (rxpackets, rx_packets), \
  _IU_DCL (txbytes, tx_bytes), \
  {"txcarrier", system_fh_missing_stat}, \
  _IU_DCL (txdropped, tx_dropped), \
  _IU_DCL (txerrors, tx_errors), \
  {"txfifoerr", system_fh_missing_stat}, \
  _IU_DCL (txpackets, tx_packets),

void system_fh_brdaddr_query (format_data_t form, int argc, char *argv[]);
void system_fh_brdaddr (format_data_t form, int argc, char *argv[]);
void system_fh_hwaddr_query (format_data_t form, int argc, char *argv[]);
void system_fh_hwaddr (format_data_t form, int argc, char *argv[]);
void system_fh_hwtype_query (format_data_t form, int argc, char *argv[]);
void system_fh_hwtype (format_data_t form, int argc, char *argv[]);
void system_fh_media_query (format_data_t form, int argc, char *argv[]);
void system_fh_media (format_data_t form, int argc, char *argv[]);
void system_fh_netmask_query (format_data_t form, int argc, char *argv[]);
void system_fh_netmask (format_data_t form, int argc, char *argv[]);
void system_fh_status_query (format_data_t form, int argc, char *argv[]);
void system_fh_status (format_data_t form, int argc, char *argv[]);
void system_fh_tunnel_query (format_data_t form, int argc, char *argv[]);
void system_fh_tundst (format_data_t form, int argc, char *argv[]);
void system_fh_tunsrc (format_data_t form, int argc, char *argv[]);

void system_fh_ifstat_query (format_data_t form, int argc, char *argv[]);
void system_fh_missing_stat (format_data_t form, int argc, char *argv[]);

_IU_EXTRN (rx_bytes)
_IU_EXTRN (tx_bytes)
_IU_EXTRN (rx_dropped)
_IU_EXTRN (tx_dropped)
_IU_EXTRN (rx_errors)
_IU_EXTRN (tx_errors)
_IU_EXTRN (rx_packets)
_IU_EXTRN (tx_packets)
_IU_EXTRN (collisions)

#endif /* IFCONFIG_SYSTEM_BSD_H */
