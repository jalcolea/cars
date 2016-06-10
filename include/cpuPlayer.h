#ifndef CPUPLAYER_H
#define CPUPLAYER_H

#include "Ogre.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsWorld.h"
#include "CarRayCast.h"
#include "iamanager.h"
#include "IAPointsDeserializer.h"

using namespace Ogre;
using namespace std;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

class cpuPlayer
{
public:
    cpuPlayer(string nombreEnPantalla, string nombreVehiculo, string nombreMaterial, string ficheroRutasIA, Vector3 posicionSalida, SceneManager* sceneMgr, DynamicsWorld* world, size_t laps)
             : _nombreEnPantalla(nombreEnPantalla), _nombreVehiculo(nombreVehiculo), _nombreMaterial(nombreMaterial), _ficheroRutasIA(ficheroRutasIA), _posicionSalida(posicionSalida),
               _sceneMgr(sceneMgr), _world(world), _laps(laps)
    {
            _car = unique_ptr<CarRayCast>(new CarRayCast(_nombreVehiculo,_posicionSalida,_sceneMgr,_world,nullptr));
            
            _iapd = unique_ptr<IAPointsDeserializer>(new IAPointsDeserializer());
            _iapd->cargarFichero(_ficheroRutasIA);
            
            _iaMgr = unique_ptr<iamanager>(new iamanager(_laps,&_iapd->getPointsPtr(),0,1));
            
            _iaMgr->print_points();
            
            
    };
    
    virtual ~cpuPlayer();

    void update(Real deltaT);
    void build();

    
protected:

private:
    string _nombreEnPantalla;
    string _nombreVehiculo;
    string _nombreMaterial;
    string _ficheroRutasIA;
    Vector3 _posicionSalida;
    unique_ptr<CarRayCast> _car;
    unique_ptr<iamanager> _iaMgr;
    unique_ptr<IAPointsDeserializer> _iapd;
    SceneManager* _sceneMgr;
    OgreBulletDynamics::DynamicsWorld* _world;
    size_t _laps;


};

#endif // CPUPLAYER_H
