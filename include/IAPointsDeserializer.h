#ifndef IAPOINTSDESERIALIZER_H
#define IAPOINTSDESERIALIZER_H

#include <iostream>
#include <string>
#include <fstream>
#include <Ogre.h>
#include <vector>
#include <mxml.h>
#include "iapoint.h"


class IAPointsDeserializer
{
    public:
        IAPointsDeserializer();
        IAPointsDeserializer(string fichero) : _fichero(fichero){};
        virtual ~IAPointsDeserializer();

        bool cargarFichero(string fichero = "");

        string getFichero(){ return _fichero; };
        std::vector<iacomplexpoint>& getPoints(){ return _vPoints;};

    protected:

    private:
        void nuevoIAPoint(mxml_node_t* node);
        std::vector<iacomplexpoint> _vPoints;
        string _fichero;
};

#endif // IAPOINTSDESERIALIZER_H
