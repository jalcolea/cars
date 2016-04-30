#ifndef OGREUTIL_H
#define OGREUTIL_H

#include "Ogre.h"

using namespace Ogre;

enum collisiontypes {
    COL_NOTHING = 0, //<Collide with nothing
    COL_FLOOR = 2, 
    COL_ACTIVATOR =4, 
    COL_STAND = 8, 
    COL_CAMERA = 16,
    COL_ROBOT = 32,
    COL_CAN = 64,
    COL_BALL = 128,
    COL_BALA = 256,
    COL_HIT_ZONE = 512, //<Para el plano del jugador, si colisiona con él, le quita vida
    COL_ESCUDO = 1024,
    COL_PUERTA = 2048
};


struct MeshInfo
{
    size_t vertex_count;
    Ogre::Vector3* vertices;
    size_t index_count;
    unsigned long* indices;
    Vector3 posicion;
    Quaternion orientacion;
    Vector3 escala;
};



class OgreUtil {



 public:

  static void destroyAllAttachedMovableObjects( SceneNode* node );
  static void destroySceneNode( Ogre::SceneNode* node );
  static void getMeshInformation(const Ogre::Mesh* const mesh,
                        size_t &vertex_count,
                        Ogre::Vector3* &vertices,
                        size_t &index_count,
                        unsigned long* &indices,
                        const Vector3 &position = Vector3::ZERO,
                        const Quaternion &orient = Quaternion::IDENTITY,
                        const Vector3 &scale = Vector3::UNIT_SCALE);
};

#endif
