#ifndef Z26_H
#define Z26_H

/* From z26.c
 **************************************************************************************/

#define Z26_RELEASE "z26 -- An Atari 2600 Emulator"

#ifndef WINDOWS
#ifndef MAC
#define do_linux_desktop_integration
#endif
#endif

void QueueSoundBytes(void);
void position_game(void);
void srv_print(char *msg);
void srv_Events(void);
void show_scanlines(void);
void show_transient_status(void);
void set_status(char *status);
void gui(void);

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

/* From globals.c
 ********************************************************************************************/
 
extern double WorkPercent;
extern dd TIACollide;	// state of the 15 collision bits

extern char FileName[260];

extern int screen_width;	/* physical width */
extern int screen_height;	/* physical height */
extern int win_width;
extern int win_height;
extern int tiawidth;		
extern int pixelspread;	/* how many times to replicate the tia pixels */
extern int scanlinespread;	/* how many display lines per scanline */
extern int lines2draw;		/* how many display lines to draw */

/* controller code definitions */
#define JS 0	/* joystick */
#define PC 1	/* paddle controller */
#define KP 2	/* keypad */
#define DC 3	/* driving controller */
#define LG 4	/* lightgun */
#define CM 5	/* Compumate keyboard */
#define KV 6	/* Kid Vid tape player */
#define ML 7	/* Mindlink */
#define ST 8	/* ST mouse / CX-80 TRAK Ball */
#define TB 9	/* CX-22 TRAK Ball */
#define AM 10	/* Amiga mouse */
#define NC 11	/* no controller connected */

extern int MessageCode;		/* gets message code for final output */
extern char msg[1100];			/* message buffer */
extern char stat_msg[100];		/* status message buffer */
extern int status_timer;	/* Counts down once per frame while status is shown,
						   status disappears when it's -1 again. */

extern int CartSize;			/* gets cart size */

extern db CartRom[0x80000];	// db CartRom[0x8000f];
extern db PCXPalette[384];		/* palette information goes here */

extern db *ScreenBuffer;			/* pointer to screen buffer (set to RealScreenBuffer 1-4) */
extern db *ScreenBufferPrev;		/* pointer to screen buffer for previous frame (RSB 1-4)*/

extern dd LinesInFrame;		/* # of lines in last frame */
extern dd PrevLinesInFrame;	/* # of lines in frame before that */
extern dd VBlankOff;			/* a valid VBlankOff was detected here */
extern dd VBlankOn;			/* a valid VBlankOn was detected here */
extern dd MaxLines;			/* # of lines in this video mode */
extern dd MaxNTSC;				/* # of lines if NTSC game */

extern dd SC_StartAddress;		/* supercharger start address */
extern db SC_ControlByte;		/* supercharger control byte */

extern dd crc;					/* holds accumulated CRC */

#define KEYTABLESIZE 512

extern db KeyTable[KEYTABLESIZE];	/* event handler should tell z26 about keystrokes here */

/* SDL event handler should tell z26 about joystick states here */
extern int JoystickAxis[16][6];
extern db JoystickButton[16][32];

extern db ShowFPS;

#define SQ_BUCKET 1024
#define SQ_MAX 8*SQ_BUCKET

extern db SoundQ[SQ_MAX];		/* sound queue */
extern dd SQ_Max;		/* must be divisible by 32 (see sdlsound.c) */
						/* but we like it divisible by 1024 */

/* make the output buffers big enough for 500 scanlines with tiawidth of 320 */

extern db RealScreenBuffer1[320*501];
extern db RealScreenBuffer2[320*501];

/* paddle charges (can be 1-240) */

#define TRIGMIN 1
#define TRIGMAX 240

extern dd ChargeTrigger0[4];

#define CHARGEMAX 0x800000;	/* don't charge capacitors over this value (MAIN.ASM)*/
extern dd ChargeCounter;		/* # of lines capacitors have been charging */

extern db InputLatch[2];	/* INPT4 + INPT5 */

extern db CM_Collumn;			/* Compumate key collumn to read - gets set by bankswitching */

extern int ExitEmulator;			/* leave the emulator when ESC or backslash are pressed */
extern int GamePaused;				/* toggle to pause emulator */
extern int GameReallyPaused;		/* ... it's a little bit complicated by the GUI */

extern int ResetEmulator;		// <-- Making this 1 instead of 0 fixes a really old
							//  bug relating to launching from command line. 
							//  It's a really great idea to reset the emulator 
							//  when launching from the command line. We also 
							//  do it down below for good measure.

extern int StartInGUI;
extern int LaunchedFromCommandline;

extern time_t MyTime;				/* structure used by the timer function to return 'Seconds' */
extern int Seconds;				/* seconds since 1.1.1970 - used to randomize RIOT timer */


/*
** The following variables are typically set by command line switches.
** They are reinitialized at startup and from the GUI.
** The GUI reinitializes them before reading command line parms.
*/

extern int UserLeftController;		/* -)  user specified left controller */
extern int LeftController;			/* controller in left port */

extern int UserRightController;	/* -(  user specified right controller */
extern int RightController;		/* controller in Right port */

extern int UserBankswitch;		/* -gN user specified bankswitch type */
extern int BSType;				/* override bankswitching type *EST* */

extern int UserPaletteNumber;		/* -cN user specified palette number */
extern int GamePaletteNumber;		/* game specific palette */
extern int PaletteNumber;			/* 0 = NTSC, 1 = PAL */

extern int UserDepth;				/* user specified Depthescence */
extern int Depth;					/* Depthescence */

