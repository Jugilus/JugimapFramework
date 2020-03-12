#include <chrono>
#include <assert.h>
#include "sceneSFML.h"



bool ParallaxSceneSFML::Init()
{
    // Engine indpenedent scene initialization
    if(ParallaxScene::Init()==false){
        //---
        if(jugimap::JugiMapBinaryLoader::error != ""){
            jugimap::settings.SetErrorMessage(jugimap::JugiMapBinaryLoader::error);
        }else{
            jugimap::settings.SetErrorMessage("Scene::Init() error!");
        }
        return false;
    }


    // If logic update uses fixed time step with 'lerpDrawing' we need to 'capture' sprite positions at start.
    backgroundMap->CaptureForLerpDrawing();
    worldMap->CaptureForLerpDrawing();
    if(testScreenMap) testScreenMap->CaptureForLerpDrawing();

    return true;
}



void ParallaxSceneSFML::Draw()
{

    static_cast<jugimap::MapSFML*>(backgroundMap)->UpdateAndDrawEngineMap();
    static_cast<jugimap::MapSFML*>(worldMap)->UpdateAndDrawEngineMap();
    if(testScreenMap) static_cast<jugimap::MapSFML*>(testScreenMap)->UpdateAndDrawEngineMap();

}





