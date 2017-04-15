/*Library by TMRh20 2012-2014*/


#define USEDMA 1

//#include <pcmConfig.h>
#include "TMRpcm.h"

#if !defined (SDFAT)
	#include <SD.h>
#else
	#include <SdFat.h>
#endif

#include <libmaple/systick.h>
#include <libmaple/timer.h>

#if !defined (SDFAT)
	File sFile;
#else
	SdBaseFile sFile;
#endif

	extern byte speakerPin = 0;
	extern byte speakerPin2 = 0;
    extern uint8_t CSPin = 0;
	extern uint16 resolution = 0;
	extern uint8 repetition = 0;
	extern byte lastSpeakPin = 0;

	//uint8 timer_x;

	//*********** Standard Global Variables ***************
	extern volatile byte TMRpcm_optionByte = B01100000; // See option flags defined above.
	extern volatile unsigned long TMRpcm_dataEnd =0;
	extern volatile boolean TMRpcm_buffEmpty[2] = {true,true}, TMRpcm_playing = false;
	extern volatile byte a = 0;
	extern volatile int16 bytes_left = 0;
	extern volatile int16 bytes_read = 0;
	extern volatile int16 bytes_filled = 0;
	extern volatile byte 	 TMRpcm_whichBuff = 0;
	extern volatile uint8 TMRpcm_spk_ch1 = 1;
	extern volatile uint8 TMRpcm_spk_ch2 = 2;
	extern timer_dev * volatile TMRpcm_dev = TIMER4;
	extern volatile uint16 TMRpcm_sample = 0;
	volatile uint16 TMRpcm_sample2 = 0;

	extern volatile byte TMRpcm_buffer[2][TMRpcm_buffSize] = {0};
	extern volatile byte TMRpcm_read_buffer[512] = {0};
	extern volatile uint16 TMRpcm_buffCount = 0;
	extern volatile int8 TMRpcm_volMod=0;

	//*** Public vars used by RF library also ***
	extern boolean TMRpcm_wavInfo(char* filename);
	//extern boolean rfPlaying;
	extern unsigned int TMRpcm_sample_RATE = 0;



//**************************************************************
//********** Core Playback Functions used in all modes *********


void TMRpcm_timerSet(){
	  #if defined (debug)
	Serial.println("About to read TMRpcm_dev:");
  #endif
	TMRpcm_dev = PIN_MAP[speakerPin].timer_device;
  #if defined (debug)
	Serial.println((uint32)TMRpcm_dev);
	Serial.println("About to read the channels");
  #endif
//	ASSERT(PIN_MAP[speakerPin2].timer_device = TMRpcm_dev);
	TMRpcm_spk_ch1 = PIN_MAP[speakerPin].timer_channel;
	TMRpcm_spk_ch2 = PIN_MAP[speakerPin2].timer_channel;
//	timer_x = (TMRpcm_dev - TIMER1) + 1;
//	 HardwareTimer TMRpcm_timer(timer_x);
  #if defined (debug)
	Serial.println(TMRpcm_spk_ch1);
	Serial.println(TMRpcm_spk_ch2);
	Serial.println("Read timer, about to pause");
  #endif
	timer_pause (TMRpcm_dev);
  #if defined (debug)
	Serial.println("Paused timer, about to change reload value");
  #endif
	timer_set_prescaler(TMRpcm_dev, 0);
  #if defined (debug)
	Serial.print("Paused timer, about to change resolution to:");
	Serial.println(resolution);
  #endif
	timer_set_reload(TMRpcm_dev, resolution);
  #if defined (debug)
	Serial.println("About to change RCR");
  #endif
	/*
	 * Todo, add a check of timer device, if advanced, then use RCR
	 */
	//(TMRpcm_dev->regs).adv->RCR = repetition;
	(TMRpcm_dev->regs).adv->PSC = repetition;
  #if defined (debug)
	Serial.println("Changed RCR, returning");
  #endif
	//timer_oc_set_mode(TMRpcm_dev, TMRpcm_spk_ch1, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
	//timer_oc_set_mode(TMRpcm_dev, TMRpcm_spk_ch2, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
	timer_cc_enable (TMRpcm_dev, TMRpcm_spk_ch1);
	timer_cc_enable (TMRpcm_dev, TMRpcm_spk_ch2);
}


void TMRpcm_setPins(){
	//TMRpcm_disable();
	pinMode(speakerPin,PWM);
	pinMode(speakerPin2,PWM);
}

