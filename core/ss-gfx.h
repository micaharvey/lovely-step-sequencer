//-----------------------------------------------------------------------------
// name: ss-gfx.h
// desc: graphics stuff for bokeh visualization
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//   date: 2013
//-----------------------------------------------------------------------------
#ifndef __SS_GFX_H__
#define __SS_GFX_H__

#include "x-def.h"
#include "x-gfx.h"
#include <string>


// entry point for graphics
bool ss_gfx_init( int argc, const char ** argv );
void ss_gfx_loop();
void ss_about();
void ss_keys();
void ss_help();
void ss_usage();
void ss_endline();
void ss_line();
bool ss_initTexture( const std::string & filename, XTexture * tex );
XTexture * ss_loadTexture( const std::string & filename );




#endif
