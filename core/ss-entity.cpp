//-----------------------------------------------------------------------------
// name: ss-entity.cpp
// desc: entities for bokeh visualization
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//   date: 2013
//-----------------------------------------------------------------------------
#include "ss-entity.h"
#include "ss-globals.h"
#include "x-fun.h"
#include <cmath> 
using namespace std;


// texture coordinates
static const GLshort g_coord[ ] = { 0, 0, 1, 0, 0, 1, 1, 1 };

//-----------------------------------------------------------------------------
// name: g_squareVertices
// desc: vertices for a cube
//-----------------------------------------------------------------------------
static const GLfloat g_squareVertices[] = 
{
    // FRONT
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    // BACK
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    // LEFT
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    // RIGHT
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    // TOP
    -0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    // BOTTOM
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,
};




//-----------------------------------------------------------------------------
// name: g_squareNormals
// desc: normals for a cube
//-----------------------------------------------------------------------------
static const GLfloat g_squareNormals[] = 
{
    // FRONT
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    // BACK
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    // LEFT
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    // RIGHT
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    // TOP
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    // BOTTOM
    0, -1, 0,
    0, -1, 0,
    0, -1, 0,
    0, -1, 0
};



//-----------------------------------------------------------------------------
// name: set()
// desc: start with this
//-----------------------------------------------------------------------------
void SSSpark::set( int _texture, float _size, float _alpha )
{
    setTexture( _texture );
    setSize( _size );
    alpha = _alpha;
}




//-----------------------------------------------------------------------------
// name: setSize()
// desc: set size
//-----------------------------------------------------------------------------
void SSSpark::setSize( float _size )
{
    _size *= 0.5f;
    vertices[0] = -_size; vertices[1] = -_size;
    vertices[2] = _size; vertices[3] = -_size;
    vertices[6] = _size; vertices[7] = _size;
    vertices[4] = -_size; vertices[5] = _size;
}




//-------------------------------------------------------------------------------
// name: setTexture()
// desc: ...
//-------------------------------------------------------------------------------
void SSSpark::setTexture( int _texture )
{
    // set it
    texture = _texture;
}



//-------------------------------------------------------------------------------
// name: update()
// desc: ...
//-------------------------------------------------------------------------------
void SSSpark::update( YTimeInterval dt )
{
    // slew
    ALPHA.interp( dt );
    // set
    this->alpha = ALPHA.value;
}




//-------------------------------------------------------------------------------
// name: render()
// desc: ...
//-------------------------------------------------------------------------------
void SSSpark::render( )
{
    // disable depth
    glDisable( GL_DEPTH_TEST );
    // enable texture
    glEnable( GL_TEXTURE_2D );
    // set blend function
    glBlendFunc( GL_ONE, GL_ONE );
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // enable blend
    glEnable( GL_BLEND );

    // bind to texture
    glBindTexture( GL_TEXTURE_2D, Globals::textures[this->texture] );

    // set color
    // glColor4f( col.x, col.y, col.z, alpha * ALPHA.value );

    // set vertex coordinates
    glVertexPointer( 2, GL_FLOAT, 0, this->vertices );
    glEnableClientState( GL_VERTEX_ARRAY );

    // set texture coordinates
    glTexCoordPointer( 2, GL_SHORT, 0, g_coord );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // draw stuff!
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // disable texture
    glDisable( GL_TEXTURE_2D );
    // disable blend
    glDisable( GL_BLEND );

    // disable client states
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}




//-------------------------------------------------------------------------------
// name: update()
// desc: ...
//-------------------------------------------------------------------------------
void SSTeapot::update( YTimeInterval dt )
{
    // do nothing for now
}




//-------------------------------------------------------------------------------
// name: render()
// desc: ...
//-------------------------------------------------------------------------------
void SSTeapot::render()
{
    // enable lighting
    glEnable( GL_LIGHTING );
    // set color
    glColor4f( col.x, col.y, col.z, alpha );
    // render stuff
    glutSolidTeapot( 1.0 );
    // disable lighting
    glDisable( GL_LIGHTING );
}


//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void SSCube::render()
{
    // push
    glPushMatrix();

    // scale
    glScalef( size.x, size.y, size.z );
    // cout << size.x << endl;
    
    // translate (added)
    glTranslatef( position.x, position.y, position.z);

    // draw it
    glutSolidCube(2.7);
    
    // pop
    glPopMatrix();

}




