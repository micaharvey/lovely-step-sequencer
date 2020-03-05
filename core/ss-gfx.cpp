//-----------------------------------------------------------------------------
// name: ss-gfx.cpp
// desc: graphics stuff for ss visualization
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//   date: 2013
//-----------------------------------------------------------------------------
#include "ss-gfx.h"
#include "ss-globals.h"
#include "ss-entity.h"
#include "ss-audio.h"

#include "x-fun.h"
#include "x-gfx.h"
#include "x-loadlum.h"
#include "x-vector3d.h"

#include <iostream>
#include <vector>
using namespace std;


// gfx globals
// YEntities
// Intro
YEntity g_introRoot;
vector<YBokeh *> g_bokehs;
YText * g_introText;

// Game
YEntity g_gameRoot;

// Line Grid
SSLineGrid * g_grid;

// hud
YEntity g_hud;

// max sim step size in seconds
#define SIM_SKIP_TIME (.25)


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void idleFunc();
void displayFunc();
void reshapeFunc( int width, int height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void specialFunc( int key, int x, int y );

void initialize_graphics();
void initialize_simulation();
bool initialize_data();
void loadTextures();
bool checkTexDim( int dim );
void setupTexture( const char * filename, GLuint texobj,
                  GLenum minFilter, GLenum maxFilter,
                  int useMipMaps = 0 );

void renderBackground();
void blendPane();
void updateNodeEntities();
void renderNodeEntities();



//-----------------------------------------------------------------------------
// name: ss_gfx_init( )
// desc: graphics/interaction entry point
//-----------------------------------------------------------------------------
bool ss_gfx_init( int argc, const char ** argv )
{
#ifdef __APPLE__
    // save working dir
    char * cwd = getcwd( NULL, 0 );
    // set path
    // #ifdef __VQ_RELEASE__
#if 1
    // HACK: use this hard-coded path
    Globals::path = "./";
#else
    // store this globally
    Globals::path = cwd;
#endif
    // compute the datapath
    Globals::datapath = Globals::path + Globals::relpath;
#endif
    
    // initialize GLUT
    glutInit( &argc, (char **)argv );
    
#ifdef __APPLE__
    //restore working dir
    chdir( cwd );
    free( cwd );
#endif
    
    // print about
    ss_about();
    ss_endline();

    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( Globals::windowWidth, Globals::windowHeight );
    // set the window postion
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow( "SS Anne");
    // full screen
    if( Globals::fullscreen )
        glutFullScreen();
    
    // set the idle function - called when idle
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );
    // for arrow keys, etc
	glutSpecialFunc (specialFunc );
    
    // do our own initialization
    initialize_graphics();
    // simulation
    initialize_simulation();
    // do data
    if( !initialize_data() )
    {
        // done
        return false;
    }
    
    // print keys
    ss_endline();
    ss_keys();
    ss_line();
    ss_endline();
    
    return true;
}




//-----------------------------------------------------------------------------
// name: ss_gfx_loop( )
// desc: hand off to graphics loop
//-----------------------------------------------------------------------------
void ss_gfx_loop()
{
    // let GLUT handle the current thread from here
    glutMainLoop();
}