extern int UserAllowAll4;			/* user specified allow all 4 directions on the joystick simultaniously */
extern int AllowAll4;				/* allow all 4 directions on the joystick simultaniously */

extern int UserSwapPortsFlag;		/* user specified port 0 to be swapped with port 1 */
extern int SwapPortsFlag;			/* controller port 0 to be swapped with port 1 */

extern int VideoMode;				/* default video mode */
extern int FullScreen;				/* run game full screen */
extern int width_adjust;		/* width adjustment */
extern int SimColourLoss;			/* simulate PAL colour loss *EST* */
extern int DoScanline;				/* do scanlines effect */
extern int Vsync; 					/* sync to monitor */

extern int GameOffset;				/* game specific vertical offset */
extern int CFirst;					/* first game line to display (zero has VBlank trigger a new frame) */
extern int OldCFirst;				/* remember original CFirst (for homing the display) */
extern int DefaultCFirst;			/* emu recommends game start here */

extern db quiet;					/* set if we want no sound */
extern int dsp;					/* do digital signal processing */

extern int UserP0Diff;				/* user specified player zero difficulty */
extern int UserP1Diff;				/* user specified player one difficulty */
extern int UserConsoleBW;			/* user specified console forced to B/W */

extern db IOPortA;					/* IO Port A (joystick) */
extern db IOPortA_Controllers;		/* remember controller data throughout frame */
extern db IOPortA_UnusedBits;		/* bits unused by selected controller = 1 */
extern db IOPortA_Write;			/* value written to SWCHA */
extern db IOPortB;					/* IO Port B (switches) */

extern int UserPaddleSensitivity;
extern int PaddleSensitivity;		/* keyboard sensitivity for paddle emulation */

extern int UserTraceCount;
extern int TraceCount;				/* gets trace count */
extern int OldTraceCount;			/* for saving trace count if we turn off tracing */

extern int UserTraceEnabled;
extern int TraceEnabled;			/* if trace is enabled */

extern int UserMouseBaseX;
extern int UserMouseBaseY;
extern int MouseBaseX;				/* paddle to emulate with mouse X axis (0ffH = joystick) *EST* */
extern int MouseBaseY;				/* paddle to emulate with mouse Y axis (0ffH = joystick) *EST* */

extern char UserLightgun;
extern char UserLGadjust;
extern int Lightgun;				/* emulate lightgun and adjust horizontally *EST* */
extern int LGadjust;				/* adjust lightgun vertically *EST* */

extern int ShowLineCount;			/* display stats on game exit */
extern int theme;					/* color theme for GUI */
extern db KidVid;					/* ID byte on tapes for KidVid game *EST* */
extern db KidVidTape;				/* tape number to be played *EST* */
extern int PaddleAdjust;			/* wait for N lines before starting to evaluate paddle charge */

extern int UserJoystickEnabled;	/* user wants the joystick enabled */
extern int JoystickEnabled;		/* allow PC joystick input for VCS controllers with multiple PC inputs */
extern int MouseEnabled;			/* allow PC mouse input for VCS controllers with multiple PC inputs */
extern int KeyboardEnabled;		/* allow PC keyboard input for VCS controllers with multiple PC inputs */

void def_LoadDefaults(void);

// C-engine specific

extern void (* ReadAccess[0x10000])(void);
extern void (* WriteAccess[0x10000])(void);
extern void (* TIARIOTReadAccess[0x1000])(void);
extern void (* TIARIOTWriteAccess[0x1000])(void);

extern db RiotRam[128];	/* RIOT RAM (must be zero initially) */
extern db TIA[64];			/* TIA registers (also zeroed) */
extern db Ram[32768];		/* Extra RAM (used or not depends on bankswitch scheme) */

extern int Frame;		/* Frame counter */
extern int PrevFrame;	/* Previous value of frame counter */

extern int VBlanking;	/* 0 if vblanking, -1 otherwise */
extern int VBlank;		/* VBlank flag */
extern int VSyncFlag;	/* VSync flag */
extern int VSyncCount;	/* VSync count this frame */

extern int ScanLine;	/* Current scan line */

extern db *DisplayPointer;	/* where to store the next TIA graphics output byte */

#define CYCLESPERSCANLINE 76
#define TRIGMIN 1

extern db DataBus;			/* state of the data bus - used for  moving data to and from the CPU emu */
extern dw AddressBus;		/* state of the address bus - used for read and write accesses */
extern void (* ReadROMAccess)(void);	/* the bankswitching read handlers will be called though this */
extern void (* WriteROMAccess)(void);	/* the bankswitching write handlers will be called through this */

/* variables from CPU emulation - used in tracing too */

extern dw reg_pc;		/*  program counter */
extern db reg_sp;		/*  stack pointer */
extern db reg_a;		/*  accumulator (stored in AL) */
extern db flag_C;		/*  carry bit   (stored in AH) */
extern db reg_x;		/*  x register */
extern db reg_y;		/*  y register */
extern db flag_Z;		/*  zero test value (Z set when 0) */
extern db flag_N;		/*  sign test value (N set when negative) */
extern db flag_D;		/*  decimal flag */
extern db flag_V;		/*  overflow flag */
extern db flag_I;		/*  interrupt disable flag */
extern db flag_B;		/*  break flag */
extern db RCycles;		/*  cycles per instruction */
extern db RClock;		/*  clock cycles */

extern db TriggerWSYNC;	/* TIA tells CPU to pause on next read cycle */


/* 
** Init C Variables every time emulator() is called.
**
** called from init.asm
*/

void InitCVars(void);

/* z_defs.c
 **********************************************************************/
 
#define CYCLESPERSCANLINE 76	// ;  TIA timing constant

