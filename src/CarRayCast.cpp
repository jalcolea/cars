#include "CarRayCast.h"

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
    
    //nombreMallaRueda;
    _entChasis = _scnMgr->createEntity(param.nombre + "_ent", param.nombreMallaChasis);
    _nodoChasis = _scnMgr->createSceneNode(param.nombre + "_nodo");
    _nodoChasis->attachObject(_entChasis);
    if(this->_nodoPadre) _nodoPadre->addChild(_nodoChasis);
    else _scnMgr->getRootSceneNode()->addChild(_nodoPadre);
    
    _nodoChasis->setPosition(param.posicion);
    
    _frictionSlip = param.frictionSlip;
    _fuerzaMotor = param.aceleracion;
    _giro = param.velocidadGiro;
    _fuerzaMotorInversa = param.aceleracionMarchaAtras;
    _frenada = param.frenada;
    
    _tuneo = new OgreBulletDynamics::VehicleTuning(param.suspensionStiffness,  // suspensionStiffness: dureza de la suspensión
                                                   param.suspensionCompression,  // suspensionCompression: indice de compresión de la suspensión
                                                   param.suspensionDamping,  // suspensionDamping: indice de restitución de la suspensión
                                                   param.maxSuspensionTravelCm,  // maxSuspensionTravelCm: limite del recorrido de la suspensión (entiendo que al comprimirse el muelle)
                                                   param.maxSuspensionForce,  // maxSuspensionForce: límite máximo de la fuerza de la suspensión
                                                   param.frictionSlip); // frictionSlip: indice de fricción (AÚN NO SÉ EXACTAMENTE QUE ES)
                                
    _vehiculoRayCaster = new VehicleRayCaster(_world);
    
    _vehiculo = new RaycastVehicle(_bodyWheeled, _tuneo, _vehiculoRayCaster);
    
    // A ver, esto está (IRONIC MODE ON) "COJONUDAMENTE EXPLICADO, SI SEÑOR" (IRONIC MODE OFF) 
    // Me encanta lidiar con librerías cuya única documentación es... (redoble de tambores) LEERTE EL PUTO CÓDIGO!!!!!!!!!
    // Bien, una vez desahogado, procedamos a la explicación:
    // Esto establece el sistema de coordenadas, es decir, si en vez de xyz lo quieres, por ejemplo, xzy como en blender. 
    // Y el signo también influye claro está, podrías hacer que el x fuera positivo a la izquierda en vez de a la derecha. Nah!, si un día
    // estás aburrido y quieres joder al personal, pues le cambias el sistema y de paso les fríes el cerebro.
    // Para liarlo un poquito más, pues lo "ocultas" un poco más, como es el caso que nos ocupa. Para cambiar el sistema de coordenadas
    // lo haces indicando el índice que quieres asignarle de un, digo yo, vector que almacena el orden del sistema de coordenadas.
    // Aquí, indicamos que el eje X va primero (indice 0), eje Y segundo (indice 1) y Z el tercero (indice 2), o sea, lo más natural (pienso yo).
    // Que lo quieres como en Blender? pues: setCoordinateSystem(0,2,1)  
    // Es decir, cada parámetro se corresponde con los ejes X,Y,Z y el valor establece el orden en el que se van a interpretar.
    _vehiculo->setCoordinateSystem(0, 1, 2);
    
    // Ahora le ponemos las rueditas :D
    for (size_t i=0; i<4; ++i) // Por ahora solo vehículos de 4 ruedas, las motos, camiones de 16 ruedas y esas cosas, otra día ya si eso.
        _ruedas.push_back(Rueda(param.radioRuedas, param.anchoRuedas, param.friccionRueda, param.influenciaRodado,
                                param.indiceRestitucionSuspension, _nodoChasis, param.nombreMallaRueda,
                                ((i<2)?true:false), // Si rueda menor que 2, o sea, 0 ó 1; entonces son delanteras, si no traseras.
                                Vector3(0,0,0))); // Punto de conexión de los rayos que se lanzan desde las ruedas al chasis, simulando
                                                  // la suspensión. Son coordenadas locales del chasis. Aún tengo que ver que valores son los
                                                  // adecuados. Probablemente varíe bastante según el modelo de coche que se cargue.
                                                  
    // TODO: ENLAZAR RUEDAS CON CHASIS (QUE FÁCIL ES DECIRLO :D)

}

