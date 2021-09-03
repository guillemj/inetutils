/*
  Copyright (C) 1995-2021 Free Software Foundation, Inc.

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

/*
 * Copyright (c) 1988, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <unistd.h>

#ifndef BSD
# define BSD 43
#endif


#define USE_TERMIO

/*
 * ucb stdio.h defines BSD as something wierd
 */
#if defined sun && defined __svr4__
# define BSD 43
#endif

#ifndef USE_TERMIO
# if BSD > 43 || defined SYSV_TERMIO
#  define USE_TERMIO
# endif
#endif

#include <stdio.h>
#include <setjmp.h>
#if defined CRAY && !defined NO_BSD_SETJMP
# include <bsdsetjmp.h>
#endif
#include <sys/ioctl.h>
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif
#include <errno.h>
#ifdef	USE_TERMIO
# ifndef VINTR
#  ifdef SYSV_TERMIO
#   include <termio.h>
#  else
#   include <termios.h>
#  endif
# endif
#endif
#if defined NO_CC_T || !defined USE_TERMIO
# if !defined USE_TERMIO
typedef char cc_t;
# else
typedef unsigned char cc_t;
# endif
#endif

#if defined USE_TERMIO && !defined SYSV_TERMIO
# define termio termios
#endif

#ifndef NO_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#ifndef _POSIX_VDISABLE
# ifdef sun
#  include <sys/param.h>	/* pick up VDISABLE definition, maybe */
# endif
# ifdef VDISABLE
#  define _POSIX_VDISABLE VDISABLE
# else
#  define _POSIX_VDISABLE ((cc_t)'\377')
# endif
#endif

#define SUBBUFSIZE	256

extern int autologin,		/* Autologin enabled */
  skiprc,			/* Don't process the ~/.telnetrc file */
  eight,			/* use eight bit mode (binary in and/or out */
  flushout,			/* flush output */
  connected,			/* Are we connected to the other side? */
  globalmode,			/* Mode tty should be in */
  In3270,			/* Are we in 3270 mode? */
  telnetport,			/* Are we connected to the telnet port? */
  localflow,			/* Flow control handled locally */
  restartany,			/* If flow control, restart output on any character */
  localchars,			/* we recognize interrupt/quit */
  donelclchars,			/* the user has set "localchars" */
  showoptions, net,		/* Network file descriptor */
  tin,				/* Terminal input file descriptor */
  tout,				/* Terminal output file descriptor */
  crlf,				/* Should '\r' be mapped to <CR><LF> (or <CR><NUL>)? */
  autoflush,			/* flush output when interrupting? */
  autosynch,			/* send interrupt characters with SYNCH? */
  SYNCHing,			/* Is the stream in telnet SYNCH mode? */
  donebinarytoggle,		/* the user has put us in binary */
  dontlecho,			/* do we suppress local echoing right now? */
  crmod, netdata,		/* Print out network data flow */
  prettydump,			/* Print "netdata" output in user readable format */
#if defined TN3270
  cursesdata,			/* Print out curses data flow */
  apitrace,			/* Trace API transactions */
#endif
	/* defined(TN3270) */
  termdata,			/* Print out terminal data flow */
  debug;			/* Debug level */

extern cc_t escape;		/* Escape to command mode */
extern cc_t rlogin;		/* Rlogin mode escape character */
#ifdef	KLUDGELINEMODE
extern cc_t echoc;		/* Toggle local echoing */
#endif

extern char *prompt;		/* Prompt for command. */

extern char doopt[], dont[], will[], wont[], options[],	/* All the little options */
 *hostname;			/* Who are we connected to? */
#ifdef	ENCRYPTION
extern void (*encrypt_output) (unsigned char *, int);
extern int (*decrypt_input) (int);
#endif /* ENCRYPTION */

/*
 * We keep track of each side of the option negotiation.
 */

