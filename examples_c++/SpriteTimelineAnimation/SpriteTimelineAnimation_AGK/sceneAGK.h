#ifndef API_DEMO_SCENE_AGK_H
#define API_DEMO_SCENE_AGK_H


#include "jugimapAGK/jmAGK.h"

#include "engineIndependent/scene.h"





class DemoSceneAGK : public DemoScene
{
public:

    bool Init() override;
    void UpdateEngineObjects() override;

private:

    void AddErrorMessageTextNode();

};










#endif
