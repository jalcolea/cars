#include "SceneNodeConfig.h"

template<> SceneNodeConfig *Ogre::Singleton<SceneNodeConfig>::msSingleton = 0;

SceneNodeConfig *SceneNodeConfig::getSingletonPtr()
{
    return msSingleton;
}

SceneNodeConfig &SceneNodeConfig::getSingleton()
{
    assert(msSingleton);
    return *msSingleton;
}


bool SceneNodeConfig::load_xml(string fichero)
{
    FILE *fp = nullptr;

    mxml_node_t *data;  // Nodo xml que engloba a todos los demás, el nodo raiz.
    mxml_node_t *node;  // Apuntará a un nodo xml con la información de un SceneNode

//    fichero = "./" + fichero;
    fp = fopen(fichero.c_str(), "r");
    if (fp) _fichero = fichero; else return false;

    cout << "fichero leído" << endl;

    _tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);

    data = mxmlFindElement(_tree, _tree, "dataSceneNode", NULL,NULL,MXML_DESCEND);
    for (node = mxmlFindElement(data, data, "scenenode", NULL, NULL, MXML_DESCEND);
         node != NULL;
         node = mxmlFindElement(node,data, "scenenode", NULL, NULL, MXML_DESCEND))
    {
        nuevoSceneNode(node);
    }
    
    data = mxmlFindElement(_tree,_tree,"dataCamera", NULL, NULL, MXML_DESCEND);
    for (node = mxmlFindElement(data, data, "camera", NULL, NULL, MXML_DESCEND);
         node != NULL;
         node = mxmlFindElement(node,data, "camera", NULL, NULL, MXML_DESCEND))
    {
        nuevoCamera(node);
    }
    
    data = mxmlFindElement(_tree,_tree,"dataVehiculoRayCast", NULL, NULL, MXML_DESCEND);
    for (node = mxmlFindElement(data, data, "dataRayCast", NULL, NULL, MXML_DESCEND);
         node != NULL;
         node = mxmlFindElement(node,data, "dataRayCast", NULL, NULL, MXML_DESCEND))
    {
        nuevoVehiculoRayCast(node);
    }

    


    return true;
}

void SceneNodeConfig::nuevoSceneNode(mxml_node_t* node)
{
    mxml_node_t* nombreSceneNode = nullptr;
    mxml_node_t* nombreEntidad = nullptr;
    mxml_node_t* nombreMalla = nullptr;
    mxml_node_t* nombreMaterial = nullptr;
    mxml_node_t* friction = nullptr;
    mxml_node_t* masa = nullptr;
    mxml_node_t* orientacionShape = nullptr;
    mxml_node_t* posInicial = nullptr;
    mxml_node_t* posShape = nullptr;
    mxml_node_t* restitution = nullptr;
    mxml_node_t* orientacion = nullptr;
    mxml_node_t* escala = nullptr;
    mxml_node_t* dificultad = nullptr;

    nombreSceneNode = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_NODO)]).c_str(),NULL,NULL,MXML_DESCEND);
    nombreEntidad = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_ENTIDAD)]).c_str(),NULL,NULL,MXML_DESCEND);
    nombreMalla = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_MALLA)]).c_str(),NULL,NULL,MXML_DESCEND);
    nombreMaterial = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_MATERIAL)]).c_str(),NULL,NULL,MXML_DESCEND);
    friction = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::FRICTION)]).c_str(),NULL,NULL,MXML_DESCEND);
    masa = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::MASA)]).c_str(),NULL,NULL,MXML_DESCEND);
    orientacionShape = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ORIENTACION_SHAPE)]).c_str(),NULL,NULL,MXML_DESCEND);
    posInicial = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POS_INICIAL)]).c_str(),NULL,NULL,MXML_DESCEND);
    posShape = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POS_SHAPE)]).c_str(),NULL,NULL,MXML_DESCEND);
    restitution = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::RESTITUTION)]).c_str(),NULL,NULL,MXML_DESCEND);
    orientacion = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ORIENTACION)]).c_str(),NULL,NULL,MXML_DESCEND);
    escala = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ESCALA)]).c_str(),NULL,NULL,MXML_DESCEND);
    dificultad = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::DIFICULTAD)]).c_str(),NULL,NULL,MXML_DESCEND);
    
    nodoOgre_t nodo;
    nodo.nombreNodo = string(mxmlGetText(nombreSceneNode,NULL));
    nodo.nombreEntidad = string(mxmlGetText(nombreEntidad,NULL));
    nodo.nombreMalla = string(mxmlGetText(nombreMalla,NULL));
    nodo.nombreMaterial = string(mxmlGetText(nombreMaterial,NULL));
    nodo.frictionBullet = Ogre::Real(std::stof(mxmlGetText(friction,NULL)));
    nodo.masaBullet = Ogre::Real(std::stof(mxmlGetText(masa,NULL)));
    nodo.orientacionShapeBullet = extraeQuaternio(orientacionShape);
    nodo.posInicial = extraeVector3(posInicial);
    nodo.posShapeBullet = extraeVector3(posShape);
    nodo.bodyRestitutionBullet = Ogre::Real(std::stof(mxmlGetText(restitution,NULL)));
    nodo.escala = extraeVector3(escala);
    if (orientacion) nodo.orientacion = extraeQuaternio(orientacion); else nodo.orientacion = Ogre::Quaternion(1,0,0,0);
    nodo.dificultad = (dificultad)?std::stoi(mxmlGetText(dificultad,NULL)):0;
    
    map_nodos[nodo.nombreNodo] = nodo;

}

