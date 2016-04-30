#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#define CROSSHAIR_MATERIAL_NAME "crosshair"

#include <Ogre.h>
#include <string>
#include <vector>
#include "OgreUtil.h"

using namespace std;
using namespace Ogre;

class Crosshair
{
public:
    Crosshair(SceneManager* sceneMgr, Camera* camera) : _sceneMgr(sceneMgr), 
                                                        _camera(camera), 
                                                        _mouseray(new Ogre::Ray), 
                                                        _nodeCrosshair(nullptr),
                                                        _actualHitPoint(Ogre::Vector3(0,0,0)),
                                                        _entCrossHair(nullptr)
    {
        _plane = Ogre::Plane(_camera->getDerivedDirection(),Ogre::Vector3(0,0,-4));
    };
    
    ~Crosshair();
    Ogre::SceneNode* createCrossHairManual(const std::string & crosshairImg);
    Ogre::SceneNode* createCrossHair(const Ogre::Entity & entCrosshair);
    void setActualHitPoint(Ogre::Real xMouse, Ogre::Real yMouse); // OJO: VALORES NORMALIZADOS [0..1], SINO EL CÁLCULO TE MANDA A BOSTON :D
    const Ogre::Vector3 & getActualHitPoint();
    void setCamera(Camera* camera); // Si cambiamos la camara automáticamente regeneraremos el plano.
    void setMaterialCrosshair(const string & crosshairImg);
    
private:
    Ogre::SceneManager* _sceneMgr;
    Ogre::Camera* _camera;
    std::unique_ptr<Ogre::Ray> _mouseray; // {new Ogre::Ray};
    Ogre::SceneNode* _nodeCrosshair;
    Ogre::Vector3 _actualHitPoint;
    std::pair<bool,Ogre::Real> _intersectionResult;
    Ogre::Plane _plane;
    Ogre::MaterialPtr _material;
    Ogre::Entity* _entCrossHair;
};

#endif // CROSSHAIR_H
