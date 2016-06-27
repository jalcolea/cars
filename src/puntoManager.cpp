#include "puntoManager.h"


puntoManager::~puntoManager()
{
    //dtor
}

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
void puntoManager::derivaPuntos(Ogre::Real magnitud, size_t cuantos, bool distribuir, Ogre::Vector3 eje, Ogre::Real margen)
{
    Ogre::Real minimoTotal = - (magnitud * 0.5); // Obtenemos el valor mínimo del rango global. (Si magnitud=14 entonces -7
    Ogre::Real maximoTotal = -minimoTotal;       // El valor máximo del rango global igual al minimo pero signo cambiado (Si magnitud=14 entonces 7).

    if (cuantos < 1) cuantos = 1; // cascar un assert es un poco brusco no? :D Lo mínimo es calcular un resultado.

    for (size_t j = 0; j<_puntos.size(); j++)
    {

        if (distribuir)
        {
            Ogre::Real magnitudRango = (magnitud / cuantos); // magnitud de cada rango.
            assert(margen < cuantos  && "Error: el margen es demasiado grande, ha de ser menor que 'cuantos'");
            Ogre::Real minVal = minimoTotal;
            Ogre::Real maxVal = minVal + abs(magnitudRango - margen);
            for (size_t i = 1; i<= cuantos; i++)
            {
                Ogre::Vector3 aux = (deriva(minVal,maxVal) // obtenemos un valor aleatorio en el rango indicado
                                    + Ogre::Vector3::ZERO) // Se lo sumamos al un Vector3::ZERO, p.ej: (0,0,0) + 0.34 = (0.34,0.34,0.34)
                                    * eje;           // Y al vector resultante lo multiplicamos por el eje, p.ej: (0.34,0.34,0.34)*(1,0,0) = (0.34,0,0)
                _puntos.at(j).derivados.push_back(aux);
                minVal = maxVal + margen;
                if (i<cuantos-1)
                    maxVal = minVal + abs(magnitudRango - margen);
                else
                    maxVal = minVal + magnitudRango; // El último rango tendrá como máximo el máximo posible.
            }
        }
        else
        {
            for (size_t i = 1; i<= cuantos; i++)
            {
                Ogre::Vector3 aux = (deriva(minimoTotal,maximoTotal) + Ogre::Vector3::ZERO) * eje;
                _puntos.at(j).derivados.push_back(aux);
            }
        }
    }

}

// Si aleatorio = true, "i_derivado" será ignorado y se dará uno cualquiera de la lista de derivados para el punto "i"
// NO_SANITY_CHECK!!!!! si el usuario quiere uno en concreto tendrá que ser válido, de lo contrario "CORE DUMP" muyyyyy probable.
// Si un punto no tiene derivados se devolverá ese mismo punto.
Ogre::Vector3 puntoManager::getPuntoDerivado(size_t i_punto,size_t i_derivado, bool aleatorio)
{
    Ogre::Vector3 aux;

    if (_puntos.at(i_punto).derivados.size())
    {
        if (aleatorio)
        {
            srand(time(nullptr));
            aux = _puntos.at(i_punto).derivados.at(rand()%_puntos.at(i_punto).derivados.size());
        }
        else
            aux = _puntos.at(i_punto).derivados.at(i_derivado);
    }
    else aux = _puntos.at(i_punto).p;

    return aux;
}

Ogre::Real puntoManager::deriva(Ogre::Real minVal,Ogre::Real maxVal)
{
   static std::random_device rdev;
   static std::default_random_engine e(rdev());
   std::uniform_real_distribution<float> dis(minVal,maxVal);

   float rf = dis(e);

   std::cout << "min: " << minVal << std::endl;
   std::cout << "max: " << maxVal << std::endl;
   std::cout << "nº aleatorio generado: " << rf << std::endl;
   return rf;

}


void puntoManager::clearDerivados()
{
    for (size_t i=0; i<_puntos.size(); i++)
        _puntos.at(i).derivados.clear();
            
}

Ogre::Vector3 puntoManager::conviertePuntoAWS(Ogre::SceneNode* nodoOgre,const Ogre::Vector3& punto, const Ogre::Vector3& trasladaAlBorde)
{
    return nodoOgre->convertLocalToWorldPosition(punto + trasladaAlBorde);
}

Ogre::Vector3 puntoManager::trasladaAlBorde(Ogre::SceneNode* nodoOgre, const Ogre::Vector3& punto, const Ogre::Vector3& borde)
{
    return conviertePuntoAWS(nodoOgre,nodoOgre->convertWorldToLocalPosition(punto),borde);
}

