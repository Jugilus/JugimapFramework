#ifndef API_DEMO_TEST__SCENE_AGK_H
#define API_DEMO_TEST__SCENE_AGK_H


#include "jugimapAGK/jmAGK.h"
#include "jugiApp/app.h"
#include "jugiApp/scene.h"



namespace jugiApp{



// PlatformerSceneAGK extends PlatformerScene to add engine specific features.
class PlatformerSceneAGK : public PlatformerScene
{
public:

    bool Init() override;

protected:
    void SetDynamicCrystalsPhysics() override;
    void UpdateTexts() override;

};



class SceneFactoryAGK : public SceneFactory
{
public:
    PlatformerScene* NewPlatformerScene() override { return new PlatformerSceneAGK(); }
};


}


#endif