boolean TMRpcm_wavInfo(char* filename){

  //check for the string WAVE starting at 8th bit in header of file
  #if !defined (SDFAT)
  sFile = SD.open(filename);
  #else
  sFile.open(filename);
  #endif
  #if defined (debug)
	Serial.println("File opening");
  #endif
  if( !TMRpcm_ifOpen() ){
	    #if defined (debug)
		Serial.println("Failed to open");
		#endif
	  return 0; }
  TMRpcm_seek(8);
  char wavStr[] = {'W','A','V','E'};
  for (byte i =0; i<4; i++){
	  if(sFile.read() != wavStr[i]){
		  #if defined (debug)
		  	Serial.println("WAV ERROR");
		  #endif
		  break; }
  }
  		  #if defined (debug)
		  	Serial.println("WAV identified");
		  #endif
	    byte stereo, bps;
		TMRpcm_seek(22);
	    stereo = sFile.read();
	    TMRpcm_seek(24);

        TMRpcm_sample_RATE = sFile.read();
        TMRpcm_sample_RATE = sFile.read() << 8 | TMRpcm_sample_RATE;
		#if defined (debug)
		  	Serial.print("TMRpcm_sample Rate: ");
			Serial.println(TMRpcm_sample_RATE);
		#endif

        //Set bits per sample and stereo/mono
		TMRpcm_seek(34);
	    bps = sFile.read();
	    bps = sFile.read() << 8 | bps;
		bitClear(TMRpcm_optionByte,stereo_bit);
		bitClear(TMRpcm_optionByte,bps16_bit);
	    if( stereo == 2){ //_2bytes=1;
	    	bitSet(TMRpcm_optionByte,stereo_bit);
		}
		if( bps == 16 ){
			bitSet(TMRpcm_optionByte,bps16_bit);
	    }
	#if defined (debug)
		  	Serial.print("Channels and bps: ");
			Serial.println(stereo);
			Serial.println(bps);
	#endif
	/*
	#if defined (HANDLE_TAGS)

		TMRpcm_seek(36);
	    char datStr[4] = {'d','a','t','a'};
	    for (byte i =0; i<4; i++){
			if(sFile.read() != datStr[i]){
			 	TMRpcm_seek(40);
			 	unsigned int siz = sFile.read();
			 	siz = (sFile.read() << 8 | siz)+2;
			 	TMRpcm_seek(TMRpcm_fPosition() + siz);
 	     		for (byte i =0; i<4; i++){
					if(sFile.read() != datStr[i]){
						return 0;
					}
		 		}
			}
		 }

		unsigned long dataBytes = sFile.read();
	    for (byte i =8; i<32; i+=8){
			dataBytes = sFile.read() << i | dataBytes;
		}
		#if !defined (SDFAT)
			TMRpcm_dataEnd = sFile.size() - TMRpcm_fPosition() - dataBytes + TMRpcm_buffSize;
		#else
			TMRpcm_dataEnd = sFile.fileSize() - TMRpcm_fPosition() - dataBytes + TMRpcm_buffSize;
		#endif
	// I dont think all the above calculations work at all.
	
	#else //No Tag handling
	*/
		TMRpcm_seek(44); TMRpcm_dataEnd = TMRpcm_buffSize;

	//#endif
	#if defined (debug)
		  	Serial.println("Returning from wavinfo");
	#endif
	return 1;

}


//*************** General Playback Functions *****************

void TMRpcm_quality(boolean q){
	if(!TMRpcm_playing){	bitWrite(TMRpcm_optionByte,quality_bit,q); } //qual = q; }
}

void TMRpcm_stopPlayback(){
	TMRpcm_playing = false;
	timer_pause (TMRpcm_dev);
	if(TMRpcm_ifOpen()){ sFile.close(); }
}

void TMRpcm_pause(){
	//paused = !paused;
	if(bitRead(TMRpcm_optionByte,paused_bit) && TMRpcm_playing){
		bitClear(TMRpcm_optionByte,paused_bit);
		timer_resume (TMRpcm_dev);

/*		
		#if !defined (USE_TIMER2)
			*TIMSK[tt] |= ( _BV(ICIE1) | _BV(TOIE1) );
		#else
			*TIMSK[tt] |= ( _BV(OCIE2B) | _BV(TOIE1) );
		#endif
*/
	}else if(!bitRead(TMRpcm_optionByte,paused_bit) && TMRpcm_playing){
		bitSet(TMRpcm_optionByte,paused_bit);
		timer_pause (TMRpcm_dev);
//		*TIMSK[tt] &= ~( _BV(TOIE1) );
	}
}

void TMRpcm_loop(boolean set){
	bitWrite(TMRpcm_optionByte,loop_bit,set);
}
/**************************************************
This section used for translation of functions between
 SDFAT library and regular SD library
Prevents a whole lot more #if defined statements */

