/* bsd.c -- BSD specific code for ifconfig
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

#include <config.h>

#include <unistd.h>
#include "../ifconfig.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_types.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#ifdef HAVE_NETINET_ETHER_H
# include <netinet/ether.h>
#endif
#include <netinet/if_ether.h>
#include <ifaddrs.h>

#include <unused-parameter.h>


/* Output format stuff.  */

const char *system_default_format = "unix";


/* Argument parsing stuff.  */

const char *system_help = "\
NAME [ADDR [DSTADDR]] [broadcast BRDADDR] [netmask MASK] "
"[metric N] [mtu N] [up|down]";

struct argp_child system_argp_child;

int
system_parse_opt (struct ifconfig **ifp _GL_UNUSED_PARAMETER,
		  char option _GL_UNUSED_PARAMETER,
		  char *optarg _GL_UNUSED_PARAMETER)
{
  return 0;
}

int
system_parse_opt_rest (struct ifconfig **ifp, int argc, char *argv[])
{
  int i = 0, mask, rev;
  enum
  {
    EXPECT_NOTHING,
    EXPECT_COMMAND,
    EXPECT_AF,
    EXPECT_BROADCAST,
    EXPECT_NETMASK,
    EXPECT_METRIC,
    EXPECT_MTU
  } expect = EXPECT_COMMAND;

  *ifp = parse_opt_new_ifs (argv[0]);

  while (++i < argc)
    {
      switch (expect)
	{
	case EXPECT_BROADCAST:
	  parse_opt_set_brdaddr (*ifp, argv[i]);
	  break;

	case EXPECT_NETMASK:
	  parse_opt_set_netmask (*ifp, argv[i]);
	  break;

	case EXPECT_MTU:
	  parse_opt_set_mtu (*ifp, argv[i]);
	  break;

	case EXPECT_METRIC:
	  parse_opt_set_metric (*ifp, argv[i]);
	  break;

	case EXPECT_COMMAND:
	  expect = EXPECT_AF;		/* Applicable at creation.  */
	  if (!strcmp (argv[i], "create"))
	    {
	      error (0, 0, "interface creation is not supported");
	      return 0;
	    }
	  else if (!strcmp (argv[i], "destroy"))
	    {
	      error (0, 0, "interface destruction is not supported");
	      return 0;
	    }
	  break;

	case EXPECT_AF:
	case EXPECT_NOTHING:
	  break;
	}

      if (expect == EXPECT_AF)	/* Address selection is single shot.  */
	{
	  expect = EXPECT_NOTHING;
	  if (!strcmp (argv[i], "inet"))
	    continue;
	  else if (!strcmp (argv[i], "inet6"))
	    {
	      error (0, 0, "%s is not a supported address family", argv[i]);
	      return 0;
	    }
	}

      if (expect != EXPECT_NOTHING)
	expect = EXPECT_NOTHING;
      else if (!strcmp (argv[i], "broadcast"))
	expect = EXPECT_BROADCAST;
      else if (!strcmp (argv[i], "netmask"))
	expect = EXPECT_NETMASK;
      else if (!strcmp (argv[i], "metric"))
	expect = EXPECT_METRIC;
      else if (!strcmp (argv[i], "mtu"))
	expect = EXPECT_MTU;
      else if (!strcmp (argv[i], "up"))
	parse_opt_set_flag (*ifp, IFF_UP | IFF_RUNNING, 0);
      else if (!strcmp (argv[i], "down"))
	parse_opt_set_flag (*ifp, IFF_UP, 1);
      else if (((mask = if_nameztoflag (argv[i], &rev))
		& ~IU_IFF_CANTCHANGE) != 0)
	parse_opt_set_flag (*ifp, mask, rev);
      else
	{
	  /* Also alias, -alias.  */
	  if (!((*ifp)->valid & IF_VALID_ADDR))
	    parse_opt_set_address (*ifp, argv[i]);
	  else if (!((*ifp)->valid & IF_VALID_DSTADDR))
	    parse_opt_set_dstaddr (*ifp, argv[i]);
	}
    }

  switch (expect)
    {
    case EXPECT_BROADCAST:
      error (0, 0, "option `broadcast' requires an argument");
      break;

    case EXPECT_NETMASK:
      error (0, 0, "option `netmask' requires an argument");
      break;

    case EXPECT_METRIC:
      error (0, 0, "option `metric' requires an argument");
      break;

    case EXPECT_MTU:
      error (0, 0, "option `mtu' requires an argument");
      break;

    case EXPECT_AF:		/* dummy */
    case EXPECT_COMMAND:	/* dummy */
    case EXPECT_NOTHING:
      return 1;
    }
  return 0;
}

