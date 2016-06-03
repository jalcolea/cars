#include <unistd.h>
#include "iamanager.h"
#include <cstdio>

int main (int argc, char * argv[])
{
  // vector con los puntos reales de la ruta a seguir
  vector <iapoint*> path; 

  // punto 3d donde se ubica el coche
  iapoint car(0,0,0);

  // tipo complejo donde el api devuelve los puntos derivados (derived) y una referencia al original en el que cada punto se basa (base)
  iacomplexpoint *next;

  // distancia que va devolviendo el api
  double distance;

  //anadido de puntos reales al vector
  path.push_back(new iapoint (1,1,1));
  path.push_back(new iapoint (15,1,1));
  path.push_back(new iapoint (30,1,1));
  path.push_back(new iapoint (45,1,1));
  path.push_back(new iapoint (60,1,1));
  path.push_back(new iapoint (60,10,1));
  path.push_back(new iapoint (60,20,1));
  path.push_back(new iapoint (60,30,1));
  path.push_back(new iapoint (60,40,1));
  path.push_back(new iapoint (45,40,1));
  path.push_back(new iapoint (30,40,1));
  path.push_back(new iapoint (15,40,1));
  path.push_back(new iapoint (1,40,1));
  path.push_back(new iapoint (1,30,1));
  path.push_back(new iapoint (1,20,1));
  path.push_back(new iapoint (1,10,1));

  //construccion del objecto ia, con numero de vueltas, vector de puntos y offset para el random
  //el constructor permite un cuarto parametro min_distance que tiene un valor por defecto a un Define que 
  //esta en la cabacera iamanager.h
  iamanager ia (3, &path,2);

  //pinta la imagen de los puntos en 2d , solo coordenadas x e y
  //cout << "PAINT 2D POINTS" << endl;
  //ia.paint_2d_path();

  //enumera los puntos 3d, (x,y,z)
  cout << "PRINT 3D COORDENATES" << endl;
  ia.print_points();


  // comprobacion metodo follow, este metodo va devolviendo la referencia al siguiente punto hasta que se pasa por el a una distancia menor que min_distance
  //printf("TESTING FOLLOW METHOD: CAR %d:%d:%d\n",car.x(),car.y(),car.z());
  printf("TESTING FOLLOW METHOD: CAR %f:%f:%f\n",car.x(),car.y(),car.z());
  next = ia.follow(&car);
  //printf("TESTING FOLLOW METHOD: CAR %d:%d:%d  POINT %d:%d:%d\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());
  printf("TESTING FOLLOW METHOD: CAR %f:%f:%f  POINT %f:%f:%f\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());
  car.xyz(2,2,0);
  next = ia.follow(&car);
  //printf("TESTING FOLLOW METHOD: CAR %d:%d:%d  POINT %d:%d:%d\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());
  printf("TESTING FOLLOW METHOD: CAR %f:%f:%f  POINT %f:%f:%f\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());

  // comprobacion metodo next, este metodo va devolviendo la referencia al siguiente punto mas cercano a una distancia menor que min_distance sin orden fijo
  car.xyz(0,0,0);
  //printf("TESTING NEXT METHOD: CAR %d:%d:%d\n",car.x(),car.y(),car.z());
  printf("TESTING NEXT METHOD: CAR %f:%f:%f\n",car.x(),car.y(),car.z());
  distance = ia.next(&car,next);
  cout <<"TESTING NEXT METHOD: CAR "<<car.x()<<":"<<car.y()<<":"<<car.z()<<" POINT " << next->derived.x()<<":"<<next->derived.y()<<":"<<next->derived.z()<<" DISTANCE " << distance << endl;

  car.xyz(30,30,1);
  cout << "TESTING NEXT METHOD: CAR "<< car.x()<<":"<<car.y()<<":"<<car.z()<<endl;
  distance = ia.next(&car,next);
  cout << "TESTING NEXT METHOD: CAR "<<car.x()<<":"<<car.y()<<":"<<car.z()<<" POINT " << next->derived.x()<<":"<<next->derived.y()<<":"<<next->derived.z()<<" DISTANCE " << distance << endl;

  cout << "RESET MIN DISTANCE TO 10"<<endl;
  ia.min_distance(10);

  cout << "TESTING FOLLOW METHOD: CAR "<< car.x()<<":"<<car.y()<<":"<<car.z()<<endl;
  next = ia.follow(&car);
  //printf("TESTING FOLLOW METHOD: CAR %d:%d:%d  POINT %d:%d:%d\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());
  printf("TESTING FOLLOW METHOD: CAR %f:%f:%f  POINT %f:%f:%f\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());
  car.xyz(2,2,0);
  next = ia.follow(&car);
  //printf("TESTING MOVE METHOD: CAR %d:%d:%d  POINT %d:%d:%d\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());
  printf("TESTING MOVE METHOD: CAR %f:%f:%f  POINT %f:%f:%f\n",car.x(),car.y(),car.z(), next->derived.x(),next->derived.y(),next->derived.z());

  //reajuste de coordenadas x,y,z para un iapoint 
  car.xyz(0,0,0);
  cout << "TESTING NEXT METHOD: CAR "<< car.x()<<":"<<car.y()<<":"<<car.z()<<endl;
  distance = ia.next(&car,next);
  cout <<"TESTING NEXT METHOD: CAR "<<car.x()<<":"<<car.y()<<":"<<car.z()<<" POINT " << next->derived.x()<<":"<<next->derived.y()<<":"<<next->derived.z()<<" DISTANCE " << distance << endl;

  car.xyz(30,30,1);
  cout << "TESTING NEXT METHOD: CAR "<< car.x()<<":"<<car.y()<<":"<<car.z()<<endl;
  distance = ia.next(&car,next);
  cout << "TESTING NEXT METHOD: CAR "<<car.x()<<":"<<car.y()<<":"<<car.z()<<" POINT " << next->derived.x()<<":"<<next->derived.y()<<":"<<next->derived.z()<<" DISTANCE " << distance << endl;






  return 0;
}

