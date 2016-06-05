#include "IAPointsDeserializer.h"

IAPointsDeserializer::IAPointsDeserializer()
{
    //ctor
}

IAPointsDeserializer::~IAPointsDeserializer()
{
    //dtor
}

bool IAPointsDeserializer::cargarFichero(string fichero)
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

void IAPointsDeserializer::nuevoIAPoint(mxml_node_t* node)
{
    iapoint p;
    p.x(std::stof((mxmlElementGetAttr(node,"x"))));
    p.y(std::stof((mxmlElementGetAttr(node,"y"))));
    p.z(std::stof((mxmlElementGetAttr(node,"z"))));
    // offset = std::stof((mxmlElementGetAttr(node,"offset")));

    _vPoints.push_back(p);
    _vPointsPtr.push_back(&p);
}
