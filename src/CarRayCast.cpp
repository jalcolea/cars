#include "CarRayCast.h"
#include "OgreUtil.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;
using namespace Ogre;
using namespace std;


//CarRayCast::CarRayCast()
//{
//    _tuneo = nullptr;
//}

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
// PRUEBA SIN COMPOUND SHAPE
//    cout << "chasis boundingbox halfsize: " << _entChasis->getBoundingBox().getHalfSize() << endl;
//    CompoundCollisionShape* formaCompuesta = new CompoundCollisionShape();
//    formaCompuesta->addChildShape(formaChasis,param.posShapeBullet); //Vector3(0,1,0)); // En la demo de OgreBullet (que ni compila :( ) desplaza la forma 1 unidad, a saber por qué???
    
    _bodyWheeled = new OgreBulletDynamics::WheeledRigidBody(param.nombre + "_body",_world);
//    _bodyWheeled->setShape(_nodoChasis,formaChasis,param.bodyRestitutionBullet,param.frictionBullet,param.masaBullet,param.posicion,Quaternion::IDENTITY);
    _bodyWheeled->setShape(_nodoChasis,formaChasis,param.bodyRestitutionBullet,param.frictionBullet,param.masaBullet,param.posicion,Quaternion::IDENTITY);
    _bodyWheeled->setDamping(param.suspensionDamping,param.suspensionDamping); //YA VEREMOS SI HACE FALTA
    _bodyWheeled->disableDeactivation();
    
    // Al parecer los flags de colisión no funcionan con VehicleRayCast "asínque".... (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Vehicles)
//    _bodyWheeled->getBulletRigidBody()->setFlags(COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION | COL_CHECK); // NI PUTO CASO OIGA :(
//    cout << "flags" << _bodyWheeled->getBulletRigidBody()->getFlags() << endl;
    
    _tuneo = new OgreBulletDynamics::VehicleTuning(param.suspensionStiffness,  // suspensionStiffness: dureza de la suspensión
                                                   param.suspensionCompression,  // suspensionCompression: indice de compresión de la suspensión
                                                   param.suspensionDamping,  // suspensionDamping: indice de restitución de la suspensión
                                                   param.maxSuspensionTravelCm,  // maxSuspensionTravelCm: limite del recorrido de la suspensión (entiendo que al comprimirse el muelle)
                                                   param.maxSuspensionForce,  // maxSuspensionForce: límite máximo de la fuerza de la suspensión
                                                   param.frictionSlip); // frictionSlip: indice de fricción (AÚN NO SÉ EXACTAMENTE QUE ES)
                                
    _vehiculoRayCaster = new VehicleRayCaster(_world);
    
    _vehiculo = new RaycastVehicle(_bodyWheeled, _tuneo, _vehiculoRayCaster);
    
    // Aquí, indicamos que el eje X va primero (indice 0), eje Y segundo (indice 1) y Z el tercero (indice 2), o sea, lo más natural (pienso yo).
    // Que lo quieres como en Blender? pues: setCoordinateSystem(0,2,1)  
    // Es decir, cada parámetro se corresponde con los ejes X,Y,Z y el valor establece el orden en el que se van a interpretar. */
    _vehiculo->setCoordinateSystem(0, 1, 2);
    
//    Vector3Array vPosicionRuedas { Vector3(0.18,-0.12,0.21), Vector3(-0.18,-0.12,0.21), Vector3(-0.18,-0.12,-0.215), Vector3(0.18,-0.12,-0.215) }; // Para el kart
//    Vector3Array vPosicionRuedas { Vector3(0.4,-0.10,0.69), Vector3(-0.4,-0.10,0.69), Vector3(-0.41,-0.10,-0.58), Vector3(0.41,-0.10,-0.58) }; // Para el farara
//    Vector3Array vPosicionRuedas { Vector3(0.4,-0.10,0.65), Vector3(-0.4,-0.10,0.65), Vector3(-0.41,-0.10,-0.75), Vector3(0.41,-0.10,-0.75) }; // Para el formula
    //HAY QUE PASAR UN FACTOR DE ESCALA, HAY COCHES QUE USAN RUEDAS TRASERAS MÁS ANCHAS
    
    // Ahora le ponemos las rueditas :D
    for (size_t i=0; i<4; ++i) // Por ahora solo vehículos de 4 ruedas, las motos, camiones de 16 ruedas y esas cosas, otra día ya si eso.
        _ruedas.push_back(Rueda(param,              // parametros de configuracion de la rueda (friccion, influencia rodado, etc)
                                nullptr,            // nodo padre en el Grafo de Escena de Ogre, nullptr hace que el padre sea el Root. PARECE QUE HA DE SER EL ROOT SINO, NO CHUTA
                                _scnMgr,            // Puntero al SceneManager, usado en la clase Rueda para generar las entitys y SceneNodes
                                ((i<2)?true:false), // Si rueda menor que 2, o sea, 0 ó 1; entonces son delanteras, si no traseras.
                                param.posRuedas[i], //vPosicionRuedas[i], // Punto de conexión de los rayos que se lanzan desde las ruedas al chasis, simulando
                                                    // la suspensión. Son coordenadas locales del chasis. Aún tengo que ver que valores son los
                                                    // adecuados. Probablemente varíe bastante según el modelo de coche que se cargue.
                                i,                  // Id de la rueda, me lo daría el vector, pero la clase rueda no sabe donde se aloja :D
                                param.escala));
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

