/*
**	z26 -- an Atari 2600 emulator
*/

#define Z26_RELEASE "z26 -- An Atari 2600 Emulator"

#ifndef WINDOWS
#ifndef MAC
#define do_linux_desktop_integration
#endif
#endif

void QueueSoundBytes(void);
void position_game(void);
void srv_Events(void);
void show_scanlines(void);
void show_transient_status(void);

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>		// not in vc++ -- must be supplied 
#include <sys/stat.h>
#include <time.h>
#include <math.h>

const char *homedir;
char z26cli[1024] = {0};
char z26gui[1024] = {0};
char z26home[1024] = {0};
char z26log[1024] = {0};
char z26wav[1024] = {0};

#ifdef LINUX
#include <unistd.h>		// for chdir
#include <sys/types.h>
#include <pwd.h>
#include <assert.h>		// for mkpath()
#include <errno.h>		// ditto
#endif

FILE *parmfp = NULL;

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_keyboard.h>

typedef unsigned long long int  dq;
typedef unsigned int			dd;
typedef unsigned short int		dw;
typedef unsigned char			db;

void draw_trace_column_headers(void);

#include "globals.c"
#include "palette.c"
#include "vcs_slot.c"
#include "ct.c"
#include "carts.c"
#include "cli.c"
#include "kidvid.c"
#include "controls.c"
#include "position.c"
#include "2600core.c"

/**
	z26 is Copyright 1997-2019 by John Saeger and contributors.
	z26 is released subject to the terms and conditions of the 
	GNU General Public License Version 2 (GPL).
	z26 comes with no warranty.
	Please see COPYING.TXT for details.
*/
