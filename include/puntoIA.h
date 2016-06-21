#ifndef _PUNTOIA_H
#define _PUNTOIA_H

#include "Ogre.h"
#include <vector>

struct punto
{
    std::vector<Ogre::Vector3> derivados;
    Ogre::Vector3 p;
    Ogre::Real speed_factor;
    void* user_data;
};

typedef punto punto_t;

#endif
