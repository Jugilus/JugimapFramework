#include <chrono>
#include <assert.h>
#include "sceneCOCOS2D-X.h"



namespace apiTestDemo{


namespace jm = jugimap;



bool PlatformerSceneCC::Init()
{


    // Set if physics should be enabled; if true the cosos2d scene node will be initialized via 'initWithPhysics()'
    SetPhysicsEnabled(true);

    // Set starting z-order values for the maps in the scene.
    zOrderBackgroundMap = 1000;
    zOrderWorldMap = 2000;
    zOrderTestParallaxMap = 3000;
    zOrderGuiMap = 4000;
    zOrderInfoMap = 5000;


    // Engine indpenedent scene initialization
    if(PlatformerScene::Init()==false){
        //---
        if(jm::JugiMapBinaryLoader::error != ""){
            jm::settings.SetErrorMessage(jm::JugiMapBinaryLoader::error);
        }else{
            jm::settings.SetErrorMessage("Scene::Init() error!");
        }

        //--- show error message
        jm::SceneCCNode * sceneCCNode = jm::SceneCCNode::Create(this);
        if(sceneCCNode==nullptr) return false;

        AddErrorMessageTextNode();

        return false;
    }


    // Initialize few remaining things that can not be done in engine independent way.

    // Create font (its size may vary between engines).
    font = jm::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 14, jm::FontKind::TRUE_TYPE);

    // Set few parameters regarding precise text positioning needed in 'MakeButtonsAndTexts'
    buttonLabelOffset = jm::Vec2f(20,-25);
    extraLabelsYSpacingFactor = 1.1;
    extraLabelsYOffset = -70;
    extraLabelsCount = 7;

    // Skip buttons for features not supported by the engine.
    if(HasPhysicsEnabled()==false){
        skippedButtons = std::vector<int>{ButtonTag::DYNAMIC_CRYSTALS};
    }

    MakeButtonsAndTexts();


    // Add map nodes to the scene node in correct order.
    jm::SceneCCNode * sceneCCNode = jm::SceneCCNode::Create(this);      // This function also stores sceneCCNode into this class.
    if(sceneCCNode==nullptr) return false;

    EventsLayer* eventsLayer = EventsLayer::Create();           // for mouse input
    sceneCCNode->addChild(eventsLayer);
    sceneCCNode->addChild(static_cast<jm::MapCC*>(backgroundMap)->GetMapNode());
    sceneCCNode->addChild(static_cast<jm::MapCC*>(worldMap)->GetMapNode());
    sceneCCNode->addChild(static_cast<jm::MapCC*>(guiMap)->GetMapNode());
    sceneCCNode->addChild(static_cast<jm::MapCC*>(infoMap)->GetMapNode());

    AddErrorMessageTextNode();


    cocos2d::PhysicsWorld *physicsWorld = sceneCCNode->getPhysicsWorld();
    if(physicsWorld){
        jm::settings.EnableEnginePhysics(true);
        physicsWorld->setGravity(cocos2d::Vec2(0, -500));
        //physicsWorld->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
        SetPhysicsSimulationDisabled(true);                                     // Disable all physics stuff when not needed.
    }else{
        jm::settings.EnableEnginePhysics(false);
    }

    return true;
}


void PlatformerSceneCC::AddErrorMessageTextNode()
{
    assert(GetEngineSceneLink());

    jm::SceneCCNode * sceneCCNode = static_cast<jm::SceneCCNode*>(GetEngineSceneLink());

    cocos2d::DrawNode *drawNode = cocos2d::DrawNode::create();
    sceneCCNode->addChild(drawNode);

    cocos2d::Label* label = cocos2d::Label::createWithTTF("Label", "media/fonts/OpenSans-Regular.ttf", 14);
    label->setAnchorPoint(cocos2d::Vec2(0,1));
    label->setPosition(10, cocos2d::Director::getInstance()->getVisibleSize().height-10);
    label->setString("");

    drawNode->addChild(label);

    sceneCCNode->_SetErrorMessageLabel(label);
}