// ;*
// ;* TIA register definitions
// ;*

#define VSYNC 	0x00         // ; * vertical sync set/clear       \- 
#define VBLANK 	0x01         // ; * vertical blank set/clear       \-  immediate
#define WSYNC 	0x02         // ; * wait for horizontal blank      /-  action
#define RSYNC 	0x03         // ; * reset horizontal sync counter /-

#define NUSIZ0 	0x04         // ;  missile/player size controls
#define NUSIZ1 	0x05 
#define COLUP0 	0x06         // ;  colors
#define COLUP1 	0x07 
#define COLUPF 	0x08 
#define COLUBK 	0x09 
#define CTRLPF 	0x0A         // ;  REF, SCORE, PFP, ball width
#define REFP0 	0x0B         // ;  reflect player
#define REFP1 	0x0C 
#define PF0 	0x0D         // ;  playfield bits
#define PF1 	0x0E 
#define PF2 	0x0F 
#define RESP0 	0x10         // ;  horizonal position
#define RESP1 	0x11 
#define RESM0 	0x12 
#define RESM1 	0x13 
#define RESBL 	0x14 

#define AUDC0 	0x15         // ; * audio control
#define AUDC1 	0x16         // ; *
#define AUDF0 	0x17         // ; * audio frequency
#define AUDF1 	0x18         // ; *
#define AUDV0 	0x19         // ; * audio volume
// #define AUDV1 	0x1A         // ; * <-- defined in c_tiasnd.c

#define GRP0 	0x1B         // ;  graphics
#define GRP1 	0x1C 
#define ENAM0 	0x1D         // ;  enables
#define ENAM1 	0x1E 
#define ENABL 	0x1F 
#define HMP0 	0x20         // ;  horizontal motion
#define HMP1 	0x21 
#define HMM0 	0x22 
#define HMM1 	0x23 
#define HMBL 	0x24 
#define VDELP0 	0x25         // ;  vertical delay
#define VDELP1 	0x26 
#define VDELBL 	0x27 
#define RESMP0 	0x28         // ;  missile locked to player
#define RESMP1 	0x29 

#define HMOVE 	0x2A         // ;  apply horizontal motion
#define HMCLR 	0x2B         // ;  clear horizontal motion registers
#define CXCLR 	0x2C		 // ;  clear collision latches

// ;*
// ;* to make macros easier to write
// ;*

#define NUSIZM0 NUSIZ0 
#define NUSIZM1 NUSIZ1 
#define NUSIZP0 NUSIZ0 
#define NUSIZP1 NUSIZ1 



// ;*
// ;* TIA bit mask definitions
// ;*

#define REF 	0x01         // ;  (CTRLPF) reflect playfield
#define SCORE 	0x02         // ;  (CTRLPF) score mode
#define PFP 	0x04  	     // ;  (CTRLPF) playfield gets priority


// ;*
// ;* pixel  bit definitions
// ;*

#define PF_BIT 	1 
#define BL_BIT 	2 
#define P1_BIT 	4 
#define M1_BIT 	8 
#define P0_BIT 	16 
#define M0_BIT 	32 
#define DL_BIT 	64 


// ; some display related stuff

#define DEFAULT_CFirst	39 
#define MEDIUM_Offset 	19   	// ;  offset a medium game this much
                        		// ;  tune so that game in mode 3 and mode 5
                        		// ;  appear at same vertical position
                        		// ;  (unless it's a known tall game)

#define MAX_TallGame 	240		// ;  size of a tall game

/* z_tiavars.c
 ***********************************************************************/
 
extern dd Timer;

extern db  Starpath;
extern db  KoolAide;
extern db  RSBoxing;

extern dd TopLine;
extern dd BottomLine;

void CatchUpPixels(void);
void ShowAdjusted(void);
void QueueSoundBytes(void);
void do_Instruction(void);

extern dd TIACollide;
extern db LooseColour;			// ;  and this with pixels to turn
                                // ;  frame gray *EST*
                                // ;  see tiawrite.asm
extern db TIA_VBLANK;

extern dd eax, ebx, ecx, edx, esi, edi;	// some temporary variables
extern db al, ah, bl, bh, cl, ch, dl, dh;

extern int sebx, sedx;		// signed "registers"

extern db t_graphics;					// graphics save for DoNUSIZ
extern dd t_ebx;						// for HandleMotion
extern int t_motion;					// for HandleMotion
extern db t_data;						// temp for DataBus
extern dd playfield_bits;				// all of the PF regs

/* Masks (used for disabling graphics objects) */
extern db p0_mask;
extern db p1_mask;
extern db m0_mask;
extern db m1_mask;
extern db bl_mask;
extern db pf_mask;

// ;*
// ;* private color registers
// ;*
// ;* we use old normal translation table to index into these (TIADisplayToColour)
// ;* update registers to handle SCORE and PFP
// ;*

extern dd BK_Color;        // ; \-
extern dd PF_Color;        // ;  \-  these no longer need to be kept in order
extern dd P1_Color;        // ;  /-  
extern dd P0_Color;        // ; /-
extern dd BL_Color;        // ; ball color

extern dd* ColorTab[];

extern int RenderingHBLANK;

extern int ActiveObjects;
extern int CosmicScanLine;
extern int HBlanking;
extern int SetHBlanking;
extern int Invisible;
extern int HMOVE_Pending;
extern int HMOVE_Cycle;       // ; remember cycle of last HMOVE
extern int Last_HMOVE_Cycle;
extern int M0_Confused;

extern db CosmicGraphicsTable[] = { 0x40,0x40,0xc0,0 };


// ;*
// ;* sprite related stuff
// ;*

