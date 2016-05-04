#ifndef SCENENODECONFIG_H
#define SCENENODECONFIG_H

#include <iostream>
#include <string>
#include <fstream>
#include <Ogre.h>
#include <vector>
#include <mxml.h>
#include <map>

using namespace Ogre;
using namespace std;

typedef struct                                  // struct con los campos que se leerán del fichero de configuración.
{
    string nombreNodo;                          // nombre interno del SceneNode para el SceneManager de Ogre
    string nombreEntidad;                       // nombre interno del Entity de Ogre
    string nombreMalla;                         // nombre de la malla, es decir, el fichero.mesh
    string nombreMaterial;                      // nombre del material que usará el Entity, en caso de poder cambiarse.
    Ogre::Vector3 posInicial;                   // posicion inicial del SceneNode
    Ogre::Quaternion orientacionShapeBullet;    // orientación del shape de bullet que envolverá al modelo
    Ogre::Vector3 posShapeBullet;               // posición inicial del shape de bullet
    Ogre::Real masaBullet;                      // propiedad masa del rigidbody
    Ogre::Real frictionBullet;                  // propiedad índice de fricción del rigidbody
    Ogre::Real bodyRestitutionBullet;           // propiedad índice de restitución (elasticidad) del rigidbody

} nodoOgre_t;                                   // definición del tipo nodoOgre_t


typedef std::map <string,nodoOgre_t> map_nodos_t;
typedef map_nodos_t::iterator it_map_nodos;

class SceneNodeConfig : public Ogre::Singleton<SceneNodeConfig>
{
    private:
        void nuevoSceneNode(mxml_node_t* node);
        string _fichero;
        map_nodos_t map_nodos;
        mxml_node_t* _tree;
        enum class xmlElementsIndex { NOMBRE_NODO,
                                      NOMBRE_ENTIDAD,
                                      NOMBRE_MALLA,
                                      NOMBRE_MATERIAL,
                                      POS_INICIAL,
                                      ORIENTACION_SHAPE,
                                      POS_SHAPE,
                                      MASA,
                                      FRICTION,
                                      RESTITUTION,
                                      TOTAL_COUNT }; // Este último es un truquillo para saber el número de elementos de esta enum class.
        string _xmlElements[static_cast<size_t>(xmlElementsIndex::TOTAL_COUNT)] = {"nombreNodo",
                                                                                 "nombreEntidad",
                                                                                 "nombreMalla",
                                                                                 "nombreMaterial",
                                                                                 "posInicial",
                                                                                 "orientacionShapeBullet",
                                                                                 "posShapeBullet",
                                                                                 "masaBullet",
                                                                                 "frictionBullet",
                                                                                 "bodyRestitutionBullet"};

    public:
        // Heredados de Ogre::Singleton.
        static SceneNodeConfig &getSingleton();
        static SceneNodeConfig *getSingletonPtr();

        SceneNodeConfig() : _fichero(""), _tree(nullptr){};

        virtual ~SceneNodeConfig()                                                         // Se liberan los recursos adquiridos.
        {
            mxmlDelete(_tree);
        };

        bool load_xml (string fichero);                                            // carga un fichero de configuración dado por parámetro, actualiza el miembro _fichero interno.
        inline bool load_xml () { return load_xml(_fichero); };                    // si _fichero no ha cambiado vendría a ser un reload del mismo fichero de configuración.
        void setFicheroConfiguracion(string fichero, bool Cargar = false);         // establece que fichero de configuracón manejar. Llamar a load_xml para actualizar (parámetro Cargar).
        inline string getFicheroConfiguracion(){ return _fichero; };               // un getter de _fichero.
        inline nodoOgre_t getInfoNodoOgre(string clave) { return map_nodos[clave]; };// devuelve un paquete de info nodoOgre_t
        inline map_nodos_t& getMapNodos(){ return map_nodos;};
        inline void cleanOptions() { map_nodos.clear(); };                       // se cepilla las opciones cargadas actualmente.



};

#endif // SCENENODECONFIG_H
