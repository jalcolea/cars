#include "CarRayCast.h"
#include "OgreUtil.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;
using namespace Ogre;
using namespace std;


CarRayCast::CarRayCast()
{
    _tuneo = nullptr;
}

CarRayCast::CarRayCast(const string& nombre, Vector3 posicion, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, Ogre::SceneNode* nodoPadre)
                       : _nombre(nombre), _posicion(posicion), _world(world), _scnMgr(sceneMgr), _nodoPadre(nodoPadre)
{
}

CarRayCast::~CarRayCast()
{
}


void CarRayCast::buildVehiculo()
{
    _snc = SceneNodeConfig::getSingletonPtr();
    nodoVehiculoRayCast param = _snc->getInfoVehiculoRayCast(_nombre);
    
    _entChasis = _scnMgr->createEntity(param.nombre + "_ent", param.nombreMallaChasis);
    _nodoChasis = _scnMgr->createSceneNode(param.nombre + "_nodo");
    _nodoChasis->attachObject(_entChasis);

//    if(this->_nodoPadre) _nodoPadre->addChild(_nodoChasis);
//    else 
        _scnMgr->getRootSceneNode()->addChild(_nodoChasis);

    _nodoPadre = nullptr; // Parece que si los nodos del vehiculoraycast cuelgan de otro que no sea el Root tampoco funciona :D Así que su padre será el root.
    
//    SceneNode* nodo = _scnMgr->getRootSceneNode()->createChildSceneNode();
//    nodo->addChild(_nodoChasis);
    
    _nodoChasis->setPosition(param.posicion);
    
    _frictionSlip = param.frictionSlip;
    _fuerzaMotor = param.aceleracion;
    _giro = param.velocidadGiro;
    _fuerzaMotorInversa = param.aceleracionMarchaAtras;
    _frenada = param.frenada;
    
    OgreBulletCollisions::BoxCollisionShape* formaChasis = new BoxCollisionShape(_entChasis->getBoundingBox().getHalfSize());
    cout << "chasis boundingbox halfsize: " << _entChasis->getBoundingBox().getHalfSize() << endl;
    CompoundCollisionShape* formaCompuesta = new CompoundCollisionShape();
    formaCompuesta->addChildShape(formaChasis,param.posShapeBullet); //Vector3(0,1,0)); // En la demo de OgreBullet (que ni compila :( ) desplaza la forma 1 unidad, a saber por qué???
    
    _bodyWheeled = new OgreBulletDynamics::WheeledRigidBody(param.nombre + "_body",_world);
    _bodyWheeled->setShape(_nodoChasis,formaChasis,param.bodyRestitutionBullet,param.frictionBullet,param.masaBullet,param.posicion,Quaternion::IDENTITY);
    _bodyWheeled->setDamping(param.suspensionDamping,param.suspensionDamping); //YA VEREMOS SI HACE FALTA
    _bodyWheeled->disableDeactivation();

    // Al parecer los flags de colisión no funcionan con VehicleRayCast "asínque".... (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Vehicles)
//    _bodyWheeled->getBulletRigidBody()->setFlags(COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION); // NI PUTO CASO OIGA :(
//    cout << "flags" << _bodyWheeled->getBulletRigidBody()->getFlags() << endl;
    
    _tuneo = new OgreBulletDynamics::VehicleTuning(param.suspensionStiffness,  // suspensionStiffness: dureza de la suspensión
                                                   param.suspensionCompression,  // suspensionCompression: indice de compresión de la suspensión
                                                   param.suspensionDamping,  // suspensionDamping: indice de restitución de la suspensión
                                                   param.maxSuspensionTravelCm,  // maxSuspensionTravelCm: limite del recorrido de la suspensión (entiendo que al comprimirse el muelle)
                                                   param.maxSuspensionForce,  // maxSuspensionForce: límite máximo de la fuerza de la suspensión
                                                   param.frictionSlip); // frictionSlip: indice de fricción (AÚN NO SÉ EXACTAMENTE QUE ES)
                                
    _vehiculoRayCaster = new VehicleRayCaster(_world);
    
    _vehiculo = new RaycastVehicle(_bodyWheeled, _tuneo, _vehiculoRayCaster);
    
    /* A ver, esto está (IRONIC MODE ON) "COJONUDAMENTE EXPLICADO, SI SEÑOR" (IRONIC MODE OFF) 
    // Me encanta lidiar con librerías cuya única documentación es... (redoble de tambores) LEERTE EL PUTO CÓDIGO!!!!!!!!!
    // Bien, una vez desahogado, procedamos a la explicación:
    // Esto establece el sistema de coordenadas, es decir, si en vez de xyz lo quieres, por ejemplo, xzy como en blender. 
    // Y el signo también influye claro está, podrías hacer que el x fuera positivo a la izquierda en vez de a la derecha. Nah!, si un día
    // estás aburrido y quieres joder al personal, pues le cambias el sistema y de paso les fríes el cerebro.
    // Para liarlo un poquito más, pues lo "ocultas" un poco más, como es el caso que nos ocupa. Para cambiar el sistema de coordenadas
    // lo haces indicando el índice que quieres asignarle de un, digo yo, vector que almacena el orden del sistema de coordenadas.
    // Aquí, indicamos que el eje X va primero (indice 0), eje Y segundo (indice 1) y Z el tercero (indice 2), o sea, lo más natural (pienso yo).
    // Que lo quieres como en Blender? pues: setCoordinateSystem(0,2,1)  
    // Es decir, cada parámetro se corresponde con los ejes X,Y,Z y el valor establece el orden en el que se van a interpretar. */
    _vehiculo->setCoordinateSystem(0, 1, 2);
    
    Vector3Array vPosicionRuedas { Vector3(0.18,-0.12,0.21), Vector3(-0.18,-0.12,0.21), Vector3(-0.18,-0.12,-0.215), Vector3(0.18,-0.12,-0.215) };
//    Vector3Array vPosicionRuedas { Vector3(0.18,0.12,0.21), Vector3(-0.18,0.12,0.21), Vector3(-0.18,0.12,-0.215), Vector3(0.18,0.12,-0.215) };
    
    // Ahora le ponemos las rueditas :D
    for (size_t i=0; i<4; ++i) // Por ahora solo vehículos de 4 ruedas, las motos, camiones de 16 ruedas y esas cosas, otra día ya si eso.
        _ruedas.push_back(Rueda(param,              // parametros de configuracion de la rueda (friccion, influencia rodado, etc)
                                nullptr,            // nodo padre en el Grafo de Escena de Ogre, nullptr hace que el padre sea el Root.
                                _scnMgr,            // Puntero al SceneManager, usado en la clase Rueda para generar las entitys y SceneNodes
                                ((i<2)?true:false), // Si rueda menor que 2, o sea, 0 ó 1; entonces son delanteras, si no traseras.
                                vPosicionRuedas[i], // Punto de conexión de los rayos que se lanzan desde las ruedas al chasis, simulando
                                                    // la suspensión. Son coordenadas locales del chasis. Aún tengo que ver que valores son los
                                                    // adecuados. Probablemente varíe bastante según el modelo de coche que se cargue.
                                i));                // Id de la rueda, me lo daría el vector, pero la clase rueda no sabe donde se aloja :D
    // TODO: ENLAZAR RUEDAS CON CHASIS (QUE FÁCIL ES DECIRLO :D)
    int i = 0;
    for (auto it = _ruedas.begin(); it != _ruedas.end(); ++it)
    {
        cout << (*it) << endl;
        _vehiculo->addWheel((*it).getSceneNode(),(*it).getPuntoConexionChasis(),(*it).getDireccionCS(), (*it).getEjeCS(),
                            (*it).getIndiceRestitucionSuspension(), (*it).getRadioRueda(), (*it).getDelantera(), (*it).getFriccionRuedas(),
                            (*it).getInfluenciaRodado());
    }
    
    _vehiculo->setWheelsAttached();

}

