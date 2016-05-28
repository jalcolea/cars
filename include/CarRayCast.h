#ifndef CARRAYCAST_H
#define CARRAYCAST_H

#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "Shapes/OgreBulletCollisionsCompoundShape.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "Debug/OgreBulletCollisionsDebugDrawer.h"
#include "Constraints/OgreBulletDynamicsRaycastVehicle.h"
#include "OgreBulletCollisionsRay.h"
#include <OgreSceneNode.h>
#include <string>
#include <SceneNodeConfig.h>

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;
using namespace Ogre;
using namespace std;

class Rueda // SOLO GUARDA EL ESTADO DE LA RUEDA, NO LA ENLAZA CON EL CHASIS. ESO ES TRABAJO DE LA CLASE QUE MANEJE EL CHASIS
{
    public:
    Rueda(){};
    Rueda(const nodoVehiculoRayCast_t & param, SceneNode* nodoPadre, SceneManager* scnMgr, bool delantera, Vector3 conexionChasis, size_t index)
          : _nodoPadre(nodoPadre), _scnMgr(scnMgr), _delantera(delantera), _puntoConexionChasis(conexionChasis)
    {
        _entRueda = _scnMgr->createEntity(param.nombre + "_ent" + std::to_string(index), param.nombreMallaRueda);
        _entRueda->setCastShadows(true);
        _nodo = _scnMgr->createSceneNode(param.nombre + "_nodo" + std::to_string(index));
        _nodo->attachObject(_entRueda);
        if (_nodoPadre) _nodoPadre->addChild(_nodo);
        else _scnMgr->getRootSceneNode()->addChild(_nodo);
        
        this->_nombre = param.nombre + to_string(index);
        this->_anchoRueda = _entRueda->getBoundingBox().getSize().x;// * 0.3;
        this->_radioRueda = _entRueda->getBoundingBox().getSize().y;// * 1.5; // La z en este caso también valdría.
        this->_friccionRueda = param.friccionRueda;
        this->_indiceRestitucionSuspension = param.indiceRestitucionSuspension;
        this->_influenciaRodado = param.influenciaRodado;
        
    };

    virtual ~Rueda(){};
    inline Real getIndiceRestitucionSuspension() const { return _indiceRestitucionSuspension; };
    inline void setIndiceRestitucionSuspension(float indiceRestitucionSuspension ) { _indiceRestitucionSuspension = indiceRestitucionSuspension; }; 
    inline Real getRadioRueda() const { return _radioRueda; };
    inline Real getAnchoRueda() const { return _anchoRueda; };
    inline Real getFriccionRuedas() const { return _friccionRueda; };
    inline Real getInfluenciaRodado() const { return _influenciaRodado; }; // ESTO TENGO QUE VER QUE COÑO ES
    inline void setRadioRueda(float radioRueda ) { _radioRueda = radioRueda; }; // Obviamente el radio es común a todas las ruedas
    inline void setAnchoRueda(float anchoRueda ) { _anchoRueda = anchoRueda; };
    inline void setFriccionRueda(float friccionRueda ) { _friccionRueda = friccionRueda; };
    inline void setInfluenciaRodado(float influenciaRodado ) { _influenciaRodado = influenciaRodado; }; //influencia al rodar de las ruedas
    inline bool getDelantera(){ return _delantera; };
    inline void setDelantera(bool valor){ _delantera = valor; }; // si la rueda es delantera (entonces puede girar, o eso entiendo yo)
                                                                 // aunque no le veo mucho sentido (por ahora). Si quisiéramos hacer un 
                                                                 // toro (cargador de palés) que tiene ruedas traseras que giran pues 
                                                                 // sería igual solo que invirtiendo el sentido de rotación de las mismas no???
    inline void setPuntoConexionChasis(Vector3 puntoEnSistemaLocalChasis){ _puntoConexionChasis = puntoEnSistemaLocalChasis; };
    inline Vector3 & getPuntoConexionChasis(){ return _puntoConexionChasis; };
    inline Vector3 & getDireccionCS(){ return _direccionCS; };
    inline Vector3 & getEjeCS(){ return _ejeCS; };
    inline SceneNode* getSceneNode(){ return _nodo; }; 
    
