/*2015 Library by Victor G. Perez heavily based on one from TMRh20 2012-2014

Contributors:

  ftp27 (GitHub) - setVolume(); function and code
  muessigb (GitHub) - metatata (ID3) support

*/

#ifndef TMRpcm_h   // if x.h hasn't been included yet...
#define TMRpcm_h   //   #define this so the compiler knows it has been included

/* Print debug messages to serial          */
//	#define debug
/****************** ADVANCED USER DEFINES ********************************
   See https://github.com/TMRh20/TMRpcm/wiki for info on usage           */

/* Use the SDFAT library from Greyman          */
	#define SDFAT

/* Enables 16-bit samples, which can be used for stereo playback, or to produce a
	   pseudo 16-bit output.                                                   */
	#define STEREO_OR_16BIT

//********************* Timer arrays and pointers **********************
//********** Enables use of different timers on different boards********

		#if !defined TMRpcm_buffSize
			#define TMRpcm_buffSize 1024
		#endif

//*** Options/Indicators from MSb to LSb: paused, qual, rampUp, 2-byte TMRpcm_samples, loop, loop2nd track, 16-bit ***

#define paused_bit	7
#define quality_bit 6
#define rampUp_bit	5
#define stereo_bit	4
#define loop_bit	3
#define loop2nd_bit	2
#define bps16_bit	1

#include <Arduino.h>

#include <HardwareTimer.h>
//#include <libmaple/systick.h>
#if !defined (SDFAT)
	#include <SD.h>
#else
	#include "SdFat/SdFat.h"
	#include <SdFatUtil.h>
#endif




 	//*** General Playback Functions and Vars ***
	extern void TMRpcm_play(char* filename);
	extern void TMRpcm_play(char* filename, unsigned long seekPoint);
	extern void TMRpcm_stopPlayback();
	extern void TMRpcm_volume(char vol);
	extern void TMRpcm_setVolume(char vol);
	extern void TMRpcm_disable();
	extern void TMRpcm_pause();
	extern void TMRpcm_quality(boolean q);
	extern void TMRpcm_loop(boolean set);
	extern void TMRpcm_systick_buffer_load(); //__attribute__ ((interrupt ("IRQ")));
	extern void TMRpcm_update_ccr(void); //__attribute__ ((interrupt ("IRQ")));
	extern void TMRpcm_setPins();
	extern void TMRpcm_timerSt();
	extern unsigned long TMRpcm_fPosition();
	extern byte TMRpcm_metaInfo(boolean infoType, char* filename, char* tagData, byte whichInfo);
	extern boolean TMRpcm_seek(unsigned long pos);
	extern boolean TMRpcm_isPlaying();
	extern boolean TMRpcm_ifOpen();
	extern void TMRpcm_buffer_load();
	void TMRpcm_DMAISR();
	void TMRpcm_DMASet();


	


	//*** Advanced usage Vars ***
	extern byte TMRpcm_listInfo(char* filename, char *tagData, byte infoNum);
	extern byte TMRpcm_id3Info(char* filename, char *tagData, byte infoNum);
	extern byte TMRpcm_getInfo(char* filename, char* tagData, byte infoNum);

	extern byte speakerPin, speakerPin2;
    extern uint8_t CSPin;
	extern uint16 resolution;
	extern uint8 repetition;
	extern byte lastSpeakPin;

	//uint8 timer_x;

	//*********** Standard Global Variables ***************
	extern volatile byte TMRpcm_optionByte; // See option flags defined above.
	extern volatile unsigned long TMRpcm_dataEnd;
	extern volatile boolean TMRpcm_buffEmpty[2], TMRpcm_playing;
	extern volatile byte a;
	extern volatile int16 bytes_left, bytes_read;
	extern volatile int16 bytes_filled;
	extern volatile byte TMRpcm_whichBuff;
	extern volatile uint8 TMRpcm_spk_ch1;
	extern volatile uint8 TMRpcm_spk_ch2;
	extern timer_dev * volatile TMRpcm_dev;
	extern volatile uint16 TMRpcm_sample, TMRpcm_sample2;

	extern volatile byte TMRpcm_buffer[2][TMRpcm_buffSize];
	extern volatile byte * TMRpcm_buffer_ptr;
	extern volatile byte TMRpcm_read_buffer[512];
	extern volatile uint16 TMRpcm_buffCount;
	extern volatile int8 TMRpcm_volMod;


	#if !defined (SDFAT)
		extern boolean TMRpcm_searchMainTags(File xFile, char *datStr);
	#else
		extern unsigned long TMRpcm_searchMainTags(SdFile xFile, char *datStr);
	#endif


	//*** Public vars used by RF library also ***
	extern boolean TMRpcm_wavInfo(char* filename);
	extern boolean rfPlaying;
	extern unsigned int TMRpcm_sample_RATE;



	//*** Recording WAV files ***
/*	void createWavTemplate(char* filename,unsigned int sampleRate);
	void finalizeWavTemplate(char* filename);
	#if defined (ENABLE_RECORDING)
		void startRecording(char* fileName, unsigned int SAMPLE_RATE, byte pin);
		void startRecording(char *fileName, unsigned int SAMPLE_RATE, byte pin, byte passThrough);
		void stopRecording(char *fileName);
	#endif
*/


#endif



