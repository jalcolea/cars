#include <iostream>
#include "iapoint.h"
#include <vector>


#ifndef _IAMAN_
#define _IAMAN_

#define MIN_DISTANCE 0.2
using namespace std;
class iamanager
{
  protected:
  int _laps;
  int _offset;
  float _min_distance;
  std::vector <iapoint*>* _path;
  std::vector <iacomplexpoint*> * points;
  bool _fixed_x;
  bool _fixed_y;
  bool _fixed_z;

  private:
  iamanager(){};

  public:

  iamanager(int laps, std::vector <iapoint*> * path, int offset, float min_distance=MIN_DISTANCE);
  int lap(){return _laps;}
  int offset(){return _offset;}
  float min_distance(){return _min_distance;}
  void min_distance(float m) {_min_distance=m;} 
  iacomplexpoint* follow (iapoint * car);
  double next (iapoint * car, iacomplexpoint * result);
  void paint_2d_path ();
  void print_points ();

  //int distance(iapoint *from,iapoint *to);
  float distance(iapoint *from,iapoint *to);
  void fixed_coord(bool x, bool y, bool z);
};

#endif
