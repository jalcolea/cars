#include "iamanager.h"
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <climits>

iamanager::iamanager(int laps, vector <iapoint*> * path, int offset, float min_distance):_laps(laps),_offset(offset),_min_distance(min_distance),_path(path)
{
  iacomplexpoint * p;
  _fixed_x=false;
  _fixed_y=true;
  _fixed_z=false;

  srand(time(NULL));
  points = new vector <iacomplexpoint*>();
  int range=(offset*2)-1;
  for (int a=0;a<_laps;a++)
  {
    for (unsigned int c=0;c<_path->size();c++)
    {
      p=new iacomplexpoint();
      p->derived.x((!_fixed_x)?rand()%range+(*_path)[c]->x():(*_path)[c]->x());
      p->derived.y((!_fixed_y)?rand()%range+(*_path)[c]->y():(*_path)[c]->y());
      p->derived.z((!_fixed_z)?rand()%range+(*_path)[c]->z():(*_path)[c]->z());
      p->derived.userdata((*_path)[c]->userdata());
      p->base.x((*_path)[c]->x());
      p->base.y((*_path)[c]->y());
      p->base.z((*_path)[c]->z());
      p->base.distance((*_path)[c]->distance());
      p->check=false;
      points->push_back(p);
    }
  }
}
  
iacomplexpoint * iamanager::next ()
{
  iacomplexpoint * result = NULL;
  iacomplexpoint * node=NULL;
  for (unsigned int a=0;a<points->size();a++)
  {
    node=(*points)[a];
    if (!node->check)
    {
      node->check=true;
      if ((a+1)<points->size()) 
      {
        node = (*points)[a+1];
        result =new iacomplexpoint();
        result->derived.x(node->derived.x());
        result->derived.y(node->derived.y());
        result->derived.z(node->derived.z());
        result->base.x(node->base.x());
        result->base.y(node->base.y());
        result->base.z(node->base.z());
        result->check=node->check;
        break;
      }
    }
  }
  return result;
}

iacomplexpoint * iamanager::follow (iapoint * car)
{
  iacomplexpoint * result = NULL;
  iacomplexpoint * node=NULL;
  float dis = _min_distance;
  for (unsigned int a=0;a<points->size();a++)
  {
    node=(*points)[a];
    if (!node->check)
    {
      if (node->derived.distance()>0) 
          dis= node->derived.distance();
      if (distance(&node->derived,car) < dis)
      {
        node->check=true;
        if ((a+1)<points->size()) 
        {
          node = (*points)[a+1];
          result =new iacomplexpoint();
          result->derived.x(node->derived.x());
          result->derived.y(node->derived.y());
          result->derived.z(node->derived.z());
          result->base.x(node->base.x());
          result->base.y(node->base.y());
          result->base.z(node->base.z());
          result->check=node->check;
          break;
        }
      }
    }
  }
 
  return result;
}

double iamanager::near (iapoint * car, iacomplexpoint * result)
{
  double result_d=UINT_MAX;
  double d;
  iapoint * node=NULL;
  iacomplexpoint * nodecomplex=NULL;

  for (unsigned int a=0;a<points->size();a++)
  {
    node=&(*points)[a]->derived;
    nodecomplex=(*points)[a];
    d=distance(node,car);
    //cout << "NEXT NODE " << node->x() << ":" << node->y() << ":" << node->z() << " DISTANCE " << d << " SORTER " << result_d << endl;
    if ((d<result_d) && (d > _min_distance))
    {
      //cout << "STORED " << endl;
      result_d = d;
      result->base.x(nodecomplex->base.x());
      result->base.y(nodecomplex->base.y());
      result->base.z(nodecomplex->base.z());
      result->derived.x(nodecomplex->derived.x());
      result->derived.y(nodecomplex->derived.y());
      result->derived.z(nodecomplex->derived.z());
    }
  }
  return result_d;
}

void iamanager::paint_2d_path ()
{
 char  matrix[80][80];
 memset (matrix,' ',sizeof(matrix));
 for (unsigned int a=0;a<_path->size();a++) 
 {
    printf("%f,%f\n", (*_path)[a]->x(),(*_path)[a]->y());
    matrix [(int)(*_path)[a]->x()][(int)(*_path)[a]->y()]='X'; 
 }
 for (unsigned int a=0;a<80;a++) 
 {
   for (unsigned int b=0;b<80;b++) printf("%c",matrix[a][b]);
   printf("\n");
 }
}
void iamanager::print_points ()
{
 cout << "LAPS: "<< _laps << " OFFSET: " << _offset << " MIN DISTANCE " << _min_distance << endl;
 cout << "POINTS (base->derived)" << endl;

 for (unsigned int a=0;a<points->size();a++)
   // printf("%d,%d,%d->%d,%d,%d\n", (*points)[a]->base.x(),(*points)[a]->base.y(), (*points)[a]->base.z(),
    printf("%f.3,%f.3,%f.3->%f.3,%f.3,%f.3\n", (*points)[a]->base.x(),(*points)[a]->base.y(), (*points)[a]->base.z(),
    (*points)[a]->derived.x(),(*points)[a]->derived.y(), (*points)[a]->derived.z());
}

//int iamanager::distance(iapoint *from,iapoint *to)
float iamanager::distance(iapoint *from,iapoint *to)
{
  return sqrt(pow((from->x() - to->x()), 2) + pow((from->y() - to->y()), 2) + pow((from->z() - to->z()), 2));

}

void iamanager::fixed_coord(bool x, bool y, bool z)
{
  _fixed_x = x;
  _fixed_y = y;
  _fixed_z = z;
}

