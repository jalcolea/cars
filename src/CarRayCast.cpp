#include "CarRayCast.h"
#include "OgreUtil.h"
#include "bulletUtil.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;
using namespace Ogre;
using namespace std;


//CarRayCast::CarRayCast()
//{
//    _tuneo = nullptr;
//}

CarRayCast::CarRayCast(const string& nombre, Vector3 posicion, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, Ogre::SceneNode* nodoPadre, size_t id)
                       : _nombre(nombre), _posicion(posicion), _world(world), _scnMgr(sceneMgr), _nodoPadre(nodoPadre), _id(id)
{
}

CarRayCast::~CarRayCast()
{
}

/*
void CarRayCast::buildVehiculo()
{
    _snc = SceneNodeConfig::getSingletonPtr();
    nodoVehiculoRayCast param = _snc->getInfoVehiculoRayCast(_nombre);
    
    _entChasis = _scnMgr->createEntity(param.nombre + "_ent_" + std::to_string(_id), param.nombreMallaChasis);
    _nodoChasis = _scnMgr->createSceneNode(param.nombre + "_nodo_" + to_string(_id));
    _nodoChasis->attachObject(_entChasis);

//    if(this->_nodoPadre) _nodoPadre->addChild(_nodoChasis);
//    else 
        _scnMgr->getRootSceneNode()->addChild(_nodoChasis);

    _nodoPadre = nullptr; // Parece que si los nodos del vehiculoraycast cuelgan de otro que no sea el Root tampoco funciona :D Así que su padre será el root.
    
//    SceneNode* nodo = _scnMgr->getRootSceneNode()->createChildSceneNode();
//    nodo->addChild(_nodoChasis);
    
    //_nodoChasis->setPosition(param.posicion);
    cout << "possalida carRayCast: " << _posicion << endl;
    _nodoChasis->setPosition(_posicion);
    
    _frictionSlip = param.frictionSlip;
    _fuerzaMotor = param.aceleracion;
    _giro = param.velocidadGiro;
    _fuerzaMotorInversa = param.aceleracionMarchaAtras;
    _frenada = param.frenada;
    _potenciadorPrimera = param.potenciadorPrimera;
    
    OgreBulletCollisions::BoxCollisionShape* formaChasis = new BoxCollisionShape(_entChasis->getBoundingBox().getHalfSize());
// PRUEBA SIN COMPOUND SHAPE
//    cout << "chasis boundingbox halfsize: " << _entChasis->getBoundingBox().getHalfSize() << endl;
//    CompoundCollisionShape* formaCompuesta = new CompoundCollisionShape();
//    formaCompuesta->addChildShape(formaChasis,param.posShapeBullet); //Vector3(0,1,0)); // En la demo de OgreBullet (que ni compila :( ) desplaza la forma 1 unidad, a saber por qué???
    
    _bodyWheeled = new OgreBulletDynamics::WheeledRigidBody(param.nombre + "_body_" + to_string(_id),_world);
//    _bodyWheeled->setShape(_nodoChasis,formaCompuesta,param.bodyRestitutionBullet,param.frictionBullet,param.masaBullet,param.posicion,Quaternion::IDENTITY);
    _bodyWheeled->setShape(_nodoChasis,formaChasis,param.bodyRestitutionBullet,param.frictionBullet,param.masaBullet,_posicion,Quaternion::IDENTITY);
    _bodyWheeled->setDamping(param.suspensionDamping,param.suspensionDamping); //YA VEREMOS SI HACE FALTA
    _bodyWheeled->disableDeactivation();
    _bodyWheeled->getBulletObject()->setUserPointer(new rigidBody_data(tipoRigidBody::COCHE,_bodyWheeled->getBulletObject()));
    
    // Al parecer los flags de colisión no funcionan con VehicleRayCast "asínque".... (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Vehicles)
//    _bodyWheeled->getBulletRigidBody()->setFlags(COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION | COL_CHECK); // NI CASO OIGA :(
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
    // Es decir, cada parámetro se corresponde con los ejes X,Y,Z y el valor establece el orden en el que se van a interpretar. 
    _vehiculo->setCoordinateSystem(0, 1, 2);
    
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
                                _id,                // id del coche (el cliente debe proporcionar un id válido, debe controlar su unicidad)
                                param.escala));     // FACTOR DE ESCALA, HAY COCHES QUE USAN RUEDAS TRASERAS MÁS ANCHAS
                                
    // ENLAZAR RUEDAS CON CHASIS (QUE FÁCIL ES DECIRLO :D)
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
*/

