#include <iostream>

#ifndef _IAPOINT_
#define _IAPOINT_
using namespace std;
class iapoint
{
  protected:
  int _x;
  int _y;
  int _z;

  public:
  iapoint(){};
  iapoint(int x,int y,int z):_x(x),_y(y),_z(z){};

  int x() { return _x;}
  int y() { return _y;}
  int z() { return _z;}
  void x(int n) {_x=n;}
  void y(int n) {_y=n;}
  void z(int n) {_z=n;}
  void xyz(int x,int y,int z){_x=y;_y=y;_z=z;}
};


typedef struct iacomplexpoint
{
  iapoint base;
  iapoint derived;
  bool check;
} iacomplexpoint;

#endif
