#include <iostream>
#include "iapoint.h"
#include <vector>


#ifndef _IAMAN_
#define _IAMAN_

#define MIN_DISTANCE 2
using namespace std;
class iamanager
{
  protected:
  int _laps;
  int _offset;
  int _min_distance;
  vector <iapoint*>* _path;
  vector <iacomplexpoint*> * points;

  private:
  iamanager(){};

  public:
  iamanager(int laps, vector <iapoint*> * path, int offset, int min_distance=MIN_DISTANCE);
  int lap(){return _laps;}
  int offset(){return _offset;}
  int min_distance(){return _min_distance;}
  void min_distance(int m) {_min_distance=m;} 
  iacomplexpoint* follow (iapoint * car);
  double next (iapoint * car, iacomplexpoint * result);
  void paint_2d_path ();
  void print_points ();

  //int distance(iapoint *from,iapoint *to);
  float distance(iapoint *from,iapoint *to);
};

#endif
