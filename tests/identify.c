/* identify -- Probe system and report characteristica.
  Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021
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

/* Written by Mats Erik Andersson.  */

/* Collect information about this system and report it.
 * Some identified characteristica of relevance to GNU Inetutils
 * are displayed for use in bug reporting and resolution.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/param.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifdef HAVE_TERMIOS_H
# include <errno.h>
# if HAVE_FCNTL_H
#  include <fcntl.h>
# endif
# include <termios.h>

# include <pty.h>
#endif /* HAVE_TERMIOS_H */

#define tell_macro(a,b)	\
  printf ("%s: %s\n", (b) ? "Available macro" : "Not defined", (a));

#ifdef HAVE_TCGETATTR
# define test_flag(a, b, name)	\
  if (tc.a & b) printf (" %s", name);

void
do_termcap(void)
{
  int pty, tty;
  struct termios tc;

  pty = openpty (&pty, &tty, NULL, NULL, NULL);
  if (pty < 0)
    {
      fprintf (stderr, "openpt: errno = %d, %s\n",
	       errno, strerror (errno));
      return;
    }

  if (tcgetattr (tty, &tc) < 0)
    {
      fprintf (stderr, "tcgetattr; errno = %d, %s\n",
	       errno, strerror (errno));
      return;
    }

  printf ("_POSIX_VDISABLE = 0x%02x\n\n", _POSIX_VDISABLE);

  /* Report the most interesting observations.  */
  puts ("Default terminal settings (octal and hexadecimal):");
  printf ("iflag = 0%06o = 0x%04x,  oflag = 0%06o = 0x%04x\n",
	  tc.c_iflag, tc.c_iflag, tc.c_oflag, tc.c_oflag);
  printf ("cflag = 0%06o = 0x%04x,  lflag = 0%06o = 0x%04x\n",
	  tc.c_cflag, tc.c_cflag, tc.c_lflag, tc.c_lflag);

  puts ("\nThe most relevant properties being set:");

  printf ("iflag:");
  test_flag (c_iflag, IXOFF, "IXOFF");
  test_flag (c_iflag, IXON, "IXON");
  test_flag (c_iflag, IXANY, "IXANY");
  test_flag (c_iflag, BRKINT, "BRKINT");
  test_flag (c_iflag, ISTRIP, "ISTRIP");
  test_flag (c_iflag, IGNCR, "IGNCR");
  test_flag (c_iflag, INLCR, "INLCR");
  test_flag (c_iflag, ICRNL, "ICRNL");
  puts ("");

  printf ("oflag:");
  test_flag (c_oflag, OPOST, "OPOST");
  test_flag (c_oflag, OCRNL, "OCRNL");
  test_flag (c_oflag, ONLCR, "ONLCR");
  test_flag (c_oflag, ONLRET, "ONLRET");

# ifdef TABDLY
  printf (" TAB");
  switch (tc.c_oflag & TABDLY)
    {
    case TAB3:
      putchar ('3');
      break;
#  ifdef TAB2
    case TAB2:
      putchar ('2');
      break;
#  endif /* TAB2 */
#  ifdef TAB1
    case TAB1:
      putchar ('1');
      break;
#  endif /* TAB1 */
    case TAB0:
      putchar ('0');
      break;
    }
# elif defined OXTABS /* !TABDLY */
  test_flag (c_oflag, OXTABS, "OXTABS");
# endif /* OXTABS */

  puts ("");

  printf ("cflag:");
  switch (tc.c_cflag & CSIZE)
    {
    case CS8:
      printf (" CS8");
      break;
    case CS7:
      printf (" CS7");
    }
  puts ("");

  printf ("lflag:");
  test_flag (c_lflag, ICANON, "ICANON");
  test_flag (c_lflag, ISIG, "TRAPSIG");
  test_flag (c_lflag, IEXTEN, "IEXTEN");
  test_flag (c_lflag, ECHO, "ECHO");
  test_flag (c_lflag, ECHOE, "ECHOE");
  test_flag (c_lflag, ECHOK, "ECHOK");
  test_flag (c_lflag, ECHOKE, "ECHOKE");
  test_flag (c_lflag, ECHONL, "ECHONL");
  test_flag (c_lflag, ECHOCTL, "ECHOCTL");
  puts ("");

  return;
}
# undef test_flag
#endif /* HAVE_TCGETATTR */


int
main (void)
{
  int a, ux;
  struct utsname uts;
  struct sockaddr_un su;

  if (uname (&uts) < 0)
    {
      fprintf (stderr, "Not able to identify running system.\n");
      exit (EXIT_FAILURE);
    }

  /* Identify the hardware.  */
  printf ("Running system: %s, %s\n",
	  uts.sysname, uts.machine);
  printf (" Variant: %s\n", uts.release);
  printf (" Variant: %s\n", uts.version);
  puts ("");

  /*
   * Report on macros that determine alternate code.
   * These depend on toolchains and hardware.
   */
  ux = 0;
#ifdef unix
  ux = 1;
#endif
  tell_macro ("unix", ux);

  ux = 0;
#ifdef __unix
  ux = 1;
#endif
  tell_macro ("__unix", ux);

  ux = 0;
#ifdef __unix__
  ux = 1;
#endif
  tell_macro ("__unix__", ux);

  ux = 0;
#ifdef __sun
  ux = 1;
#endif
  tell_macro ("__sun", ux);

  ux = 0;
#ifdef __sun__
  ux = 1;
#endif
  tell_macro ("__sun__", ux);

  a = 0;
#ifdef TN3270
  a = 1;
#endif
  tell_macro ("TN3270", a);

  /*
   * Implementation specific charateristica.
   */
  puts ("");

  a = 0;
#ifdef BSD
  a = 1;
#endif
  tell_macro ("BSD", a);

  printf ("Size of 'struct sockaddr_un.sun_path': %zu\n",
	  sizeof (su.sun_path));

#if HAVE_TCGETATTR
  puts("");
  do_termcap ();
#endif

  return 0;
}