void PlatformerSceneCC::SetPhysicsSimulationDisabled(bool _disabled)
{

    if(jm::settings.EnginePhysicsEnabled()==false) return;

    jm::SceneCCNode* sceneCCNode = static_cast<jm::SceneCCNode*>(GetEngineSceneLink());

    if(_disabled){
        sceneCCNode->getPhysicsWorld()->setAutoStep(false);
        sceneCCNode->getPhysicsWorld()->step(0.0);
    }else{
        sceneCCNode->getPhysicsWorld()->setAutoStep(true);
    }
}


void PlatformerSceneCC::SetDynamicCrystalsPhysics()
{

    if(jm::settings.EnginePhysicsEnabled()==false) return;

    if(dynamicCrystals){


        //---- turn ON static physics mode for main world tiles
        jm::SpriteLayer *layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteCC*>(s)->SetPhysicsMode(jm::StandardSpriteCC::PhysicsMode::STATIC);
            }
        }

        //---- turn ON static physics mode for characters
        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteCC*>(s)->SetPhysicsMode(jm::StandardSpriteCC::PhysicsMode::STATIC);      //or static
            }
        }


        //---- turn ON dynamic physics mode for crystals
        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<jm::StandardSpriteCC*>(s)->SetPhysicsMode(jm::StandardSpriteCC::PhysicsMode::DYNAMIC);
                    static_cast<jm::StandardSpriteCC*>(s)->SetPhysicsMaterial(cocos2d::PhysicsMaterial(1.0, 0.3, 0.7));
                    s->SetEngineSpriteUsedDirectly(true);   // the sprite is no longer controlled via jugimap interface
                }
            }
        }

        SetPhysicsSimulationDisabled(entities.IsPaused());


    }else{


        //---- turn OFF physics for all sprites in simulation
        jm::SpriteLayer *layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteCC*>(s)->SetPhysicsMode(jm::StandardSpriteCC::PhysicsMode::NO_PHYSICS);
            }
        }

        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteCC*>(s)->SetPhysicsMode(jm::StandardSpriteCC::PhysicsMode::NO_PHYSICS);
            }
        }


        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<jm::StandardSpriteCC*>(s)->SetPhysicsMode(jm::StandardSpriteCC::PhysicsMode::NO_PHYSICS);
                    s->SetEngineSpriteUsedDirectly(false);            // ! The sprite is again used via jugimap interface (so that we can restore it to its initial position)

                    //--- restore transformation properties from jugimap sprite which were not changed during physics simulation
                    s->SetChangeFlags(jm::Sprite::Property::TRANSFORMATION);
                    s->UpdateEngineSprite();
                  }
            }
        }

        SetPhysicsSimulationDisabled(true);

    }
}


void PlatformerSceneCC::UpdateTexts()
{

    PlatformerScene::UpdateTexts();

    //----
    float fps = cocos2d::Director::getInstance()->getFrameRate();
    std::string str = " FPS: " + std::to_string(int(std::roundf(fps)));
    if(str != labels[4]->GetTextString()) labels[4]->SetTextString(str);

    //----
    str = "GL calls: " + std::to_string(jm::SceneCCNode::glDrawnBatchesString);
    if(str != labels[5]->GetTextString()) labels[5]->SetTextString(str);

    //----
    str = "GL drawn vertices: " + std::to_string(jm::SceneCCNode::glDrawnVerticesString);
    if(str != labels[6]->GetTextString()) labels[6]->SetTextString(str);

}


void PlatformerSceneCC::AssignCamerasToMaps()
{

    if(doubleCamera){
        static_cast<jm::MapCC*>(backgroundMap)->SetCameras(std::vector<jm::Camera*>{&worldCameraA, &worldCameraB});
        static_cast<jm::MapCC*>(worldMap)->SetCameras(std::vector<jm::Camera*>{&worldCameraA, &worldCameraB});
    }else{
        static_cast<jm::MapCC*>(backgroundMap)->SetCameras(std::vector<jm::Camera*>{&worldCamera});
        static_cast<jm::MapCC*>(worldMap)->SetCameras(std::vector<jm::Camera*>{&worldCamera});
    }
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

    mouse.SetScreenPosition(jm::Vec2f(x,y));

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



}