void CarRayCast::buildVehiculo()
{
    _snc = SceneNodeConfig::getSingletonPtr();
    nodoVehiculoRayCast param = _snc->getInfoVehiculoRayCast(_nombre);
    
    _nodoPadre = _scnMgr->getRootSceneNode()->createChildSceneNode ();

    _entChasis = _scnMgr->createEntity(param.nombre + "_ent_" + std::to_string(_id), param.nombreMallaChasis);
    _nodoChasis = _scnMgr->createSceneNode(param.nombre + "_nodo_" + to_string(_id));
    _nodoChasis->attachObject(_entChasis);
    _nodoChasis->setPosition(Vector3(0,1,0));
    
    _nodoPadre->addChild(_nodoChasis);

//    _scnMgr->getRootSceneNode()->addChild(_nodoChasis);
//    _nodoPadre = nullptr; // Parece que si los nodos del vehiculoraycast cuelgan de otro que no sea el Root tampoco funciona :D Así que su padre será el root.
    
//    SceneNode* nodo = _scnMgr->getRootSceneNode()->createChildSceneNode();
//    nodo->addChild(_nodoChasis);
    
    //_nodoChasis->setPosition(param.posicion);
    cout << "possalida carRayCast: " << _posicion << endl;
    _nodoChasis->setPosition(_posicion);
    
    _frictionSlip = param.frictionSlip;
    _fuerzaMotor = param.aceleracion;
    _giro = param.velocidadGiro;
    _fuerzaMotorInversa = param.aceleracionMarchaAtras;
    _frenada = param.frenada;
    _potenciadorPrimera = param.potenciadorPrimera;
    _traccionTrasera = param.traccionTrasera;
    _skidding = param.skid;
    
    OgreBulletCollisions::BoxCollisionShape* formaChasis = new BoxCollisionShape(_entChasis->getBoundingBox().getHalfSize());
// PRUEBA SIN COMPOUND SHAPE
//    cout << "chasis boundingbox halfsize: " << _entChasis->getBoundingBox().getHalfSize() << endl;
    CompoundCollisionShape* formaCompuesta = new CompoundCollisionShape();
//    formaCompuesta->addChildShape(formaChasis,param.posShapeBullet); //Vector3(0,1,0)); // En la demo de OgreBullet (que ni compila :( ) desplaza la forma 1 unidad, a saber por qué???
    formaCompuesta->addChildShape(formaChasis,Vector3(0,0.01,0)); // En la demo de OgreBullet (que ni compila :( ) desplaza la forma 1 unidad, a saber por qué???
                                                    //  0.01
    _bodyWheeled = new OgreBulletDynamics::WheeledRigidBody(param.nombre + "_body_" + to_string(_id),_world);
    _bodyWheeled->setShape(_nodoChasis,formaCompuesta,param.bodyRestitutionBullet,param.frictionBullet,param.masaBullet,_posicion,Quaternion::IDENTITY);
//    _bodyWheeled->setShape(_nodoChasis,formaChasis,param.bodyRestitutionBullet,param.frictionBullet,param.masaBullet,_posicion,Quaternion::IDENTITY);
    _bodyWheeled->setDamping(param.suspensionDamping,param.suspensionDamping); //YA VEREMOS SI HACE FALTA
    _bodyWheeled->disableDeactivation();
    _bodyWheeled->getBulletObject()->setUserPointer(new rigidBody_data(tipoRigidBody::COCHE,_bodyWheeled->getBulletObject()));
    
    // Al parecer los flags de colisión no funcionan con VehicleRayCast "asínque".... (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Vehicles)
//    _bodyWheeled->getBulletRigidBody()->setFlags(COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION | COL_CHECK); // NI CASO OIGA :(
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
                                _id,                // id del coche (el cliente debe proporcionar un id válido, debe controlar su unicidad)
                                param.escala));     // FACTOR DE ESCALA, HAY COCHES QUE USAN RUEDAS TRASERAS MÁS ANCHAS
                                
    // ENLAZAR RUEDAS CON CHASIS (QUE FÁCIL ES DECIRLO :D)
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
        _valorGiro = 0;
        enderezar();
    }    