#if !defined (SDFAT)

	boolean TMRpcm_seek( unsigned long pos ){
		return sFile.seek(pos);
	}

	unsigned long TMRpcm_fPosition( ){
		return sFile.position();
	}

	boolean TMRpcm_ifOpen(){
		if(sFile){ return 1;}
	}

#else

	boolean TMRpcm_seek(unsigned long pos ){
		return sFile.seekSet(pos);
	}

	unsigned long TMRpcm_fPosition(){
		return sFile.curPosition();
	}

	boolean TMRpcm_ifOpen(){
		return sFile.isOpen();
	}

#endif



//***************************************************************************************
//********************** Functions for single track playback ****************************

//#if !defined (ENABLE_MULTI) //Below section for normal playback of 1 track at a time


void TMRpcm_play(char* filename){
	TMRpcm_play(filename,0);
}

void TMRpcm_play(char* filename, unsigned long seekPoint){
  if (TMRpcm_playing) TMRpcm_stopPlayback();
  	#if defined (debug)
  		Serial.println("About to set pins");
  	#endif
  if(speakerPin != lastSpeakPin){
	  TMRpcm_setPins();
	  lastSpeakPin=speakerPin;
  }
  #if defined (debug)
  	Serial.println("About to open file");
  #endif
  if(!TMRpcm_wavInfo(filename)){
  	#if defined (debug)
  		Serial.println("WAV ERROR");
  	#endif
  return;
  }//verify its a valid wav file

  	if(seekPoint > 0){ seekPoint = (TMRpcm_sample_RATE*seekPoint) + TMRpcm_fPosition();
  	TMRpcm_seek(seekPoint); //skip the header info

  }
	
	if(TMRpcm_sample_RATE > 48000 ){ TMRpcm_sample_RATE = 48000;
	#if defined (debug)
  	  	Serial.print("TMRpcm_sample RATE TOO HIGH: ");
  	  	Serial.println(TMRpcm_sample_RATE);
  	#endif
  	}
	/*
	if(TMRpcm_sample_RATE == 48000 ){
		resolution = 250; //250
		repetition = 5;
	}
	else if(TMRpcm_sample_RATE == 44100 ){
		resolution = 272; //272
		repetition = 5;
	}
	else if(TMRpcm_sample_RATE == 22050 ){
		resolution = 272; //272
		repetition = 11;
	}
	else if(TMRpcm_sample_RATE == 8000 ){
		resolution = 250; //250
		repetition = 35;
	}
	else {
	*/
		resolution = 72000000/TMRpcm_sample_RATE;
		repetition = resolution/256;
		resolution = resolution/repetition;
		repetition = repetition-1;
	//}

	#if defined (debug)
  	  	Serial.println("Peeking tmp byte ");
  	#endif

    //byte tmp = (sFile.read() + sFile.peek()) / 2;

	//rampUp = 0;
/* Write new soft start routine to avoid clicks and noise.
	unsigned int mod;
	if(TMRpcm_volMod > 0){ mod = *OCRnA[tt] >> TMRpcm_volMod; }else{ mod = *OCRnA[tt] << (TMRpcm_volMod*-1); }
	if(tmp > mod){
		for(unsigned int i=0; i<TMRpcm_buffSize; i++){ mod = constrain(mod+1,mod, tmp); TMRpcm_buffer[0][i] = mod; }
		for(unsigned int i=0; i<TMRpcm_buffSize; i++){ mod = constrain(mod+1,mod, tmp); TMRpcm_buffer[1][i] = mod; }
	}else{
		for(unsigned int i=0; i<TMRpcm_buffSize; i++){ mod = constrain(mod-1,tmp ,mod); TMRpcm_buffer[0][i] = mod; }
		for(unsigned int i=0; i<TMRpcm_buffSize; i++){ mod = constrain(mod-1,tmp, mod); TMRpcm_buffer[1][i] = mod; }
	}
*/
    TMRpcm_whichBuff = 0; TMRpcm_buffEmpty[0] = true; TMRpcm_buffEmpty[1] = true; TMRpcm_buffCount = 0;
    /*
     * Clean up buffers and timers to remove some clicks.
     */
    memset ((void *)&TMRpcm_buffer[0], 0, TMRpcm_buffSize);
    memset ((void *)&TMRpcm_buffer[1], 0, TMRpcm_buffSize);
	timer_set_compare(TMRpcm_dev, TMRpcm_spk_ch1, 0);
	timer_set_compare(TMRpcm_dev, TMRpcm_spk_ch2, 0);
//    noInterrupts();

	#if defined (debug)
  	  	Serial.println("About to setup timer.");
  	#endif
	TMRpcm_timerSet();
	timer_generate_update (TMRpcm_dev);
//	update_ccr_ptr = (void (*)())&TMRpcm_update_ccr;
	#if defined (debug)
  	  	Serial.println("About to set selfptr");
  	#endif
	//update_ccr_ptr = &this->update_ccr;
//	me = this;
	#if defined (debug)
  	  	Serial.println("About to set update interrupt.");
  	#endif
	//just replaced the isr
#if defined (USEDMA)
  	  	TMRpcm_DMASet();
#else
	timer_attach_interrupt(TMRpcm_dev, TIMER_UPDATE_INTERRUPT, &TMRpcm_update_ccr);
#endif
	#if defined (debug)
  	  	Serial.println("About to attach systick.");
  	#endif
	
	//systick_attach_callback(&TMRpcm_buffer_load);
	TMRpcm_playing = true; bitClear(TMRpcm_optionByte,paused_bit); //paused = 0;
	timer_resume (TMRpcm_dev);
}
/*
 * Todo: DMA Dev and Channel in variables, get their value depending on pins used.
 */

