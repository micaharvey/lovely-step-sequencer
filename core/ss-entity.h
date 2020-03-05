//-----------------------------------------------------------------------------
// name: ss-entity.h
// desc: entities for visquin visualization
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//   date: 2013
//-----------------------------------------------------------------------------
#ifndef __SS_ENTITY_H__
#define __SS_ENTITY_H__
using namespace std;

#include "y-entity.h"
#include "x-buffer.h"
#include <vector>




//-----------------------------------------------------------------------------
// name: class SSSpark
// desc: ...
//-----------------------------------------------------------------------------
class SSSpark : public YEntity
{
public:
    // constructor
    SSSpark() : texture(0), ALPHA( 1, 1, 1 ) { }

public:
    // set
    void set( int _texture, float _size, float _alpha );
    void setTexture( int _texture );
    void setSize( float _size );
    
public:
    // update
    void update( YTimeInterval dt );
    // render
    void render();
    
public:
    // which ripple texture
    GLuint texture;
    // vertices
    GLfloat vertices[8];
    // alpha ramp
    Vector3D ALPHA;
};




//-----------------------------------------------------------------------------
// name: class SSTeapot
// desc: for testing
//-----------------------------------------------------------------------------
class SSTeapot : public YEntity
{
public:
    // update
    void update( YTimeInterval dt );
    // render
    void render();
};


//-----------------------------------------------------------------------------
// ADDED BY MICAH
// name: class SSCube
// desc: for grid of buttons (mainly adding location attribute)
//-----------------------------------------------------------------------------
class SSCube : public YEntity
{
public:
    SSCube(){
        size = Vector3D( 1, 1, 1.0f );
        position = Vector3D( 1, 1, 1 );
        ialpha = Vector3D(1,1,0.5);
    }

public:
    // set size
    void setSize( const Vector3D _size );
    // set position
    void setPosition(const Vector3D _position);
    void setAlpha(const Vector3D _ialpha);
    void showThenFade();

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    Vector3D size;
    Vector3D position;
    // alpha slew
    Vector3D ialpha;
};


class SSLineGrid : public YEntity
{
public:
    SSLineGrid(const unsigned int _size){
        size = _size; 
        center = Vector3D(0,0,0);
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    unsigned int size;
    Vector3D center;
};

class SSItem : public YEntity
{
public:
    SSItem(){
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:

};

class SSInventory : public YEntity
{
public:
    SSInventory( const Vector3D _topCenter ){
        loc = _topCenter;
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();
    virtual void addItem(SSItem item);

public:
    Vector3D loc;
    vector<SSItem> items;

};

class SSPlayPlace : public YEntity
{
public:
    SSPlayPlace( const Vector3D _center ){
        loc = _center;
        bool k = false;
        bool s = false; 
        bool h = false; 
        bool p = false;
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    Vector3D loc;
    bool k;
    bool s; 
    bool h;
    bool p;
};




#endif














