// name: ss-audio.cpp
// desc: audio stuff for ss
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//   date: 2013
//-----------------------------------------------------------------------------
#include "ss-audio.h"
#include "y-fluidsynth.h"
#include "ss-globals.h"
#include "y-fft.h"
#include "y-waveform.h"
#include <iostream>
using namespace std;

#define DRUM_CHANNEL 9

// globals
YFluidSynth * g_synth;
GLfloat g_BPM = 240;
GLfloat g_BPS = g_BPM / 60;
GLfloat g_period = 1 / g_BPS;
GLfloat g_periodInSamples = g_period * SS_SRATE;
SAMPLE* g_soloBuf;
unsigned long g_timeSinceLastPlayedInSamples = 0;

// changing velocity
float g_velocity[16] = {1,0.5,0.7,0.5,\
                        1,0.5,0.7,0.5,\
                        1,0.5,0.7,0.5,\
                        1,0.5,0.7,0.5};


// Note( int c, float p, float v, float d )


// play some notes
void play( int beat )
{
    //ascii to terminal
    printState(beat);
    updatePlayPlaces();

    int lastBeat = beat-1 == -1 ? 15 : beat-1;
    vector<int> lastDrumVec = Globals::drumPitchVecs[lastBeat];
    vector<int> drumVec = Globals::drumPitchVecs[beat];
    vector<int> lastPitchVec = Globals::pitchVecs[lastBeat];
    vector<int> pitchVec = Globals::pitchVecs[beat];

    //TURN OFF LAST BEAT
    // drums
    if(lastDrumVec.size()){
        for (int i = 0; i < lastDrumVec.size(); ++i)
            g_synth->noteOff( DRUM_CHANNEL, lastDrumVec[i] );
    }
    if(lastPitchVec.size()){
        for (int i = 0; i < lastPitchVec.size(); ++i)
            g_synth->noteOff( 0, lastPitchVec[i] );
    }

    //PLAY THIS BEAT
    //drums
    if(drumVec.size()){
        for (int i = 0; i < drumVec.size(); ++i)
            g_synth->noteOn( DRUM_CHANNEL, drumVec[i], g_velocity[beat] * 127 );
    }
    if(pitchVec.size()){
        for (int i = 0; i < pitchVec.size(); ++i)
            g_synth->noteOn( 0, pitchVec[i], 100 );
    }
    
}

//-----------------------------------------------------------------------------
// name: audio_callback
// desc: audio callback
//-----------------------------------------------------------------------------
static void audio_callback( SAMPLE * buffer, unsigned int numFrames, void * userData )
{

    // no sound during intro
    if(Globals::isWelcome){
        memset( buffer, 0, sizeof(SAMPLE)*numFrames*XAudioIO::numChannels() );
        return;
    }

    g_synth->synthesize2( buffer, numFrames );
    Globals::now += numFrames;
    g_timeSinceLastPlayedInSamples += numFrames;

    // hack to make it seem smoother
    if( g_timeSinceLastPlayedInSamples >= g_periodInSamples-4096 ){
            Globals::playheads[Globals::beats%16]->showThenFade();
    }
    // progress the beat!
    if( g_timeSinceLastPlayedInSamples >= g_periodInSamples ){
        // play!
        play(Globals::beats%16);
        // book keep!
        g_timeSinceLastPlayedInSamples -= g_periodInSamples;
        Globals::beats++;
    }

}




//-----------------------------------------------------------------------------
// name: ss_audio_init()
// desc: initialize audio system
//-----------------------------------------------------------------------------
bool ss_audio_init( unsigned int srate, unsigned int frameSize, unsigned channels )
{
    // initialize
    if( !XAudioIO::init( 0, 0, srate, frameSize, channels, audio_callback, NULL ) )
    {
        // done
        return false;
    }

    g_soloBuf = new SAMPLE[frameSize*channels];
    
    // instantiate a YFluidsynth
    g_synth = new YFluidSynth();
    g_synth->init( srate, 32 );
    g_synth->load( "data/sfonts/rocking8m11e.sf2", "" );
    g_synth->programChange( 0, 0 );

    // fill vecs with empty stuff
    for (int i = 0; i < 16; ++i)
    {
        vector<int> vec, vec2;
        Globals::pitchVecs.push_back( vec );
        Globals::drumPitchVecs.push_back( vec2 );
    }

    for (int i = 0; i < 16; i+=2)
    {
        Globals::drumPitchVecs[i].push_back( SS_HIHAT );
    }

    /*
    // fill them with notes for fun and debugging
    Globals::drumPitchVecs[0].push_back( SS_KICK );
    Globals::drumPitchVecs[0].push_back( SS_HIHAT );
    Globals::drumPitchVecs[2].push_back( SS_HIHAT );
    Globals::drumPitchVecs[4].push_back( SS_SNARE );
    Globals::drumPitchVecs[4].push_back( SS_HIHAT );
    Globals::drumPitchVecs[6].push_back( SS_HIHAT );
    Globals::drumPitchVecs[7].push_back( SS_KICK );
    Globals::drumPitchVecs[8].push_back( SS_HIHAT );
    Globals::drumPitchVecs[9].push_back( SS_KICK );
    Globals::drumPitchVecs[10].push_back( SS_HIHAT );
    Globals::drumPitchVecs[11].push_back( SS_KICK );
    Globals::drumPitchVecs[12].push_back( SS_HIHAT );
    Globals::drumPitchVecs[12].push_back( SS_SNARE);
    Globals::drumPitchVecs[14].push_back( SS_HIHAT );
    */
    

    return true;
}

// printState
void updatePlayPlaces(){
    for(int beat = 0; beat < 16; beat++){
        Globals::playPlaces[beat]->k = false;
        Globals::playPlaces[beat]->s = false;
        Globals::playPlaces[beat]->h = false;
        Globals::playPlaces[beat]->p = false;
        for( int i = 0; i < Globals::drumPitchVecs[beat].size(); i++){
            if(Globals::drumPitchVecs[beat][i] == SS_KICK )  Globals::playPlaces[beat]->k = true;
            if(Globals::drumPitchVecs[beat][i] == SS_SNARE ) Globals::playPlaces[beat]->s = true;
            if(Globals::drumPitchVecs[beat][i] == SS_HIHAT ) Globals::playPlaces[beat]->h = true;
        }
        if( Globals::pitchVecs[beat].size() )
            Globals::playPlaces[beat]->p = true;
    }
}


// printState
void printState(int beat){
        // ASCII
    if(beat == 0)
        cerr << "\n";
    if(beat%4 == 0)
        cerr << "\n";
    cerr << "[";

    bool k = false;
    bool s = false;
    bool h = false;

    for( int i = 0; i < Globals::drumPitchVecs[beat].size(); i++){
        if(Globals::drumPitchVecs[beat][i] == SS_KICK )  k = true;
        if(Globals::drumPitchVecs[beat][i] == SS_SNARE ) s = true;
        if(Globals::drumPitchVecs[beat][i] == SS_HIHAT ) h = true;
    }
    if(k) cerr << 'k';
    if(s) cerr << 's';
    if(h) cerr << 'h';
    if(!(k||s||h)) cerr << beat;

    cerr << "]\t"; //<< Globals::beats[beat][0];
}



//-----------------------------------------------------------------------------
// name: vq_audio_start()
// desc: start audio system
//-----------------------------------------------------------------------------
bool ss_audio_start()
{
    // start the audio
    if( !XAudioIO::start() )
    {
        // done
        return false;
    }
    
    return true;
}