//-----------------------------------------------------------------------------
// Name: initialize_graphics( )
// Desc: sets initial OpenGL states and initializes any application data
//-----------------------------------------------------------------------------
void initialize_graphics()
{
    // log
    cerr << "[ss]: initializing graphics system..." << endl;
    
    // reset time
    XGfx::resetCurrentTime();
    // set simulation speed
    XGfx::setDeltaFactor( 1.0f );
    // get the first
    XGfx::getCurrentTime( true );
    // random
    XFun::srand();
    
    // set the GL clear color - use when the color buffer is cleared
    glClearColor( Globals::bgColor.actual().x, Globals::bgColor.actual().y, Globals::bgColor.actual().z, 1.0f );
    // set the shading model to 'smooth'
    glShadeModel( GL_SMOOTH );
    // enable depth
    glEnable( GL_DEPTH_TEST );
    // set the front faces of polygons
    glFrontFace( GL_CCW );
    // set fill mode
    glPolygonMode( GL_FRONT_AND_BACK, Globals::fillmode );
    // enable lighting
    glEnable( GL_LIGHTING );
    // enable lighting for front
    glLightModeli( GL_FRONT_AND_BACK, GL_TRUE );
    // material have diffuse and ambient lighting
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    // enable color
    glEnable( GL_COLOR_MATERIAL );
    // normalize (for scaling)
    glEnable( GL_NORMALIZE );
    // line width
    glLineWidth( Globals::linewidth );
    
    // enable light 0
    glEnable( GL_LIGHT0 );
    
    // setup and enable light 1
    glLightfv( GL_LIGHT1, GL_AMBIENT, Globals::light1_ambient );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, Globals::light1_diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, Globals::light1_specular );
    glEnable( GL_LIGHT1 );
    
    // load textures
    loadTextures();
    
    // fog
    Globals::fog_mode[0] = 0;
    Globals::fog_mode[1] = GL_LINEAR;
    // fog_mode[1] = GL_EXP; fog_mode[2] = GL_EXP2;
    Globals::fog_filter = 0;
    Globals::fog_density = .04f;
    
    // fog color
    GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};
    // fog mode
    if( Globals::fog_filter ) glFogi(GL_FOG_MODE, Globals::fog_mode[Globals::fog_filter]);
    // set fog color
    glFogfv( GL_FOG_COLOR, fogColor );
    // fog density
    glFogf( GL_FOG_DENSITY, Globals::fog_density );
    // fog hint
    glHint( GL_FOG_HINT, GL_DONT_CARE );
    // fog start depth
    glFogf( GL_FOG_START, 1.0f );
    // fog end depth
    glFogf( GL_FOG_END, 10.5f );
    // enable
    if( Globals::fog_filter ) glEnable( GL_FOG );
    
    // check global flag
    if( Globals::fog )
    {
        // fog mode
        glFogi(GL_FOG_MODE, Globals::fog_mode[Globals::fog_filter]);
        // enable
        glEnable(GL_FOG);
    }
    else
    {
        // disable
        glDisable(GL_FOG);
    }
    
    // clear the color buffer once
    glClear( GL_COLOR_BUFFER_BIT );
}




//-----------------------------------------------------------------------------
// name: initialize_simulation( )
// desc: simulation setup
//-----------------------------------------------------------------------------
void initialize_simulation()
{

    // Intro
    // this part modded from  bokeh
    for( int i = 0; i < 1024; i++ )
    {
        // create a spark
        YBokeh * bokeh = new YBokeh();
        // set attributes
        bokeh->set( 1.0f, 1.0f, 1.0f, 1.0f, SS_TEX_FLARE_TNG_1 );
        // set bokeh
        bokeh->setBokehParams( // initial time
                               XFun::rand2f(0,10),
                               // freq
                               XFun::rand2f(1,3),
                               // time step
                               50,
                               // location
                               Vector3D(XFun::rand2f(-5,6),XFun::rand2f(-4,4), XFun::rand2f(-1,1)),
                               // color
                               Vector3D(XFun::rand2f(0,.1),XFun::rand2f(0,.2), XFun::rand2f(.3,.5)) );
        // alpha
        bokeh->setAlpha( .1 );
        // add to simulation
        g_introRoot.addChild( bokeh );
        g_bokehs.push_back( bokeh );
    }

    YText * loveText = new YText(1);
    loveText->set("lovely");
    loveText->setCenterLocation(Vector3D(.5,0,0));
    g_introRoot.addChild( loveText );
    g_introText = loveText;
    Globals::sim.addChild( & g_introRoot );


    // Game (16 is board size)
    g_grid = new SSLineGrid(16);
    g_gameRoot.addChild(g_grid);
    //playheads
    float cubeSquare = 1.16; // NOTE: seems to be radius
    for (int row = 0; row < 4; row++){
        for (int col = 0; col < 4; col++){
            // play place
            SSPlayPlace * play = new SSPlayPlace(Vector3D(cubeSquare*(-4.5+3*col), cubeSquare*(4.5-3*row), 0.2));
            g_gameRoot.addChild( play );
            Globals::playPlaces.push_back(play);

            // play heads
            SSCube * playhead = new SSCube();
            playhead->setSize(Vector3D(cubeSquare, cubeSquare, cubeSquare/4));
            playhead->setPosition(Vector3D(cubeSquare*(-4.5+3*col), cubeSquare*(4.5-3*row), -2));
            playhead->setAlpha(Vector3D(0,0,0));
            g_gameRoot.addChild( playhead );
            Globals::playheads.push_back( playhead );
        }
    }
    g_gameRoot.active = false;
    Globals::sim.addChild( & g_gameRoot );



    // HUD (its not really a hud, just doesn't belong to game)
    YText * hudTitle = new YText(1);
    hudTitle->set("Step Sequencer");
    hudTitle->setCenterLocation(Vector3D(-3,8.5,0));
    hudTitle->setWidth(3.0);
    hudTitle->sca.x = 10.0f;
    hudTitle->sca.y = 10.0f;
    g_hud.addChild(hudTitle);

    SSInventory * inv = new SSInventory(Vector3D(-10,4,0));

    YText * invTitle = new YText(1);
    invTitle->set("Inventory");
    invTitle->setCenterLocation(Vector3D(-3,0,0));
    invTitle->setWidth(3.0);
    invTitle->sca.x = 10.0f;
    invTitle->sca.y = 10.0f;
    inv->addChild(invTitle);

    YText * kickTitle = new YText(1);
    kickTitle->set("spacebar: Kick");
    kickTitle->setCenterLocation(Vector3D(-2.5,-2, 0.3));
    kickTitle->setWidth(3.0);
    kickTitle->sca.x = 5.0f;
    kickTitle->sca.y = 5.0f;
    inv->addChild(kickTitle);

    YText * snareTitle = new YText(1);
    snareTitle->set("j: Snare");
    snareTitle->setCenterLocation(Vector3D(-1,-4, 0.3));
    snareTitle->setWidth(3.0);
    snareTitle->sca.x = 5.0f;
    snareTitle->sca.y = 5.0f;
    inv->addChild(snareTitle);

    YText * hihatTitle = new YText(1);
    hihatTitle->set("f: HiHat");
    hihatTitle->setCenterLocation(Vector3D(-1,-6, 0.3));
    hihatTitle->setWidth(3.0);
    hihatTitle->sca.x = 5.0f;
    hihatTitle->sca.y = 5.0f;
    inv->addChild(hihatTitle);

    YText * rowTitle = new YText(1);
    rowTitle->set("zxcvbnm,.");
    rowTitle->setCenterLocation(Vector3D(-1,-8, 0.3));
    rowTitle->setWidth(3.0);
    rowTitle->sca.x = 5.0f;
    rowTitle->sca.y = 5.0f;
    inv->addChild(rowTitle);

    YText * pitchTitle = new YText(1);
    pitchTitle->set("Pitches");
    pitchTitle->setCenterLocation(Vector3D(-1,-8.5, 0.3));
    pitchTitle->setWidth(3.0);
    pitchTitle->sca.x = 5.0f;
    pitchTitle->sca.y = 5.0f;
    inv->addChild(pitchTitle);

    g_hud.addChild(inv);

    g_hud.active = false;
    Globals::sim.addChild( & g_hud );

  }

