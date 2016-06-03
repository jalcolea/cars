#ifndef IAPOINTSSERIALIZER_H
#define IAPOINTSSERIALIZER_H

#include <iostream>
#include <string>
#include <fstream>
#include <Ogre.h>
#include <vector>
#include <mxml.h>
#include <map>
#include "iapoint.h"



class IAPointsSerializer
{
    public:
        IAPointsSerializer() : _xml(nullptr), _data(nullptr), _nodeIAPoint(nullptr), _fp(nullptr){};
        virtual ~IAPointsSerializer();

        void nuevoXMLIAPoints(); // genera la estructura necesaria para crear los elementos del xml
        void addNodoXMLIAPoints(size_t id, iacomplexpoint point); // crea un nodo xml con los datos pasados por parametro.
        bool guardarXMLIAPoints(string nombreFichero); // Guarda los datos xml en fichero dado.

    protected:

    private:
        mxml_node_t* _xml;    /* <?xml ... ?> */
        mxml_node_t* _data;   /* <data> */
        mxml_node_t* _nodeIAPoint;   /* <node> */
        FILE* _fp;
        static const char* retornoCarro_Callback(mxml_node_t *node, int where);
};

#endif // IAPOINTSSERIALIZER_H