void SceneNodeConfig::nuevoCamera(mxml_node_t* node)
{
    mxml_node_t* nombreCamera = nullptr;
    mxml_node_t* orientacion = nullptr;
    mxml_node_t* posInicial = nullptr;
    mxml_node_t* direccion = nullptr;
    mxml_node_t* lookAt = nullptr;
    mxml_node_t* farclipdistance = nullptr;
    mxml_node_t* nearclipdistance = nullptr;
    

    nombreCamera = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_CAMARA)]).c_str(),NULL,NULL,MXML_DESCEND);
    orientacion = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ORIENTACION)]).c_str(),NULL,NULL,MXML_DESCEND);
    posInicial = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POS_INICIAL)]).c_str(),NULL,NULL,MXML_DESCEND);
    direccion = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::DIRECCION)]).c_str(),NULL,NULL,MXML_DESCEND);
    lookAt = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::LOOKAT)]).c_str(),NULL,NULL,MXML_DESCEND);
    farclipdistance = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::FAR_CLIPDISTANCE)]).c_str(),NULL,NULL,MXML_DESCEND);
    nearclipdistance = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NEAR_CLIPDISTANCE)]).c_str(),NULL,NULL,MXML_DESCEND);
    
    nodoCamera_t cam;
    cam.nombreCamera = string(mxmlGetText(nombreCamera,NULL));
    cam.orientacion = extraeQuaternio(orientacion);
    cam.posInicial = extraeVector3(posInicial);
    cam.direccion = extraeVector3(direccion);
    cam.lookAt = extraeVector3(lookAt);
    cam.nearClipDistance = Ogre::Real(std::stof(mxmlGetText(nearclipdistance,NULL)));
    cam.farClipDistance = Ogre::Real(std::stof(mxmlGetText(farclipdistance,NULL)));
    
    map_cameras[cam.nombreCamera] = cam;
}