//-------------------------------------------------------------------------------
// name: systemCascade()
// desc: trigger system wide update with time steps and draws the scene
//-------------------------------------------------------------------------------
void systemCascade()
{
    // get current time (once per frame)
    XGfx::getCurrentTime( true );

    // Timing loop
    YTimeInterval timeElapsed = XGfx::getCurrentTime() - Globals::simTime;
    Globals::simTime += timeElapsed;
    
    // special case: first update
    if( Globals::first )
    {
        // set time just enough for one update
        timeElapsed = 1.0f/30.0f;
        // set flag
        Globals::first = false;
    }
    
    // clamp it
    if( timeElapsed > SIM_SKIP_TIME )
        timeElapsed = SIM_SKIP_TIME;
    
    // update it
    // check paused
    if( !Globals::isPaused )
    {
        // update the world with a fixed timestep
        Globals::sim.updateAll( timeElapsed );
    }
    
    // redraw
    Globals::sim.drawAll();
    
    // set
    Globals::lastDelta = timeElapsed;
}


//-----------------------------------------------------------------------------
// name: initialize_data( )
// desc: load initial data
//-----------------------------------------------------------------------------
bool initialize_data()
{
    return true;
}




//-----------------------------------------------------------------------------
// name: ss_about()
// desc: ...
//-----------------------------------------------------------------------------
void ss_about()
{
    ss_line();
    fprintf( stderr, "[ss]: Lovely Step Sequencer\n" );
    ss_line();
    fprintf( stderr, "   | author: Micah Arvey\n" );
    fprintf( stderr, "   | Stanford University | CCRMA\n" );
    fprintf( stderr, "   | http://ccrma.stanford.edu/~marvey/\n" );
    fprintf( stderr, "   | version: %s\n", Globals::version.c_str() );
}




