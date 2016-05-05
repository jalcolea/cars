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

    data = mxmlFindElement(_tree, _tree, "data", NULL,NULL,MXML_DESCEND);
    for (node = mxmlFindElement(data, data, "scenenode", NULL, NULL, MXML_DESCEND);
         node != NULL;
         node = mxmlFindElement(node,data, "scenenode", NULL, NULL, MXML_DESCEND))
    {
        nuevoSceneNode(node);
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
    
    nodoOgre_t nodo;
    nodo.nombreNodo = string(mxmlGetText(nombreSceneNode,NULL));
    nodo.nombreEntidad = string(mxmlGetText(nombreEntidad,NULL));
    nodo.nombreMalla = string(mxmlGetText(nombreMalla,NULL));
    nodo.nombreMaterial = string(mxmlGetText(nombreMaterial,NULL));
    nodo.frictionBullet = Ogre::Real(mxmlGetReal(friction));
    nodo.masaBullet = Ogre::Real(mxmlGetReal(masa));
    nodo.orientacionShapeBullet = Ogre::Quaternion(0,0,0,1); // Hacer funcion auxiliar que convierta el texto a un quaternio
    nodo.posInicial = Ogre::Vector3(0,0,0); // Hacer función auxiliar que convierta el texto a Vector3
    nodo.posShapeBullet = Ogre::Vector3(0,0,0);// Hacer función auxiliar que convierta el texto a Vector3
    nodo.bodyRestitutionBullet = Ogre::Real(mxmlGetReal(restitution));
    map_nodos[nodo.nombreNodo] = nodo;
    

    cout << string(mxmlGetText(nombreSceneNode,NULL)) << endl;
    cout << string(mxmlGetText(nombreEntidad,NULL)) << endl;
    cout << string(mxmlGetText(nombreMalla,NULL)) << endl;
    cout << string(mxmlGetText(nombreMaterial,NULL)) << endl;
    cout << string(mxmlGetText(friction,NULL)) << endl;
    cout << string(mxmlGetText(masa,NULL)) << endl;
    cout << string(mxmlGetText(orientacionShape,NULL)) << endl;
    cout << string(mxmlGetText(posInicial,NULL)) << endl;
    cout << string(mxmlGetText(posShape,NULL)) << endl;
    cout << string(mxmlGetText(restitution,NULL)) << endl;
    
    cout << map_nodos.size() << " nodos en el mapa de nodos." << endl;


}

void SceneNodeConfig::setFicheroConfiguracion(string fichero, bool Cargar)
{
    _fichero = fichero;
    if (Cargar) load_xml();
}
