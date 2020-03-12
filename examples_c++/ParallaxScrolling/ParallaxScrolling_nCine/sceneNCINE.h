#ifndef API_DEMO_TEST__SCENE_NCINE_H
#define API_DEMO_TEST__SCENE_NCINE_H

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <ncine/TimeStamp.h>
#include "jugimapNCINE/jmNCine.h"
#include "engineIndependent/parallaxScene.h"






// SceneNC extends Scene with engine specific features.
// We could 'merge' these two classes and get rid of some extra parameters for dealing with different engines
// but this approach allow easier porting.

class ParallaxSceneNC : public ParallaxScene
{
public:
    virtual ~ParallaxSceneNC() override;
    bool Init() override;

    ncine::SceneNode * GetNCNode(){ return ncNode;}


protected:


private:
    ncine::SceneNode * ncNode = nullptr;

    //void AddErrorMessageTextNode();

};





#endif