#define MY_STATE_WILL		0x01
#define MY_WANT_STATE_WILL	0x02
#define MY_STATE_DO		0x04
#define MY_WANT_STATE_DO	0x08

/*
 * Macros to check the current state of things
 */

#define my_state_is_do(opt)		(options[opt]&MY_STATE_DO)
#define my_state_is_will(opt)		(options[opt]&MY_STATE_WILL)
#define my_want_state_is_do(opt)	(options[opt]&MY_WANT_STATE_DO)
#define my_want_state_is_will(opt)	(options[opt]&MY_WANT_STATE_WILL)

#define my_state_is_dont(opt)		(!my_state_is_do(opt))
#define my_state_is_wont(opt)		(!my_state_is_will(opt))
#define my_want_state_is_dont(opt)	(!my_want_state_is_do(opt))
#define my_want_state_is_wont(opt)	(!my_want_state_is_will(opt))

#define set_my_state_do(opt)		{options[opt] |= MY_STATE_DO;}
#define set_my_state_will(opt)		{options[opt] |= MY_STATE_WILL;}
#define set_my_want_state_do(opt)	{options[opt] |= MY_WANT_STATE_DO;}
#define set_my_want_state_will(opt)	{options[opt] |= MY_WANT_STATE_WILL;}

#define set_my_state_dont(opt)		{options[opt] &= ~MY_STATE_DO;}
#define set_my_state_wont(opt)		{options[opt] &= ~MY_STATE_WILL;}
#define set_my_want_state_dont(opt)	{options[opt] &= ~MY_WANT_STATE_DO;}
#define set_my_want_state_wont(opt)	{options[opt] &= ~MY_WANT_STATE_WILL;}

/*
 * Make everything symetrical
 */

#define HIS_STATE_WILL			MY_STATE_DO
#define HIS_WANT_STATE_WILL		MY_WANT_STATE_DO
#define HIS_STATE_DO			MY_STATE_WILL
#define HIS_WANT_STATE_DO		MY_WANT_STATE_WILL

#define his_state_is_do			my_state_is_will
#define his_state_is_will		my_state_is_do
#define his_want_state_is_do		my_want_state_is_will
#define his_want_state_is_will		my_want_state_is_do

#define his_state_is_dont		my_state_is_wont
#define his_state_is_wont		my_state_is_dont
#define his_want_state_is_dont		my_want_state_is_wont
#define his_want_state_is_wont		my_want_state_is_dont

#define set_his_state_do		set_my_state_will
#define set_his_state_will		set_my_state_do
#define set_his_want_state_do		set_my_want_state_will
#define set_his_want_state_will		set_my_want_state_do

#define set_his_state_dont		set_my_state_wont
#define set_his_state_wont		set_my_state_dont
#define set_his_want_state_dont		set_my_want_state_wont
#define set_his_want_state_wont		set_my_want_state_dont


extern FILE *NetTrace;		/* Where debugging output goes */
extern unsigned char NetTraceFile[];	/* Name of file where debugging output goes */
extern void SetNetTrace (char *);	/* Function to change where debugging goes */

extern jmp_buf peerdied, toplevel;	/* For error conditions. */

extern void
command (int, char *, int),
Dump (char, unsigned char *, int),
init_3270 (void),
printoption (char *, int, int),
printsub (char, unsigned char *, int),
sendnaws (void),
setconnmode (int),
setcommandmode (void),
setneturg (void),
sys_telnet_init (void),
telnet (char *),
tel_enter_binary (int),
tel_leave_binary (int),
TerminalFlushOutput (void),
TerminalNewMode (int),
TerminalRestoreState (void),
TerminalSaveState (void),
tninit (void), upcase (char *), willoption (int), wontoption (int);

extern int TerminalWindowSize (long *rows, long *cols);
extern void TerminalSpeeds (long *ispeed, long *ospeed);
extern void TerminalDefaultChars (void);
extern int TerminalSpecialChars (int c);
extern int TerminalAutoFlush (void);
extern int TerminalWrite (char *buf, int n);