void TMRpcm_DMASet(){
	dma_init(DMA1);
	dma_disable(DMA1, DMA_CH7);
    dma_setup_transfer(DMA1, DMA_CH7, &(TMRpcm_dev->regs).gen->DMAR, DMA_SIZE_16BITS,
                       TMRpcm_buffer, DMA_SIZE_8BITS, DMA_MINC_MODE |  DMA_FROM_MEM | DMA_CIRC_MODE | DMA_HALF_TRNS | DMA_TRNS_CMPLT);// Transmit buffer DMA
    dma_set_num_transfers(DMA1, DMA_CH7, 2*TMRpcm_buffSize);
    (TMRpcm_dev->regs).gen->DCR = 0x10D; //DBL = 2 transfers << 8, DBA = 0xD, so starts at CCR1
    dma_attach_interrupt(DMA1, DMA_CH7, &TMRpcm_DMAISR);
    (TMRpcm_dev->regs).gen->DIER |= TIMER_DIER_UDE; // Enable Update DMA request, core needs new timer_dma_enable_upd_req(timer_dev *dev)
    (TMRpcm_dev->regs).bas->CR1 |= TIMER_CR1_URS; // Are we getting too many updates?
    dma_enable(DMA1, DMA_CH7);
}
/** This function is called at Half and Complete DMA transfer.
 *  It marks the corresponding half of the buffer as empty so the buffer_load function can reload to it.
 *
 */

void TMRpcm_DMAISR(){
	volatile dma_irq_cause event = dma_get_irq_cause(DMA1, DMA_CH7);
	switch(event) {
	    //Second half completed
	    case DMA_TRANSFER_COMPLETE:
	      //Buffer 2 is done, mark it free
	    	TMRpcm_buffEmpty[1] = true;
	    	TMRpcm_whichBuff = 0;
	      break;
	    //First half completed
	    case DMA_TRANSFER_HALF_COMPLETE:
	    //Buffer 1 is done, mark it free
	    	TMRpcm_buffEmpty[0] = true;
	    	TMRpcm_whichBuff = 1;
	      break;
	    default:
	    	break;
	  }
}

void TMRpcm_volume(char upDown){

  if(upDown){
	  TMRpcm_volMod++;
	  if (TMRpcm_volMod>4) TMRpcm_volMod=4;
  }else{
	  TMRpcm_volMod--;
	  if (TMRpcm_volMod<-4) TMRpcm_volMod=-4;
  }
}

void TMRpcm_setVolume(char vol) {
    TMRpcm_volMod = vol - 4 ;
}

