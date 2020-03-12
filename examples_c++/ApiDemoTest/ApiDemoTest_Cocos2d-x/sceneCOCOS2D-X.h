#ifndef API_DEMO_TEST__SCENE_COCOS2D_H
#define API_DEMO_TEST__SCENE_COCOS2D_H


#include "jugimapCOCOS2D-X/jmCocos2d-x.h"
#include "engineIndependent/platformerScene.h"



namespace apiTestDemo{


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

private:
    void AddErrorMessageTextNode();

};


//=====================================================================================


// Events layer for mouse reading. used with PlatformerSceneCC.
class EventsLayer : public cocos2d::Layer
{
public:
    static EventsLayer * Create();
    bool Init();

private:
    void OnMouseDown(cocos2d::Event *_event);
    void OnMouseUp(cocos2d::Event *_event);
    void OnMouseMove(cocos2d::Event *_event);
    void OnMouseScroll(cocos2d::Event *_event);
    void OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
};


}


#endif
