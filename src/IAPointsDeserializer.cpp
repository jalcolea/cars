#include "IAPointsDeserializer.h"

IAPointsDeserializer::IAPointsDeserializer()
{
    //ctor
}

IAPointsDeserializer::~IAPointsDeserializer()
{
    //dtor
}

bool IAPointsDeserializer::cargarFichero(std::string fichero)
{
    FILE *fp = nullptr;

    mxml_node_t* tree;
    mxml_node_t* data;  // Nodo xml que engloba a todos los demás, el nodo raiz.
    mxml_node_t* node;  // Apuntará a un nodo xml con la información de un iacomplexpoint

    if (fichero.empty())
        fp = fopen(_fichero.c_str(), "r");
    else
        fp = fopen(fichero.c_str(), "r");

    if (fp && !fichero.empty()) _fichero = fichero; else return false;

    cout << "fichero leído" << endl;

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);

    data = mxmlFindElement(tree, tree, "IAPoints", NULL,NULL,MXML_DESCEND);
    for (node = mxmlFindElement(data, data, "point", NULL, NULL, MXML_DESCEND);
         node != NULL;
         node = mxmlFindElement(node,data, "point", NULL, NULL, MXML_DESCEND))
    {
        nuevoIAPoint(node);
    }

    return false;
}

bool IAPointsDeserializer::cargarFicheroCheckPoint(string fichero)
{
    FILE *fp = nullptr;

    mxml_node_t* tree;
    mxml_node_t* data;  // Nodo xml que engloba a todos los demás, el nodo raiz.
    mxml_node_t* node;  // Apuntará a un nodo xml con la información de un iacomplexpoint

    if (fichero.empty())
        fp = fopen(_fichero.c_str(), "r");
    else
        fp = fopen(fichero.c_str(), "r");

    if (fp && !fichero.empty()) _fichero = fichero; else return false;

    cout << "fichero leído" << endl;

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);

    data = mxmlFindElement(tree, tree, "IAPoints", NULL,NULL,MXML_DESCEND);
    for (node = mxmlFindElement(data, data, "point", NULL, NULL, MXML_DESCEND);
         node != NULL;
         node = mxmlFindElement(node,data, "point", NULL, NULL, MXML_DESCEND))
    {
        nuevoCheckPoint(node);
    }

    return false;
}


void IAPointsDeserializer::nuevoIAPoint(mxml_node_t* node)
{
//    iapoint p;
//    iapoint* r;
//    p.x(std::stof((mxmlElementGetAttr(node,"x"))));
//    p.y(std::stof((mxmlElementGetAttr(node,"y"))));
//    p.z(std::stof((mxmlElementGetAttr(node,"z"))));
//    // offset = std::stof((mxmlElementGetAttr(node,"offset")));
//
//    _vPoints.push_back(p);
//    
//    r = new iapoint(p.x(),p.y(),p.z());
//    _vPointsPtr.push_back(r);
    
    punto point;
    point.p.x = std::stof((mxmlElementGetAttr(node,"x")));
    point.p.y = std::stof((mxmlElementGetAttr(node,"y")));
    point.p.z = std::stof((mxmlElementGetAttr(node,"z")));

    _vPoints.push_back(point);
    

}

void IAPointsDeserializer::nuevoCheckPoint(mxml_node_t* node)
{
    CheckPoint check;
    
    
//    check.p = new iapoint(std::stof((mxmlElementGetAttr(node,"x"))),
//                          std::stof((mxmlElementGetAttr(node,"y"))),
//                          std::stof((mxmlElementGetAttr(node,"z"))));

    check.p.p.x = std::stof((mxmlElementGetAttr(node,"x")));
    check.p.p.y = std::stof((mxmlElementGetAttr(node,"y")));
    check.p.p.z = std::stof((mxmlElementGetAttr(node,"z")));
                          
    check.quat = Ogre::Quaternion(std::stof((mxmlElementGetAttr(node,"qW"))),
                                  std::stof((mxmlElementGetAttr(node,"qX"))),
                                  std::stof((mxmlElementGetAttr(node,"qY"))),
                                  std::stof((mxmlElementGetAttr(node,"qZ"))));
                                  
    _vCheckPoints.push_back(check);
    
}
