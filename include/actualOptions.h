#ifndef ACTUALOPTIONS_H
#define ACTUALOPTIONS_H

#include "Ogre.h"
#include <vector>
#include <string>

using namespace std;
using namespace Ogre;

class actualOptions : public Ogre::Singleton<actualOptions>
{
public:
    actualOptions(){};
    ~actualOptions();

    inline std::vector<string>& getNombreMateriales(){ return _vMateriales; };
    inline std::vector<string>& getNombresCPU(){ return _nombresCPU; };
    inline string& getNombreMaterial(int idMaterial){ return _vMateriales[idMaterial]; };
    inline string& getNombreVehiculoXML(){ return _idNombreVehiculoXML; };
    inline int getDificultad(){ return _dificultad; };
    inline int getIdMaterial(){ return _idMaterialActual; };
    inline void setIdNombreVehiculoXML(string nombreVehiculo){ _idNombreVehiculoXML = nombreVehiculo; };
    inline void setDificultad(int dificultad){ _dificultad = Ogre::Math::Clamp(dificultad,1,3); };
    inline void setIdMaterialActual(int idMaterial){ _idMaterialActual = idMaterial; };
    

    // Heredados de Ogre::Singleton.
    static actualOptions& getSingleton();
    static actualOptions* getSingletonPtr();

private:

    std::vector<string> _vMateriales {"carmaterial_blue","carmaterial_citrus","carmaterial_green","carmaterial_orange","carmaterial_silver","carmaterial_violet","red.gloss"};
    std::vector<string> _nombresCPU {"Billy","Sarah","Ace","Tom","Jasper","Light","Claire","Mike","John","Jaimie",
                                     "Kate","Dugol","Jack","Ned","Jennie","Crow","Willy","Mark","Sting","Dave","Gilis"};
    std::vector<Entity*> _vEntCars;
    string _idNombreVehiculoXML;
    int _idMaterialActual;
    int _dificultad;
  
};

#endif