int
system_configure (int sfd _GL_UNUSED_PARAMETER,
		  struct ifreq *ifr _GL_UNUSED_PARAMETER,
		  struct system_ifconfig *ifs _GL_UNUSED_PARAMETER)
{
  return 0;
}


/* System hooks. */
static struct ifaddrs *ifp = NULL;

#ifdef SIOCGIFMEDIA
struct ifmediareq ifm;
#endif /* SIOCGIFMEDIA */

#define ESTABLISH_IFADDRS \
  if (!ifp) \
    getifaddrs (&ifp);

void
system_fh_ifstat_query (format_data_t form, int argc, char *argv[])
{
  /* Flush an existing interface list, thus renewing statistics.  */
  if (ifp)
    {
      freeifaddrs (ifp);
      ifp = NULL;
    }

  select_arg (form, argc, argv, getifaddrs (&ifp) ? 1 : 0);
}

struct if_nameindex* (*system_if_nameindex) (void) = if_nameindex;

static void
print_hwaddr_ether (format_data_t form, unsigned char *data)
{
  put_string (form, ether_ntoa ((struct ether_addr *) data));
  had_output = 1;
}

struct ift_symbol
{
  const char *name;
  const char *title;
  int value;
  void (*print_hwaddr) (format_data_t form, unsigned char *data);
} ift_symbols[] =
  {
#ifdef IFT_ETHER		/* Ethernet CSMA/CD */
# ifdef ETHERNAME
    { "ETHER", ETHERNAME, IFT_ETHER, print_hwaddr_ether},
# else
    { "ETHER", "ether", IFT_ETHER, print_hwaddr_ether},
# endif /* !ETHERNAME */
#endif /* IFT_ETHER */
#ifdef IFT_GIF			/* Generic tunnel (gif) */
    { "IPGIF", "IPIP tunnel", IFT_GIF, NULL},
#endif
#ifdef IFT_FAITH		/* IPv6-to-IPv4 TCP relay capture */
    { "FAITH", "TCP relay capture", IFT_FAITH, NULL},
#endif
#ifdef IFT_LOOP			/* Local loopback */
    { "LOOPBACK", "Local loopback", IFT_LOOP, NULL},
#endif
#ifdef IFT_PFLOG		/* Packet filter logging */
    { "PFLOG", "Packet filter logger", IFT_PFLOG, NULL},
#endif
#ifdef IFT_PFSYNC		/* Packet filter state synching */
    { "PFSYNC", "Packet filter state synching", IFT_PFSYNC, NULL},
#endif
#ifdef IFT_PPP			/* Point-to-Point serial protocol */
    { "PPP", "Point-to-Point over serial", IFT_PPP, NULL},
#endif
#ifdef IFT_SLIP			/* IP over generic TTY */
    { "SLIP", "Serial line IP", IFT_SLIP, NULL},
#endif
#ifdef IFT_TUNNEL		/* Encapsulation (gre) */
    { "IPGRE", "GRE over IP", IFT_TUNNEL, NULL},
#endif
    { NULL, NULL, 0, NULL}
  };

static struct ift_symbol *
ift_findvalue (int value)
{
  struct ift_symbol *ift = ift_symbols;

  while (ift->name != NULL)
    {
      if (ift->value == value)
	break;
      ift++;
    }

  if (ift->name)
    return ift;
  else
    return NULL;
}

void
system_fh_brdaddr_query (format_data_t form, int argc, char *argv[])
{
  ESTABLISH_IFADDRS
  if (!ifp)
    select_arg (form, argc, argv, 1);
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  if (fp->ifa_addr->sa_family != AF_INET ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  if ((fp->ifa_flags & IFF_BROADCAST) && fp->ifa_broadaddr)
	    missing = 0;
	  break;
	}
      select_arg (form, argc, argv, missing);
    }
}

void
system_fh_brdaddr (format_data_t form, int argc, char *argv[])
{
  ESTABLISH_IFADDRS
  if (!ifp)
    put_string (form, "(unknown)");
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  if (fp->ifa_addr->sa_family != AF_INET ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  if ((fp->ifa_flags & IFF_BROADCAST) && fp->ifa_broadaddr)
	    {
	      missing = 0;
	      put_addr (form, argc, argv, fp->ifa_broadaddr);
	    }
	  break;
	}
      if (missing)
	put_string (form, "(unknown)");
    }
}