//    else
//    {
//        fuerza *= 0.85; // Al girar perdemos potencia, en cuanto enderecemos la recuperamos.
//    }

//    cout << "traccion trasera: " << _traccionTrasera << endl;

    if (_traccionTrasera)
    {
        _vehiculo->applyEngineForce(fuerza,2);
        _vehiculo->applyEngineForce(fuerza,3);
    }
    else
    {
        _vehiculo->applyEngineForce(fuerza,0);
        _vehiculo->applyEngineForce(fuerza,1);
    }
    
//    btWheelInfo& wheelInfo0 = _vehiculo->getBulletVehicle()->getWheelInfo(0);
//    cout << "derrapando Rueda 0 " << wheelInfo0.m_skidInfo << endl;
//    cout << "Engine force Rueda 0 " << wheelInfo0.m_engineForce << endl;
//    btWheelInfo& wheelInfo1 = _vehiculo->getBulletVehicle()->getWheelInfo(1);
//    cout << "derrapando force Rueda 1 " << wheelInfo1.m_skidInfo << endl;
//    cout << "Engine force Rueda 1 " << wheelInfo1.m_engineForce << endl;
//    btWheelInfo& wheelInfo2 = _vehiculo->getBulletVehicle()->getWheelInfo(2);
//    cout << "derrapando force Rueda 2 " << wheelInfo2.m_skidInfo << endl;
//    cout << "Engine force Rueda 2 " << wheelInfo2.m_engineForce << endl;
//    btWheelInfo& wheelInfo3 = _vehiculo->getBulletVehicle()->getWheelInfo(3);
//    cout << "derrapando force Rueda 3 " << wheelInfo3.m_skidInfo << endl;
//    cout << "Engine force Rueda 3 " << wheelInfo3.m_engineForce << endl;
    
    
}


void CarRayCast::frenar(bool endereza)
{
//    _vehiculo->applyEngineForce(-_frenada,0);
//    _vehiculo->applyEngineForce(-_frenada,1);
//    _vehiculo->applyEngineForce(-_frenada,2);
//    _vehiculo->applyEngineForce(-_frenada,3);

//    btWheelInfo& wheelInfo = _vehiculo->getBulletVehicle()->getWheelInfo(0);
//    cout << "Engine force Rueda 0 " << wheelInfo.m_engineForce << endl;
//    wheelInfo = _vehiculo->getBulletVehicle()->getWheelInfo(1);
//    cout << "Engine force Rueda 1 " << wheelInfo.m_engineForce << endl;
//    wheelInfo = _vehiculo->getBulletVehicle()->getWheelInfo(2);
//    cout << "Engine force Rueda 2 " << wheelInfo.m_engineForce << endl;
//    wheelInfo = _vehiculo->getBulletVehicle()->getWheelInfo(3);
//    cout << "Engine force Rueda 3 " << wheelInfo.m_engineForce << endl;
    
    acelerar(-_frenada,endereza);
    


}


void CarRayCast::girar(short n, Real factorVelocidadGiro) // n positivo = izquierda, n negativo = derecha
{
    if (abs(_valorGiro + (_giro * n * factorVelocidadGiro)) < MAX_VALOR_GIRO_RUEDAS) // PARAMETRIZAR EL MÁXIMO QUE PUEDE GIRAR LA RUEDA?
        _valorGiro += (_giro * n * factorVelocidadGiro);
    
    _vehiculo->setSteeringValue(_valorGiro,0);
    _vehiculo->setSteeringValue(_valorGiro,1);
    
}

// La cpu si endereza lo hace del tirón (por conveniencia);
void CarRayCast::acelerarCPU(Real fuerza, bool endereza)
{
    if (endereza)
    {
        _valorGiro = 0;
        enderezar();
    }    
//    else
//    {
//        fuerza *= 0.85; // Al girar perdemos potencia, en cuanto enderecemos la recuperamos.
//    }

    _vehiculo->applyEngineForce(fuerza,0);
    _vehiculo->applyEngineForce(fuerza,1);
}


