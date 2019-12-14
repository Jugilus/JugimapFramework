#ifndef API_DEMO_TEST__SFML_H
#define API_DEMO_TEST__SFML_H


#include "jugimapSFML/jmSFML.h"
#include "engineIndependent/platformerScene.h"



namespace apiTestDemo{


// SceneSFML extends Scene with engine specific features.
// We could 'merge' these two classes and get rid of some extra parameters for dealing with different engines
// but this approach allow easier porting.

class PlatformerSceneSFML : public PlatformerScene
{
public:
    friend class SceneNode;

    virtual ~PlatformerSceneSFML(){}

    bool Init() override;
    void Draw() override;


protected:
    void UpdateTexts() override;


};



}


#endif
