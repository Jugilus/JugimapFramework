#include <chrono>
#include <assert.h>
#include "sceneSFML.h"



namespace apiTestDemo{




bool PlatformerSceneSFML::Init()
{

    // Engine indpenedent scene initialization
    if(PlatformerScene::Init()==false){
        //---
        if(jugimap::JugiMapBinaryLoader::error != ""){
            jugimap::settings.SetErrorMessage(jugimap::JugiMapBinaryLoader::error);
        }else{
            jugimap::settings.SetErrorMessage("Scene::Init() error!");
        }
        return false;
    }


    // Initialize few remaining things that can not be done in engine independent way.

    // Create font (its size may vary between engines).
    font = jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 14, jugimap::FontKind::TRUE_TYPE);

    // Set few parameters regarding precise text positioning needed in 'MakeButtonsAndTexts'
    buttonLabelOffset = jugimap::Vec2f(20,8);
    extraLabelsYSpacingFactor = 1.5;
    extraLabelsCount = 6;
    extraLabelsYOffset = 50;

    // Skip buttons for features not supported by the engine.
    skippedButtons = std::vector<int>{ButtonTag::DYNAMIC_CRYSTALS};

    MakeButtonsAndTexts();


    // If logic update uses fixed time step with 'lerpDrawing' we need to 'capture' sprite positions at start.
    backgroundMap->CaptureForLerpDrawing();
    worldMap->CaptureForLerpDrawing();
    guiMap->CaptureForLerpDrawing();
    infoMap->CaptureForLerpDrawing();

    return true;
}


void PlatformerSceneSFML::Draw()
{

    jugimap::globSFML::countSpriteDrawCalls = 0;
    jugimap::globSFML::countDrawnSprites = 0;


    if(doubleCamera){

        backgroundMap->SetCamera(&worldCameraA);
        static_cast<jugimap::MapSFML*>(backgroundMap)->UpdateAndDrawEngineMap();

        backgroundMap->SetCamera(&worldCameraB);
        static_cast<jugimap::MapSFML*>(backgroundMap)->UpdateAndDrawEngineMap();

        worldMap->SetCamera(&worldCameraA);
        static_cast<jugimap::MapSFML*>(worldMap)->UpdateAndDrawEngineMap();

        worldMap->SetCamera(&worldCameraB);
        static_cast<jugimap::MapSFML*>(worldMap)->UpdateAndDrawEngineMap();


    }else{

        backgroundMap->SetCamera(&worldCamera);
        static_cast<jugimap::MapSFML*>(backgroundMap)->UpdateAndDrawEngineMap();

        worldMap->SetCamera(&worldCamera);
        static_cast<jugimap::MapSFML*>(worldMap)->UpdateAndDrawEngineMap();

    }


    static_cast<jugimap::MapSFML*>(guiMap)->UpdateAndDrawEngineMap();
    static_cast<jugimap::MapSFML*>(infoMap)->UpdateAndDrawEngineMap();

}


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



}