// Para coches controlados por la CPU, el valor del giro se deja que se calcule por la entidad pertinente.
void CarRayCast::girarCPU(Real valor) // valorGiro positivo = izquierda, valorGiro negativo = derecha, valorGiro cuanto han de girar. 
{
    if (abs(valor) < MAX_VALOR_GIRO_RUEDAS) // PARAMETRIZAR EL MÁXIMO QUE PUEDE GIRAR LA RUEDA?
        _valorGiro = valor;
    else
    {
        if (abs(valor) == 0)
            _valorGiro = 0; // "malditos decimales (léase con acento de Don Gato)
        else if (valor > 0)
            _valorGiro = MAX_VALOR_GIRO_RUEDAS - 0.02;
        else if (valor < 0)
            _valorGiro = - MAX_VALOR_GIRO_RUEDAS + 0.02;
    }

//    cout << "Valor giro en GIRARCPU: " << _valorGiro << endl;
//    assert(!(_valorGiro > MAX_VALOR_GIRO_RUEDAS));   // Si salta este assert, podría indicar la señal de que el coche ha chocado y su angulo respecto a su destino es demasiado abierto
 
//    if(_valorGiro > 0 ) cout << "girando a la izquierda" << endl;
//    else if (_valorGiro < 0) cout << "girando a la derecha" << endl;
       
    _vehiculo->setSteeringValue(_valorGiro,0);
    _vehiculo->setSteeringValue(_valorGiro,1);
    
//    cout << "GIRARCPU(" << valorGiro << ")" << endl;
//    cout << "Giro finalmente aplicado " << _valorGiro << endl;
}

void CarRayCast::enderezar()
{
    _vehiculo->setSteeringValue(0,0);
    _vehiculo->setSteeringValue(0,1);
}


void CarRayCast::recolocar(Ogre::Vector3 donde, Ogre::Quaternion direccion)
{
    // Reseteamos todo
    _bodyWheeled->getBulletRigidBody()->getWorldTransform().setIdentity();
    _bodyWheeled->getBulletRigidBody()->getWorldTransform().setOrigin(convert(donde));
    _bodyWheeled->getBulletRigidBody()->getWorldTransform().setRotation(convert(direccion));

}


bool CarRayCast::ruedasEnContacto()
{
    for (int i = 0; i<_vehiculo->getBulletVehicle()->getNumWheels(); i++)
    {
        if (!_vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_groundObject)
        {
            return false; //O está volando :D ó a volcado
        }
        else
        {
            return true;
            
//  NOTA: EL SIGUIENTE CÓDIGO COMENTADO NO FUNCIONA POR QUE LA "very special constraint that turn a rigidbody into a vehicle." (TAL Y COMO LA DESCRIBE EL AUTOR EN EL CÓDIGO :D)
//  AL PARECER NO ESTÁ TERMINADA. EL AUTOR HA HECHO LA PARTE QUE DETERMINA SI LAS RUEDAS ESTÁN EN CONTACTO CON OTRO OBJETO PERO SIEMPRE ESTABLECE EL CONTACTO A UN S_FIXEDBODY. 
//  O SEA QUE CUANDO HAY CONTACTO A LA VARIABLE MIEMBRO wheel.m_raycastInfo.m_groundObject LE ASIGNA UN btRigidBody CON SU ESTADO POR DEFECTO Y ALA, AHÍ LO LLEVAS :(
//  Y CUANDO NO HAY CONTACTO LE CASCA UN 0. DE MODO QUE LA ÚNICA FORMA DE SABER CON QUÉ ESTÁN EN CONTACTO LAS RUEDAS, ES... NINGUNA. BUENO, PODEMOS LIARNOS A LANZAR RAYOS Y DETERMINAR
//  CON RAYRESULTCALLBACKS QUÉ TIENEN DEBAJO LAS RUEDAS EN COMBINACIÓN CON LO HECHO POR EL AUTOR DE LA "very special constraint" :D, ES DECIR SI LE ASIGNA VALOR A m_groundObject QUIERE
//  DECIR QUE HAY CONTACTO Y ENTONCES PODEMOS LANZAR EL RAYO NOSOTROS E INTENTAR AVERIGUAR QUIEN ES EL "CONTACTADO". 
            
//            btCollisionObject* auxCollisionObject = static_cast<btCollisionObject*>(_vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_groundObject);
//            rigidBody_data* data = static_cast<rigidBody_data*>(auxCollisionObject->getUserPointer());
//            if (!data) return false;  // si mi user pointer no está operativo en este objeto, ni idea de con que estamos en contacto, así que fuera de aquí.
//            tipoRigidBody t = data->_tipo;
//            switch (t)
//            {
//                case tipoRigidBody::CARRETERA: cout << "RUEDA " << i << " EN CONTACTO CON CARRETERA " << endl; return true;
//                case tipoRigidBody::CIRCUITO:  cout << "RUEDA " << i << " EN CONTACTO CON CIRCUITO " << endl; return false;
//                case tipoRigidBody::OBSTACULO: cout << "RUEDA " << i << " EN CONTACTO CON OBSTACULO " << endl; return false;
//                case tipoRigidBody::COCHE:     cout << "RUEDA " << i << " EN CONTACTO CON COCHE " << endl; return false;
//                default: cout << "RUEDA " << i << " EN CONTACTO CON.... EL ÉTER!!!!! " << endl; return false;
//            }
        }
    }
    
    return true;
}     





