#ifndef BULLETUTIL_H
#define BULLETUTIL_H

enum class tipoRigidBody
{
    CIRCUITO,
    CARRETERA,
    COCHE,
    OBSTACULO,
    CHECK_POINT,
    TOTAL_COUNT
};

struct rigidBody_data
{
    rigidBody_data(tipoRigidBody tipo,void* data) : _tipo(tipo), _data(data){}
    tipoRigidBody _tipo;
    void* _data;
};

// PARA EL USERPOINTER DE btCollisionObject*, HAY QUE VER DONDE IRÍA MEJOR ESTO O SI SE PUEDE APROVECHAR OTRA ESTRUCTURA
struct CheckPoint_data
{
    CheckPoint_data(size_t id, string nombre, Ogre::Vector3 posicion = Ogre::Vector3::ZERO, Ogre::SceneNode* ogreNode = nullptr):_id(id),_nombre(nombre),_worldPosition(posicion),_ogreNode(ogreNode){}
    size_t _id;
    string _nombre;
    Ogre::Vector3 _worldPosition;
    Ogre::SceneNode* _ogreNode;
};


#endif