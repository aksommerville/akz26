/*
**	z26 sound stuff
*/

/*
 We convert 5 input samples to 7 output samples.

 31400 * 7/5 = 43960

 At 44100 Hz with an NTSC game that has 262 scanlines, the buffer should 
 underflow. So we should need to add some extra samples. This should allow sync 
 to monitor at 60 Hz to work for these games. Games with more scanlines should 
 overflow the buffer and slow down to actual game speed, unless you set the 
 monitor to 50 Hz. Then many PAL games should sync to monitor too. The PAL 
 version of Yars' Revenge seems to sync just fine at 50 Hz. It has 314 
 scanlines.

 It's a pretty tight ship, but otherwise the quadrun voice pitch is off.
 It seems to work on most modern platforms though.
*/

/* aks: Simpler buffering.
 * In theory we should be producing 523 samples per video frame.
 */
#define AUDIO_BUFFER_LIMIT 8192
static int8_t z26_audio_buffer[AUDIO_BUFFER_LIMIT];
static int8_t *z26_audio_buffer_next=z26_audio_buffer;
static int discard=0;

int z26_get_audio_buffer(void *dstpp) {
  *(void**)dstpp=z26_audio_buffer;
  int c=z26_audio_buffer_next-z26_audio_buffer;
  z26_audio_buffer_next=z26_audio_buffer;
  if (discard) {
    fprintf(stderr,"discard %d frames\n",discard);
    discard=0;
  }
  return c;
}

void QueueSoundByte() {
  uint8_t SQ_byte=0;
	if (KidvidFileOpened && KidVid)
	{
		kv_GetNextSampleByte();
		SQ_byte = (TIA_Sound_Byte() >> 1) + KidvidSampleByte - 0x80;
	}
	else
		SQ_byte = TIA_Sound_Byte() >> 1;
	if (z26_audio_buffer_next<z26_audio_buffer+AUDIO_BUFFER_LIMIT) {
   	  *z26_audio_buffer_next=SQ_byte;
 	  z26_audio_buffer_next++;
 	} else discard++;
}


/* 
 Put sound bytes into buffer.
 Called once per scan line.
*/

void QueueSoundBytes() {
	int i=2; // 523 per video frame, but this is called every scan line. With 2, it seems to work out.
	while (i-->0) QueueSoundByte();
}

void QueueEofSoundBytes() {//aks: Added this to run at the end of each video frame to make up the difference. Doesn't seem to help.
  int i=10;
  while (i-->0) QueueSoundByte();
}


/**
 z26 is Copyright 1997-2019 by John Saeger and contributors.  
 contributors.	 z26 is released subject to the terms and conditions of the 
 GNU General Public License Version 2 (GPL).  z26 comes with no warranty.
 Please see COPYING.TXT for details.
*/