#define DefineObjectVars($1) \
extern db* $1##_Table; \
extern int $1##_Position; \
extern int $1##_Size;  \
extern int $1##_Motion; \
extern int $1##_Graphics; \
extern int $1##_Delayed; \
extern int $1##_TripleFlag; \

#define InitObjectVars($1) \
	$1##_Table       = $1##_Sprite; \
	$1##_Position    = 228-68; \
	$1##_Size        = sizeofsprite($1##_Sprite);  \
	$1##_Motion      = 0; \
	$1##_Graphics    = 0; \
	$1##_Delayed     = 0; \
	$1##_TripleFlag  = 0; \


DefineObjectVars(BL)
DefineObjectVars(M0)
DefineObjectVars(M1)
DefineObjectVars(P0)
DefineObjectVars(P1)

/* c_tiasnd.c
 *********************************************************************/
 
/* CONSTANT DEFINITIONS */

/* definitions for AUDCx (15, 16) */
#define SET_TO_1     0x00      /* 0000 */
#define POLY4        0x01      /* 0001 */
#define DIV31_POLY4  0x02      /* 0010 */
#define POLY5_POLY4  0x03      /* 0011 */
#define PURE1        0x04      /* 0100 */
#define PURE2        0x05      /* 0101 */
#define DIV31_PURE   0x06      /* 0110 */
#define POLY5_2      0x07      /* 0111 */
#define POLY9        0x08      /* 1000 */
#define POLY5        0x09      /* 1001 */
#define DIV31_POLY5  0x0a      /* 1010 */
#define POLY5_POLY5  0x0b      /* 1011 */
#define DIV3_PURE    0x0c      /* 1100 */
#define DIV3_PURE2   0x0d      /* 1101 */
#define DIV93_PURE   0x0e      /* 1110 */
#define DIV3_POLY5   0x0f      /* 1111 */
                 
#define DIV3_MASK    0x0c                 
                 
#define AUDC0        0x15
#define AUDC1        0x16
#define AUDF0        0x17
#define AUDF1        0x18
#define AUDV0        0x19
#define AUDV1        0x1a

/* the size (in entries) of the 4 polynomial tables */
#define POLY4_SIZE  0x000f
#define POLY5_SIZE  0x001f
#define POLY9_SIZE  0x01ff

/* channel definitions */
#define CHAN1       0
#define CHAN2       1

#define FALSE       0
#define TRUE        1

extern int TSB_result;

extern db Bit4[15];
extern db Bit5[31];

/*
 1 = toggle output in 5 bit poly - used when poly5 clocks other outputs
*/

extern db Bit5T[31];

/*
 The 'Div by 31' counter is treated as another polynomial because of
 the way it operates.  It does not have a 50% duty cycle, but instead
 has a 13:18 ratio (of course, 13+18 = 31).  This could also be
 implemented by using counters.
*/

extern db Div31[31];

extern db Div6[6];

/*
 The sample output is treated as another divide by N counter.
 For better accuracy, the Samp_n_cnt has a fixed binary decimal point
 which has 8 binary digits to the right of the decimal point.
*/

extern dd Samp_n_cnt;
extern dd Samp_n_max;


extern db D6[2];
extern db P4[2];
extern db P5[2];
extern db AUDC[2];
extern db AUDF[2];
extern db AUDV[2];
extern db Outvol[2];
extern db Div_n_cnt[2];
extern db Div_n_max[2];

extern dd P9_sreg[2];
extern dd prev_sample;
extern db next_sample;

void TSB_Always_High(int chan);
void TSB_Poly4(int chan);
void TSB_Div31_Poly4(int chan);
void TSB_Poly5_Poly4(int chan);
void TSB_Pure(int chan);
void TSB_Div31_Pure(int chan);
void TSB_Poly5(int chan);
void TSB_Poly9(int chan);
void TSB_Poly5_Poly5(int chan);
void TSB_Div6_Pure(int chan);
void TSB_Div31_Div6(int chan);
void TSB_Poly5_Div6(int chan);

void Init_Tiasnd();
void UTS_Chan(int chan);
void H_AUDC0();
void H_AUDC1();
void H_AUDF0();
void H_AUDF1();
void H_AUDV0();
void H_AUDV1();
void TSB_ProcessChannel(int chan);
void TSB_Ch0done();
dd TIA_Sound_Byte();

/* z_tiatab.c
 ************************************************************************/
 
dd PFClockToBitReversed[] = {
    1 << 4,     // 0     forward
    1 << 5,     // 1
    1 << 6,     // 2
    1 << 7,     // 3
    1 << 15,    // 4
    1 << 14,    // 5
    1 << 13,    // 6
    1 << 12,    // 7
    1 << 11,    // 8
    1 << 10,    // 9
    1 << 9,     // 10
    1 << 8,     // 11
    1 << 16,    // 12
    1 << 17,    // 13
    1 << 18,    // 14
    1 << 19,    // 15
    1 << 20,    // 16
    1 << 21,    // 17
    1 << 22,    // 18
    1 << 23,    // 19
 
    1 << 23,    // 20    reversed
    1 << 22,    // 21
    1 << 21,    // 22
    1 << 20,    // 23
    1 << 19,    // 24
    1 << 18,    // 25
    1 << 17,    // 26
    1 << 16,    // 27
    1 << 8,     // 28
    1 << 9,     // 29
    1 << 10,    // 30
    1 << 11,    // 31
    1 << 12,    // 32
    1 << 13,    // 33
    1 << 14,    // 34
    1 << 15,    // 35
    1 << 7,     // 36
    1 << 6,     // 37
    1 << 5,     // 38
    1 << 4      // 39
};

