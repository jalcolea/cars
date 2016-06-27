#ifndef PUNTOMANAGER_H
#define PUNTOMANAGER_H

#include <iostream>
#include <random>
#include "puntoIA.h"

typedef std::vector< punto_t > lista_puntosIA_t;

class puntoManager
{
    public:
        explicit puntoManager(){ clearPuntos(); };
        virtual ~puntoManager();

        inline void addPunto(punto_t p){ _puntos.push_back(p); };
        inline void clearPuntos(){ _puntos.clear(); };
        inline lista_puntosIA_t & getPuntos(){ return _puntos; };
        inline punto_t & getPunto(size_t i){ return _puntos.at(i); };

        // Si aleatorio = true, "i_derivado" será ignorado y se dará uno cualquiera de la lista de derivados para el punto "i"
        // NO_SANITY_CHECK!!!!! si el usuario quiere uno en concreto tendrá que ser válido, de lo contrario "CORE DUMP" muyyyyy probable.
        // Si un punto no tiene derivados se devolverá ese mismo punto.
        Ogre::Vector3 getPuntoDerivado(size_t i_punto,size_t i_derivado, bool aleatorio = false);

        // Calcula una desviacion en un punto sobre uno de sus ejes. El parámetro "magnitud" establece el ancho
        // total a lo largo de ese eje, es decir, si el ancho va de -7 a 7, magnitud valdrá 14. El parametro "cuantos"
        // sirve para saber cuantos calcular. Si "distribuir" es true, entonces el parametro "cuantos"
        // también sirve para establecer rangos separados para cada uno de los valores aleatorios resultantes.
        // El parámetro "margen" sirve para añadir un margen entre los rangos que se manejan. Es decir, si hay que calcular
        // 3 números aleatorios dentro de 3 rangos distintos, se puede establecer un margen entre cada rango de modo que cada
        // número aleatorio resultante siempre esté como mínimo a un "margen" de distancia de otro.
        // EL MARGEN NO SE TIENE EN CUENTA EN LOS EXTREMOS DE TODO EL ANCHO, SI EL USUARIO QUIERE TENERLOS EN CUENTA
        // QUE APLIQUE EL MARGEN AL PARAMETRO MAGNITUD ANTES DE LLAMAR A LA FUNCION (O SEA SI EL ANCHO QUE MANEJAMOS ES 14
        // Y EL MARGEN ES 1 Y QUEREMOS QUE SE APLIQUE EN LOS EXTREMOS, DEBEREMOS PASAR: MAGNITUD = 14 - (2 * MARGEN) = 14-(2*1) = 12
        void derivaPuntos(Ogre::Real magnitud, size_t cuantos, bool distribuir, Ogre::Vector3 eje, Ogre::Real margen = 0);
        
        void clearDerivados();
        
        // Dado un SceneNode convierte un punto local a global
        Ogre::Vector3 conviertePuntoAWS(Ogre::SceneNode* nodoOgre,const Ogre::Vector3& punto, const Ogre::Vector3& trasladaAlBorde = Ogre::Vector3::ZERO);
        
        Ogre::Vector3 trasladaAlBorde(Ogre::SceneNode* nodoOgre,  const Ogre::Vector3& punto, const Ogre::Vector3& borde);


    private:
        lista_puntosIA_t _puntos;

        Ogre::Real deriva(Ogre::Real minVal,Ogre::Real maxVal);


};

#endif // PUNTOMANAGER_H