/*  
            cout << "*****************************************************************************************************************" << endl;
            cout << "Rueda[" << i << "] en contacto?: " << _vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_isInContact << endl;
            cout << "ContactNormalWS: " << convert(_vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_contactNormalWS) << endl;
            cout << "ContactPoints: " << convert(_vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_contactPointWS) << endl;
            cout << "GroundObject: " << _vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_groundObject << endl;
            cout << "hardPointsWS: " << convert(_vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_hardPointWS) << endl;
            cout << "SuspensionLength: " << _vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_suspensionLength << endl;
            cout << "wheelAxleWS: " << convert(_vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_wheelAxleWS) << endl;
            cout << "wheelDirectionWS: " << convert(_vehiculo->getBulletVehicle()->getWheelInfo(i).m_raycastInfo.m_wheelDirectionWS) << endl; */


void CarRayCast::cambiarMaterialVehiculo(string& nombreMaterial)
{
    // NOTA: getSubEntity(0)  El índice de getSubEntity, en este caso 0, se establece cuando exportamos el modelo desde Blender, 
    // el exportador establece el indice del submesh según el orden de los materiales que use el modelo en Blender. En esta parte del juego
    // queremos cambiar el material del chasis del vehiculo. Es decir, de la parte del modelo más grande. Si esa parte del modelo en 
    // blender usa un material cuyo índice no sea 0, es decir el primero, el exportador le asignará a esa parte del modelo (el submesh)
    // el índice que tenga el material en Blender. Luego si hardcodeamos el índice del subEntity, como en este caso, tendremos que tener
    // en cuenta este detalle, de lo contrario estaríamos intentando cambiar el material al subEntity (submesh) erróneo.
    
    static_cast<Entity *>(this->_nodoChasis->getAttachedObject(0))->getSubEntity(0)->setMaterialName(nombreMaterial);
    

}

void CarRayCast::stop()
{
    _bodyWheeled->getBulletRigidBody()->clearForces();
    _bodyWheeled->getBulletRigidBody()->setInterpolationLinearVelocity( btVector3( 0, 0, 0 ) );
    _bodyWheeled->getBulletRigidBody()->setInterpolationAngularVelocity( btVector3( 0, 0, 0 ) );
    _bodyWheeled->getBulletRigidBody()->setLinearVelocity(btVector3( 0, 0, 0 ));
    _bodyWheeled->getBulletRigidBody()->setAngularVelocity(btVector3( 0, 0, 0 ));
    _vehiculo->getBulletVehicle()->resetSuspension();

}

void CarRayCast::getRuedasChirriando(std::vector<Ogre::Real>& valoresResult)
{
    valoresResult.clear();
    for (size_t i=0; i<_ruedas.size(); i++)
    {
        btWheelInfo& wheelInfo = _vehiculo->getBulletVehicle()->getWheelInfo(i);
        valoresResult.push_back(wheelInfo.m_skidInfo);
    }
    
    std::sort(valoresResult.begin(),valoresResult.end());
    
}