extern int telrcv (void);
extern int getconnmode (void);
extern void optionstatus (void);
extern int NetClose (int);
extern void Exit (int);
extern void ExitString (char *, int);
extern int netflush (void);
extern int opt_welldefined (char *);
extern int stilloob (void);
extern int process_rings (int, int, int, int, int, int);

extern void init_terminal (void);
extern void init_network (void);
extern void init_telnet (void);
extern void init_sys (void);

extern void set_escape_char (char*);
extern int tn (int argc, char **argv);

extern void
send_do (int, int),
send_dont (int, int), send_will (int, int), send_wont (int, int);

extern void sendabort (void);
extern void sendeof (void);
extern void sendsusp (void);
extern int ttyflush (int);
extern int rlogin_susp (void);
extern void env_init (void);

/* FIXME: Not needed */
int SetSockOpt (int fd, int level, int option, int yesno);

extern int quit (void);

extern void
lm_will (unsigned char *, int),
lm_wont (unsigned char *, int),
lm_do (unsigned char *, int),
lm_dont (unsigned char *, int), lm_mode (unsigned char *, int, int);

extern void
slc_init (void),
slcstate (void),
slc_mode_export (void),
slc_mode_import (int),
slc_import (int),
slc_export (void),
slc (unsigned char *, int),
slc_check (void),
slc_start_reply (void),
slc_add_reply (unsigned char, unsigned char, cc_t), slc_end_reply (void);
extern int slc_update (void);

extern void
env_opt (unsigned char *, int),
env_opt_start (void),
env_opt_start_info (void), env_opt_add (unsigned char *), env_opt_end (int);

extern unsigned char *env_default (int, int), *env_getvalue (const char *);

extern int get_status (void), dosynch (void);

extern cc_t *tcval (int);

#ifndef USE_TERMIO

extern struct tchars ntc;
extern struct ltchars nltc;
extern struct sgttyb nttyb;

# define termEofChar		ntc.t_eofc
# define termEraseChar		nttyb.sg_erase
# define termFlushChar		nltc.t_flushc
# define termIntChar		ntc.t_intrc
# define termKillChar		nttyb.sg_kill
# define termLiteralNextChar	nltc.t_lnextc
# define termQuitChar		ntc.t_quitc
# define termSuspChar		nltc.t_suspc
# define termRprntChar		nltc.t_rprntc
# define termWerasChar		nltc.t_werasc
# define termStartChar		ntc.t_startc
# define termStopChar		ntc.t_stopc
# define termForw1Char		ntc.t_brkc
extern cc_t termForw2Char;
extern cc_t termAytChar;

# define termEofCharp		(cc_t *)&ntc.t_eofc
# define termEraseCharp		(cc_t *)&nttyb.sg_erase
# define termFlushCharp		(cc_t *)&nltc.t_flushc
# define termIntCharp		(cc_t *)&ntc.t_intrc
# define termKillCharp		(cc_t *)&nttyb.sg_kill
# define termLiteralNextCharp	(cc_t *)&nltc.t_lnextc
# define termQuitCharp		(cc_t *)&ntc.t_quitc
# define termSuspCharp		(cc_t *)&nltc.t_suspc
# define termRprntCharp		(cc_t *)&nltc.t_rprntc
# define termWerasCharp		(cc_t *)&nltc.t_werasc
# define termStartCharp		(cc_t *)&ntc.t_startc
# define termStopCharp		(cc_t *)&ntc.t_stopc
# define termForw1Charp		(cc_t *)&ntc.t_brkc
# define termForw2Charp		(cc_t *)&termForw2Char
# define termAytCharp		(cc_t *)&termAytChar

#else

extern struct termio new_tc;

# define termEofChar		new_tc.c_cc[VEOF]
# define termEraseChar		new_tc.c_cc[VERASE]
# define termIntChar		new_tc.c_cc[VINTR]
# define termKillChar		new_tc.c_cc[VKILL]
# define termQuitChar		new_tc.c_cc[VQUIT]

