#include "emuhost.h"
#include "eh_inmgr.h"
#include <stdio.h>
#include <string.h>

/* z26 API.
 */
 
typedef uint8_t db;
typedef unsigned int dd;
 
void def_LoadDefaults(); // globals.c
void InitData(); // 
void Controls(); //
void Reset_emulator(); // 
void ScanFrame(); // 
extern db *ScreenBuffer; // globals.c
int z26_get_audio_buffer(void *dstpp); // c_soundq.c, my own addition
extern char FileName[260]; // globals.c
int cli_LoadROM(char *filename); // globals.c ; nonzero=success
extern char ROMLoaded; // cli.c
extern char ROMSeen; // cli.c
extern db PCXPalette[384]; // globals.c
extern int PaletteNumber; // globals.c
void GeneratePalette(); // palette.c
extern db IOPortA; // globals.c
extern db IOPortB; // globals.c
extern db InputLatch[2]; // globals.c
extern dd ChargeTrigger0[4]; // globals.c; 1..240
extern db IOPortA_UnusedBits; // globals.c

#define AKZ26_INPUT_JOYSTICKS 1
#define AKZ26_INPUT_PADDLES 2

static int pv_PaletteNumber=-1;
static uint16_t pv_p1=0,pv_p2=0;
static int diff_p1=0,diff_p2=0,color=0; // difficulty and color switch hold their values
static int akz26_paddlev[4]={0}; // 0..999; set at akz26_cb_input
static int akz26_input_devices=AKZ26_INPUT_JOYSTICKS;
static int reverse_joysticks=0;

/* Joysticks input.
 */
 
static void akz26_update_joysticks() {
  uint16_t p1,p2;
  if (reverse_joysticks) {
    p1=eh_input_get(2);
    p2=eh_input_get(1);
  } else {
    p1=eh_input_get(1);
    p2=eh_input_get(2);
  }
  
  IOPortA=0xff;
  IOPortB=0xff;
  InputLatch[0]=0x80;
  InputLatch[1]=0x80;
  
  /* Joysticks for 2 players.
   * SOUTH is the button.
   * AUX1 is Reset, either player.
   * AUX2 is Select, either player.
   */
  if (p1&EH_BTN_RIGHT) IOPortA&=~0x80;
  if (p1&EH_BTN_LEFT)  IOPortA&=~0x40;
  if (p1&EH_BTN_DOWN)  IOPortA&=~0x20;
  if (p1&EH_BTN_UP)    IOPortA&=~0x10;
  if (p1&EH_BTN_SOUTH) InputLatch[0]=0;
  if (p1&EH_BTN_AUX1)  IOPortB&=~0x01; // RESET
  if (p1&EH_BTN_AUX2)  IOPortB&=~0x02; // SELECT
  
  if (p2&EH_BTN_RIGHT) IOPortA&=~0x08;
  if (p2&EH_BTN_LEFT)  IOPortA&=~0x04;
  if (p2&EH_BTN_DOWN)  IOPortA&=~0x02;
  if (p2&EH_BTN_UP)    IOPortA&=~0x01;
  if (p2&EH_BTN_SOUTH) InputLatch[1]=0;
  if (p2&EH_BTN_AUX1)  IOPortB&=~0x01; // RESET
  if (p2&EH_BTN_AUX2)  IOPortB&=~0x02; // SELECT
  
  /* Color and difficulty switches hold their state on a physical console.
   * We simulate that with a toggle button, and hold the state here.
   *  - Color: NORTH, either player.
   *  - Left difficulty: EAST, player 1.
   *  - Right difficulty: EAST, player 2.
   */
  if (p1!=pv_p1) {
    if ((p1&EH_BTN_NORTH)&&!(pv_p1&EH_BTN_NORTH)) color=color?0:1;
    if ((p1&EH_BTN_EAST)&&!(pv_p1&EH_BTN_EAST)) diff_p1=diff_p1?0:1;
    pv_p1=p1;
  }
  if (p2!=pv_p2) {
    if ((p2&EH_BTN_NORTH)&&!(pv_p2&EH_BTN_NORTH)) color=color?0:1;
    if ((p2&EH_BTN_EAST)&&!(pv_p2&EH_BTN_EAST)) diff_p2=diff_p2?0:1;
    pv_p2=p2;
  }
  
  if (diff_p1) IOPortB&=~0x40;
  if (diff_p2) IOPortB&=~0x80;
  if (color) IOPortB&=~0x08;
  
  IOPortA_UnusedBits=IOPortA_UnusedBits&0x0f;
  IOPortA_UnusedBits=IOPortA_UnusedBits&0xf0;
}

