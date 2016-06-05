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
        virtual ~IAPointsDeserializer();

        bool cargarFichero(string fichero = "");

        string getFichero(){ return _fichero; };
        std::vector<iapoint>& getPoints(){ return _vPoints;};
        std::vector<iapoint*>& getPointsPtr(){ return _vPointsPtr;};

    protected:

    private:
        void nuevoIAPoint(mxml_node_t* node);
        std::vector<iapoint> _vPoints;
        std::vector<iapoint*> _vPointsPtr;
        string _fichero;
};

#endif // IAPOINTSDESERIALIZER_H
