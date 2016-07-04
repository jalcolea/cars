#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Ogre.h"
#include "OgreBullet/Dynamics/OgreBulletDynamicsWorld.h"
#include "CarRayCast.h"
#include "bulletUtil.h"
#include "OgreUtil.h"
#include "soundUtil.h"

using namespace Ogre;
using namespace std;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

#define MAX_TIME_STOPPED 1.5 // segundos
#define MAX_MARCHA 5
#define MAX_TIME_MARCHA_HUMAN 1
#define MAX_TIME_HANDICAP 3 // segundos
#define MAX_TIME_WRONGWAY 2

class humanPlayer
{
    public:
        humanPlayer(string nombreEnPantalla, string nombreVehiculo, string nombreMaterial, Vector3 posicionSalida, 
                    SceneManager* sceneMgr, DynamicsWorld* world, size_t laps, size_t checksPerLap, canalesSonidoCoche canalesSonido,
                    void* groundObject = nullptr, size_t id = 0, bool buildForMe = false);
        
        virtual ~humanPlayer();

        void update(Real deltaT, size_t keys);
        void build();
        inline void activarMaterial(){ _car->cambiarMaterialVehiculo(_nombreMaterial); };
        inline void start() { _onHisWay = true; };
        void stop();
        inline string& getNombreMaterial(){ return _nombreMaterial; };
        inline void setNombreMaterial(const string& nombreMaterial){ _nombreMaterial = nombreMaterial; };
        inline string& getNombreEnPantalla(){ return _nombreEnPantalla; };
        inline size_t getLap(){ return  _lapActual; }; //_totalCheckPoints / (_checksPerLap-1); };
        inline Vector3 getPosicionActual(){ return _car->getPosicionActual(); };
        inline Real getVelocidadActual(){ return _car->getVelocidadKmH(); };
        inline bool finished(){ return _finish; };
        inline size_t getTotalCheckPoints(){ return _totalCheckPoints; };
        
        void recoloca();
        void compruebaRecolocar();
        bool compruebaCheckPoint();
        void compruebaSuelo();

    private:
        void dibujaLinea(Vector3 inicio, Vector3 fin, bool consoleOut = false);
        void cambiaMarcha();
        void coutTipoCollisionObject(tipoRigidBody t);
        void compruebaChirriar();

        std::vector<Ogre::Real> _valoresSkidRuedas;
        unique_ptr<CarRayCast> _car;
        string _nombreEnPantalla;
        string _nombreVehiculo;
        string _nombreMaterial;
        btQuaternion _ultimaOrientacionBuena;
        Vector3 _posicionSalida;
        SceneManager* _sceneMgr;
        OgreBulletDynamics::DynamicsWorld* _world;
        Ogre::SceneNode* _nodoCheckPointSiguiente;
        Ogre::SceneNode* _nodoCheckPointAnterior;
        Ogre::Real _timeWrongWay;
        Ogre::Real _timeAtajando;
        Ogre::Real _timeVolandoVolcado;
        Ogre::Real _timeFueraPista;
        Ogre::Real _deltaT;
        Ogre::Real _incrementoMarcha;
        Ogre::Real _timeMarcha;
        Ogre::Real _aceleracion;
        Ogre::Real _posicionY; // cuando recolocamos nos aseguramos de tener el coche en una posición con componente Y adecuada.
        size_t _idCheck_destino;
        size_t _idCheck_origen;
        size_t _idCheck_meta;
        size_t _totalCheckPoints;
        size_t _laps;
        size_t _lapActual;
        size_t _checksPerLap;
        // Para el sonido, como máximo vamos a tener 3 canales de sonido: 
        // motorUp, motorDown, derrapando. Los choques se gestionan externamente
        canalesSonidoCoche _canalesSonido;
        void * _groundObject;
        size_t _id;
        size_t _marchaActual;

        bool _finish;
        bool _sentidoContrario;
        bool _onHisWay;
        bool _starting;
        bool _atajando;
        bool _fueraPista;
        
};

#endif // HUMANPLAYER_H