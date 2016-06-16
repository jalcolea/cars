#include <iostream>
#include "iapoint.h"
#include <vector>
#include "OgreVector3.h"


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

  //constructor
  // laps: numero de vueltas
  // path: ruta creada con el editor de puntos
  // offset: rango de aleatoriedad a la hora de generar la ruta alternativa
  // distance: distancia minima a la que un punto se considera superado
  iamanager(int laps, std::vector <iapoint*> * path, int offset, float min_distance=MIN_DISTANCE);


  // devuelve el siguiente punto a superar, la ia considera un punto superado cuando el coche se acerca a menos de la distancia minima fijada 
  iacomplexpoint* follow (iapoint * car);

  // devuelve el siguiente punto desde la ultima llamada a esta funcion independientemente de que el punto haya sido superado o no.
  iacomplexpoint* next ();
  
  // devuelve simplemente un punto de la lista de puntos que se maneja actualmente. 
  inline iacomplexpoint* getPoint(size_t id){ return (*points)[id]; };

  // devuelve el punto mas cercano a la posicion actual del coche
  double near (iapoint * car, iacomplexpoint * result);
  int lap(){return _laps;}
  int offset(){return _offset;}
  float min_distance(){return _min_distance;}
  void min_distance(float m) {_min_distance=m;} 

  void paint_2d_path ();
  void print_points ();
  
  inline Ogre::Vector3 vec(iapoint point) { return Ogre::Vector3(point.x(),point.y(),point.z()); };

  //int distance(iapoint *from,iapoint *to);
  float distance(iapoint *from,iapoint *to);
  void fixed_coord(bool x, bool y, bool z);
  
  // un getter de los complexpoints
  std::vector <iacomplexpoint*> * getVectorPtrPoints(){ return points; };
};

#endif
