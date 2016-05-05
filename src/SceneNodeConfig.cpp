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
    mxml_node_t* tmp;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_NODO)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_ENTIDAD)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_MALLA)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::FRICTION)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::MASA)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::NOMBRE_MATERIAL)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::ORIENTACION_SHAPE)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POS_INICIAL)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::POS_SHAPE)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;

    tmp = mxmlFindElement(node,node,(_xmlElements[static_cast<size_t>(xmlElementsIndex::RESTITUTION)]).c_str(),NULL,NULL,MXML_DESCEND);
    cout << string(mxmlGetText(tmp,NULL)) << endl;


}

void SceneNodeConfig::setFicheroConfiguracion(string fichero, bool Cargar)
{
    _fichero = fichero;
    if (Cargar) load_xml();
}