# ifndef VSUSP
extern cc_t termSuspChar;
# else
#  define termSuspChar		new_tc.c_cc[VSUSP]
# endif
# if defined VFLUSHO && !defined VDISCARD
#  define VDISCARD VFLUSHO
# endif
# ifndef VDISCARD
extern cc_t termFlushChar;
# else
#  define termFlushChar		new_tc.c_cc[VDISCARD]
# endif
# ifndef VWERASE
extern cc_t termWerasChar;
# else
#  define termWerasChar		new_tc.c_cc[VWERASE]
# endif
# ifndef VREPRINT
extern cc_t termRprntChar;
# else
#  define termRprntChar		new_tc.c_cc[VREPRINT]
# endif
# ifndef VLNEXT
extern cc_t termLiteralNextChar;
# else
#  define termLiteralNextChar	new_tc.c_cc[VLNEXT]
# endif
# ifndef VSTART
extern cc_t termStartChar;
# else
#  define termStartChar		new_tc.c_cc[VSTART]
# endif
# ifndef VSTOP
extern cc_t termStopChar;
# else
#  define termStopChar		new_tc.c_cc[VSTOP]
# endif
# ifndef VEOL
extern cc_t termForw1Char;
# else
#  define termForw1Char		new_tc.c_cc[VEOL]
# endif
# ifndef VEOL2
extern cc_t termForw2Char;
# else
#  define termForw2Char		new_tc.c_cc[VEOL]
# endif
# ifndef VSTATUS
extern cc_t termAytChar;
# else
#  define termAytChar		new_tc.c_cc[VSTATUS]
# endif

# if !defined CRAY || defined __STDC__
#  define termEofCharp		&termEofChar
#  define termEraseCharp	&termEraseChar
#  define termIntCharp		&termIntChar
#  define termKillCharp		&termKillChar
#  define termQuitCharp		&termQuitChar
#  define termSuspCharp		&termSuspChar
#  define termFlushCharp	&termFlushChar
#  define termWerasCharp	&termWerasChar
#  define termRprntCharp	&termRprntChar
#  define termLiteralNextCharp	&termLiteralNextChar
#  define termStartCharp	&termStartChar
#  define termStopCharp		&termStopChar
#  define termForw1Charp	&termForw1Char
#  define termForw2Charp	&termForw2Char
#  define termAytCharp		&termAytChar
# else
	/* Work around a compiler bug */
#  define termEofCharp		0
#  define termEraseCharp	0
#  define termIntCharp		0
#  define termKillCharp		0
#  define termQuitCharp		0
#  define termSuspCharp		0
#  define termFlushCharp	0
#  define termWerasCharp	0
#  define termRprntCharp	0
#  define termLiteralNextCharp	0
#  define termStartCharp	0
#  define termStopCharp		0
#  define termForw1Charp	0
#  define termForw2Charp	0
#  define termAytCharp		0
# endif
#endif


/* Ring buffer structures which are shared */

extern Ring netoring, netiring, ttyoring, ttyiring;

/* Tn3270 section */
#if defined TN3270

extern int HaveInput,		/* Whether an asynchronous I/O indication came in */
  noasynchtty,			/* Don't do signals on I/O (SIGURG, SIGIO) */
  noasynchnet,			/* Don't do signals on I/O (SIGURG, SIGIO) */
  sigiocount,			/* Count of SIGIO receptions */
  shell_active;			/* Subshell is active */

extern char *Ibackp,		/* Oldest byte of 3270 data */
  Ibuf[],			/* 3270 buffer */
 *Ifrontp,			/* Where next 3270 byte goes */
  tline[], *transcom;		/* Transparent command */

extern int settranscom (int, char **);

extern void inputAvailable (int);
#endif /* defined(TN3270) */