    friend ostream& operator<<(ostream& o, Rueda &n)
    {
        o << "DATOS RUEDA"                  << endl
          << "\t Nombre: "                  << n._nombre << endl
          << "\t Radio rueda: "             << n._radioRueda << endl
          << "\t Ancho rueda: "             << n._anchoRueda << endl
          << "\t Friccion: "                << n._friccionRueda << endl
          << "\t Influencia rodado: "       << n._influenciaRodado << endl
          << "\t Restitucion Suspension: "  << n._indiceRestitucionSuspension << endl
          << "\t @Entity: "                 << n._entRueda << endl
          << "\t @SceneNode: "              << n._nodo << endl
          << "\t @SceneNode Padre: "        << n._nodoPadre << endl
          << "\t @SceneManager: "           << n._scnMgr << endl
          << "\t Delantera?: "              << n._delantera << endl
          << "\t Punto conexion chasis: "   << n._puntoConexionChasis << endl
          << "\t DireccionCS: "             << n._direccionCS << endl
          << "\t EjeCS: "                   << n._ejeCS  << endl;
        return o;
    }
    
private:
    string _nombre;
    Ogre::Real _radioRueda;
    Ogre::Real _anchoRueda;
    Ogre::Real _friccionRueda;
    Ogre::Real _influenciaRodado;
    Ogre::Real _indiceRestitucionSuspension;
    Entity* _entRueda;
    SceneNode* _nodo;
    SceneNode* _nodoPadre;
    SceneManager* _scnMgr;
    bool _delantera;

    // DOCUMENTACION SACADA DE http://blender3d.org.ua/forum/game/iwe/upload/Vehicle_Simulation_With_Bullet.pdf por Kester Maddock
    // The starting point of the ray, where the suspension connects to the
    // chassis (chassis space) (see also: m_raycastInfo.m_hardPointWS)
    // btVector3m_chassisConnectionPointCS;
    Vector3 _puntoConexionChasis;
    
    // The direction of ray cast (chassis space) (see also: m_raycastInfo.m_wheelDirectionWS)
    // btVector3m_wheelDirectionCS;
    // Esto indica la dirección del rayo que sale desde el chasis.
    // (0,-1,0) indica que se lanza el rayo sobre el eje Y y hacia abajo (signo negativo). Si pones (0,1,0) verás
    // de forma graciosa como las ruedas quedan suspendidas encima del chasis.
    // No se parametriza pues pienso que lo normal es tenerlo así siempre.
    Vector3 _direccionCS = Vector3(0,-1,0); // En la demo de OgreBullet lo pone así
    
    // The axis the wheel rotates around (chassis space) (see also:m_raycastInfo.m_wheelAxleWS)
    // Esto indica sobre que eje giran las ruedas (pitch en este caso pues el modelo está modelado teniendo a la rueda de frente,
    // es decir, la banda de rodadura mira hacia el observador).
    Vector3 _ejeCS = Vector3(-1,0,0); // En la demo de OgreBullet lo pone así
    
};

class CarRayCast
{
public:
    CarRayCast();
    CarRayCast(const string& nombre, Vector3 posicion, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, Ogre::SceneNode* nodoPadre = nullptr);
    virtual ~CarRayCast();

