#include <chrono>
#include <assert.h>
#include "jugiAppCOCOS2D-X.h"



namespace jugiApp{



using namespace jugimap;





bool PlatformerSceneCC::Init()
{


    // Set if physics should be enabled; if true the cosos2d scene node will be initialized via 'initWithPhysics()'
    //SetPhysicsEnabled(true);

    /*
    // Set starting z-order values for the maps in the scene.
    zOrderBackgroundMap = 1000;
    zOrderWorldMap = 2000;
    zOrderTestParallaxMap = 3000;
    zOrderGuiMap = 4000;
    zOrderInfoMap = 5000;


    // Engine indpenedent scene initialization
    if(PlatformerScene::Init()==false){
        //---
        if(JugiMapBinaryLoader::error != ""){
            settings.SetErrorMessage(JugiMapBinaryLoader::error);
        }else{
            settings.SetErrorMessage("Scene::Init() error!");
        }

        //--- show error message
        SceneCCNode * sceneCCNode = SceneCCNode::Create(this);
        if(sceneCCNode==nullptr) return false;

        AddErrorMessageTextNode();

        return false;
    }


    // Initialize few remaining things that can not be done in engine independent way.

    // Create font (its size may vary between engines).
    font = objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 14, FontKind::TRUE_TYPE);

    // Set few parameters regarding precise text positioning needed in 'MakeButtonsAndTexts'
    buttonLabelOffset = Vec2f(20,-25);
    extraLabelsYSpacingFactor = 1.1;
    extraLabelsYOffset = -70;
    extraLabelsCount = 7;

    // Skip buttons for features not supported by the engine.
    if(HasPhysicsEnabled()==false){
        skippedButtons = std::vector<int>{ButtonTag::DYNAMIC_CRYSTALS};
    }

    MakeButtonsAndTexts();


    // Add map nodes to the scene node in correct order.
    SceneCCNode * sceneCCNode = SceneCCNode::Create(this);      // This function also stores sceneCCNode into this class.
    if(sceneCCNode==nullptr) return false;

    EventsLayer* eventsLayer = EventsLayer::Create();           // for mouse input
    sceneCCNode->addChild(eventsLayer);
    sceneCCNode->addChild(static_cast<MapCC*>(backgroundMap)->GetMapNode());
    sceneCCNode->addChild(static_cast<MapCC*>(worldMap)->GetMapNode());
    sceneCCNode->addChild(static_cast<MapCC*>(guiMap)->GetMapNode());
    sceneCCNode->addChild(static_cast<MapCC*>(infoMap)->GetMapNode());

    AddErrorMessageTextNode();
    */

    PlatformerScene::Init();


    EngineAppCC* engineApp = static_cast<EngineAppCC*>(application->GetEngineApp());
    AppCCNode* ccNode = static_cast<AppCCNode*>(engineApp->GetAppNode());

    cocos2d::PhysicsWorld *physicsWorld = ccNode->getPhysicsWorld();
    if(physicsWorld){
        settings.EnableEnginePhysics(true);
        physicsWorld->setGravity(cocos2d::Vec2(0, -500));
        //physicsWorld->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
        SetPhysicsSimulationDisabled(true);                                     // Disable all physics stuff when not needed.
    }else{
        settings.EnableEnginePhysics(false);
    }

    return true;
}




void PlatformerSceneCC::SetPhysicsSimulationDisabled(bool _disabled)
{

    if(settings.EnginePhysicsEnabled()==false) return;

    EngineAppCC* engineApp = static_cast<EngineAppCC*>(application->GetEngineApp());
    AppCCNode* ccNode = static_cast<AppCCNode*>(engineApp->GetAppNode());

    if(_disabled){
        ccNode->getPhysicsWorld()->setAutoStep(false);
        ccNode->getPhysicsWorld()->step(0.0);
    }else{
        ccNode->getPhysicsWorld()->setAutoStep(true);
    }
}


void PlatformerSceneCC::SetDynamicCrystalsPhysics()
{

    if(settings.EnginePhysicsEnabled()==false) return;

    if(dynamicCrystals){


        //---- turn ON static physics mode for main world tiles
        SpriteLayer *layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteCC*>(s)->SetPhysicsMode(StandardSpriteCC::PhysicsMode::STATIC);
            }
        }