/* Paddles input.
 */
 
static void akz26_fake_paddle(int padp,uint16_t state) {
  const int speed=15;
  switch (state&(EH_BTN_LEFT|EH_BTN_RIGHT)) {
    case EH_BTN_LEFT: if ((akz26_paddlev[padp]-=speed)<0) akz26_paddlev[padp]+=1000; break;
    case EH_BTN_RIGHT: if ((akz26_paddlev[padp]+=speed)>999) akz26_paddlev[padp]-=1000; break;
  }
}
 
static void akz26_update_paddles() {
  uint16_t p1=eh_input_get(1);
  uint16_t p2=eh_input_get(2);
  uint16_t p3=eh_input_get(3);
  uint16_t p4=eh_input_get(4);
  
  // We'll read from Atari joysticks, but also use the generic dpad.
  akz26_fake_paddle(0,p1);
  akz26_fake_paddle(1,p2);
  akz26_fake_paddle(2,p3);
  akz26_fake_paddle(3,p4);
  
  IOPortA=0xcc;
  IOPortB=0xff;
  if (p1&EH_BTN_WEST) IOPortA&=~0x80;
  if (p1&EH_BTN_SOUTH) IOPortA&=~0x40;
  // TODO The above agrees with Bumper Bash. Not sure where the player 2..4 buttons are supposed to go.
  //if (p1&EH_BTN_WEST) IOPortA&=~0x20;
  //if (p2&EH_BTN_WEST) IOPortA&=~0x10;
  if (p2&EH_BTN_WEST) IOPortA&=~0x08;
  if (p2&EH_BTN_SOUTH) IOPortA&=~0x04;
  ChargeTrigger0[0]=1+((999-akz26_paddlev[0])*239)/999;
  ChargeTrigger0[1]=1+((999-akz26_paddlev[1])*239)/999;
  //ChargeTrigger0[2]=1+((999-akz26_paddlev[2])*239)/999;
  //ChargeTrigger0[3]=1+((999-akz26_paddlev[3])*239)/999;
  ChargeTrigger0[2]=ChargeTrigger0[0];
  ChargeTrigger0[3]=ChargeTrigger0[1];
  
  if (p1&EH_BTN_AUX1) IOPortB&=~0x01; // RESET
  if (p1&EH_BTN_AUX2) IOPortB&=~0x02; // SELECT
  if (p2&EH_BTN_AUX1) IOPortB&=~0x01; // RESET
  if (p2&EH_BTN_AUX2) IOPortB&=~0x02; // SELECT
  if (p3&EH_BTN_AUX1) IOPortB&=~0x01; // RESET
  if (p3&EH_BTN_AUX2) IOPortB&=~0x02; // SELECT
  if (p4&EH_BTN_AUX1) IOPortB&=~0x01; // RESET
  if (p4&EH_BTN_AUX2) IOPortB&=~0x02; // SELECT
  
  /* Color and difficulty switches hold their state on a physical console.
   * We simulate that with a toggle button, and hold the state here.
   *  - Color: NORTH, either player.
   *  - Left difficulty: EAST, player 1.
   *  - Right difficulty: EAST, player 2.
   */
  if (p1!=pv_p1) {
    if ((p1&EH_BTN_NORTH)&&!(pv_p1&EH_BTN_NORTH)) color=color?0:1;
    if ((p1&EH_BTN_EAST)&&!(pv_p1&EH_BTN_EAST)) diff_p1=diff_p1?0:1;
    pv_p1=p1;
  }
  if (p2!=pv_p2) {
    if ((p2&EH_BTN_NORTH)&&!(pv_p2&EH_BTN_NORTH)) color=color?0:1;
    if ((p2&EH_BTN_EAST)&&!(pv_p2&EH_BTN_EAST)) diff_p2=diff_p2?0:1;
    pv_p2=p2;
  }
  
  if (diff_p1) IOPortB&=~0x40;
  if (diff_p2) IOPortB&=~0x80;
  if (color) IOPortB&=~0x08;
  
  IOPortA_UnusedBits=IOPortA_UnusedBits&0x3f;
  IOPortA_UnusedBits=IOPortA_UnusedBits&0xf3;
}

/* Deliver input.
 */
 
static void akz26_update_input() {
  switch (akz26_input_devices) {
    case AKZ26_INPUT_JOYSTICKS: akz26_update_joysticks(); break;
    case AKZ26_INPUT_PADDLES: akz26_update_paddles(); break;
  }
}

/* Update.
 */
 