void
system_fh_hwaddr_query (format_data_t form, int argc, char *argv[])
{
  ESTABLISH_IFADDRS
  if (!ifp)
    select_arg (form, argc, argv, 1);
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  struct sockaddr_dl *dl;

	  if (fp->ifa_addr->sa_family != AF_LINK ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  dl = (struct sockaddr_dl *) fp->ifa_addr;
	  if (dl && (dl->sdl_len > 0))
	    {
	      struct ift_symbol *ift = ift_findvalue (dl->sdl_type);

	      if (ift && ift->print_hwaddr)
		missing = 0;
	    }
	  break;
	}

      select_arg (form, argc, argv, missing);
    }
}

void
system_fh_hwaddr (format_data_t form, int argc _GL_UNUSED_PARAMETER,
		  char *argv[] _GL_UNUSED_PARAMETER)
{
  ESTABLISH_IFADDRS
  if (!ifp)
    put_string (form, "(hwaddr unknown)");
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  struct sockaddr_dl *dl;

	  if (fp->ifa_addr->sa_family != AF_LINK ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  dl = (struct sockaddr_dl *) fp->ifa_addr;
	  if (dl && (dl->sdl_len > 0))
	    {
	      struct ift_symbol *ift = ift_findvalue (dl->sdl_type);

	      if (ift && ift->print_hwaddr)
		{
		  missing = 0;
		  ift->print_hwaddr (form,
				     (unsigned char *) LLADDR (dl));
		}
	    }
	  break;
	}
      if (missing)
	put_string (form, "(hwaddr unknown)");
    }
}

void
system_fh_hwtype_query (format_data_t form, int argc, char *argv[])
{
  int missing = 1;

  ESTABLISH_IFADDRS
  if (ifp)
    {
      struct ifaddrs *fp;
      struct sockaddr_dl *dl = NULL;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  if (fp->ifa_addr->sa_family != AF_LINK ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  dl = (struct sockaddr_dl *) fp->ifa_addr;
	  if (dl && (dl->sdl_len > 0))
	    {
	      struct ift_symbol *ift = ift_findvalue (dl->sdl_type);

	      if (ift && ift->title)
		missing = 0;
	    }
	  break;
	}
    }

  select_arg (form, argc, argv, missing);
}

void
system_fh_hwtype (format_data_t form, int argc _GL_UNUSED_PARAMETER,
		  char *argv[] _GL_UNUSED_PARAMETER)
{
  int found = 0;

  ESTABLISH_IFADDRS
  if (ifp)
    {
      struct ifaddrs *fp;
      struct sockaddr_dl *dl = NULL;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  if (fp->ifa_addr->sa_family != AF_LINK ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  dl = (struct sockaddr_dl *) fp->ifa_addr;
	  if (dl && (dl->sdl_len > 0))
	    {
	      struct ift_symbol *ift = ift_findvalue (dl->sdl_type);

	      if (ift && ift->title)
		{
		  found = 1;
		  put_string (form, ift->title);
		}
	    }
	  break;
	}
    }

  if (!found)
    put_string (form, "(unknown hwtype)");
}

/* Lookup structures provided by the system, each decoding
 * to clear text the meaning of media and status flags.
 *
 * The naming is different in NetBSD and in FreeBSD.
 */
static const struct ifmedia_description media_descr[] =
			IFM_TYPE_DESCRIPTIONS;

#ifdef IFM_SUBTYPE_DESCRIPTIONS
static const struct ifmedia_description subtype_descr[] =
			IFM_SUBTYPE_DESCRIPTIONS;
#endif

#ifdef IFM_SUBTYPE_ETHERNET_DESCRIPTIONS
static const struct ifmedia_description ethernet_descr[] =
			IFM_SUBTYPE_ETHERNET_DESCRIPTIONS;
#endif

#ifdef IFM_SUBTYPE_SHARED_DESCRIPTIONS
static const struct ifmedia_description subtype_shared_descr[] =
			IFM_SUBTYPE_SHARED_DESCRIPTIONS;
#endif

static const struct ifmedia_description option_descr[] =
#ifdef IFM_OPTION_DESCRIPTIONS
			IFM_OPTION_DESCRIPTIONS;
#elif defined IFM_SHARED_OPTION_DESCRIPTIONS
			IFM_SHARED_OPTION_DESCRIPTIONS;
#else
			{ { 0, NULL }, };
#endif


#ifdef IFM_STATUS_DESCRIPTIONS
static const struct ifmedia_status_description status_descr[] =
			IFM_STATUS_DESCRIPTIONS;
#endif

