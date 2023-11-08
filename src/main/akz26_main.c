#include "emuhost.h"
#include <stdio.h>
#include <string.h>

/* z26 API.
 */
 
typedef uint8_t db;
 
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

static int pv_PaletteNumber=-1;
static uint16_t pv_p1=0,pv_p2=0;
static int diff_p1=0,diff_p2=0,color=0; // difficulty and color switch hold their values

/* Deliver input.
 */
 
static void akz26_update_input() {
  uint16_t p1=eh_input_get(1);
  uint16_t p2=eh_input_get(2);
  
  IOPortA=0xff;
  InputLatch[0]=0x80;
  InputLatch[1]=0x80;
  if (p1&EH_BTN_RIGHT) IOPortA&=~0x80;
  if (p1&EH_BTN_LEFT) IOPortA&=~0x40;
  if (p1&EH_BTN_DOWN) IOPortA&=~0x20;
  if (p1&EH_BTN_UP) IOPortA&=~0x10;
  if (p1&EH_BTN_SOUTH) InputLatch[0]=0;
  if (p2&EH_BTN_RIGHT) IOPortA&=~0x8;
  if (p2&EH_BTN_LEFT) IOPortA&=~0x04;
  if (p2&EH_BTN_DOWN) IOPortA&=~0x02;
  if (p2&EH_BTN_UP) IOPortA&=~0x01;
  if (p2&EH_BTN_SOUTH) InputLatch[1]=0;
  
  if (p1!=pv_p1) {
    if ((p1&EH_BTN_L2)&&!(pv_p1&EH_BTN_L2)) diff_p1=diff_p1?0:1;
    if ((p1&EH_BTN_R2)&&!(pv_p1&EH_BTN_R2)) color=color?0:1;
    pv_p1=p1;
  }
  if (p2!=pv_p2) {
    if ((p2&EH_BTN_L2)&&!(pv_p2&EH_BTN_L2)) diff_p2=diff_p2?0:1;
    pv_p2=p2;
  }
  
  IOPortB=0xff;
  if (p1&EH_BTN_L1) IOPortB&=~0x01; // RESET
  if (p1&EH_BTN_R1) IOPortB&=~0x02; // SELECT
  if (diff_p1) IOPortB&=~0x40;
  if (diff_p2) IOPortB&=~0x80;
  if (color) IOPortB&=~0x08;
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
  fprintf(stderr,"%s\n",__func__);
  def_LoadDefaults();
  InitData();
  Reset_emulator();
  PaletteNumber=0;
  GeneratePalette();
  eh_ctab_write(0,128,PCXPalette);
  return 0;
}

/* Load game and start emulator.
 */
 
static int _akz26_load_file(const char *path) {
  fprintf(stderr,"%s %s\n",__func__,path);
  def_LoadDefaults();
  
  /* cli.c */
  strncpy(FileName,path,255);
  ROMLoaded = cli_LoadROM(FileName);
  if (!ROMLoaded) return -1;
  ROMSeen = 1;
  
  Reset_emulator();
  GeneratePalette();
  eh_ctab_write(0,128,PCXPalette);
  
  return 0;
}

/* Configure.
 */
 
static int _akz26_configure(const char *k,int kc,const char *v,int vc,int vn) {
  fprintf(stderr,"%s '%.*s' = '%.*s' (%d)\n",__func__,kc,k,vc,v,vn);
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
    .video_format=EH_VIDEO_FORMAT_I8,//TODO confirm
    .video_rate=60,
    .audio_rate=31400,
    .audio_chanc=1,
    .audio_format=EH_AUDIO_FORMAT_S8,
    .playerc=2,
    .configure=_akz26_configure,
    .load_file=_akz26_load_file,
    .load_serial=0,
    .update=_akz26_update,
    .generate_pcm=0,
    .reset=_akz26_reset,
  };
  return eh_main(argc,argv,&delegate);
}
