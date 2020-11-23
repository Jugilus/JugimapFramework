#include <chrono>
#include <assert.h>
#include <sstream>

#include "scene.h"
#include "app.h"



using namespace jugimap;


namespace jugiApp{


bool DemoApp::Init()
{

    // Enable physics - this will initialize cocos2d scene node with physics.
    SetPhysicsEnabled(true);


    GetEngineApp()->PreInit();

    if(sceneFactory.get()==nullptr){
        sceneFactory.reset(new SceneFactory());
    }

    textColorsLibrary.Add(ColorRGBA(255,255,255), "white");     // 0
    textColorsLibrary.Add(ColorRGBA(255,255,0), "yellow");     // 1

    //-------------------------------------------------------
    // fonts
    if(settings.GetEngine()==Engine::AGK){
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 20, jugimap::FontKind::TRUE_TYPE), "OpenSans-14");

    }else if(settings.GetEngine()==Engine::nCine){
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.fnt", 20, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-20");

    }else{

        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 14, jugimap::FontKind::TRUE_TYPE), "OpenSans-14");

    }

    demoScene = sceneFactory->NewPlatformerScene();
    AddScene(demoScene);

    //-------------------------------------------------------

    GetEngineApp()->PostInit();


    //-------------------------------------------------------
    //  SET STARTING SCENE
    //-------------------------------------------------------
    SetActiveScene(demoScene);

    return true;
}




DemoApp *application = nullptr;


std::string jugimapAppName = "Jugimap framework demo";




PlatformerScene* SceneFactory::NewPlatformerScene()
{
    return new PlatformerScene();
}


std::unique_ptr<SceneFactory>sceneFactory;


}
