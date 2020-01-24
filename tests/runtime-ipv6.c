/* runtime-ipv6 - Check that system is able to set IPv6 address.
  Copyright (C) 2020 Free Software Foundation, Inc.

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

/* Runtime-ipv6 determines by default whether IPv6 address ::1 can
 * be selected.  The switch `-6' is identical, whereas `-4' probes
 * for 127.0.0.1.
 *
 * Invocation:
 *
 *   runtime-ipv6 [-4] [-6]
 *
 * Return value is 0 (zero) when successful, otherwise -1.
 * On failure a message is printed to stderr: `IPv# disabled in system'.
 */

#include <config.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include <sys/socket.h>
#include <netdb.h>

#include <argp.h>
#include <progname.h>
#include <unused-parameter.h>
#include "libinetutils.h"

#define RESOLVE_OK            0
#define RESOLVE_FAIL         -1

/* The default action is to investigate localhost as IPv6.  */
int tested_family = AF_INET6;
char *tested_family_name = "IPv6";
char *tested_localhost = "::1";

const char doc[] = "Detect presence of address family";

const char *program_authors[] = { "Mats Erik Andersson", NULL };

static struct argp_option argp_options[] = {
#define GRP 1
  {"ipv4", '4', NULL, 0, "test for IPv4", GRP },
  {"ipv6", '6', NULL, 0, "test for IPv6 (default)", GRP },
#undef GRP
  {NULL, 0, NULL, 0, NULL, 0}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state _GL_UNUSED_PARAMETER)
{
  switch (key)
    {
    case '4':
      tested_family = AF_INET;
      tested_family_name = "IPv4";
      tested_localhost = "127.0.0.1";
      break;

    case '6':
      tested_family = AF_INET6;
      tested_family_name = "IPv6";
      tested_localhost = "::1";
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp =
  {argp_options, parse_opt, NULL, doc, NULL, NULL, NULL};

int
main (int argc, char *argv[])
{
  int err;
  struct sockaddr_storage ss;
  struct addrinfo hints, *aiptr;

  set_program_name (argv[0]);
  iu_argp_init ("runtime-ipv6", program_authors);
  argp_parse (&argp, argc, argv, 0, NULL, NULL);

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = tested_family;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_NUMERICHOST;
#ifdef AI_ADDRCONFIG
  hints.ai_flags |= AI_ADDRCONFIG;
#endif

  err = getaddrinfo (tested_localhost, "1237", &hints, &aiptr);
  if (!err)
    {
      /* Should not really happen.  */
      if (aiptr == NULL)
#ifdef EAI_NODATA
	err = EAI_NODATA;
#else
	err = EAI_FAIL;
#endif

      freeaddrinfo (aiptr);
    }

  if (err)
    {
      /* Not able to select localhost as IPv6.  */
      fprintf (stderr, "%s is disabled in this running system: \"%s\" %s\n",
	       tested_family_name, tested_localhost, gai_strerror (err));

      return RESOLVE_FAIL;
    }

  return RESOLVE_OK;
}
