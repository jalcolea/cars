#include <iostream>
#include "Ogre.h"

#ifndef _IAPOINT_
#define _IAPOINT_
using namespace std;

class iapoint
{
  protected:
  float _x;
  float _y;
  float _z;
  float _distance;
  void * _userdata;
  public:
  iapoint(){};
  iapoint(float x,float y,float z, float distance=0, void * userdata=NULL):_x(x),_y(y),_z(z),_distance(distance),_userdata(userdata){};

  float x() { return _x;}
  float y() { return _y;}
  float z() { return _z;}
  float distance () {return _distance;}
  void * userdata(){return _userdata;}

  void x(float n) {_x=n;}
  void y(float n) {_y=n;}
  void z(float n) {_z=n;}
  void xyz(float x,float y,float z){_x=y;_y=y;_z=z;}
  void distance (float n) {_distance=n;}
  void userdata (void * n) {_userdata=n;}
};

typedef struct point_data 
{
  float speed_factor;
} point_data;

typedef struct iacomplexpoint
{
  iapoint base;
  iapoint derived;
  bool check;
} iacomplexpoint;

// PARA EL USERPOINTER DE btCollisionObject*, HAY QUE VER DONDE IRÍA MEJOR ESTO O SI SE PUEDE APROVECHAR OTRA ESTRUCTURA
//struct CheckPoint_data
//{
//    CheckPoint_data(size_t id, string nombre, Ogre::Vector3 posicion = Ogre::Vector3::ZERO):_id(id),_nombre(nombre),_worldPosition(posicion){}
//    size_t _id;
//    string _nombre;
//    Ogre::Vector3 _worldPosition;
//};

#endif