//-----------------------------------------------------------------------------
// name: update()
// desc: ...
//-----------------------------------------------------------------------------
void SSCube::update( YTimeInterval dt )
{
    // interp
    size.interp( dt );
    ialpha.interp( dt );
    // set it
    alpha = ialpha.value;
}

void SSCube::setAlpha(const Vector3D _ialpha)
{
    ialpha = _ialpha;
}

void SSCube::showThenFade()
{
    ialpha = Vector3D(1,0,5);
}


//-----------------------------------------------------------------------------
// name: setSize
// desc: ...
//-----------------------------------------------------------------------------
void SSCube::setSize( const Vector3D _size )
{
    // set size
    size = _size;
}


//-----------------------------------------------------------------------------
// name: pos()
// desc: ...
//-----------------------------------------------------------------------------
void SSCube::setPosition( const Vector3D _position )
{
    // set posish
    position = _position;
}

void SSLineGrid::update( YTimeInterval dt )
{
    // do nothing
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void SSLineGrid::render(  )
{

    // draw 'plus' at each diagonal point
    for (int row = 0; row <= size; row++){
        for (int col = 0; col <= size; col++){
            if(row!=col) continue;
            //color
            if(row == size/2.0 && col == size/2.0)
                glColor3f( 1,1,1 );
            else if (abs(row-size/2.0) == size/4.0 || abs(col-size/2.0) == size/4.0)
                glColor3f(0.5,0.9,0.5);
            else
                glColor3f( 0.1, 0.5, 0.1 );
            // vertical
            glBegin( GL_LINE_STRIP );
                glVertex2f( col-size/2.0, 0.0-size/2.0 );
                glVertex2f( col-size/2.0, 0.0+size/2.0 );
            glEnd();
            // horizontal
            glBegin( GL_LINE_STRIP );
                glVertex2f( 0.0-size/2.0, row -size/2.0 );
                glVertex2f( 0.0+size/2.0, row -size/2.0 );
            glEnd();
        }
    }

    //debug
    //glutSolidTeapot(1);
}

void SSInventory::update( YTimeInterval dt ){
    //nada
}

void SSInventory::render( ){

    glTranslatef( loc.x, loc.y, loc.z);

    glPushMatrix();
    glColor3f( 0.7, 0.1, 0.1 );
    glTranslatef( 0, -2, 0);
    glutSolidCube(0.5);
    glPopMatrix();


    glPushMatrix();
    glColor3f( 0.3, 0.3, 0.9 );
    glTranslatef( 0, -4, 0);
    glutSolidCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glColor3f( 0.7, 0.7, 0.1 );
    glTranslatef( 0, -6, 0);
    glutSolidCube(0.5);
    glPopMatrix();


    glPushMatrix();
    glColor3f( 0.7, 0.8, 0.6 );
    glTranslatef( 0, -8, 0);
    glutSolidCube(0.5);
    glPopMatrix();
    
}

void SSInventory::addItem( SSItem item ){
    //nada
}

void SSItem::update( YTimeInterval dt ){
    //nada
}

void SSItem::render( ){
    //nada
}

void SSPlayPlace::update( YTimeInterval dt ){

}

void SSPlayPlace::render( ){

    // print for debug
    // glPushMatrix();
    // glColor3f( 0.1, 0.5, 0.1 );
    // glTranslatef( loc.x, loc.y, loc.z);
    // glutSolidCube(1);
    // glPopMatrix();

    if(k){
        glPushMatrix();

        glColor3f( 0.7, 0.1, 0.1 );
        glTranslatef( loc.x, loc.y-0.5, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
    if(s){
        glPushMatrix();

        glColor3f( 0.3, 0.3, 0.9 );
        glTranslatef( loc.x+0.25, loc.y, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
    if(h){
        glPushMatrix();

        glColor3f( 0.7, 0.7, 0.1 );
        glTranslatef( loc.x-0.25, loc.y, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
    if(p){
        glPushMatrix();

        glColor3f( 0.7, 0.8, 0.6 );
        glTranslatef( loc.x, loc.y+0.5, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
}