//-----------------------------------------------------------------------------
// name: ss_keys()
// desc: ...
//-----------------------------------------------------------------------------
void ss_keys()
{
    ss_line();
    fprintf( stderr, "[ss]: controls \n" );
    ss_line();
    fprintf( stderr, "  'q' - quit\n" );
    fprintf( stderr, "  's' - toggle fullscreen\n" );
    fprintf( stderr, "  'h' - print this help message\n" );
    fprintf( stderr, "  'a' - print about statement\n" );
    fprintf( stderr, "  '[' and ']' - rotate automaton\n" );
    fprintf( stderr, "  '-' and '+' - zoom away/closer to center of automaton\n" );
    fprintf( stderr, "  'f' - Place Hihat\n" );
    fprintf( stderr, "  'j' - Place Snare\n" );
    fprintf( stderr, "  'spacebar' - Place Kick\n" );
    fprintf( stderr, "  'd' - clear current beat\n" );
    fprintf( stderr, "  'D' - clear all beats\n" );
    fprintf( stderr, "  'zxcvbnm,.' - bottom row of keyboard for pitched sound\n" );
    
}




//-----------------------------------------------------------------------------
// name: ss_help()
// desc: ...
//-----------------------------------------------------------------------------
void ss_help()
{
    ss_endline();
    ss_keys();
    ss_endline();
}




//-----------------------------------------------------------------------------
// name: ss_usage()
// desc: ...
//-----------------------------------------------------------------------------
void ss_usage()
{
    ss_line();
    fprintf( stderr, "[ss]: no command line arguments\n" );
    ss_line();

}




//-----------------------------------------------------------------------------
// name: ss_endline()
// desc: ...
//-----------------------------------------------------------------------------
void ss_endline()
{
    fprintf( stderr, "\n" );
}




//-----------------------------------------------------------------------------
// name: ss_line()
// desc: ...
//-----------------------------------------------------------------------------
void ss_line()
{
    fprintf( stderr, "---------------------------------------------------------\n" );
}




//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( int w, int h )
{
    // save the new window size
    Globals::windowWidth = w; Globals::windowHeight = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
}




//-----------------------------------------------------------------------------
// Name: look( )
// Desc: ...
//-----------------------------------------------------------------------------
void look( )
{
    // go
    Globals::fov.interp( XGfx::delta() );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    gluPerspective( Globals::fov.value, (GLfloat)Globals::windowWidth / (GLfloat)Globals::windowHeight, .005, 500.0 );
    
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity();
    // position the view point
    gluLookAt( 0.0f,
              Globals::viewRadius.x * sin( Globals::viewEyeY.x ),
              Globals::viewRadius.x * cos( Globals::viewEyeY.x ),
              0.0f, 0.0f, 0.0f,
              0.0f, ( cos( Globals::viewEyeY.x ) < 0 ? -1.0f : 1.0f ), 0.0f );
    
    // set the position of the lights
    glLightfv( GL_LIGHT0, GL_POSITION, Globals::light0_pos );
    glLightfv( GL_LIGHT1, GL_POSITION, Globals::light1_pos );
}


//-----------------------------------------------------------------------------
// added by micah
// end welcome 'lovely' screen
//-----------------------------------------------------------------------------
void ss_endWelcome(){
    // end the welcome
    Globals::isWelcome = FALSE;
    // zoom out
    Globals::viewRadius.y = 100;
    // flip it for real
    Globals::viewEyeY.y -= 13.1f;

}



