#include <iostream>

#ifndef _IAPOINT_
#define _IAPOINT_
using namespace std;
class iapoint
{
  protected:
  float _x;
  float _y;
  float _z;

  public:
  iapoint(){};
  iapoint(float x,float y,float z):_x(x),_y(y),_z(z){};

  float x() { return _x;}
  float y() { return _y;}
  float z() { return _z;}
  void x(float n) {_x=n;}
  void y(float n) {_y=n;}
  void z(float n) {_z=n;}
  void xyz(float x,float y,float z){_x=y;_y=y;_z=z;}
};


typedef struct iacomplexpoint
{
  iapoint base;
  iapoint derived;
  bool check;
} iacomplexpoint;

#endif
