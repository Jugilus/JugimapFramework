#ifndef API_DEMO_TEST__SCENE_NCINE_H
#define API_DEMO_TEST__SCENE_NCINE_H


#include "jugimapNCINE/jmNCine.h"
#include "jugiApp/app.h"
#include "jugiApp/scene.h"



namespace jugiApp{



class PlatformerSceneNCINE : public PlatformerScene
{
public:


protected:
    void UpdateTexts() override;


};




class SceneFactoryNCINE : public SceneFactory
{
public:

    PlatformerScene* NewPlatformerScene() override { return new PlatformerSceneNCINE(); }

};



}


#endif