dd PFClockToBitForward[] = {
    1 << 4,     // 0     forward
    1 << 5,     // 1
    1 << 6,     // 2
    1 << 7,     // 3
    1 << 15,    // 4
    1 << 14,    // 5
    1 << 13,    // 6
    1 << 12,    // 7
    1 << 11,    // 8
    1 << 10,    // 9
    1 << 9,     // 10
    1 << 8,     // 11
    1 << 16,    // 12
    1 << 17,    // 13
    1 << 18,    // 14
    1 << 19,    // 15
    1 << 20,    // 16
    1 << 21,    // 17
    1 << 22,    // 18
    1 << 23,    // 19
 
    1 << 4,     // 0     forward
    1 << 5,     // 1
    1 << 6,     // 2
    1 << 7,     // 3
    1 << 15,    // 4
    1 << 14,    // 5
    1 << 13,    // 6
    1 << 12,    // 7
    1 << 11,    // 8
    1 << 10,    // 9
    1 << 9,     // 10
    1 << 8,     // 11
    1 << 16,    // 12
    1 << 17,    // 13
    1 << 18,    // 14
    1 << 19,    // 15
    1 << 20,    // 16
    1 << 21,    // 17
    1 << 22,    // 18
    1 << 23     // 19
};


//
// pixel to collision word
//
dw TIAColTab[] = {
    0,          // 0
    0,          // 0
    0,          // 0
    8192,       // 2000
    0,          // 0
    128,        // 80
    64,         // 40
    8384,       // 20c0
    0,          // 0
    2048,       // 800
    1024,       // 400
    11264,      // 2c00
    4,          // 4
    2180,       // 884
    1092,       // 444
    11460,      // 2cc4
    0,          // 0
    32,         // 20
    16,         // 10
    8240,       // 2030
    32768,      // 8000
    32928,      // 80a0
    32848,      // 8050
    41200,      // a0f0
    8,          // 8
    2088,       // 828
    1048,       // 418
    11320,      // 2c38
    32780,      // 800c
    34988,      // 88ac
    33884,      // 845c
    44284,      // acfc
    0,          // 0
    512,        // 200
    256,        // 100
    8960,       // 2300
    2,          // 2
    642,        // 282
    322,        // 142
    9154,       // 23c2
    16384,      // 4000
    18944,      // 4a00
    17664,      // 4500
    28416,      // 6f00
    16390,      // 4006
    19078,      // 4a86
    17734,      // 4546
    28614,      // 6fc6
    1,          // 1
    545,        // 221
    273,        // 111
    9009,       // 2331
    32771,      // 8003
    33443,      // 82a3
    33107,      // 8153
    41971,      // a3f3
    16393,      // 4009
    18985,      // 4a29
    17689,      // 4519
    28473,      // 6f39
    49167,      // c00f
    51887,      // caaf
    50527,      // c55f
    61439       // efff
};

db TIADisplayToColour[] = {
    0,1,4,4,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
};

db TIADisplayToColour2[] = {
    0,1,4,1,2,1,4,1,2,1,4,1,2,1,4,1,
    3,1,4,1,3,1,4,1,3,1,4,1,3,1,4,1,
    3,1,4,1,3,1,4,1,3,1,4,1,3,1,4,1,
    3,1,4,1,3,1,4,1,3,1,4,1,3,1,4,1
};

dd* PF_Table = PFClockToBitForward;         // current pointer to playfield mask bits
db* PixelColorTable = TIADisplayToColour;   // pixel to color translation table

// ;*
// ;* sprite definitions
// ;*
// ;* The top bit of the delay byte is the triple flag.
// ;* This means subsequent RESPx outputs the leading sprite.
// ;*
// ;* Note:  There is an entry just before the first entry of a single-width 
// ;*        player sprite that isn't there, that should have the triple flag
// ;*        set.  That is there should be four entries with the triple flag set.
// ;* 

#define LeadingPlayer \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      10, \
    0,      9, \
    0x80,   8, \
    0x40,   7, \
    0x20,   6, \
    0x10,   5, \
    0x8,    4, \
    0x4,    3, \
    0x2,    2, \
    0x1,    1, \
    0,      0, \

#define BlankPlayer \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      10, \
    0,      9, \
    0,      8, \
    0,      7, \
    0,      6, \
    0,      5, \
    0,      4, \
    0,      3, \
    0,      2, \
    0,      1, \
    0,      0, \

#define TrailingPlayer \
    0,      0, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      10, \
    0,      9, \
    0x80,   8, \
    0x40,   7, \
    0x20,   6, \
    0x10,   5, \
    0x8,    4, \
    0x4,    3, \
    0x2,    2, \
    0x1,    1, \
    0,      0, \

#define LeadingMissile \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0, \
    0x80,   8, \
    0x40,   7, \
    0x20,   6, \
    0x10,   5, \
    0x8,    4, \
    0x4,    3, \
    0x2,    2, \
    0x1,    1, \
    0,      0, \

#define BlankMissile \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0, \
    0,      8, \
    0,      7, \
    0,      6, \
    0,      5, \
    0,      4, \
    0,      3, \
    0,      2, \
    0,      1, \
    0,      0, \

#define TrailingMissile \
    0,      0, \
    0,      0, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0, \
    0x80,   8, \
    0x40,   7, \
    0x20,   6, \
    0x10,   5, \
    0x8,    4, \
    0x4,    3, \
    0x2,    2, \
    0x1,    1, \
    0,      0, \

#define MediumGap \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

#define WideGap \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

