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

struct nodoOgre                                 // struct con los campos que se leerán del fichero de configuración.
{
    string nombreNodo;                          // nombre interno del SceneNode para el SceneManager de Ogre
    string nombreEntidad;                       // nombre interno del Entity de Ogre
    string nombreMalla;                         // nombre de la malla, es decir, el fichero.mesh
    string nombreMaterial;                      // nombre del material que usará el Entity, en caso de poder cambiarse.
    Ogre::Quaternion orientacion;               // orientacion del scenenode
    Ogre::Quaternion orientacionShapeBullet;    // orientación del shape de bullet que envolverá al modelo
    Ogre::Vector3 posInicial;                   // posicion inicial del SceneNode
    Ogre::Vector3 posShapeBullet;               // posición inicial del shape de bullet
    Ogre::Real masaBullet;                      // propiedad masa del rigidbody
    Ogre::Real frictionBullet;                  // propiedad índice de fricción del rigidbody
    Ogre::Real bodyRestitutionBullet;           // propiedad índice de restitución (elasticidad) del rigidbody
    
    friend ostream& operator<<(ostream& o, nodoOgre &n)
    {
       o << "Nombre del SceneNode: "            << n.nombreNodo << endl <<
            "\t Nombre del Entity adjunto: "    << n.nombreEntidad << endl <<
            "\t Malla que se está usando: "     << n.nombreMalla << endl <<
            "\t Material que se está usando: "  << n.nombreMaterial << endl <<
            "\t Posicion inicial: "             << n.posInicial << endl <<
            "\t Orientacion: "                  << n.orientacion << endl <<
            "\t Orientacion shape de Bullet: "  << n.orientacionShapeBullet << endl <<
            "\t Posicion del shape de Bullet "  << n.posShapeBullet << endl <<
            "\t Masa del objeto de Bullet: "    << n.masaBullet << endl <<
            "\t Indice de Friccion Bullet: "    << n.frictionBullet << endl <<
            "\t Indice de Restitucion Bullet: " << n.bodyRestitutionBullet << endl;
       return o;
    };

};   

struct nodoCamera
{
    Ogre::Quaternion orientacion;
    string nombreCamera;
    Ogre::Vector3 posInicial;
    Ogre::Vector3 lookAt;
    Ogre::Vector3 direccion;
    Ogre::Real nearClipDistance;
    Ogre::Real farClipDistance;

    friend ostream& operator<<(ostream& o, nodoCamera &n)
    {
       o << "Nombre de la camara: "             << n.nombreCamera << endl <<
            "\t Posicion inicial: "             << n.posInicial << endl <<
            "\t Orientación: "                  << n.orientacion << endl <<
            "\t LookAt: "                       << n.lookAt << endl <<
            "\t Dirección: "                    << n.direccion << endl <<
            "\t Near Clip Distance: "           << n.nearClipDistance << endl <<
            "\t Far Clip Distance: "            << n.farClipDistance << endl;
       return o;
    };
    
    
};                             

typedef nodoOgre nodoOgre_t;                    // declaración del tipo nodoOgre_t a partir del struct declarado "above" :D
typedef nodoCamera nodoCamera_t;                // declaración del tipo nodoCamera_t a partir del struct declarado "above" :D


typedef std::map <string,nodoOgre_t> map_nodos_t;
typedef map_nodos_t::iterator it_map_nodos;
typedef std::map <string,nodoCamera_t> map_cameras_t;
typedef map_cameras_t::iterator it_map_cameras;



class SceneNodeConfig : public Ogre::Singleton<SceneNodeConfig>
{
    private:
        void nuevoSceneNode(mxml_node_t* node);              // Añade un nuevo par(clave,valor) al mapa map_nodos
        void nuevoCamera(mxml_node_t* node);
        Ogre::Vector3 extraeVector3(mxml_node_t* node);      // Crea un Vector3 a partir de los valores en texto
        Ogre::Quaternion extraeQuaternio(mxml_node_t* node); // Crea un Quaternio a partir de los valores en texto
        string _fichero;                                     // nombre del fichero que contiene la info (ruta absoluta please!)
        map_nodos_t map_nodos;                               // mapa que alberga la info de un SceneNode (clave: nombre del SceneNode, valor: objeto de tipo nodoOgre_t)
        map_cameras_t map_cameras;                           // mapa que alberga la info de una camara 
        mxml_node_t* _tree;                                  // puntero a la raíz del arbol de nodos xml.
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
                                      X,
                                      Y,
                                      Z,
                                      W,
                                      NOMBRE_CAMARA,
                                      ORIENTACION,
                                      DIRECCION,
                                      LOOKAT,
                                      NEAR_CLIPDISTANCE,
                                      FAR_CLIPDISTANCE,
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
                                                                                 "bodyRestitutionBullet",
                                                                                 "x",
                                                                                 "y",
                                                                                 "z",
                                                                                 "w",
                                                                                 "nombreCamera",
                                                                                 "orientacion",
                                                                                 "direccion",
                                                                                 "posLookAt",
                                                                                 "nearClipDistance",
                                                                                 "farClipDistance"}; // Lista de cadenas para los atributos de un nodo xml

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
        inline nodoOgre_t getInfoNodoOgre(const string& clave) { return map_nodos[clave]; };// devuelve un paquete de info nodoOgre_t
        inline map_nodos_t& getMapNodos(){ return map_nodos;};                     // getter para el mapa de SceneNodes. 
        inline nodoCamera_t getInfoCamera(const string& clave){ return map_cameras[clave]; }; // devuelve un paquete de info nodoCamera_t
        inline map_cameras_t& getMapCameras(){ return map_cameras;};               // getter para el mapa de Cameras
        inline void cleanOptions() { map_nodos.clear(); };                         // se cepilla las opciones cargadas actualmente.
};

#endif // SCENENODECONFIG_H
