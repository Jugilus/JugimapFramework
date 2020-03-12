#include <chrono>
#include <assert.h>
#include "sceneCOCOS2D-X.h"





bool ParallaxSceneCC::Init()
{

    jugimap::SceneCCNode * sceneCCNode = jugimap::SceneCCNode::Create(this);
    if(sceneCCNode==nullptr) return false;


    // Set starting z-order values for the maps in the scene.
    zOrderBackgroundMap = 1000;
    zOrderWorldMap = 2000;
    zOrderScreenMap = 3000;

    if(ParallaxScene::Init()==false){

        if(jugimap::JugiMapBinaryLoader::error != ""){
            jugimap::settings.SetErrorMessage(jugimap::JugiMapBinaryLoader::error);
        }else{
            jugimap::settings.SetErrorMessage("Scene::Init() error!");
        }

        AddErrorMessageTextNode();      // for displaying error messages on screen

        return false;
    }


    // Add map cocos nodes to the cocos scene node in correct order.
    EventsLayer* eventsLayer = EventsLayer::Create();                   // for mouse input
    sceneCCNode->addChild(eventsLayer);
    if(backgroundMap) sceneCCNode->addChild(static_cast<jugimap::MapCC*>(backgroundMap)->GetMapNode());
    if(worldMap) sceneCCNode->addChild(static_cast<jugimap::MapCC*>(worldMap)->GetMapNode());
    if(testScreenMap) sceneCCNode->addChild(static_cast<jugimap::MapCC*>(testScreenMap)->GetMapNode());

    AddErrorMessageTextNode();              // for displaying error messages on screen

    return true;
}



void ParallaxSceneCC::AddErrorMessageTextNode()
{
    assert(GetEngineSceneLink());

    jugimap::SceneCCNode * sceneCCNode = static_cast<jugimap::SceneCCNode*>(GetEngineSceneLink());

    cocos2d::DrawNode *drawNode = cocos2d::DrawNode::create();
    sceneCCNode->addChild(drawNode);

    cocos2d::Label* label = cocos2d::Label::createWithTTF("Label", "media/fonts/OpenSans-Regular.ttf", 14);
    label->setAnchorPoint(cocos2d::Vec2(0,1));
    label->setPosition(10, cocos2d::Director::getInstance()->getVisibleSize().height-10);
    label->setString("");

    drawNode->addChild(label);

    sceneCCNode->_SetErrorMessageLabel(label);
}




//=====================================================================================



EventsLayer* EventsLayer::Create()
{
    EventsLayer* el = new (std::nothrow) EventsLayer();
    if (el && el->Init())
    {
        el->autorelease();
        return el;
    }
    CC_SAFE_DELETE(el);
    return nullptr;
}


bool EventsLayer::Init()
{

    cocos2d::EventListenerMouse* mouseListener = cocos2d::EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(EventsLayer::OnMouseMove, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(EventsLayer::OnMouseUp, this);
    mouseListener->onMouseDown = CC_CALLBACK_1(EventsLayer::OnMouseDown, this);
    mouseListener->onMouseScroll = CC_CALLBACK_1(EventsLayer::OnMouseScroll, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    cocos2d::EventListenerKeyboard* listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(EventsLayer::OnKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(EventsLayer::OnKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}


void EventsLayer::OnMouseDown(cocos2d::Event *event)
{
    //EventMouse* e = (EventMouse*)event;
    //std::string str = "Mouse Down detected, Key: ";
    //str += std::to_string((int) e->getMouseButton());
    //jm::DbgOutput(str);

    mouse.SetPressed(true);
    mouse.SetHit(true);
}


void EventsLayer::OnMouseUp(cocos2d::Event *event)
{
    //cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //std::string str = "Mouse Up detected, Key: ";
    //str += std::to_string((int) e->getMouseButton());
    //jm::DbgOutput(str);

    mouse.SetPressed(false);
    mouse.SetHit(false);
}


void EventsLayer::OnMouseMove(cocos2d::Event *event)
{

    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;

    float x = e->getCursorX();
    float y = e->getCursorY();

    mouse.SetScreenPosition(jugimap::Vec2f(x,y));

    /*
    cocos2d::Scene *scene = cocos2d::Director::getInstance()->getRunningScene();
    if(dynamic_cast<SceneNode*>(scene)){
        SceneNode* sceneNode = static_cast<SceneNode*>(scene);
        sceneNode->GetJMScene()->UpdateProjectedMousePositions();
    }
    */
}



void EventsLayer::OnMouseScroll(cocos2d::Event *event)
{
    //cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //std::string str = "Mouse Scroll detected, X: ";
    //str = str + std::to_string(e->getScrollX()) + " Y: " + std::to_string(e->getScrollY());
    //jm::DbgOutput(str);
}


void EventsLayer::OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    //std::string str = "Pressed key: " + std::to_string((int)keyCode);
    //jm::DbgOutput(str);
}


void EventsLayer::OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    //std::string str = "Released key: " + std::to_string((int)keyCode);
    //jm::DbgOutput(str);

}