// ;*
// ;* Medium and wide players have an extra leading clock.
// ;* They have the extra triple flag too.
// ;*

#define MediumPlayer \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      18, \
    0,      17, \
    0x80,   16, \
    0x80,   15, \
    0x40,   14, \
    0x40,   13, \
    0x20,   12, \
    0x20,   11, \
    0x10,   10, \
    0x10,   9, \
    0x8,    8, \
    0x8,    7, \
    0x4,    6, \
    0x4,    5, \
    0x2,    4, \
    0x2,    3, \
    0x1,    2, \
    0x1,    1, \
    0,      0, \

#define MediumBlankPlayer \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      18, \
    0,      17, \
    0,      16, \
    0,      15, \
    0,      14, \
    0,      13, \
    0,      12, \
    0,      11, \
    0,      10, \
    0,      9, \
    0,      8, \
    0,      7, \
    0,      6, \
    0,      5, \
    0,      4, \
    0,      3, \
    0,      2, \
    0,      1, \
    0,      0, \

#define WidePlayer \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      34, \
    0,      33, \
    0x80,   32, \
    0x80,   31, \
    0x80,   30, \
    0x80,   29, \
    0x40,   28, \
    0x40,   27, \
    0x40,   26, \
    0x40,   25, \
    0x20,   24, \
    0x20,   23, \
    0x20,   22, \
    0x20,   21, \
    0x10,   20, \
    0x10,   19, \
    0x10,   18, \
    0x10,   17, \
    0x8,    16, \
    0x8,    15, \
    0x8,    14, \
    0x8,    13, \
    0x4,    12, \
    0x4,    11, \
    0x4,    10, \
    0x4,    9, \
    0x2,    8, \
    0x2,    7, \
    0x2,    6, \
    0x2,    5, \
    0x1,    4, \
    0x1,    3, \
    0x1,    2, \
    0x1,    1, \
    0,      0, \

#define WideBlankPlayer \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      0x80, \
    0,      34, \
    0,      33, \
    0,      32, \
    0,      31, \
    0,      30, \
    0,      29, \
    0,      28, \
    0,      27, \
    0,      26, \
    0,      25, \
    0,      24, \
    0,      23, \
    0,      22, \
    0,      21, \
    0,      20, \
    0,      19, \
    0,      18, \
    0,      17, \
    0,      16, \
    0,      15, \
    0,      14, \
    0,      13, \
    0,      12, \
    0,      11, \
    0,      10, \
    0,      9, \
    0,      8, \
    0,      7, \
    0,      6, \
    0,      5, \
    0,      4, \
    0,      3, \
    0,      2, \
    0,      1, \
    0,      0, \

// ;*
// ;* sprite size calculation macro
// ;*

// %macro SpriteSize 1
// %1_Size equ ($-%1)/2-1 
// %endmacro

#define sizeofsprite($1) (sizeof($1)/2 - 1)

//
// default sprite definitions
//

// P_0 -- Player NUSIZ = 0

extern db P_0[1]; fuck fuck fuck these aren't right. the small counts i got here, those were all names of giant macros

// M_0 -- Missile NUSIZ = 0

extern db M_0[1];

// P_1 -- Player NUSIZ = 1

extern db P_1[2];

// M_1 -- Missile NUSIZ = 1

extern db M_1[2];

// P_2 -- Player NUSIZ = 2

extern db P_2[3];

// M_2 -- Missile NUSIZ = 2

extern db M_2[3];

// P_3 -- Player NUSIZ = 3

extern db P_3[3];

// M_3 -- Missile NUSIZ = 3

extern db M_3[3];

// P_4 -- Player NUSIZ = 4

extern db P_4[3];

// M_4 -- Missile NUSIZ = 4

extern db M_4[3];

// P_5 -- Player NUSIZ = 5

extern db P_5[1];

// M_5 -- Missile NUSIZ = 5

extern db M_5[1];

// P_6 -- Player NUSIZ = 6

extern db P_6[5];

// M_6 -- Missile NUSIZ = 6

extern db M_6[5];

// P_7 -- Player NUSIZ = 7

extern db P_7[1];

// M_7 -- Missile NUSIZ = 7

extern db M_7[1];


//
// multi sprite definitions
//

// MP_0 -- Player NUSIZ = 0

extern db MP_0[1];

// MM_0 -- Missile NUSIZ = 0

extern db MM_0[1];

// MP_1 -- Player NUSIZ = 1

extern db MP_1[2];

// MM_1 -- Missile NUSIZ = 1

extern db MM_1[2];
 
// MP_2 -- Player NUSIZ = 2

extern db MP_2[3];

// MM_2 -- Missile NUSIZ = 2

extern db MM_2[3];
 
// MP_3 -- Player NUSIZ = 3

extern db MP_3[3];
 
// MM_3 -- Missile NUSIZ = 3

extern db MM_3[3];
 
// MP_4 -- Player NUSIZ = 4

extern db MP_4[3];

// MM_4 -- Missile NUSIZ = 4

extern db MM_4[3];

// MP_5 -- Player NUSIZ = 5

extern db MP_5[1];

// MM_5 -- Missile NUSIZ = 5

extern db MM_5[1];

// MP_6 -- Player NUSIZ = 6

extern db MP_6[5];

// MM_6 - Missile NUSIZ = 6

extern db MM_6[5];
 
// MP_7 -- Player NUSIZ = 7

extern db MP_7[1];

// MM_7 -- Missile NUSIZ = 7

extern db MM_7[1];

//
// regular sprite tables
//
 
extern db* P0_RegularTable[8];
extern db** P1_RegularTable;

extern db P0_SizeTable[8];
extern db* P1_SizeTable;

