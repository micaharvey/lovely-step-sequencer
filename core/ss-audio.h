//-----------------------------------------------------------------------------
// name: ss-audio.h
// desc: audio stuff
//
// author: Micah (artfully stolen from Ge Wang)
//   date: 2013
//-----------------------------------------------------------------------------
#ifndef __SS_AUDIO_H__
#define __SS_AUDIO_H__




// init audio
bool ss_audio_init( unsigned int srate, unsigned int frameSize, unsigned channels );
// start audio
bool ss_audio_start();

// play some notes
void play( float pitch, float velocity );
void printState(int beat);
void updatePlayPlaces();



#endif