void
system_fh_media_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFMEDIA
  memset (&ifm, 0, sizeof (ifm));
  strncpy (ifm.ifm_name, form->ifr->ifr_name, sizeof (ifm.ifm_name));

  if (ioctl (form->sfd, SIOCGIFMEDIA, &ifm) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif /* SIOCGIFMEDIA */
    select_arg (form, argc, argv, 1);
}

void
system_fh_media (format_data_t form, int argc _GL_UNUSED_PARAMETER,
		 char *argv[] _GL_UNUSED_PARAMETER)
{
#ifdef SIOCGIFMEDIA
  memset (&ifm, 0, sizeof (ifm));
  strncpy (ifm.ifm_name, form->ifr->ifr_name, sizeof (ifm.ifm_name));
  if (ioctl (form->sfd, SIOCGIFMEDIA, &ifm) >= 0)
    {
      const char *medium = NULL, *options = NULL;
      const char *subtype = NULL, *active_subtype = NULL;
      const struct ifmedia_description *item;

      /* Determine media type of adapter.  */
      for (item = media_descr; item->ifmt_word; item++)
	if (item->ifmt_word == IFM_TYPE (ifm.ifm_current))
	  {
	    medium = item->ifmt_string;
	    break;
	  }

#ifdef IFM_SUBTYPE_DESCRIPTIONS
      for (item = subtype_descr; item->ifmt_word || item->ifmt_string; item++)
	{
	  if (item->ifmt_word == IFM_SUBTYPE (ifm.ifm_current) && !subtype)
	    subtype = item->ifmt_string;

	  if (item->ifmt_word == (ifm.ifm_active & (IFM_NMASK | IFM_TMASK))
	      && !active_subtype)
	    active_subtype = item->ifmt_string;
	}
#else /* !IFM_SUBTYPE_DESCRIPTIONS */
      /* Systems like FreeBSD with sub-types split
       * into shared and media specific structures.
       *
       * Sub-type specific media labels.
       */
      switch (IFM_TYPE (ifm.ifm_current))
	{
	case IFM_ETHER:
	  for (item = ethernet_descr; item->ifmt_word; item++)
	    {
	      /* Configured sub-type.  */
	      if (item->ifmt_word == IFM_SUBTYPE (ifm.ifm_current))
		subtype = item->ifmt_string;

	      /* Active subtype.  */
	      if (item->ifmt_word == IFM_SUBTYPE (ifm.ifm_active))
		active_subtype = item->ifmt_string;
	    }
	  ;
	default:
	  ;
	}

      /* Shared media sub-types.  Only works if the previous
       * loops did not assign any string value.
       *
       * One valid instance of 'ifmt_word' is naught,
       * so loop condition needs care.
       */
      for (item = subtype_shared_descr; item->ifmt_word || item->ifmt_string; item++)
	{
	  if (item->ifmt_word == IFM_SUBTYPE (ifm.ifm_current))
	    subtype = item->ifmt_string;

	  if (item->ifmt_word == IFM_SUBTYPE (ifm.ifm_active))
	    active_subtype = item->ifmt_string;
	}
#endif /* !IFM_SUBTYPE_DESCRIPTIONS */

      /* Negotiated mode of operation.  */
      for (item = option_descr; item->ifmt_word; item++)
	if (item->ifmt_word == (ifm.ifm_active & IFM_GMASK) && !options)
	  options = item->ifmt_string;

      /* Print the gathered information.  */
      if (options)
        printf ("%s %s (%s <%s>)", medium, subtype, active_subtype, options);
      else
        printf ("%s %s (%s)", medium, subtype,
		active_subtype ? active_subtype : "none");

      had_output++;
    }
  else
#endif /* SIOCGIFMEDIA */
    put_string (form, "(not known)");
}

void
system_fh_netmask_query (format_data_t form, int argc, char *argv[])
{
  ESTABLISH_IFADDRS
  if (!ifp)
    select_arg (form, argc, argv, 1);
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  if (fp->ifa_addr->sa_family != AF_INET ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  if (fp->ifa_netmask)
	    missing = 0;
	  break;
	}
      select_arg (form, argc, argv, missing);
    }
}

void
system_fh_netmask (format_data_t form, int argc, char *argv[])
{
  ESTABLISH_IFADDRS
  if (!ifp)
    put_string (form, "(unknown)");
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  if (fp->ifa_addr->sa_family != AF_INET ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  if (fp->ifa_netmask)
	    {
	      missing = 0;
	      put_addr (form, argc, argv, fp->ifa_netmask);
	    }
	  break;
	}
      if (missing)
	put_string (form, "(netmask unknown)");
    }
}