void TMRpcm_buffer_load(){
	
	if (TMRpcm_playing == false) return;
/*
	#if defined (debug)
  	  	Serial.println("Systick");
  	#endif
*/

	a = TMRpcm_whichBuff == 0 ? 1 : 0;
	if (TMRpcm_buffEmpty[a] == false) return; // If not empty just return
	else {
	#if defined (debug)
		Serial.print("Buffer not in use: ");
	    Serial.print(a);
		Serial.print(" empty");
  	#endif
		int32 bytes_left = sFile.available();
		if(bytes_left <= 0){
/*			#if defined (debug)
			Serial.print("Bytes left: ");
			Serial.println(bytes_left);
			#endif
			TMRpcm_stopPlayback();
			return;
		}
		else if( bytes_left <= TMRpcm_dataEnd){
			sFile.read((byte*)TMRpcm_buffer[a],bytes_left);
				for (long x=bytes_left; x < TMRpcm_dataEnd; x++){
				TMRpcm_buffer[a][x] = 0;
				}
			TMRpcm_buffEmpty[a] = false;
			#if defined (debug)
			Serial.print("Finishing file play.");
			Serial.println(bytes_left);
			#endif
			while (TMRpcm_buffEmpty[a] == false) { // wait till the last bit is played.
				delay (10);
			}
*/		
	#if defined (debug)
  	  	Serial.println("systick file end");
  	#endif
		  	#if !defined (SDFAT)
		  		if(bitRead(TMRpcm_optionByte,loop_bit)){
					sFile.seek(44);
					return;
				}
			#else
				if(bitRead(TMRpcm_optionByte,loop_bit)){
					sFile.seekSet(44);
					return;
				}
			#endif
			TMRpcm_stopPlayback();
			return;
	  	}
		#if defined (debug)
  	  	Serial.println("fill buffer");
		#endif

		if(bitRead(TMRpcm_optionByte,stereo_bit) ){ // Stereo wav
		
			if(!bitRead(TMRpcm_optionByte,bps16_bit)){  //8 bit TMRpcm_samples.
				bytes_filled = sFile.read((byte*)&TMRpcm_buffer[a],TMRpcm_buffSize); // We can load this straight to the buffer
			}
			else { //Stereo 16bit samples
					bytes_filled = 0;
					while ((bytes_filled < TMRpcm_buffSize) && (bytes_left > 0)){
						bytes_read = sFile.read((byte*)TMRpcm_read_buffer,512);
						bytes_left -= bytes_read;
						for (uint16 n = 0; n < bytes_read/2; n++){
							TMRpcm_buffer[a][bytes_filled+n] = (TMRpcm_read_buffer[1+(n*2)]+128);
						}
						bytes_filled += bytes_read/2;
					}
			}
		}
#if defined (USEDMA)
		else { // Mono Wav
			if(!bitRead(TMRpcm_optionByte,bps16_bit)){  //8 bit TMRpcm_samples.
				//bytes_filled = sFile.read((byte*)&TMRpcm_buffer[a],TMRpcm_buffSize); // for 8bit stereo read directly to the buffer.
				bytes_filled = 0;
				while (bytes_filled < TMRpcm_buffSize && bytes_left > 0){
					bytes_read = sFile.read((byte*)TMRpcm_read_buffer,512);
					bytes_left -= bytes_read;
					for (uint16 n = 0; n < bytes_read; n++){ // Fill buffer for both channels with the same information.
						TMRpcm_buffer[a][n*2] = TMRpcm_read_buffer[n];
						TMRpcm_buffer[a][(n*2)+1] = TMRpcm_read_buffer[n];
					}
					bytes_filled += bytes_read*2;
				}

			}
			else { //Mono 16bit samples
					bytes_filled = 0;
					while (bytes_filled < TMRpcm_buffSize && bytes_left > 0){
						bytes_read = sFile.read((byte*)TMRpcm_read_buffer,512);
						bytes_left -= bytes_read;
						for (uint16 n = 0; n < bytes_read/2; n++){
							TMRpcm_buffer[a][bytes_filled+(n*2)] = (TMRpcm_read_buffer[1+(n*2)]+128);
							TMRpcm_buffer[a][bytes_filled+1+(n*2)] = (TMRpcm_read_buffer[1+(n*2)]+128);
						}
						bytes_filled += bytes_read;
					}
			}
		}
#else
		else { // Mono Wav
			if(!bitRead(TMRpcm_optionByte,bps16_bit)){  //8 bit TMRpcm_samples.
				bytes_filled = sFile.read((byte*)&TMRpcm_buffer[a],TMRpcm_buffSize); // for 8bit stereo read directly to the buffer.

			}
	//
			else { //Mono 16bit samples
					bytes_filled = 0;
					while (bytes_filled < TMRpcm_buffSize && bytes_left > 0){
						bytes_read = sFile.read((byte*)TMRpcm_read_buffer,512);
						bytes_left -= bytes_read;
						for (uint16 n = 0; n < bytes_read/2; n++){
							TMRpcm_buffer[a][bytes_filled+n] = (TMRpcm_read_buffer[1+(n*2)]+128);
						}
						bytes_filled += bytes_read/2;
					}
			}
		}
#endif
		if (bytes_filled < TMRpcm_buffSize){
			uint8 n = TMRpcm_buffer[a][bytes_filled-1];
			for (int16 x = bytes_filled - 1; x < TMRpcm_buffSize; x++){
				TMRpcm_buffer[a][x] = n;
				n = ( n > 0 ? n -=1 : 0); //decrement n until 0.
			}
		}
		TMRpcm_buffEmpty[a] = false;
		return;
   	}
}

