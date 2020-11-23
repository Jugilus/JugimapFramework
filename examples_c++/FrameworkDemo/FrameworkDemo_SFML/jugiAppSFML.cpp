#include <chrono>
#include <assert.h>
#include "jugiAppSFML.h"



namespace jugiApp{



using namespace jugimap;





//bool PlatformerSceneSFML::Init()
//{


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

    //PlatformerScene::Init();

    /*
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
    */

    //return true;
//}


void PlatformerSceneSFML::UpdateTexts()
{

    PlatformerScene::UpdateTexts();


    //---- FPS
    static int frameRate = 0;
    static int secondsCounter = 0;
    static int numFramesStored = 0;

    //---- Obtain frame rate
    if(1.0 + jugimap::time.GetPassedNetTimeMS()/1000.0 > secondsCounter){
        secondsCounter++;
        frameRate = jugimap::globSFML::countDrawnFrames - numFramesStored;
        numFramesStored = jugimap::globSFML::countDrawnFrames;
    }
    std::string str = " FPS: " + std::to_string(frameRate);
    if(str != labels[4]->GetTextString()) labels[4]->SetTextString(str);

    //----
    str = " Drawn sprites: " + std::to_string(jugimap::globSFML::countDrawnSprites);
    if(str != labels[5]->GetTextString()) labels[5]->SetTextString(str);

}


void PlatformerSceneSFML::Draw()
{


    jugimap::globSFML::countSpriteDrawCalls = 0;
    jugimap::globSFML::countDrawnSprites = 0;


    if(doubleCamera){

        /*
        backgroundMap->SetCamera(&worldCameraA);
        static_cast<jugimap::MapSFML*>(backgroundMap)->UpdateAndDrawEngineMap();

        backgroundMap->SetCamera(&worldCameraB);
        static_cast<jugimap::MapSFML*>(backgroundMap)->UpdateAndDrawEngineMap();

        worldMap->SetCamera(&worldCameraA);
        static_cast<jugimap::MapSFML*>(worldMap)->UpdateAndDrawEngineMap();

        worldMap->SetCamera(&worldCameraB);
        static_cast<jugimap::MapSFML*>(worldMap)->UpdateAndDrawEngineMap();
        */

        backgroundMap->SetCamera(&worldCameraA);
        static_cast<jugimap::MapSFML*>(backgroundMap)->DrawEngineObjects();

        backgroundMap->SetCamera(&worldCameraB);
        static_cast<jugimap::MapSFML*>(backgroundMap)->DrawEngineObjects();

        worldMap->SetCamera(&worldCameraA);
        static_cast<jugimap::MapSFML*>(worldMap)->DrawEngineObjects();

        worldMap->SetCamera(&worldCameraB);
        static_cast<jugimap::MapSFML*>(worldMap)->DrawEngineObjects();

    }else{
        backgroundMap->SetCamera(&worldCamera);
        static_cast<jugimap::MapSFML*>(backgroundMap)->DrawEngineObjects();

        worldMap->SetCamera(&worldCamera);
        static_cast<jugimap::MapSFML*>(worldMap)->DrawEngineObjects();
    }

    static_cast<jugimap::MapSFML*>(guiMap)->DrawEngineObjects();
    static_cast<jugimap::MapSFML*>(infoMap)->DrawEngineObjects();

}


}
