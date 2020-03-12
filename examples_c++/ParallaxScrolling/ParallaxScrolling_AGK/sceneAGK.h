#ifndef API_DEMO_TEST__SCENE_AGK_H
#define API_DEMO_TEST__SCENE_AGK_H


#include "jugimapAGK/jmAGK.h"

#include "engineIndependent/parallaxScene.h"





class ParallaxSceneAGK : public ParallaxScene
{
public:

    bool Init() override;
    void UpdateEngineObjects() override;

private:

    void AddErrorMessageTextNode();

};










#endif
