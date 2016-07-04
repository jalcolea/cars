#ifndef SOUNDUTIL_H
#define SOUNDUTIL_H
#include "Ogre.h"

struct infoCanalSonido
{
    int id;
    int volumen;
    int distancia;
};

struct canalesSonidoCoche
{
    infoCanalSonido MOTORUP;
    infoCanalSonido MOTORDOWN;
    infoCanalSonido SKIDING;
};

struct skidValues
{
    Ogre::Real maxSkidValue;
    Ogre::Real SkidCorto;
    Ogre::Real SkidMedio;
    Ogre::Real SkidLargo;
};



#endif // SOUNDUTIL_H