extern db* M0_RegularTable[8];
extern db** M1_RegularTable;

extern db M0_SizeTable[8];
extern db* M1_SizeTable;

//
// multiple sprite tables
//
// note: originally there were no size tables for these
//       maybe it's a good idea to use sizes that match the tables...
//

extern db* P0_MultipleTable[8];
extern db** P1_MultipleTable;

extern db P0_MultipleSizeTable[8];
extern db* P1_MultipleSizeTable;

extern db* M0_MultipleTable[8];
extern db** M1_MultipleTable;

extern db M0_MultipleSizeTable[8];
extern db* M1_MultipleSizeTable;

extern db BL_Sprite[1];
extern db M0_Sprite[1];
extern db M1_Sprite[1];
extern db P0_Sprite[1];
extern db P1_Sprite[1];
 
extern int DeepHMOVE[8192];
extern dd WeirdRespCorrection[26];
extern db PFDelay[4];
extern db BallSize[4];
extern db TIAReflect8[256];
extern dd MissileOffset[8];
extern dd MaxMotion[21];
extern dd HiTable[20];

/* z_tialine.c
 *****************************************************************************/
 
void nTIALineTo(void);

/* c_init.c
 ***************************************************************************/
 
void InitData();

/* carts.c
 **************************************************************************/
 
void RecognizeCart(void);

/* c_banks.c
 * Tons of other symbols here, not sure if they'll be used externally.
 ************************************************************************/
 
void SetupBanks();

/* c_cpu.c
 *************************************************************************/
 
void Init_CPU(void);
void Reset(void);
void do_Instruction(void);

/* sdlsrv.c -- SDL? We'll probably want to drop this.
 *********************************************************************/
 
extern SDL_Joystick *JoystickTable[16];

extern SDL_Surface *srv_screen;
extern SDL_Surface *small_screen;
extern SDL_Surface *large_screen;
extern SDL_Surface *tiny_screen;

extern db *srv_buffer;
extern dd srv_pitch;

extern dd tex_width, tex_height;	/* dimensions of current window or video display in pixels */

extern db *screen_pixels;      /* pointer to display screen */
extern db *emu_pixels;         /* pointer to current emulator buffer */
extern db *emu_pixels_prev;    /* pointer to previous emulator buffer */

extern db screen_buffer_count;
extern db srv_done;

extern dd srv_colortab_hi[128];		/* for mapping atari 8-bit colors to 32-bit colors */
extern dd srv_colortab_med[128];		/* for soft scanlines */
extern dd srv_colortab_low[128];

void Init_SDL();
void Init_Service();
void srv_Events();
void srv_Cleanup();

/* sdlvideo.c -- ''
 ********************************************************************/
 
void ClearScreenBuffers();
void CreateScreen();
void srv_SetPalette();
void srv_InitJoystick();
void srv_ChooseScreenResolution();
void srv_set_screen_size(void);
void srv_set_fullscreen(void);
void srv_SetVideoMode();
void srv_CopyScreen();
void gui_CopyScreen();

/* sdlsound.c -- ''
 *************************************************************************/
 
extern dd SQ_resample;	/* a counter for resampling sound to 44100 Hz */

extern db SQ_byte;		/* byte to put in the sound queue */
extern db *SQ_Input;	/* pointer to next available byte for storing */
extern db *SQ_Output;	/* pointer to next available byte for fetching */
extern db *SQ_Top;		/* pointer to the top of the queue */

void Init_SoundQ();
int SQ_Count();
void SQ_Store();
void srv_sound_on();
void srv_sound_off();

/* sdlopengl.c -- ''
 ****************************************************************/
 
extern dd texture_buffer[1280*1280];

void gl_InitOpenGL();

/* guiutil.c -- gui is hopefully going the way of sdl?
 *********************************************************************/
 
/* gui_action is an abstraction layer between SDL_Event and gui code.
	This allows both gui() and file_selector() (and later on whatever
	else we have) to use the same set of key/joystick bindings. See
	gui_navigate(). If we ever allow gui key remapping, gui_navigate()
	is where the map will be applied. */
	
typedef enum {
	GUI_NO_ACTION,
	GUI_UP,
	GUI_DOWN,
	GUI_PAGEUP,
	GUI_PAGEDOWN,
	GUI_HOME,
	GUI_END,
	GUI_ACTIVATE_PRI,
	GUI_ACTIVATE_SEC,
	GUI_RANDOM,
	GUI_EXIT_GUI
} gui_action;

/* gui_entry is a struct that represents a GUI menu option.

	label is the text, which must be less than GUI_WIDTH chars. It may
	contain one %s, which will be replaced by the data (uses sprintf())

	data is NULL for a menu item with no options (like `Resume Emulator'),
	or else the string value of the currently-selected option (such as
	"Yes" or "No" for e.g. `Show FPS' option). If you like, you can think
	of an item with options as a drop-down or radio list, and an item
	with no options as a button.

	Handlers are callbacks that actually perform the menu functions.
	They are declared to return void and accept no parameters.

	Each gui_entry has one primary handler (or NULL if no handler is
	desired) which will get called whenever the user presses (Enter,
	space, rightarrow, joybutton0, or joystick-right) and optionally
	a secondary handler which will get called when the user presses
	leftarrow, joybutton1, or joystick-left. If no secondary handler
	is desired, set sec_handler to NULL.

	It is possible to define a gui_entry with a NULL primary handler and
	an actual callable sec_handler. Please don't do this, it will only
	make the GUI confusing and serves no useful purpose.

	handlers must never assume that they're the only ones that can change
	a value: there are still keystrokes that can change values while the
	emulator is running (e.g. alt-enter for fullscreen/window).
*/
typedef struct {
	char *label;
	char *data;
	int disabled;
	void (*handler)();
	void (*sec_handler)();
} gui_entry;

