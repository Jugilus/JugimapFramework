#ifndef API_DEMO_TEST__SCENE_SFML_H
#define API_DEMO_TEST__SCENE_SFML_H


#include "jugimapSFML/jmSFML.h"
#include "jugiApp/app.h"
#include "jugiApp/scene.h"



namespace jugiApp{



class PlatformerSceneSFML;




// PlatformerSceneCC extends PlatformerScene with engine specific features.
// We could 'merge' these two classes and get rid of some extra parameters for dealing with different engines
// but this approach allow easier porting.

class PlatformerSceneSFML : public PlatformerScene
{
public:

    //bool Init() override;
    void Draw() override;

protected:
    void UpdateTexts() override;


};




class SceneFactorySFML : public SceneFactory
{
public:

    PlatformerScene* NewPlatformerScene() override { return new PlatformerSceneSFML(); }

};



}


#endif