//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    // Quit and Screen resize are most important
    switch( key )
    {
        case 'q':
        {
            ss_endline();
            ss_line();
            ss_endline();
            exit(0);
        }
        case 's':
        {
            if( !Globals::fullscreen )
            {
                Globals::lastWindowWidth = Globals::windowWidth;
                Globals::lastWindowHeight = Globals::windowHeight;
                glutFullScreen();
            }
            else
                glutReshapeWindow( Globals::lastWindowWidth, Globals::lastWindowHeight );
            
            Globals::fullscreen = !Globals::fullscreen;
            //fprintf( stderr, "[ss]: fullscreen:%s\n", Globals::fullscreen ? "ON" : "OFF" );
            return;
        }   
        case 'a':
        {
            ss_endline();
            ss_about();
            ss_endline();
            return;
        }
        case 'h':
        {
            ss_help();
            return;
        }
    }
    
    // Then check to end intro
    if(Globals::isWelcome){
        ss_endWelcome();
        return;
    }

    // check if something else is handling viewing
    bool handled = false;

    // post visualizer handling (if not handled
    if( !handled )
    {
        switch( key )
        {
            case 'D': //delete all
                for (int i = 0; i < 16; ++i){
                    Globals::drumPitchVecs[i].clear();
                    Globals::pitchVecs[i].clear();
                    //allNotesOff( int channel );
                }
                break;
            case 'd': //delete
                Globals::drumPitchVecs[Globals::beats%16].clear();
                Globals::pitchVecs[Globals::beats%16].clear();
                break;
            case 32: //spacebar
                Globals::drumPitchVecs[Globals::beats%16].push_back(SS_KICK);
                break;
            case 'f':
                Globals::drumPitchVecs[Globals::beats%16].push_back(SS_HIHAT);
                break;
            case 'j':
                Globals::drumPitchVecs[Globals::beats%16].push_back(SS_SNARE);
                break;
            case ']':
                Globals::viewEyeY.y -= .1f;
                //fprintf( stderr, "[vismule]: yview:%f\n", g_eye_y.y );
                break;
            case '[':
                Globals::viewEyeY.y += .1f;
                //fprintf( stderr, "[vismule]: yview:%f\n", g_eye_y.y );
                break;
            case '=':
                Globals::viewRadius.y = .975 * Globals::viewRadius.y;
                if( Globals::viewRadius.y < .001 ) Globals::viewRadius.y = .001;
                // fprintf( stderr, "[vismule]: view radius:%f->%f\n", Globals::viewRadius.x, Globals::viewRadius.y );
                break;
            case '-':
                Globals::viewRadius.y = 1.025 * Globals::viewRadius.y;
                // fprintf( stderr, "[vismule]: view radius:%f->%f\n", Globals::viewRadius.x, Globals::viewRadius.y );
                break;
            case '_':
            case '+':
                Globals::viewRadius.y = Globals::viewRadius.x + .7*(Globals::viewRadius.y-Globals::viewRadius.x);
                break;
        }

        //
        switch( key )
        {
            case 'z':
                Globals::pitchVecs[Globals::beats%16].push_back(55);
                break;
            case 'x':
                Globals::pitchVecs[Globals::beats%16].push_back(57);
                break;
            case 'c':
                Globals::pitchVecs[Globals::beats%16].push_back(59);
                break;
            case 'v':
                Globals::pitchVecs[Globals::beats%16].push_back(60);
                break;
            case 'b':
                Globals::pitchVecs[Globals::beats%16].push_back(62);
                break;
            case 'n':
                Globals::pitchVecs[Globals::beats%16].push_back(64);
                break;
            case 'm':
                Globals::pitchVecs[Globals::beats%16].push_back(65);
                break;
            case ',':
                Globals::pitchVecs[Globals::beats%16].push_back(67);
                break;
            case '.':
                Globals::pitchVecs[Globals::beats%16].push_back(69);
                break;
        }
    }
    
    // do a reshape since viewEyeY might have changed
    reshapeFunc( Globals::windowWidth, Globals::windowHeight );
    // post redisplay
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: specialFunc( )
// Desc: handles arrow stuff
//-----------------------------------------------------------------------------
void specialFunc( int key, int x, int y )
{
    // Check Welcome first
    if(Globals::isWelcome){
        ss_endWelcome();
        return;
    }

    // check
    bool handled = false;
    
    // if not handled
    if( !handled )
    {
        switch( key )
        {
            case GLUT_KEY_LEFT:
                break;
            case GLUT_KEY_RIGHT:
                break;
            case GLUT_KEY_UP:
                break;
            case GLUT_KEY_DOWN:
                break;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}

//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{
    // get current time (once per frame)
    XGfx::getCurrentTime( true );

    // update
    Globals::bgColor.interp( XGfx::delta() );
    Globals::blendAlpha.interp( XGfx::delta() );
    
    // clear or blend
    if( Globals::blendScreen && Globals::blendAlpha.value > .0001 )
    {
        // clear the depth buffer
        glClear( GL_DEPTH_BUFFER_BIT );
        // blend screen
        blendPane();
    }
    else
    {
        // set the GL clear color - use when the color buffer is cleared
        glClearColor( Globals::bgColor.actual().x, Globals::bgColor.actual().y, Globals::bgColor.actual().z, 1.0f );
        // clear the color and depth buffers
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
    
    // enable depth test
    glEnable( GL_DEPTH_TEST );
    
    // save state
    glPushMatrix();
    
    // slew
    Globals::viewEyeY.interp( XGfx::delta());
    Globals::viewRadius.interp( XGfx::delta() );
    look();
    
    // cascade simulation
    systemCascade();
    
    // pop state
    glPopMatrix();
    

    // ADDED FOR GAME START (AFTER FIRST BUTTON PRESS) beforeGame->false, start the game
    if(g_introRoot.active && Globals::viewRadius.value > 75){
        g_gameRoot.active = true;
        g_hud.active = true;
        g_introRoot.active = false;
        Globals::viewRadius.y = 10.0f;
        Globals::viewEyeY.y   = 0.0f;
    }
    
    // flush gl commands
    glFlush();
    // swap the buffers
    glutSwapBuffers();
}




//-----------------------------------------------------------------------------
// name: blendPane()
// desc: blends a pane into the current scene
//-----------------------------------------------------------------------------
void blendPane()
{
    // enable blending
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // disable lighting
    glDisable( GL_LIGHTING );
    // disable depth test
    glDisable( GL_DEPTH_TEST );
    // blend in a polygon
    glColor4f( Globals::bgColor.actual().x, Globals::bgColor.actual().y, Globals::bgColor.actual().z, Globals::blendAlpha.value );
    // glColor4f( Globals::blendRed, Globals::blendRed, Globals::blendRed, Globals::blendAlpha );
    // reduce the red component
    // Globals::blendRed -= .02f;
    // if( Globals::blendRed < 0.0f ) Globals::blendRed = 0.0f;
    
    GLfloat h = 10;
    GLfloat d = -1;
    
    // draw the polyg
    glBegin( GL_QUADS );
    glVertex3f( -h, -h, d );
    glVertex3f( h, -h, d );
    glVertex3f( h, h, d );
    glVertex3f( -h, h, d );
    glEnd();
    
    // enable lighting
    glEnable( GL_LIGHTING );
    // enable depth test
    glEnable( GL_DEPTH_TEST );
    // disable blending
    glDisable( GL_BLEND );
}




//-----------------------------------------------------------------------------
// name: renderBackground()
// desc: ...
//-----------------------------------------------------------------------------
void renderBackground()
{
    // save the current matrix
    glPushMatrix( );
    
    // restore
    glPopMatrix( );
}




//-------------------------------------------------------------------------------
// name: loadTexture()
// desc: load textures
//-------------------------------------------------------------------------------
void loadTextures()
{
    char filename[256];
    GLenum minFilter, maxFilter;
    int i;
    
    // log
    fprintf( stderr, "[ss]: loading textures...\n" );
    
    // set store alignment
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    
    // set filter types
    minFilter = GL_LINEAR;
    maxFilter = GL_LINEAR;
    
    // load tng flares
    for( i = SS_TEX_FLARE_TNG_1; i <= SS_TEX_FLARE_TNG_5; i++ )
    {
        glGenTextures( 1, &(Globals::textures[i]) );
        sprintf( filename, "%sflare-tng-%d.bw", Globals::datapath.c_str(), i - SS_TEX_FLARE_TNG_1 + 1 );
        setupTexture( filename, Globals::textures[i], minFilter, maxFilter );
    }
}



//--------------------------------------------------------------------------------
// name: setupTexture()
// desc: ...
//--------------------------------------------------------------------------------
void setupTexture( const char * filename, GLuint texobj,
                  GLenum minFilter, GLenum maxFilter, int useMipMaps )
{
    unsigned char * buf = NULL;
    int width = 0, height = 0, components = 0;
    
    glBindTexture( GL_TEXTURE_2D, texobj );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter );
    
    // log
    // fprintf( stderr, "[ss]: loading %s...\n", filename );
    
    // load luminance
    buf = loadLuminance( filename, &width, &height, &components );
    
    // log
    // fprintf( stderr, "[ss]: '%s' : %dx%dx%d\n", filename, width, height, components);
    
    // build mip maps
    if( useMipMaps )
    {
        gluBuild2DMipmaps( GL_TEXTURE_2D, 1, width, height,
                          GL_LUMINANCE, GL_UNSIGNED_BYTE, buf );
    }
    else
    {
        glTexImage2D( GL_TEXTURE_2D, 0, 1, width, height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, buf );
    }
    
    free(buf);
}




//-----------------------------------------------------------------------------
// name: checkTexDim( )
// desc: checks to see if a dim is a valid opengl texture dimension
//-----------------------------------------------------------------------------
bool checkTexDim( int dim )
{
    if( dim < 0 ) 
        return false;
    
    int i, count = 0;
    
    // count bits
    for( i = 0; i < 31; i++ )
        if( dim & ( 0x1 << i ) )
            count++;
    
    // this is true only if dim is power of 2
    return count == 1;
}
