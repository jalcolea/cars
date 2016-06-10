#ifndef IAPOINTSDESERIALIZER_H
#define IAPOINTSDESERIALIZER_H

#include <iostream>
#include <string>
#include <fstream>
#include <Ogre.h>
#include <vector>
#include <mxml.h>
#include "iapoint.h"


struct CheckPoint
{
  iapoint* p;
  Ogre::Quaternion quat;  
};

class IAPointsDeserializer
{
    public:
        IAPointsDeserializer();
        virtual ~IAPointsDeserializer();

        bool cargarFichero(string fichero = "");

        string getFichero(){ return _fichero; };
        std::vector<iapoint>& getPoints(){ return _vPoints;};
        std::vector<iapoint*>& getPointsPtr(){ return _vPointsPtr;};
        std::vector<CheckPoint>& getCheckPoints() { return _vCheckPoints; };

    protected:

    private:
        void nuevoIAPoint(mxml_node_t* node);
        void nuevoCheckPoint(mxml_node_t* node);
        std::vector<iapoint> _vPoints;
        std::vector<iapoint*> _vPointsPtr;
        std::vector<CheckPoint> _vCheckPoints;
        string _fichero;
};

#endif // IAPOINTSDESERIALIZER_H