void TMRpcm_update_ccr(){
/*
 * Todo. Perhaps change this to DMA. Then will need to fill the buffers with 2 channels data even if mono file.
*/
	if( !bitRead(TMRpcm_optionByte,stereo_bit) ){
		timer_set_compare(TMRpcm_dev, TMRpcm_spk_ch1, TMRpcm_buffer[TMRpcm_whichBuff][TMRpcm_buffCount]);
		timer_set_compare(TMRpcm_dev, TMRpcm_spk_ch2, TMRpcm_buffer[TMRpcm_whichBuff][TMRpcm_buffCount]);
		TMRpcm_buffCount ++;
	}
	else {
		timer_set_compare(TMRpcm_dev, TMRpcm_spk_ch1, TMRpcm_buffer[TMRpcm_whichBuff][TMRpcm_buffCount]);
		timer_set_compare(TMRpcm_dev, TMRpcm_spk_ch2, TMRpcm_buffer[TMRpcm_whichBuff][TMRpcm_buffCount+1]);
		TMRpcm_buffCount +=2;
	}
	
  	if(TMRpcm_buffCount >= TMRpcm_buffSize){
	  TMRpcm_buffCount = 0;
      TMRpcm_buffEmpty[TMRpcm_whichBuff] = true;
	  TMRpcm_whichBuff = TMRpcm_whichBuff == 0 ? 1 : 0;
  	}
}


void TMRpcm_disable(){
	TMRpcm_playing = false;
	timer_pause (TMRpcm_dev);
	timer_detach_interrupt(TMRpcm_dev, TIMER_UPDATE_INTERRUPT);
	if(TMRpcm_ifOpen()){ sFile.close();}
/* Write new soft shutdown routine.
	if(bitRead(*TCCRnA[tt],7) > 0){
		int current = *OCRnA[tt];
		for(int i=0; i < resolution; i++){
				*OCRnB[tt] = constrain((current - i),0,resolution);
				*OCRnA[tt] = constrain((current - i),0,resolution);
			for(int i=0; i<10; i++){ while(*TCNT[tt] < resolution-50){} }
		}
	}
*/
    bitSet(TMRpcm_optionByte,5);
    //rampUp = 1;
}



boolean TMRpcm_isPlaying(){
	return TMRpcm_playing;
}






//****************** Metadata Features ****************************
//****************** ID3 and LIST Tags ****************************



byte TMRpcm_getInfo(char* filename, char* tagData, byte infoNum){
	byte gotInfo = 0;
	if( (gotInfo = TMRpcm_metaInfo(1,filename, tagData, infoNum)) < 1){
		gotInfo = TMRpcm_metaInfo(0,filename, tagData, infoNum);
	}
	return gotInfo;

}

byte TMRpcm_listInfo(char* filename, char* tagData, byte infoNum){
	return TMRpcm_metaInfo(0, filename, tagData, infoNum);

}



//http://id3.org/id3v2.3.0
byte TMRpcm_id3Info(char* filename, char* tagData, byte infoNum){
	return TMRpcm_metaInfo(1, filename, tagData, infoNum);
}