void SceneNodeConfig::nuevoVehiculoRayCast(mxml_node_t* node)
{
    mxml_node_t* nombre;
    mxml_node_t* nombreMallaRueda;
    mxml_node_t* nombreMallaChasis;
    mxml_node_t* nombreMaterial;                      // nombre del material que usará el Entity, en caso de poder cambiarse.
    mxml_node_t* orientacion;               // orientacion del scenenode
    mxml_node_t* orientacionShapeBullet;    // orientación del shape de bullet que envolverá al modelo
    mxml_node_t* posShapeBullet;               // posición inicial del shape de bullet
    mxml_node_t* masaBullet;                      // propiedad masa del rigidbody
    mxml_node_t* frictionBullet;                  // propiedad índice de fricción del rigidbody
    mxml_node_t* bodyRestitutionBullet;           // propiedad índice de restitución (elasticidad) del rigidbody    
    mxml_node_t* radioRuedas;
    mxml_node_t* anchoRuedas;
    mxml_node_t* friccionRueda;
    mxml_node_t* influenciaRodado;
    mxml_node_t* indiceRestitucionSuspension;
    mxml_node_t* posicion;
    mxml_node_t* velocidadGiro;
    mxml_node_t* aceleracion; 
    mxml_node_t* frenada;
    mxml_node_t* aceleracionMarchaAtras;
    mxml_node_t* suspensionStiffness;   //  dureza de la suspensión
    mxml_node_t* suspensionCompression; // indice de compresión de la suspensión
    mxml_node_t* suspensionDamping;     // indice de restitución de la suspensión
    mxml_node_t* maxSuspensionTravelCm; // limite del recorrido de la suspensión (entiendo que al comprimirse el muelle)
    mxml_node_t* maxSuspensionForce;    // límite máximo de la fuerza de la suspensión
    mxml_node_t* frictionSlip;
    mxml_node_t* posRueda0;              
    mxml_node_t* posRueda1;              
    mxml_node_t* posRueda2;              
    mxml_node_t* posRueda3;              
    mxml_node_t* escalaRueda; 
    mxml_node_t* escala;
    mxml_node_t* potenciadorPrimera;
    mxml_node_t* traccionTrasera;
    mxml_node_t* maxSkidValue;
    mxml_node_t* skidCorto;
    mxml_node_t* skidMedio;
    mxml_node_t* skidLargo;
    
    nombre = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_NODO)]).c_str(),NULL,NULL,MXML_DESCEND);
    nombreMallaRueda = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_MALLA_RUEDA)]).c_str(),NULL,NULL,MXML_DESCEND);
    nombreMallaChasis = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_MALLA_CHASIS)]).c_str(),NULL,NULL,MXML_DESCEND);
    nombreMaterial = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_MATERIAL)]).c_str(),NULL,NULL,MXML_DESCEND);
    frictionBullet = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::FRICTION)]).c_str(),NULL,NULL,MXML_DESCEND);
    masaBullet = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::MASA)]).c_str(),NULL,NULL,MXML_DESCEND);
    orientacionShapeBullet = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ORIENTACION_SHAPE)]).c_str(),NULL,NULL,MXML_DESCEND);
    posShapeBullet = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POS_SHAPE)]).c_str(),NULL,NULL,MXML_DESCEND);
    bodyRestitutionBullet = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::RESTITUTION)]).c_str(),NULL,NULL,MXML_DESCEND);
    orientacion = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ORIENTACION)]).c_str(),NULL,NULL,MXML_DESCEND);
    radioRuedas = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::RADIO_RUEDAS)]).c_str(),NULL,NULL,MXML_DESCEND);
    anchoRuedas = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ANCHO_RUEDAS)]).c_str(),NULL,NULL,MXML_DESCEND);
    friccionRueda = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::FRICCION_RUEDA)]).c_str(),NULL,NULL,MXML_DESCEND);
    influenciaRodado = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::INFLUENCIA_RODADO)]).c_str(),NULL,NULL,MXML_DESCEND);
    indiceRestitucionSuspension = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::INDICE_REST_SUSP)]).c_str(),NULL,NULL,MXML_DESCEND);
    posicion = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POSICION)]).c_str(),NULL,NULL,MXML_DESCEND);
    velocidadGiro = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::VELOCIDAD_GIRO)]).c_str(),NULL,NULL,MXML_DESCEND);
    aceleracion = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ACELERACION)]).c_str(),NULL,NULL,MXML_DESCEND);
    frenada = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::FRENADA)]).c_str(),NULL,NULL,MXML_DESCEND);
    aceleracionMarchaAtras = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ACELERACION_MARCHA_ATRAS)]).c_str(),NULL,NULL,MXML_DESCEND);
    suspensionStiffness = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::SUSPENSION_STIFFNESS)]).c_str(),NULL,NULL,MXML_DESCEND);
    suspensionCompression = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::SUSPENSION_COMPRESION)]).c_str(),NULL,NULL,MXML_DESCEND);
    suspensionDamping = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::SUSPENSION_DAMPING)]).c_str(),NULL,NULL,MXML_DESCEND);
    maxSuspensionTravelCm = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::MAX_SUSPENSION_TRAVEL_CM)]).c_str(),NULL,NULL,MXML_DESCEND);
    maxSuspensionForce = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::MAX_SUSPENSION_FORCE)]).c_str(),NULL,NULL,MXML_DESCEND);
    frictionSlip = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::FRICTION_SLIP)]).c_str(),NULL,NULL,MXML_DESCEND);
    posRueda0 = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POSRUEDA0)]).c_str(),NULL,NULL,MXML_DESCEND);
    posRueda1 = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POSRUEDA1)]).c_str(),NULL,NULL,MXML_DESCEND);
    posRueda2 = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POSRUEDA2)]).c_str(),NULL,NULL,MXML_DESCEND);
    posRueda3 = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POSRUEDA3)]).c_str(),NULL,NULL,MXML_DESCEND);
    escalaRueda = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ESCALARUEDA)]).c_str(),NULL,NULL,MXML_DESCEND);
    escala = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ESCALA)]).c_str(),NULL,NULL,MXML_DESCEND);
    potenciadorPrimera = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POTENCIADORPRIMERA)]).c_str(),NULL,NULL,MXML_DESCEND);
    traccionTrasera = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::TRACCIONTRASERA)]).c_str(),NULL,NULL,MXML_DESCEND);
    maxSkidValue = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::MAXSKIDVALUE)]).c_str(),NULL,NULL,MXML_DESCEND);
    skidCorto = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::SKIDCORTO)]).c_str(),NULL,NULL,MXML_DESCEND);
    skidMedio = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::SKIDMEDIO)]).c_str(),NULL,NULL,MXML_DESCEND);
    skidLargo = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::SKIDLARGO)]).c_str(),NULL,NULL,MXML_DESCEND);
    
    nodoVehiculoRayCast_t nodo;
    
    nodo.nombre = string(mxmlGetText(nombre,NULL));
    nodo.nombreMallaRueda = string(mxmlGetText(nombreMallaRueda,NULL));
    nodo.nombreMallaChasis = string(mxmlGetText(nombreMallaChasis,NULL));

    nodo.nombreMaterial = string(mxmlGetText(nombreMaterial,NULL));
    nodo.frictionBullet = Ogre::Real(std::stof(mxmlGetText(frictionBullet,NULL)));
    nodo.masaBullet = Ogre::Real(std::stof(mxmlGetText(masaBullet,NULL)));
    nodo.orientacionShapeBullet = extraeQuaternio(orientacionShapeBullet);
    nodo.posShapeBullet = extraeVector3(posShapeBullet);
    nodo.bodyRestitutionBullet = Ogre::Real(std::stof(mxmlGetText(bodyRestitutionBullet,NULL)));
    if (orientacion) nodo.orientacion = extraeQuaternio(orientacion); else nodo.orientacion = Ogre::Quaternion(1,0,0,0);
    
    nodo.radioRuedas = Ogre::Real(std::stof(mxmlGetText(radioRuedas,NULL)));
    nodo.anchoRuedas = Ogre::Real(std::stof(mxmlGetText(anchoRuedas,NULL)));
    nodo.friccionRueda = Ogre::Real(std::stof(mxmlGetText(friccionRueda,NULL)));
    nodo.influenciaRodado = Ogre::Real(std::stof(mxmlGetText(influenciaRodado,NULL)));
    nodo.indiceRestitucionSuspension = Ogre::Real(std::stof(mxmlGetText(indiceRestitucionSuspension,NULL)));
    nodo.posicion = extraeVector3(posicion);
    nodo.velocidadGiro = Ogre::Real(std::stof(mxmlGetText(velocidadGiro,NULL)));
    nodo.aceleracion = Ogre::Real(std::stof(mxmlGetText(aceleracion,NULL)));
    nodo.frenada = Ogre::Real(std::stof(mxmlGetText(frenada,NULL)));
    nodo.aceleracionMarchaAtras = Ogre::Real(std::stof(mxmlGetText(aceleracionMarchaAtras,NULL)));
    nodo.suspensionStiffness = Ogre::Real(std::stof(mxmlGetText(suspensionStiffness,NULL)));
    nodo.suspensionCompression = Ogre::Real(std::stof(mxmlGetText(suspensionCompression,NULL)));
    nodo.suspensionDamping = Ogre::Real(std::stof(mxmlGetText(suspensionDamping,NULL)));
    nodo.maxSuspensionTravelCm = Ogre::Real(std::stof(mxmlGetText(maxSuspensionTravelCm,NULL)));
    nodo.maxSuspensionForce = Ogre::Real(std::stof(mxmlGetText(maxSuspensionForce,NULL)));
    nodo.frictionSlip = Ogre::Real(std::stof(mxmlGetText(frictionSlip,NULL)));
    nodo.posRuedas.push_back(extraeVector3(posRueda0));
    nodo.posRuedas.push_back(extraeVector3(posRueda1));
    nodo.posRuedas.push_back(extraeVector3(posRueda2));
    nodo.posRuedas.push_back(extraeVector3(posRueda3));
    nodo.escalaRueda = extraeVector3(escalaRueda);
    nodo.escala = extraeVector3(escala);
    nodo.potenciadorPrimera = (potenciadorPrimera)?Ogre::Real(std::stof(mxmlGetText(potenciadorPrimera,NULL))):1;
    if (traccionTrasera) nodo.traccionTrasera = (atoi(mxmlGetText(traccionTrasera,NULL))>0?true:false);
    else nodo.traccionTrasera = false;
    if (maxSkidValue) nodo.skid.maxSkidValue = Ogre::Real(std::stof(mxmlGetText(maxSkidValue,NULL)));
    if (skidCorto) nodo.skid.SkidCorto = Ogre::Real(std::stof(mxmlGetText(skidCorto,NULL)));
    if (skidMedio) nodo.skid.SkidMedio = Ogre::Real(std::stof(mxmlGetText(skidMedio,NULL)));
    if (skidLargo) nodo.skid.SkidLargo = Ogre::Real(std::stof(mxmlGetText(skidLargo,NULL)));
    
    map_vehiculos_raycast[nodo.nombre] = nodo;
    
}


Ogre::Vector3 SceneNodeConfig::extraeVector3(mxml_node_t* node)
{
    return Ogre::Vector3(std::stof((mxmlElementGetAttr(node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::X)]).c_str()))),
                         std::stof((mxmlElementGetAttr(node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::Y)]).c_str()))),
                         std::stof((mxmlElementGetAttr(node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::Z)]).c_str()))));
}

Ogre::Quaternion SceneNodeConfig::extraeQuaternio(mxml_node_t* node)
{
    return Ogre::Quaternion(std::stof((mxmlElementGetAttr(node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::W)]).c_str()))),
                            std::stof((mxmlElementGetAttr(node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::X)]).c_str()))),
                            std::stof((mxmlElementGetAttr(node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::Y)]).c_str()))),
                            std::stof((mxmlElementGetAttr(node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::Z)]).c_str()))));
}

void SceneNodeConfig::setFicheroConfiguracion(string fichero, bool Cargar)
{
    _fichero = fichero;
    if (Cargar) load_xml();
}
