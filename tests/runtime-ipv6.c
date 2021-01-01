/* runtime-ipv6 - Check that system is able to set IPv6 address.
  Copyright (C) 2020, 2021 Free Software Foundation, Inc.

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
 *
 * By defining the shell environment variable VERBOSE, a short affirmative
 * message is printed also in case of successful testing.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include <sys/socket.h>
#include <netdb.h>

#define RESOLVE_OK            0
#define RESOLVE_FAIL         -1

/* The default action is to investigate localhost as IPv6.  */
int tested_family = AF_INET6;
char *tested_family_name = "IPv6";
char *tested_localhost = "::1";

int
main (int argc, char *argv[])
{
  int err, ch;
  struct addrinfo hints, *aiptr;

  while ((ch = getopt (argc, argv, "46")) != -1)
    {
      switch (ch)
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
	  fprintf (stderr, "Usage: %s [-4 | -6]\n"
		   "Determines presence of selected INET address family.\n",
		   argv[0]);
	  return EXIT_FAILURE;
	}
    }

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = tested_family;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
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
  else if (getenv ("VERBOSE"))
    printf ("Detection of %s address %s was successful.\n",
	    tested_family_name, tested_localhost);

  return RESOLVE_OK;
}
