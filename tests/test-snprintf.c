/* test-snprintf - Check whether snprintf allows doubly used strings.
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

/*
 * It is not in violation of POSIX, but implementations of snprintf()
 * differ in their ability to handle a situation where the target string
 * to be produced by snprintf() is at the same time providing data, which
 * are to be passed through the format string, and to be part of the new
 * resulting string.  The different BSD unices, as well as OpenSolaris,
 * are known to correctly handle this, whereas glibc is known to fail.
 *
 * This test issues a warning that non-portable behaviour is possible,
 * should the program detect a discrepancy on this system.  If the
 * program detects the macro PACKAGE to have value "inetutils", then
 * get attention with the exit value 77, to fit with our testing
 * frame work.
 *
 * When the environment variable VERBOSE is defined, an encouraging
 * message is issued also in the presence of an enhanced implementation
 * of snprintf().
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE		"try a fool"
#define WRONG_MESSAGE	"fool"

int
main (void)
{
  int err = 0;
  char msg[sizeof (MESSAGE)] = "try a ";

  snprintf (msg, sizeof (msg), "%s%s", msg, WRONG_MESSAGE);

  if (!strcmp (msg, WRONG_MESSAGE))
    {
      printf ("Warning! This implementation of snprintf() is not able\n"
	      "to handle string variables that appear as target and at the\n"
	      "same time as source. This might be a portability issue!\n");
#ifdef PACKAGE
      if (!strcmp (PACKAGE, "inetutils"))
	err = 77;
#endif
    }
  else if (getenv ("VERBOSE"))
    {
      printf ("The present implementation of snprintf() allows the same\n"
	      "string to act as target as well as source.  Good news!\n");
    }

  /* Always return successfully, as we are only producing a reminder,
     rather than a show stopper.  */
  return err;
}
