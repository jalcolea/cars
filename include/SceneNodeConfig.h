#ifndef SCENENODECONFIG_H
#define SCENENODECONFIG_H

#include <iostream>
#include <string>
#include <fstream>
#include <Ogre.h>
#include <vector>
#include <mxml.h>
#include <map>
#include "soundUtil.h"

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
    Ogre::Vector3 escala;                       // Escalado inicial del modelo
    Ogre::Real masaBullet;                      // propiedad masa del rigidbody
    Ogre::Real frictionBullet;                  // propiedad índice de fricción del rigidbody
    Ogre::Real bodyRestitutionBullet;           // propiedad índice de restitución (elasticidad) del rigidbody
    int dificultad;                             
    
    friend ostream& operator<<(ostream& o, nodoOgre &n)
    {
       o << "Nombre del SceneNode: "            << n.nombreNodo << endl <<
            "\t Nombre del Entity adjunto: "    << n.nombreEntidad << endl <<
            "\t Malla que se está usando: "     << n.nombreMalla << endl <<
            "\t Material que se está usando: "  << n.nombreMaterial << endl <<
            "\t Posicion inicial: "             << n.posInicial << endl <<
            "\t Escala: "                       << n.escala << endl <<
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

struct nodoVehiculoRayCast
{
    string nombre;
    string nombreMallaRueda;
    string nombreMallaChasis;
    string nombreMaterial;                      // nombre del material que usará el Entity, en caso de poder cambiarse.
    Ogre::Quaternion orientacion;               // orientacion del scenenode
    Ogre::Quaternion orientacionShapeBullet;    // orientación del shape de bullet que envolverá al modelo
    Ogre::Vector3 posShapeBullet;               // posición inicial del shape de bullet
    Ogre::Vector3 escala;                       // escalado inicial del modelo
    Ogre::Real masaBullet;                      // propiedad masa del rigidbody
    Ogre::Real frictionBullet;                  // propiedad índice de fricción del rigidbody
    Ogre::Real bodyRestitutionBullet;           // propiedad índice de restitución (elasticidad) del rigidbody
    Ogre::Real radioRuedas;
    Ogre::Real anchoRuedas;
    Ogre::Real friccionRueda;
    Ogre::Real influenciaRodado;
    Ogre::Real indiceRestitucionSuspension;
    Ogre::Vector3 posicion;
    Ogre::Real velocidadGiro;
    Ogre::Real aceleracion; 
    Ogre::Real frenada;
    Ogre::Real aceleracionMarchaAtras;
    Ogre::Real suspensionStiffness;   //  dureza de la suspensión
    Ogre::Real suspensionCompression; // indice de compresión de la suspensión
    Ogre::Real suspensionDamping;     // indice de restitución de la suspensión
    Ogre::Real maxSuspensionTravelCm; // limite del recorrido de la suspensión (entiendo que al comprimirse el muelle)
    Ogre::Real maxSuspensionForce;    // límite máximo de la fuerza de la suspensión
    Ogre::Real frictionSlip;          // indice de fricción (valor muy gordo, del orden 10e20 o 10e30)
    std::vector<Vector3> posRuedas;        // Vector que almacena la posición de las ruedas respecto a su chasis
    Vector3 escalaRueda;                   // factor de escala para aumentar el ancho de las ruedas traseras si se quiere.
    Ogre::Real potenciadorPrimera;
    bool traccionTrasera;
    skidValues skid;
    
    friend ostream& operator<<(ostream& o, nodoVehiculoRayCast &n)
    {
       o << "Nombre del Vehiculo Ray Cast: "    << n.nombre << endl <<
            "\t mesh Ruedas: "                  << n.nombreMallaRueda << endl <<
            "\t mesh Chasis: "                  << n.nombreMallaChasis << endl <<
            "\t Material que se está usando: "  << n.nombreMaterial << endl <<
            "\t Orientacion: "                  << n.orientacion << endl <<
            "\t Orientacion shape de Bullet: "  << n.orientacionShapeBullet << endl <<
            "\t Posicion del shape de Bullet "  << n.posShapeBullet << endl <<
            "\t Escala: "                       << n.escala << endl <<
            "\t Masa del objeto de Bullet: "    << n.masaBullet << endl <<
            "\t Indice de Friccion Bullet: "    << n.frictionBullet << endl <<
            "\t Indice de Restitucion Bullet: " << n.bodyRestitutionBullet << endl <<
            "\t radio rueda: "                  << n.radioRuedas << endl <<
            "\t ancho rueda: "                  << n.anchoRuedas << endl <<
            "\t coeficiente friccion rueda: "   << n.friccionRueda << endl <<
            "\t influencia de rodado: "         << n.influenciaRodado << endl <<
            "\t indice restitucion susp.: "     << n.indiceRestitucionSuspension << endl <<
            "\t posicion: "                     << n.posicion << endl <<
            "\t velocidad de giro: "            << n.velocidadGiro << endl <<
            "\t acelaracion: "                  << n.aceleracion << endl <<
            "\t frenada: "                      << n.frenada << endl << 
            "\t aceleracion marcha atras: "     << n.aceleracionMarchaAtras << endl <<
            "\t dureza de la suspensión: "      << n.suspensionStiffness << endl <<
            "\t indice de compresión de la suspensión: " << n.suspensionCompression << endl <<
            "\t indice de restitución de la suspensión: " << n.suspensionDamping << endl <<
            "\t limite del recorrido de la suspensión: " << n.maxSuspensionTravelCm << endl <<
            "\t límite máximo de la fuerza de la suspensión: " << n.maxSuspensionForce << endl <<
            "\t indice de fricción (valor muy gordo, del orden 10e20 o 10e30): " << n.frictionSlip << endl <<
            "\t factor de escala rueda trasera: " << n.escalaRueda << endl;
       return o;
    };
};                          

typedef nodoOgre nodoOgre_t;                       // declaración del tipo nodoOgre_t a partir del struct declarado "above" :D
typedef nodoCamera nodoCamera_t;                   // declaración del tipo nodoCamera_t a partir del struct declarado "above" :D
typedef nodoVehiculoRayCast nodoVehiculoRayCast_t; // ídem


typedef std::map <string,nodoOgre_t> map_nodos_t;
typedef map_nodos_t::iterator it_map_nodos;
typedef std::map <string,nodoCamera_t> map_cameras_t;
typedef map_cameras_t::iterator it_map_cameras;
typedef std::map <string,nodoVehiculoRayCast_t> map_vehiculos_ray_cast_t;
typedef map_vehiculos_ray_cast_t::iterator it_map_vehiculos_ray_cast;



class SceneNodeConfig : public Ogre::Singleton<SceneNodeConfig>
{
    private:
        void nuevoSceneNode(mxml_node_t* node);              // Añade un nuevo par(clave,valor) al mapa map_nodos
        void nuevoCamera(mxml_node_t* node);
        void nuevoVehiculoRayCast(mxml_node_t* node);
        Ogre::Vector3 extraeVector3(mxml_node_t* node);      // Crea un Vector3 a partir de los valores en texto
        Ogre::Quaternion extraeQuaternio(mxml_node_t* node); // Crea un Quaternio a partir de los valores en texto
        string _fichero;                                     // nombre del fichero que contiene la info (ruta absoluta please!)
        map_nodos_t map_nodos;                               // mapa que alberga la info de un SceneNode (clave: nombre del SceneNode, valor: objeto de tipo nodoOgre_t)
        map_cameras_t map_cameras;                           // mapa que alberga la info de una camara 
        map_vehiculos_ray_cast_t map_vehiculos_raycast;      // mapa que alberga info de un VehiculoRayCast
        mxml_node_t* _tree;                                  // puntero a la raíz del arbol de nodos xml.
        enum class xmlElementsIndex { NOMBRE_NODO,NOMBRE_ENTIDAD,NOMBRE_MALLA,NOMBRE_MATERIAL,POS_INICIAL,
                                      ORIENTACION_SHAPE,POS_SHAPE,MASA,FRICTION,RESTITUTION,X,Y,Z,W,
                                      NOMBRE_CAMARA,ORIENTACION,DIRECCION,LOOKAT,NEAR_CLIPDISTANCE,FAR_CLIPDISTANCE,
                                      // A partir de aquí para cadenas de un VehiculoRayCast, alguna anterior se usa para este mismo tipo de nodo                                      
                                      NOMBRE_MALLA_RUEDA,NOMBRE_MALLA_CHASIS,RADIO_RUEDAS,ANCHO_RUEDAS,FRICCION_RUEDA,INFLUENCIA_RODADO,
                                      INDICE_REST_SUSP,POSICION,VELOCIDAD_GIRO,ACELERACION,FRENADA,ACELERACION_MARCHA_ATRAS,
                                      // Cadenas para los parametros de la suspension
                                      SUSPENSION_STIFFNESS,SUSPENSION_COMPRESION,SUSPENSION_DAMPING,MAX_SUSPENSION_TRAVEL_CM,
                                      MAX_SUSPENSION_FORCE,FRICTION_SLIP,POSRUEDA0,POSRUEDA1,POSRUEDA2,POSRUEDA3,ESCALARUEDA,
                                      ESCALA,DIFICULTAD,POTENCIADORPRIMERA,TRACCIONTRASERA,MAXSKIDVALUE,SKIDCORTO,SKIDMEDIO,SKIDLARGO,
                                      TOTAL_COUNT }; // Este último es un truquillo para saber el número de elementos de esta enum class.
                                      
        string _xmlElements[static_cast<size_t>(xmlElementsIndex::TOTAL_COUNT)] = {"nombreNodo","nombreEntidad","nombreMalla",
                                                                                 "nombreMaterial","posInicial","orientacionShapeBullet",
                                                                                 "posShapeBullet","masaBullet","frictionBullet",
                                                                                 "bodyRestitutionBullet","x","y","z","w",
                                                                                 "nombreCamera","orientacion","direccion","posLookAt",
                                                                                 "nearClipDistance","farClipDistance","nombreMallaRueda",
                                                                                 "nombreMallaChasis",
                                                                                 "radioRuedas", // posiblemente se pueda obtener del mesh
                                                                                 "anchoRuedas", // posiblemente se pueda obtener del mesh
                                                                                 "friccionRueda","influenciaRodado","indiceRestitucionSuspension",
                                                                                 "posicion","velocidadGiro","aceleracion",
                                                                                 "frenada","aceleracionMarchaAtras","suspensionStiffness",
                                                                                 "suspensionCompresion","suspensionDamping","maxSuspensionTravelCM",
                                                                                 "maxSuspensionForce", "frictionSlip", "posRueda0", "posRueda1",
                                                                                 "posRueda2", "posRueda3", "escalaRuedaTrasera", "escala","dificultad","potenciadorPrimera",
                                                                                 "traccionTrasera","maxSkidValue","skidCorto","skidMedio,skidLargo"
                                                                                 }; // Lista de cadenas para los atributos de un nodo xml
    public:
        // Heredados de Ogre::Singleton.
        static SceneNodeConfig &getSingleton();
        static SceneNodeConfig *getSingletonPtr();

        SceneNodeConfig() : _fichero(""), _tree(nullptr){};

        virtual ~SceneNodeConfig()                                                         // Se liberan los recursos adquiridos.
        {
            mxmlDelete(_tree);
        };

        bool load_xml (string fichero);                                                    // carga un fichero de configuración dado por parámetro, actualiza el miembro _fichero interno.
        inline bool load_xml () { return load_xml(_fichero); };                            // si _fichero no ha cambiado vendría a ser un reload del mismo fichero de configuración.
        void setFicheroConfiguracion(string fichero, bool Cargar = false);                 // establece que fichero de configuracón manejar. Llamar a load_xml para actualizar (parámetro Cargar).
        inline string getFicheroConfiguracion(){ return _fichero; };                       // un getter de _fichero.
        inline nodoOgre_t getInfoNodoOgre(const string& clave) { return map_nodos[clave]; };// devuelve un paquete de info nodoOgre_t
        inline map_nodos_t& getMapNodos(){ return map_nodos;};                              // getter para el mapa de SceneNodes. 
        inline nodoCamera_t getInfoCamera(const string& clave){ return map_cameras[clave]; }; // devuelve un paquete de info nodoCamera_t
        inline map_cameras_t& getMapCameras(){ return map_cameras;};                        // getter para el mapa de Cameras
        inline map_vehiculos_ray_cast_t& getMapVehiculosRaycast(){ return map_vehiculos_raycast; }; // getter para el mapa de vehiculos Raycast
        inline nodoVehiculoRayCast_t& getInfoVehiculoRayCast(const string& clave){ return map_vehiculos_raycast[clave]; }; // devuelve un paquete de info nodoVehiculoRayCast_t
        inline void clear() { map_nodos.clear(); map_cameras.clear(); map_vehiculos_raycast.clear(); }; // se cepilla la info de todo cargada actualmente.
};

#endif // SCENENODECONFIG_H
