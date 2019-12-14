#ifndef API_DEMO_TEST__SCENE_AGK_H
#define API_DEMO_TEST__SCENE_AGK_H


#include "jugimapAGK/jmAGK.h"
#include "engineIndependent/platformerScene.h"



namespace apiTestDemo{



// SceneAGK extends Scene with engine specific features.
// We could 'merge' these two classes and get rid of some extra parameters for dealing with different engines
// but this approach allow easier porting.

class PlatformerSceneSceneAGK : public PlatformerScene
{
public:

    bool Init() override;
    void UpdateEngineObjects() override;


protected:
    void SetDynamicCrystalsPhysics() override;
    void UpdateTexts() override;

};






}


#endif
