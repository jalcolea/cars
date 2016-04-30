#include "Crosshair.h"

Crosshair::~Crosshair()
{
    OgreUtil::destroyAllAttachedMovableObjects(this->_nodeCrosshair);
    OgreUtil::destroySceneNode(_nodeCrosshair);
    _sceneMgr->destroyManualObject("crossHair");
}

Ogre::SceneNode* Crosshair::createCrossHairManual(const std::string & crosshairImg)
{

    setMaterialCrosshair(crosshairImg);
    
    // Creamos un objeto manual para el Crosshair
    
    ManualObject* crossHair= _sceneMgr->createManualObject("crossHair");
     
    // Matriz identidad para que se vea en 2D
    crossHair->setUseIdentityProjection(true);
    crossHair->setUseIdentityView(true);
     
    //Esto es como un wrapper de openGl
    crossHair->begin(CROSSHAIR_MATERIAL_NAME, RenderOperation::OT_TRIANGLE_STRIP);  // begin(nombreMaterial, tipoPrimitiva)
        crossHair->position(-0.2, -0.2, 0.0);           //position crea un vértice en la posicion dada
        crossHair->textureCoord(0,1);                   //con textureCoord hacemos mapeado UV
        crossHair->position( 0.2, -0.2, 0.0);
        crossHair->textureCoord(1,1);
        crossHair->position( 0.2,  0.2, 0.0);
        crossHair->textureCoord(1,0);
        crossHair->position(-0.2,  0.2, 0.0);
        crossHair->textureCoord(0,0);
        crossHair->index(0);                            //Indexamos  para indicar como se va dibujando la primitiva.
        crossHair->index(1);
        crossHair->index(2);
        crossHair->index(3);
        crossHair->index(0);
    crossHair->end();
    
    _entCrossHair = _sceneMgr->createEntity(crossHair->convertToMesh("crossHairMesh"));
    //entCrossHair->setMaterial(material);
    
    // Uso de una Aligned Axis Box (AAB) infinita para estar siempre visible. O eso es 
    // lo que pone en el tutorial que he mirado
    AxisAlignedBox aabInf;
    aabInf.setInfinite();
    crossHair->setBoundingBox(aabInf);
     
    // Renderizado antes que naide :D
    crossHair->setRenderQueueGroup(RENDER_QUEUE_OVERLAY - 1);
    
    _nodeCrosshair = _sceneMgr->getRootSceneNode()->createChildSceneNode("CrossHair");
    _nodeCrosshair->attachObject(_entCrossHair);
    _nodeCrosshair->scale(2.0,2.0,2.0);
    
    return _nodeCrosshair;
}

// Esto está por ver si lo dejo. Sería para permitir pasar una entidad preparada para actuar como un 
// crosshair pero personalizado por el usuario de la clase. 
Ogre::SceneNode* Crosshair::createCrossHair(const Ogre::Entity & entCrosshair) {return nullptr;}

void Crosshair::setActualHitPoint(Ogre::Real xMouse, Ogre::Real yMouse) // Valores normlizados [0..1], es decir xMouse = xMouse/Width
{
    //Le pedimos a la cámara un rayo (devuelto en el mismo parámetro _mouseray
    //_mouseray es un unique_ptr, y la función espera un puntero, de modo que usamos _mouseray.get() para que
    //el unique_ptr nos devuelva el puntero de verdad.
    _camera->getCameraToViewportRay(xMouse,yMouse,_mouseray.get()); 
   
    //Comprobamos que el rayo devuelto por la camara "choca" con el plano en el que la cámara se proyecta
    _intersectionResult = _mouseray->intersects(_plane);
    //El par que devuelve tiene como primer elemento un booleano indicando si ha habido "choque" con el plano
    if(_intersectionResult.first)
    {
        //Si ha habido choque pues podemos pedirle al rayo que nos devuelva el vector (getPoint) para colocar 
        //el objeto (el punto de mira) en el lugar donde está el ratón.
        _actualHitPoint = _mouseray->getPoint(_intersectionResult.second);
        _nodeCrosshair->setPosition(_actualHitPoint);
    }
    
}

const Ogre::Vector3 & Crosshair::getActualHitPoint()
{
    return _actualHitPoint;
}

void Crosshair::setCamera(Camera* camera) // Si cambiamos la camara automáticamente regeneraremos el plano.
{
    _camera = camera;
    _plane = Ogre::Plane(_camera->getDerivedDirection(),Ogre::Vector3(0,0,0));
}


void Crosshair::setMaterialCrosshair(const string & crosshairImg)
{
    _material = MaterialManager::getSingleton().create(CROSSHAIR_MATERIAL_NAME, "General",false);
    _material->getTechnique(0)->getPass(0)->createTextureUnitState(crosshairImg);
    _material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    _material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    _material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
    _material->getTechnique(0)->getPass(0)->setCullingMode(CullingMode::CULL_NONE);
    _material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    
    if (_entCrossHair)
        _entCrossHair->setMaterial(_material);
}