void set_yesno_string(char *str, int yesno);
gui_action gui_navigation(SDL_Event *evp);
int set_gui_height(gui_entry *menu);
int gui_handle_action(int action, int *ptr_exit_gui, gui_entry *current_gui_items, int gui_current);
void draw_gui(gui_entry *entries, int cur);

/* guifilelist.c
 ***********************************************************************/
 
typedef int SDLKey;

#define MAX_ENTS 20000
#define MAX_LEN 256
#define FT_DIRS 0
#define FT_FILES 1
 
extern dd match;
extern SDLKey matched[100];
extern int window_line;
extern int curfile;
extern int first_filelist;
extern char *file_list[MAX_ENTS+1];

void directory_failed();
void get_list(int file_type);
void get_file_list();
int draw_file_list(int cur, int window_line, int ymax);
int find_next_rom(int curfile, SDLKey k);
int file_selector(char *result);

/* controls.c
 *********************************************************************/
 
void Exit_Game(void);
void SwapPorts();
void Controls(void);

/* kidvid.c
 ********************************************************************/
 
extern FILE *SampleFile, *SharedSampleFile;
extern db KidvidSampleByte;
extern int KidvidFileOpened;
extern dd kv_TapeBusy;
extern int kv_FilePointer, kv_SharedData, kv_Beep;
extern unsigned long int kv_SongCounter;
extern unsigned char SongPositions[44+38+42+62+80+62];
extern unsigned long int SongStart[104];
 
void kv_OpenSampleFile(void);
void kv_CloseSampleFile(void);
void kv_SetNextSong(void);
void kv_GetNextSampleByte(void);

/* palette.c
 *****************************************************************/
 
extern double brightness;
extern double warmth;

extern double NTSC_PS;	// tunable phase shift
			// 25.714 -- chroma1   == chroma15
			// 26.572 -- chroma1.5 == chroma15 (perfect split)
			// 27.429 -- chroma2   == chroma15

			// 26.2 -- vintage z26 approximately
			// 26.2 -- mame 0.212 approximately
			// 26.7 -- Sunnyvale 4-switch (20190820)

extern double PAL_PS;	// tunable phase shift
			// 21.68859 -- middle of the range
			// 21.50    -- vintage z26 approximately
			// 21.50	-- mame 0.212 approximately

extern double NTSC_luma[];
extern double PAL_luma[];

// phase corrections since the 2600 doesn't generate perfect sine waves
// Set corrections for chroma1 and chromaF to be the same to get
// phase shift numbers to match mame numbers.
extern double NTSC_corr[];

extern db NTSC_RGB_table[];
extern db PAL_RGB_table[];
extern db SECAM_RGB_table[];

void NTSC_Palette();
void PAL_Palette();
db set_luma(int luma, int val);
void vintage_NTSC_Palette();
void vintage_PAL_Palette();
void SECAM_Palette();
void mame_NTSC_Palette(void);
void GeneratePalette();

/* vcs_slot.c
 ******************************************************************/
 
// stripped down and converted to C for z26
// Aug 7, 2019

// z26 compatible cart type definitions

#define Z26_2K	0		// (by size)
#define Z26_4K	0		// (by size)
#define Z26_CV	1		// (signature mame)
#define Z26_F8S 2		// 8K superchip
#define Z26_E0	3		// (signature mame)
#define Z26_3F	4		// (signature mame)
#define Z26_FE	5		// (signature mame)
#define Z26_F6S 6		// 16K superchip
#define Z26_E7	7		// (signature Stella)
#define Z26_F4S 8		// 32K superchip
#define Z26_F8SW 9		// (signature mame) aka F8R -- 8K swapped
#define Z26_CM	10		// (CRC) Compumate
#define Z26_3FP 11		// 3F+ -- 512K tigervision (same as 3F in z26)
#define Z26_UA	12		// (signature mame)
#define Z26_EF	13		// (* undetected *) 64K Homestar Runner
#define Z26_3E	14		// (signature Stella)
#define Z26_AR	15		// Starpath
#define Z26_F6	16		// (signature mame)
#define Z26_F4	17		// (by size)
#define Z26_DC	18		// (signature mame) aka MB -- megaboy
#define Z26_FA	19		// (by size)
#define Z26_F8	20		// (by size)
#define Z26_DPC 21		// (by size) aka P2 -- pitfall 2
#define Z26_4A5 22		// (signature z26) supercat
#define Z26_084 23		// (* undetected *) EconoBanking

// these are in mame but not in z26

#define Z26_FV 0		// (signature mame)
#define Z26_JVP 0		// (signature mame)
#define Z26_32IN1 0
#define Z26_8IN1 0
#define Z26_4IN1 0
#define Z26_SS 0		// (signature mame)
#define Z26_X07 0
#define Z26_HARMONY 0

/* ct.c
 ****************************************************************/
 
dd	Player_1_hard[9];
dd	NTSC_colours[3];
dd	PAL_colours[5];
dd	Kool[5];
dd	Boxing[4];
dd	Paddle_0[73];
dd	Paddle_1[14];
dd	Paddle_3[2];
dd	MPdir_1[13];
dd	MPdir_2[3];
dd	MPdir_3[14];
dd	joy_rev[5];
dd	keypad_3[30];
dd	keypad_2[4];
dd	driving_con_2[2];
dd	driving_con_3[7];
dd      BLACKLIST[1];

#endif
