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

#define MAX_TIME_STOPPED 1.5 // segundos

class cpuPlayer
{
public:
    cpuPlayer(string nombreEnPantalla, string nombreVehiculo, string nombreMaterial, string ficheroRutasIA, Vector3 posicionSalida, 
              SceneManager* sceneMgr, DynamicsWorld* world, size_t laps, void* groundObject = nullptr, size_t id = 0)
             : _nombreEnPantalla(nombreEnPantalla), _nombreVehiculo(nombreVehiculo), _nombreMaterial(nombreMaterial), _ficheroRutasIA(ficheroRutasIA), _posicionSalida(posicionSalida),
               _sceneMgr(sceneMgr), _world(world), _laps(laps), _groundObject(groundObject), _id(id)
    {
            cout << "possalida cpuplayer: " << _posicionSalida << endl;
            _car = unique_ptr<CarRayCast>(new CarRayCast(_nombreVehiculo,_posicionSalida,_sceneMgr,_world,nullptr,_id));
            
            _iapd = unique_ptr<IAPointsDeserializer>(new IAPointsDeserializer());
            _iapd->cargarFichero(_ficheroRutasIA);
            
            _iaMgr = unique_ptr<iamanager>(new iamanager(_laps,&_iapd->getPointsPtr(),0,1));
            
            _iaMgr->print_points();
            
            _idCheck_destino = 0;
            _idCheck_origen = 0;
            _idCheck_meta = (_iaMgr->getVectorPtrPoints()->size() * _laps) - 1;
            _finish = false;
            _sentidoContrario = false;
            _onHisWay = false;
            _timeStopped = 0;
            _timeWrongWay = 0;
            
            
    };
    
    virtual ~cpuPlayer();

    void update(Real deltaT);
    void build();
    inline void start() { _onHisWay = true; };
    inline void stop() { _onHisWay = false; };
    
    inline Vector3 getPosicionActual(){ return _car->getPosicionActual(); };
    inline Real getVelocidadActual(){ return _car->getVelocidadKmH(); };
    
protected:
    bool compruebaCheckPoint();

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
    void * _groundObject;
    size_t _idCheck_destino;
    size_t _idCheck_origen;
    size_t _idCheck_meta;
    bool _finish;
    bool _sentidoContrario;
    bool _onHisWay;
    Ogre::Real _timeStopped; // Si el valor es mayor que uno dado, consideraremos que el coche se ha quedado atascado y forzaremos un respawn
    Ogre::Real _timeWrongWay; // Daremos un tiempo para que la CPU se recupere si por azar (choques, etc) acaba yendo contra sentido. Si supera el tiempo dado forzamos un respawn
    size_t _id;

    void dibujaLinea(Vector3 inicio, Vector3 fin);


};

#endif // CPUPLAYER_H
