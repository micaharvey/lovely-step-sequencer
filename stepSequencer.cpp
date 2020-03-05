//----------------------------------------------------------------------------
// name: main.cpp
// desc: ss entry point
//
// author: Micah Arvey (similar in style to Ge Wang's bokeh example)
// date: fall 2013
//----------------------------------------------------------------------------
#include <iostream>
#include "ss-audio.h"
#include "ss-gfx.h"
#include "ss-globals.h"
using namespace std;

//----------------------------------------------------------------------------
// name: main()
// desc: application entry point
//----------------------------------------------------------------------------
int main( int argc, const char ** argv )
{
    system( "pwd" );
    // invoke graphics setup and loop
    if( !ss_gfx_init( argc, argv ) )
    {
        // error message
        cerr << "[ss]: cannot initialize graphics/data system..." << endl;
        return -1;
    }
    
    // start real-time audio
    if( !ss_audio_init( SS_SRATE, SS_FRAMESIZE, SS_NUMCHANNELS ) )
    {
        // error message
        cerr << "[ss]: cannot initialize real-time audio I/O..." << endl;
        return -1;
    }
    
    // start audio
    if( !ss_audio_start() )
    {
        // error message
        cerr << "[ss]: cannot start real-time audio I/O..." << endl;
        return -1;
    }
    
    // graphics loop
    ss_gfx_loop();
    
    return 0;
}