void
system_fh_status_query (format_data_t form, int argc, char *argv[])
{
  system_fh_media_query (form, argc, argv);
}

void
system_fh_status (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFMEDIA
  memset (&ifm, 0, sizeof (ifm));
  strncpy (ifm.ifm_name, form->ifr->ifr_name, sizeof (ifm.ifm_name));
  if (ioctl (form->sfd, SIOCGIFMEDIA, &ifm) >= 0)
    {
#ifdef IFM_STATUS_DESCRIPTIONS
      const struct ifmedia_status_description *item;

      for (item = status_descr; item->ifms_type; item++)
	if (item->ifms_type == IFM_TYPE (ifm.ifm_current))
	  break;

      if (item->ifms_type)
	put_string (form, IFM_STATUS_DESC (item, ifm.ifm_status));
      else
	put_int (form, argc, argv, ifm.ifm_status);
#else /* !IFM_STATUS_DESCRIPTIONS */
      if (ifm.ifm_status & IFM_ACTIVE)
	put_string (form, "active");
      else
	put_string (form, "no carrier");
#endif /* IFM_STATUS_DESCRIPTIONS */
    }
  else
#endif /* SIOCGIFMEDIA */
    put_string (form, "(not known)");
}

void
system_fh_tunnel_query (format_data_t form, int argc, char *argv[])
{
#if defined SIOCGIFPSRCADDR && defined SIOCGIFPDSTADDR
  if (ioctl (form->sfd, SIOCGIFPSRCADDR, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif /* SIOCGIFPSRCADDR && SIOCGIFPDSTADDR */
  select_arg (form, argc, argv, 1);
}

void
system_fh_tundst (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFPDSTADDR
  if (ioctl (form->sfd, SIOCGIFPDSTADDR, form->ifr) >= 0)
    put_addr (form, argc, argv, &form->ifr->ifr_addr);
  else
    put_string (form, "(no phydst)");
#endif /* SIOCGIFPDSTADDR */
}

void
system_fh_tunsrc (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFPSRCADDR
  if (ioctl (form->sfd, SIOCGIFPSRCADDR, form->ifr) >= 0)
    put_addr (form, argc, argv, &form->ifr->ifr_addr);
  else
    put_string (form, "(no physrc)");
#endif /* SIOCGIFPSRCADDR */
}

static struct if_data *
get_if_data_by_name (format_data_t form)
{
  struct ifaddrs *fp;
  struct if_data *data = NULL;

  ESTABLISH_IFADDRS
  if (!ifp)
    return NULL;

  for (fp = ifp; fp; fp = fp->ifa_next)
    {
      /* The choice of AF_LINK is the only portable alternative.
       * Then IP, ARP etcera are included in all counters.
       */
      if (fp->ifa_addr->sa_family != AF_LINK ||
	  strcmp (fp->ifa_name, form->ifr->ifr_name))
	continue;

      data = (struct if_data *) fp->ifa_data;
      break;
    }

  return data;
}

void
system_fh_missing_stat (format_data_t form, int argc, char *argv[])
{
  /* FIXME: Would a dash be a better answer?  */
  put_ulong (form, argc, argv, 0);
}

#define _IU_DECLARE2(fld, udata)	\
void					\
_IU_CAT2 (system_fh_, fld) (format_data_t form, int argc, char *argv[])	\
{				\
  struct if_data *data = get_if_data_by_name (form);	\
  if (data)			\
    put_ulong (form, argc, argv, data->_IU_CAT2(ifi_, udata));	\
  else				\
    put_string (form, "(" #fld " unknown)");		\
}

_IU_DECLARE2 (rx_bytes, ibytes)
_IU_DECLARE2 (rx_dropped, iqdrops)
_IU_DECLARE2 (rx_errors, ierrors)
_IU_DECLARE2 (rx_packets, ipackets)
_IU_DECLARE2 (tx_bytes, obytes)
_IU_DECLARE2 (tx_errors, oerrors)
_IU_DECLARE2 (tx_packets, opackets)
_IU_DECLARE2 (collisions, collisions)

void
system_fh_tx_dropped (format_data_t form, int argc, char *argv[])
{
#ifdef _IFI_OQDROPS
  struct if_data *data = get_if_data_by_name (form);

  if (data)
    put_ulong (form, argc, argv, data->ifi_oqdrops);
  else
    put_string (form, "(txerrors unknown)");
#else /* !_IFI_OQDROPS */
  /* FIXME: Would a dash be a better answer?  */
  put_ulong (form, argc, argv, 0);
#endif
}