void CarRayCast::acelerar(Real fuerza, bool endereza)
{
    if (endereza && _valorGiro != 0.0) // Si queremos enderezar y además _valorGiro es distinto de cero
    {
        cout << "enderezando" << endl;
        if (_valorGiro > 0) girar(-1);
        else  girar(1);
    }    
    _vehiculo->applyEngineForce(fuerza,0);
    _vehiculo->applyEngineForce(fuerza,1);
}


void CarRayCast::frenar()
{
    _vehiculo->applyEngineForce(-_frenada,0);
    _vehiculo->applyEngineForce(-_frenada,1);
}


void CarRayCast::girar(short n) // n positivo = izquierda, n negativo = derecha
{
    if (abs(_valorGiro + (_giro * n)) < 0.6) // PARAMETRIZAR EL MÁXIMO QUE PUEDE GIRAR LA RUEDA
        _valorGiro += (_giro * n);
        
    _vehiculo->setSteeringValue(_valorGiro,0);
    _vehiculo->setSteeringValue(_valorGiro,1);
    
    
    cout << _valorGiro << endl;
}

void CarRayCast::recolocar(Ogre::Vector3 donde)
{
    // Reseteamos todo
    _bodyWheeled->getBulletRigidBody()->getWorldTransform().setIdentity();
    _bodyWheeled->getBulletRigidBody()->getWorldTransform().setOrigin(convert(donde));
    
    
//    getCarChassisPtr()->getBulletRigidBody ()->getWorldTransform().setIdentity();
//    getCarChassisPtr()->getBulletRigidBody ()->getWorldTransform().setOrigin(btVector3(_x, _y, _z));
//    // Reiniciar fuerzas
//    getCarChassisPtr()->getBulletRigidBody ()->clearForces();
//    getCarChassisPtr()->getBulletRigidBody ()->setInterpolationLinearVelocity( btVector3( 0, 0, 0 ) );
//    getCarChassisPtr()->getBulletRigidBody ()->setInterpolationAngularVelocity( btVector3( 0, 0, 0 ) );
//    getCarChassisPtr()->getBulletRigidBody ()->setLinearVelocity(btVector3( 0, 0, 0 ));
//    getCarChassisPtr()->getBulletRigidBody ()->setAngularVelocity(btVector3( 0, 0, 0 ));
//    _mVehicle->getBulletVehicle ()->resetSuspension();
//
//    // Colocar ruedas
//    _mSteering = 0.0;
//    _mVehicle->setSteeringValue ( _mSteering, 0 );
//    _mVehicle->setSteeringValue ( _mSteering, 1 );
}