void CarRayCast::acelerar(Real fuerza, bool endereza, Real factorEnderezamiento)
{
    if (endereza && _valorGiro != 0.0) // Si queremos enderezar y además _valorGiro es distinto de cero
    {
        cout << "enderezando" << endl;
        if (_valorGiro > 0) girar(-1,factorEnderezamiento); // factorEnderezamiento = 1.5 por defecto
        else  girar(1,factorEnderezamiento);
    }    
    _vehiculo->applyEngineForce(fuerza,0);
    _vehiculo->applyEngineForce(fuerza,1);
}

// La cpu si endereza lo hace del tirón (por conveniencia);
void CarRayCast::acelerarCPU(Real fuerza, bool endereza)
{
    if (endereza)
    {
        _valorGiro = 0;
        _vehiculo->setSteeringValue(_valorGiro,0);
        _vehiculo->setSteeringValue(_valorGiro,1);
    }    
    _vehiculo->applyEngineForce(fuerza,0);
    _vehiculo->applyEngineForce(fuerza,1);
}



void CarRayCast::frenar()
{
    _vehiculo->applyEngineForce(-_frenada,0);
    _vehiculo->applyEngineForce(-_frenada,1);
}


void CarRayCast::girar(short n, Real factorVelocidadGiro) // n positivo = izquierda, n negativo = derecha
{
    if (abs(_valorGiro + (_giro * n * factorVelocidadGiro)) < MAX_VALOR_GIRO_RUEDAS) // PARAMETRIZAR EL MÁXIMO QUE PUEDE GIRAR LA RUEDA?
        _valorGiro += (_giro * n * factorVelocidadGiro);
        
    _vehiculo->setSteeringValue(_valorGiro,0);
    _vehiculo->setSteeringValue(_valorGiro,1);
    
    
//    cout << _valorGiro << endl;
}

// Para coches controlados por la CPU, el valor del giro se deja que se calcule por la entidad pertinente.
void CarRayCast::girarCPU(Real valorGiro) // valorGiro positivo = izquierda, valorGiro negativo = derecha, valorGiro cuanto han de girar. 
{
    if (abs(_valorGiro) <= MAX_VALOR_GIRO_RUEDAS) // PARAMETRIZAR EL MÁXIMO QUE PUEDE GIRAR LA RUEDA?
        _valorGiro = valorGiro;
    else
    {
        cout << "GIRARCPU(" << valorGiro << ") VALOR GIRO DEMASIADO GRANDE, AJUSTANDO" << endl;
        if (valorGiro == 0)
            _valorGiro = 0; // "malditos decimales (léase con acento de Don Gato)
        else if (valorGiro > 0)
            _valorGiro = MAX_VALOR_GIRO_RUEDAS;
        else if (valorGiro < 0)
            _valorGiro = - MAX_VALOR_GIRO_RUEDAS;
    }

    //assert(!(_valorGiro > MAX_VALOR_GIRO_RUEDAS));   // Si salta este assert, podría indicar la señal de que el coche ha chocado y su angulo respecto a su destino es demasiado abierto
 
//    if(_valorGiro > 0 ) cout << "girando a la izquierda" << endl;
//    else if (_valorGiro < 0) cout << "girando a la derecha" << endl;
       
    _vehiculo->setSteeringValue(_valorGiro,0);
    _vehiculo->setSteeringValue(_valorGiro,1);
    
    cout << "GIRARCPU(" << valorGiro << ")" << endl;
    cout << "Giro finalmente aplicado " << _valorGiro << endl;
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


     