static int _akz26_update(int partial) {
  if (!partial) akz26_update_input();
  ScanFrame();
  if (PaletteNumber!=pv_PaletteNumber) {
    pv_PaletteNumber=PaletteNumber;
    GeneratePalette();
    eh_ctab_write(0,128,PCXPalette);
  }
  eh_video_write(ScreenBuffer);
  void *audio=0;
  int samplec=z26_get_audio_buffer(&audio);
  //log_audio(audio,samplec);
  eh_audio_write(audio,samplec);
  return 0;
}

/* Reset.
 */
 
static int _akz26_reset() {
  def_LoadDefaults();
  InitData();
  Reset_emulator();
  PaletteNumber=0;
  GeneratePalette();
  eh_ctab_write(0,128,PCXPalette);
  return 0;
}

/* Raw input event from emuhost.
 * This only gets called when we're in paddle mode.
 */
 
/* Emuhost srcdevid can be any positive integer.
 * But they start at 1 when the process launches, and only grow when devices connect.
 * So if we impose a limit, it spares us a lot of hassle, and i think won't ever be a problem.
 */
#define AKZ26_DEVID_LIMIT 256
static int akz26_playerid_by_devid[AKZ26_DEVID_LIMIT]={0};
 
static int akz26_cb_input(void *userdata,const struct eh_inmgr_event *event) {
  /**
  fprintf(stderr,
    "%s %d.%04x=%d[%04x] src=%d.%08x=%d\n",
    __func__,
    event->playerid,event->btnid,event->value,event->state,
    event->srcdevid,event->srcbtnid,event->srcvalue
  );
  /**/
  
  // Capture player/device mapping from other events; the paddle events won't have a playerid.
  if (event->playerid&&(event->srcdevid>0)&&(event->srcdevid<AKZ26_DEVID_LIMIT)) {
    akz26_playerid_by_devid[event->srcdevid]=event->playerid;
  }
  
  if ((event->srcbtnid==0x00030007)&&(event->srcvalue>=0)&&(event->srcvalue<=999)) {
    if ((event->srcdevid>0)&&(event->srcdevid<AKZ26_DEVID_LIMIT)) {
      int playerid=akz26_playerid_by_devid[event->srcdevid];
      if ((playerid>=1)&&(playerid<=4)) {
        akz26_paddlev[playerid-1]=event->srcvalue;
      }
    }
  }
  return 0;
}

/* Load game and start emulator.
 */
 
static int _akz26_load_file(const char *path) {
  //fprintf(stderr,"%s %s\n",__func__,path);
  def_LoadDefaults();
  
  /* cli.c */
  strncpy(FileName,path,255);
  ROMLoaded = cli_LoadROM(FileName);
  if (!ROMLoaded) return -1;
  ROMSeen = 1;
  
  Reset_emulator();
  GeneratePalette();
  eh_ctab_write(0,128,PCXPalette);
  
  /* Prepare paddle reception.
   * Emuhost doesn't do analogue input through its general API, but we can read input devices directly too.
   * I'm hard-coding the Atari Joystick, it's the only device we support for now.
   */
  if (akz26_input_devices==AKZ26_INPUT_PADDLES) {
    struct eh_inmgr *inmgr=eh_get_inmgr();
    if (inmgr) {
      struct eh_inmgr_delegate inmgr_delegate={
        .cb_event=akz26_cb_input,
      };
      eh_inmgr_listen_source(inmgr,0,&inmgr_delegate);
    }
  }
  
  return 0;
}

/* Configure.
 */
 
static int _akz26_configure(const char *k,int kc,const char *v,int vc,int vn) {
  //fprintf(stderr,"%s '%.*s' = '%.*s' (%d)\n",__func__,kc,k,vc,v,vn);
  if ((kc==7)&&!memcmp(k,"paddles",7)) {
    akz26_input_devices=AKZ26_INPUT_PADDLES;
    return 1;
  }
  if ((kc==17)&&!memcmp(k,"reverse-joysticks",17)) {
    reverse_joysticks=1;
    return 1;
  }
  return 0;
}

/* Main.
 */
 
int main(int argc,char **argv) {
  struct eh_delegate delegate={
    .name="akz26",
    .iconrgba=0,//TODO
    .iconw=0,
    .iconh=0,
    .video_width=320,
    .video_height=256,
    .video_format=EH_VIDEO_FORMAT_I8,
    .video_rate=60,
    .audio_rate=31400,
    .audio_chanc=1,
    .audio_format=EH_AUDIO_FORMAT_S8,
    .playerc=4,
    .configure=_akz26_configure,
    .load_file=_akz26_load_file,
    .load_serial=0,
    .update=_akz26_update,
    .generate_pcm=0,
    .reset=_akz26_reset,
  };
  return eh_main(argc,argv,&delegate);
}
