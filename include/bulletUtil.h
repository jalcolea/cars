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

#endif