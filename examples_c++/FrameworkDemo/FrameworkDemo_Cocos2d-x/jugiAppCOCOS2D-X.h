#ifndef API_DEMO_TEST__SCENE_COCOS2D_H
#define API_DEMO_TEST__SCENE_COCOS2D_H


#include "jugimapCOCOS2D-X/jmCocos2d-x.h"
//#include "engineIndependent/platformerScene.h"

#include "jugiApp/app.h"
#include "jugiApp/scene.h"

namespace jugiApp{




class PlatformerSceneCC;




// PlatformerSceneCC extends PlatformerScene with engine specific features.
// We could 'merge' these two classes and get rid of some extra parameters for dealing with different engines
// but this approach allow easier porting.

class PlatformerSceneCC : public PlatformerScene
{
public:

    bool Init() override;

protected:
    void SetPhysicsSimulationDisabled(bool _disabled) override;
    void SetDynamicCrystalsPhysics() override;
    void UpdateTexts() override;
    void AssignCamerasToMaps() override;


};




class SceneFactoryCC : public SceneFactory
{
public:

    PlatformerScene* NewPlatformerScene() override { return new PlatformerSceneCC(); }

};



}


#endif