        //---- turn ON static physics mode for characters
        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteCC*>(s)->SetPhysicsMode(StandardSpriteCC::PhysicsMode::STATIC);      //or static
            }
        }


        //---- turn ON dynamic physics mode for crystals
        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<StandardSpriteCC*>(s)->SetPhysicsMode(StandardSpriteCC::PhysicsMode::DYNAMIC);
                    static_cast<StandardSpriteCC*>(s)->SetPhysicsMaterial(cocos2d::PhysicsMaterial(1.0, 0.3, 0.7));
                    s->SetDisabledEngineSpriteUpdate(true);   // the sprite is no longer controlled via jugimap interface
                }
            }
        }

        SetPhysicsSimulationDisabled(entities.IsPaused());


    }else{


        //---- turn OFF physics for all sprites in simulation
        SpriteLayer *layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteCC*>(s)->SetPhysicsMode(StandardSpriteCC::PhysicsMode::NO_PHYSICS);
            }
        }

        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteCC*>(s)->SetPhysicsMode(StandardSpriteCC::PhysicsMode::NO_PHYSICS);
            }
        }


        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<StandardSpriteCC*>(s)->SetPhysicsMode(StandardSpriteCC::PhysicsMode::NO_PHYSICS);
                    s->SetDisabledEngineSpriteUpdate(false);            // ! The sprite is again used via jugimap interface (so that we can restore it to its initial position)

                    //--- restore transformation properties from jugimap sprite which were not changed during physics simulation
                    s->SetChangeFlags(Sprite::Property::TRANSFORMATION);
                    s->UpdateEngineObjects();
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
    str = "GL calls: " + std::to_string(AppCCNode::glDrawnBatchesString);
    if(str != labels[5]->GetTextString()) labels[5]->SetTextString(str);

    //----
    str = "GL drawn vertices: " + std::to_string(AppCCNode::glDrawnVerticesString);
    if(str != labels[6]->GetTextString()) labels[6]->SetTextString(str);

}


void PlatformerSceneCC::AssignCamerasToMaps()
{

    if(doubleCamera){
        static_cast<MapCC*>(backgroundMap)->SetCameras(std::vector<Camera*>{&worldCameraA, &worldCameraB});
        static_cast<MapCC*>(worldMap)->SetCameras(std::vector<Camera*>{&worldCameraA, &worldCameraB});
    }else{
        static_cast<MapCC*>(backgroundMap)->SetCameras(std::vector<Camera*>{&worldCamera});
        static_cast<MapCC*>(worldMap)->SetCameras(std::vector<Camera*>{&worldCamera});
    }
}


//=====================================================================================


/*
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
    //DbgOutput(str);

    mouse.SetPressed(true);
    mouse.SetHit(true);
}


void EventsLayer::OnMouseUp(cocos2d::Event *event)
{
    //cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //std::string str = "Mouse Up detected, Key: ";
    //str += std::to_string((int) e->getMouseButton());
    //DbgOutput(str);

    mouse.SetPressed(false);
    mouse.SetHit(false);
}


void EventsLayer::OnMouseMove(cocos2d::Event *event)
{

    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;

    float x = e->getCursorX();
    float y = e->getCursorY();

    mouse.SetScreenPosition(Vec2f(x,y));


}



void EventsLayer::OnMouseScroll(cocos2d::Event *event)
{
    //cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //std::string str = "Mouse Scroll detected, X: ";
    //str = str + std::to_string(e->getScrollX()) + " Y: " + std::to_string(e->getScrollY());
    //DbgOutput(str);
}


void EventsLayer::OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    //std::string str = "Pressed key: " + std::to_string((int)keyCode);
    //DbgOutput(str);
}


void EventsLayer::OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    //std::string str = "Released key: " + std::to_string((int)keyCode);
    //DbgOutput(str);

}

*/

}