#if !defined (SDFAT)
boolean TMRpcm_searchMainTags(File xFile, char *datStr){
	xFile.seek(36);
#else
unsigned long TMRpcm_searchMainTags(SdFile xFile, char *datStr){
	xFile.seekSet(36);
#endif
		boolean found = 0;
	    char dChars[4] = {'d','a','t','a'};
		char tmpChars[4];

		//xFile.seek(36);
	    xFile.read((char*)tmpChars,4);
	    for (byte i =0; i<4; i++){
			if(tmpChars[i] != dChars[i]){
				#if !defined (SDFAT)
				 	xFile.seek(40);
				 	unsigned int siz = xFile.read(); siz = (xFile.read() << 8 | siz)+2;
				 	xFile.seek(xFile.position() + siz);
				#else
					xFile.seekSet(40);
					unsigned int siz = xFile.read(); siz = (xFile.read() << 8 | siz)+2;
				 	xFile.seekSet(xFile.curPosition() + siz);
				#endif
			 	xFile.read((char*)tmpChars,4);
 	     		for (byte i =0; i<4; i++){
					if(tmpChars[i] != dChars[i]){
						return 0;
					}
		 		}
			}
		 }

		unsigned long tmpp=0;
		unsigned long daBytes = xFile.read();
	    for (byte i =8; i<32; i+=8){
			tmpp = xFile.read();
			daBytes = tmpp << i | daBytes;
		}

		#if !defined (SDFAT)
			daBytes = xFile.position() + daBytes;
			if(xFile.size() == daBytes){ return 0; }
		#else
			daBytes = xFile.curPosition() + daBytes;
			if(xFile.fileSize() == daBytes){ return 0; }
		#endif

	//if(found == 0){ //Jump to file end - 1000 and search for ID3 or LIST
	#if !defined (SDFAT)
		xFile.seek(daBytes);
	#else
		xFile.seekSet(daBytes);
	#endif

		while(xFile.available() > 5){
			if(xFile.read() == datStr[0] && xFile.peek() == datStr[1]){
				xFile.read((char*)tmpChars,3);
	 			if( tmpChars[1] == datStr[2] &&  tmpChars[2] == datStr[3] ){
						found = 1;
						#if !defined (SDFAT)
							return 1; break;
						#else
							return xFile.curPosition();
						#endif
				}else{
					#if !defined (SDFAT)
						xFile.seek(xFile.position() - 1 - 4); //pos - tagSize
					#else
						unsigned long pos = xFile.curPosition()-1;
						xFile.seekSet(pos - 4);
					#endif
				}
			}
		}
		return 0;

}



byte TMRpcm_metaInfo(boolean infoType, char* filename, char* tagData, byte whichInfo){


	if(TMRpcm_ifOpen()){ noInterrupts();}

	#if !defined (SDFAT)
		File xFile;
		xFile = SD.open(filename);
		xFile.seek(36);
	#else
		SdFile xFile;
		xFile.open(filename);
		xFile.seekSet(36);
	#endif

	boolean found=0;
		char* datStr = "LIST";
		if(infoType == 1){datStr = "ID3 "; datStr[3] = 3;}
		char tmpChars[4];

	if(infoType == 0){ //if requesting LIST info, check for data at beginning of file first
		xFile.read((char*)tmpChars,4);
		for (byte i=0; i<4; i++){ //4 tagSize
			if(tmpChars[i] != datStr[i]){
				break;
		  	}else if(i==3){
				found = 1;
		  	}
		}
	}
	if(found == 0){
		#if !defined (SDFAT)
			found = TMRpcm_searchMainTags(xFile, datStr);
		#else
			unsigned long pos = TMRpcm_searchMainTags(xFile, datStr);
			xFile.seekSet(pos);
			if(pos > 0){ found = 1; }
		#endif
	}

//** This section finds the starting point and length of the tag info
	if(found == 0){ xFile.close(); if(TMRpcm_ifOpen()){ interrupts();} return 0; }

	unsigned long listEnd;
	unsigned int listLen;
	char* tagNames[] = {"INAM","IART","IPRD"};

	if(infoType == 0){ //LIST format
		listLen = xFile.read(); listLen = xFile.read() << 8 | listLen;
		#if !defined (SDFAT)
			xFile.seek(xFile.position() +6);
			listEnd = xFile.position() + listLen;
		#else
			xFile.seekSet(xFile.curPosition() +6);
			listEnd = xFile.curPosition() + listLen;
		#endif

	}else{				//ID3 format

		#if !defined (SDFAT)
			xFile.seek(xFile.position() + 5);
		#else
			xFile.seekSet(xFile.curPosition() + 5);
		#endif
			listLen = xFile.read() << 7 | listLen; listLen = xFile.read() | listLen;
			tagNames[0] = "TPE1"; tagNames[1] ="TIT2"; tagNames[2] ="TALB";
		#if !defined (SDFAT)
			listEnd = xFile.position() + listLen;
		#else
			listEnd = xFile.curPosition() + listLen;
		#endif
	}

	char tgs[4];
	unsigned int len = 0;
	unsigned long tagPos = 0;

//** This section reads the tags and gets the size of the tag data and its position
//** Should work with very long tags if a big enough buffer is provided
#if !defined (SDFAT)
	while(xFile.position() < listEnd){
#else
	while(xFile.curPosition() < listEnd){
#endif

		xFile.read((char*)tgs,4);

		if(infoType == 0){ //LIST
			len = xFile.read()-1;
			len = xFile.read() << 8 | len;
			#if !defined (SDFAT)
				xFile.seek(xFile.position()+2);
			#else
				xFile.seekSet(xFile.curPosition()+2);
			#endif
		}else{ 				//ID3
			#if !defined (SDFAT)
				xFile.seek(xFile.position()+3);
			#else
				xFile.seekSet(xFile.curPosition()+3);
			#endif
			len = xFile.read();
			len = xFile.read() << 8 | len;
			len = (len-3)/2;
			#if !defined (SDFAT)
				tagPos = xFile.position() + 4;
				xFile.seek(tagPos);
			#else
			    xFile.seekSet(xFile.curPosition() +4);
				tagPos = xFile.curPosition();
			#endif

		}

		found =0;
	//** This section checks to see if the tag we found is the one requested
	//** If so, it loads the data into the buffer
		for(int p=0; p<4;p++){
			if(tgs[p] != tagNames[whichInfo][p]){
				break;
			}else{
				if(p==3){
					if(infoType == 1){
						byte junk;
						for(byte j=0; j<len; j++){
							tagData[j] = xFile.read();
							junk=xFile.read();
						}
					}else{
						xFile.read((char*)tagData,len);
					}
					tagData[len] = '\0';
					xFile.close();
					if(TMRpcm_ifOpen()){ interrupts();}
 					return len;
				}
			}
		}

		if(found){break;}

	//**This section jumps to the next tag position if the requested tag wasn't found
	#if !defined (SDFAT)
		if(infoType == 0){
			if(!found){	xFile.seek(xFile.position()+len);}
			while(xFile.peek() == 0){xFile.read();}
		}else{
			if(!found){	xFile.seek(tagPos+len); }
			while(xFile.peek() != 'T'){xFile.read();}
		}
	#else
		if(infoType == 0){
			if(!found){	xFile.seekSet(xFile.curPosition()+len);}
			while(xFile.peek() == 0){xFile.read();}
		}else{
			if(!found){	xFile.seekSet(tagPos+len); }
			while(xFile.peek() != 'T'){xFile.read();}
		}
	#endif


 	}
 	xFile.close();
 	if(TMRpcm_ifOpen()){ interrupts();}
 	return 0;
}


/*********************************************************************************
********************** DIY Digital Audio Generation ******************************/


void TMRpcm_finalizeWavTemplate(char* filename){
	TMRpcm_disable();

	unsigned long fSize = 0;

  #if !defined (SDFAT)
		sFile = SD.open(filename,FILE_WRITE);

    if(!sFile){
		#if defined (debug)
			Serial.println("fl");
		#endif
		return;
	}
	fSize = sFile.size()-8;

  #else
		sFile.open(filename,O_WRITE );

    if(!sFile.isOpen()){
		#if defined (debug)
			Serial.println("failed to finalize");
		#endif
		return; }
    fSize = sFile.fileSize()-8;

  #endif



	TMRpcm_seek(4); byte data[4] = {(byte)lowByte(fSize),(byte)highByte(fSize), (byte)(fSize >> 16), (byte)(fSize >> 24)};
	sFile.write(data,4);
	//byte tmp;
	TMRpcm_seek(40);
	fSize = fSize - 36;
	data[0] = lowByte(fSize); data[1]=highByte(fSize);data[2]=fSize >> 16;data[3]=fSize >> 24;
	sFile.write((byte*)data,4);
	sFile.close();

	#if defined (debug)
	#if !defined (SDFAT)
		sFile = SD.open(filename);
	#else
		sFile.open(filename);
	#endif

	if(TMRpcm_ifOpen()){

		    while (TMRpcm_fPosition() < 44) {
		      Serial.print(sFile.read(),HEX);
		      Serial.print(":");
		   	}
		   	Serial.println("");

	   	//Serial.println(sFile.size());
    	sFile.close();
	}
	#endif
}



void TMRpcm_createWavTemplate(char* filename, unsigned int TMRpcm_sampleRate){
	TMRpcm_disable();

  #if !defined (SDFAT)
  	if(SD.exists(filename)){SD.remove(filename);}
  #endif



  #if !defined (SDFAT)
  		if(SD.exists(filename)){SD.remove(filename);}
		sFile = SD.open(filename,FILE_WRITE);
	if(!sFile){
		#if defined (debug)
			Serial.println("failed to open 4 writing");
		#endif
		return;
	}

  #else
   	sFile.open(filename,O_CREAT | O_WRITE);
   	if(sFile.fileSize() > 44 ){ sFile.truncate(44);}
	if(!sFile.isOpen()){
		#if defined (debug)
			Serial.println("failed to open 4 writing");
		#endif
		return;
	}

  #endif
    else{
  		//Serial.print("Sr: ");
  		//Serial.println(TMRpcm_sampleRate);
  		TMRpcm_seek(0);
		byte data[] = {16,0,0,0,1,0,1,0,lowByte(TMRpcm_sampleRate),highByte(TMRpcm_sampleRate)};
		sFile.write((byte*)"RIFF    WAVEfmt ",16);
		sFile.write((byte*)data,10);
		//unsigned int byteRate = (TMRpcm_sampleRate/8)*monoStereo*8;
		//byte blockAlign = monoStereo * (bps/8); //monoStereo*(bps/8)
		data[0] = 0; data[1] = 0; data[2] = lowByte(TMRpcm_sampleRate); data[3] =highByte(TMRpcm_sampleRate);//Should be byteRate
		data[4]=0;data[5]=0;data[6]=1; //BlockAlign
		data[7]=0;data[8]=8;data[9]=0;
		sFile.write((byte*)data,10);
		sFile.write((byte*)"data    ",8);
		//Serial.print("siz");
		//Serial.println(sFile.size());
		sFile.close();

	}
}


