//-----------------------------------------------------------------------------
// name: ss-globals.h
// desc: global stuff for ss visualization
//
// author: Micah
//   date: 2014
//-----------------------------------------------------------------------------
#ifndef __SS_GLOBALS_H__
#define __SS_GLOBALS_H__


#include "x-def.h"
#include "x-audio.h"
#include "x-gfx.h"
#include "x-vector3d.h"
#include "y-fluidsynth.h"
#include "y-waveform.h"
#include "ss-entity.h"
using namespace std;

// c++
#include <string>
#include <map>
#include <vector>
#include <utility>

// defines
#define SS_SRATE        44100
#define SS_FRAMESIZE    256
#define SS_NUMCHANNELS  2
#define SS_MAX_TEXTURES 32

#define SS_KICK  35 
#define SS_SNARE 39
#define SS_HIHAT 44


//-----------------------------------------------------------------------------
// name: enum BokehTextureNames
// desc: texture names
//-----------------------------------------------------------------------------
enum BokehTextureNames
{
    SS_TEX_NONE = 0,
    SS_TEX_FLARE_TNG_1,
    SS_TEX_FLARE_TNG_2,
    SS_TEX_FLARE_TNG_3,
    SS_TEX_FLARE_TNG_4,
    SS_TEX_FLARE_TNG_5
};


// forward reference
class SSSim;




//-----------------------------------------------------------------------------
// name: class Globals
// desc: the global class
//-----------------------------------------------------------------------------
class Globals
{
public:
    // top level root simulation
    static YEntity sim;
    // current "now"
    static YTimeInterval simTime;
    // the delta time between last two render frames
    static YTimeInterval lastDelta;
    // is this the first cascade?
    static bool first;
    // are we paused?
    static bool isPaused;

    // Now
    static double now;
    // our main sequence
    static vector< vector< int > > pitchVecs;
    // our drum sequence
    static vector< vector< int > > drumPitchVecs;
    // global beats counter
    static unsigned long beats;

    static vector<SSCube *> playheads;
    static vector<SSPlayPlace *> playPlaces;

    static YFluidSynth * soloSynth;


    // path
    static std::string path;
    // path to datapath
    static std::string relpath;
    // datapath
    static std::string datapath;
    // version
    static std::string version;

    // welcome screen and Scene info
    static bool isWelcome;
    static int endWelcomeTime;
    static bool beforeZoom;
    static bool beforeGame;

    // width and height of the window
    static GLsizei windowWidth;
    static GLsizei windowHeight;
    static GLsizei lastWindowWidth;
    static GLsizei lastWindowHeight;

    // graphics fullscreen
    static GLboolean fullscreen;
    // render waveform
    static GLboolean renderWaveform;
    // blend pane instead of clearing screen
    static GLboolean blendScreen;
    // blend screen parameters
    static Vector3D blendAlpha;
    static GLfloat blendRed;
    // fill mode
    static GLenum fillmode;
    // background color
    static iSlew3D bgColor;
    // view stuff
    static Vector3D viewRadius;
    static Vector3D viewEyeY;
    static Vector3D fov;
    
    // textures
    static GLuint textures[];
    
    // light 0 position
    static GLfloat light0_pos[4];
    // light 1 parameters
    static GLfloat light1_ambient[4];
    static GLfloat light1_diffuse[4];
    static GLfloat light1_specular[4];
    static GLfloat light1_pos[4];
    // line width
    static GLfloat linewidth;
    // do
    static GLboolean fog;
    static GLuint fog_mode[4];   // storage for three/four types of fog
    static GLuint fog_filter;    // which fog to use
    static GLfloat fog_density;  // fog density
    
    // colors
    static Vector3D ourWhite;
    static Vector3D ourRed;
    static Vector3D ourBlue;
    static Vector3D ourOrange;
    static Vector3D ourGreen;
    static Vector3D ourGray;
    static Vector3D ourYellow;
    static Vector3D ourSoftYellow;
    static Vector3D ourPurple;
};




#endif
