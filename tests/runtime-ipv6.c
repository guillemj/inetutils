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

/* Runtime-ipv6 determines whether IPv6 address ::1 can be selected.
 *
 * Invocation:
 *
 *   runtime-ipv6
 *
 * Return value is 0 (zero) when successful, otherwise -1.
 * On failure a message is printed to stderr: `IPv6 disabled in system'.
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

#include <progname.h>

#define RESOLVE_OK            0
#define RESOLVE_FAIL         -1

int
main (int argc, char *argv[])
{
  int err;
  struct sockaddr_storage ss;
  struct addrinfo hints, *aiptr;

  set_program_name (argv[0]);

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_NUMERICHOST;

  err = getaddrinfo ("::1", "tftp", &hints, &aiptr);
  if (!err)
    {
      /* Should not really happen.  */
      if (aiptr == NULL)
	err = 1;

      freeaddrinfo (aiptr);
    }

  if (err)
    {
      /* Not able to select localhost as IPv6.  */
      fprintf (stderr, "IPv6 is disabled in this running system!");

      return RESOLVE_FAIL;
    }

  return RESOLVE_OK;
}