    inline string getNombre() const { return _nombre; };
    inline Vector3 getPosicion() const { return _posicion; };
    inline Vector3 getPosicionActual() const { return _nodoChasis->getPosition(); };
    inline Real getFuerzaMotor() const { return _fuerzaMotor; };
    inline OgreBulletDynamics::WheeledRigidBody* getRigidBody() const { return _bodyWheeled; };
    inline OgreBulletDynamics::VehicleTuning* getTuneo() const { return _tuneo; };
    inline OgreBulletDynamics::VehicleRayCaster* getVehiculoRayCaster() const { return _vehiculoRayCaster; };
    inline OgreBulletDynamics::RaycastVehicle* getVehiculo() const { return _vehiculo; };
    inline Rueda getSceneNodeRueda (size_t n) const { return _ruedas[n]; };
    inline Real getGiro() const{ return _giro; };
    inline void setNombre(const string& nombre){ _nombre = nombre; };
    inline void setPosicion(Vector3 posicion){ _posicion = posicion; };
    inline void setFuerzaMotor(float fuerzaMotor ) { _fuerzaMotor = fuerzaMotor; };
    void setCarChassis(OgreBulletDynamics::WheeledRigidBody* bodyWheeled); // Tipo específico para cuerpos rígidos con ruedas.
    void setVehicleRayCaster( OgreBulletDynamics::VehicleRayCaster* vrc ); // Tipo específico para calcular la física del vehículo.
    void setVehicle(OgreBulletDynamics::RaycastVehicle* rv);
    void setChassis(Ogre::Entity* entChasis); // Establece la malla del chasis.
    void buildVehiculo();
    void acelerar(Real fuerza, bool endereza = false ); // obvio no?
    void frenar();
    void marchaAtras(bool endereza = false ); // o lo que es lo mismo, frenamos????
    void girar(short n); // el ángulo de giro lo determinará el tipo de coche, vendrá configurado
    void recolocar(Ogre::Vector3 donde); // habrá que ver donde lo recolocamos, se autorecoloca o otra entidad le pasa como parámetro donde se recoloca????
    inline std::vector<Rueda> & getRuedas() { return _ruedas; };
    
    // Parametros de tuneo del coche.
    inline void setSuspensionStiffness(Ogre::Real suspensionStiffness){ if(_tuneo) _tuneo->getBulletTuning()->m_suspensionStiffness = suspensionStiffness; };
    inline void setSuspensionCompression(Ogre::Real suspensionCompression){ if(_tuneo) _tuneo->getBulletTuning()->m_suspensionCompression = suspensionCompression; };
    inline void setSuspensionDamping(Ogre::Real suspensionDamping){ if(_tuneo) _tuneo->getBulletTuning()->m_suspensionDamping = suspensionDamping; };     
    inline void setMaxSuspensionTravelCm(Ogre::Real maxSuspensionTravelCm){ if(_tuneo) _tuneo->getBulletTuning()->m_maxSuspensionTravelCm; };
    inline void setMaxSuspensionForce(Ogre::Real maxSuspensionForce){ if(_tuneo) _tuneo->getBulletTuning()->m_maxSuspensionForce = maxSuspensionForce; };
    inline void setFrictionSlip(Ogre::Real frictionSlip){ if(_tuneo) _tuneo->getBulletTuning()->m_frictionSlip = frictionSlip; };

  private:
    string _nombre;
    Ogre::Vector3 _posicion;
    Ogre::Real _fuerzaMotor;
    Ogre::Real _fuerzaMotorInversa;
    Ogre::Real _frenada;
    Ogre::Real _giro;
    OgreBulletDynamics::WheeledRigidBody* _bodyWheeled;
    OgreBulletDynamics::VehicleTuning* _tuneo;
    OgreBulletDynamics::VehicleRayCaster* _vehiculoRayCaster;
    OgreBulletDynamics::RaycastVehicle* _vehiculo;
    OgreBulletDynamics::DynamicsWorld* _world;
    std::vector<Rueda> _ruedas;
    SceneManager* _scnMgr;
    SceneNode* _nodoChasis;
    Entity* _entChasis;
    SceneNode* _nodoPadre;
    SceneNodeConfig* _snc;
    Real _valorGiro = 0;
        
    // Parámetros de tuneo del coche
    Ogre::Real _suspensionStiffness;   // dureza de la suspensión
    Ogre::Real _suspensionCompression; // indice de compresión de la suspensión
    Ogre::Real _suspensionDamping;     // indice de restitución de la suspensión
    Ogre::Real _maxSuspensionTravelCm; // limite del recorrido de la suspensión (entiendo que al comprimirse el muelle)
    Ogre::Real _maxSuspensionForce;    // límite máximo de la fuerza de la suspensión
    Ogre::Real _frictionSlip;          // indice de fricción (AÚN NO SÉ EXACTAMENTE QUE ES ESTE TIPO DE FRICCIÓN: SLIP=RESBALAR)

    
};


#endif // CARRAYCAST_H
