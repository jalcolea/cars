#ifndef CPUPLAYER_H
#define CPUPLAYER_H

#include "Ogre.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsWorld.h"
#include "CarRayCast.h"
//#include "iamanager.h"
#include "puntoManager.h"
#include "IAPointsDeserializer.h"
#include "bulletUtil.h"

using namespace Ogre;
using namespace std;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

#define MAX_TIME_STOPPED 1.5 // segundos
#define MAX_MARCHA 5
#define MAX_TIME_MARCHA 2
#define MAX_TIME_HANDICAP 3 // segundos

enum class estadoManiobra
{
    ENRUTA,
    COCHE_DELANTE_CERCA,
    COCHE_A_IZQUIERDA_CERCA,
    COCHE_A_DERECHA_CERCA,
    COCHE_ATRAS_CERCA,
    BORDILLO_CERCA,
    MANIOBRANDO_MARCHA_ATRAS
};

class cpuPlayer
{
public:
    cpuPlayer(string nombreEnPantalla, string nombreVehiculo, string nombreMaterial, string ficheroRutasIA, Vector3 posicionSalida, 
              SceneManager* sceneMgr, DynamicsWorld* world, size_t laps, void* groundObject = nullptr, size_t id = 0);
    
    virtual ~cpuPlayer();

    void update(Real deltaT);
    void build();
    inline void activarMaterial(){ _car->cambiarMaterialVehiculo(_nombreMaterial); };
    inline void start() { _onHisWay = true; };
    void stop();
    inline bool finished(){ return _finish; };
    inline string& getNombreMaterial(){ return _nombreMaterial; };
    inline void setNombreMaterial(const string& nombreMaterial){ _nombreMaterial = nombreMaterial; };
    inline string& getNombreEnPantalla(){ return _nombreEnPantalla; };
    inline size_t getTotalCheckPoints(){ return _totalCheckPoints; };
    
    inline Vector3 getPosicionActual(){ return _car->getPosicionActual(); };
    inline Real getVelocidadActual(){ return _car->getVelocidadKmH(); };
    inline void setDistanciaSonora(Ogre::Real distancia){ _distanciaSonora = distancia; }; // valores normalizados please
    inline Ogre::Real getDistanciaSonora(){ return _distanciaSonora; };
    inline btCollisionObject* getBody(){ return _car->getRigidBody()->getBulletObject(); };
    
protected:
    bool compruebaCheckPoint();

private:
    string _nombreEnPantalla;
    string _nombreVehiculo;
    string _nombreMaterial;
    string _ficheroRutasIA;
    Vector3 _posicionSalida;
    unique_ptr<CarRayCast> _car;
    //unique_ptr<iamanager> _iaMgr;
    unique_ptr<puntoManager> _iaMgr;
    unique_ptr<IAPointsDeserializer> _iapd;
    SceneManager* _sceneMgr;
    SceneNode* _nodoCheckPointSiguiente;
    OgreBulletDynamics::DynamicsWorld* _world;
    size_t _laps;
    canalesSonidoCoche _canalesSonido;
    void * _groundObject;
    size_t _idCheck_destino;
    size_t _idCheck_origen;
    size_t _idCheck_meta;
    size_t _idxPuntoAleatorioActual;
    size_t _totalCheckPoints;
    bool _finish;
    bool _sentidoContrario;
    bool _onHisWay;
    Ogre::Real _timeStopped; // Si el valor es mayor que uno dado, consideraremos que el coche se ha quedado atascado y forzaremos un respawn
    Ogre::Real _timeWrongWay; // Daremos un tiempo para que la CPU se recupere si por azar (choques, etc) acaba yendo contra sentido. Si supera el tiempo dado forzamos un respawn
    Ogre::Real _deltaT;
    size_t _id;
    btQuaternion _ultimaOrientacionBuena;

    void dibujaLinea(Vector3 inicio, Vector3 fin, bool consoleOut = false);
    void compruebaRecolocar();
    void setListaPuntosAleatorios(); // establece el indice para obtener un punto aleatorio para un Checkpoint.
    void compruebaManiobra();
    void rayoAlFrente();
    void cambiaMarcha();
    void aplicarHandicap();
    Ogre::Vector3 getPuntoAleatorioEnWS();
    Ogre::Vector3 obtenerDestino();
    estadoManiobra _maniobra;
    size_t _marchaActual;
    Ogre::Real _incrementoMarcha;
    Ogre::Real _timeMarcha;
    Ogre::Real _timeHandicap;
    Ogre::Real _handicap;
    Ogre::Real _aceleracion;
    size_t _puntosAleatoriosPasados; // Cuando alternemos la ruta para pasar por puntos aleatorios de los checkPoints, cada N puntos alcanzados
                                     // retomaremos la ruta principal.
    bool _rutaPuntosAleatorios;
    Ogre::Real _distanciaSonora; // para que el sonido se atenue o aumente si nos acercamos a un humanPlayer;
    
    void coutTipoCollisionObject(tipoRigidBody t);
    // Para el sonido, como máximo vamos a tener 3 canales de sonido: 
    // motorUp, motorDown, derrapando. Los choques se gestionan externamente

    


};

#endif // CPUPLAYER_H
