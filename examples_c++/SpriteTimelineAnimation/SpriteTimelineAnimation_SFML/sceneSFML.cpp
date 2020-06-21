#include <chrono>
#include <assert.h>
#include "sceneSFML.h"



bool DemoSceneSFML::Init()
{
    // Engine indpenedent scene initialization
    if(DemoScene::Init()==false){
        //---
        if(jugimap::JugiMapBinaryLoader::error != ""){
            jugimap::settings.SetErrorMessage(jugimap::JugiMapBinaryLoader::error);
        }else{
            jugimap::settings.SetErrorMessage("Scene::Init() error!");
        }
        return false;
    }


    // If logic update uses fixed time step with 'lerpDrawing' we need to 'capture' sprite positions at start.
    worldMap->CaptureForLerpDrawing();

    return true;
}



void DemoSceneSFML::Draw()
{


    static_cast<jugimap::MapSFML*>(worldMap)->UpdateAndDrawEngineMap();

}





