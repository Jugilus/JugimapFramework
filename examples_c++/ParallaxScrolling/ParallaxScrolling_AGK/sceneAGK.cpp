#include <chrono>
#include <assert.h>
#include "sceneAGK.h"





bool ParallaxSceneAGK::Init()
{

    // Set starting z-order values for the maps in the scene.
    zOrderBackgroundMap = 5000;
    zOrderWorldMap = 4000;
    zOrderScreenMap = 3000;
    jugimap::JugiMapBinaryLoader::zOrderCounterStep = -10;


    // Engine indpenedent scene initialization
    if(ParallaxScene::Init()==false){

        if(jugimap::JugiMapBinaryLoader::error !=""){
            jugimap::settings.SetErrorMessage(jugimap::JugiMapBinaryLoader::error);
        }else{
            jugimap::settings.SetErrorMessage("SetupDemo() error!");
        }

        return false;
    }

    return true;
}





void ParallaxSceneAGK::UpdateEngineObjects()
{

    ParallaxScene::UpdateEngineObjects();     // update agk objects from jugimap objects

    // Convert world map camera to AGK 'view'.
    jugimap::Vec2f offset = worldCamera.GetProjectedMapPosition();
    offset = offset / worldCamera.GetScale();                           // this gives correct result
    agk::SetViewOffset(-offset.x, -offset.y);
    agk::SetViewZoom(worldCamera.GetScale());


    // Reset camera change flags (not really needed here)
    worldCamera.ClearChangeFlags();
}

