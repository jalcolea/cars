#ifndef OGREUTIL_H
#define OGREUTIL_H

#include "Ogre.h"

using namespace Ogre;

enum collisiontypes {
    COL_NOTHING = 0, //<Collide with nothing
    COL_FLOOR = 2, 
    COL_CAR =4, 
    COL_TRACK = 8, 
    COL_CAMERA = 16,
    COL_TRACK_COLISION = 32,
    COL_CHECK = 64
};

enum class keyPressed_flags
{
    NONE =           0,
    LEFT =      1 << 0,
    RIGHT =     1 << 1,
    UP =        1 << 2,
    DOWN =      1 << 3,
    INS =       1 << 4,
    DEL =       1 << 5,
    PGUP =      1 << 6,
    PGDOWN =    1 << 7,
    NUMPAD1 =   1 << 8,
    NUMPAD2 =   1 << 9,
    NUMPAD3 =   1 << 10,
    NUMPAD5 =   1 << 11,
    RECOLOCAR = 1 << 12
};

enum class camara_view : int
{
    SEMICENITAL,
    TRASERA_ALTA,
    TRASERA_BAJA,
    INTERIOR,
    TOTAL_COUNT       // Su valor será el número total de elementos de esta enum. Útil para modular.
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
