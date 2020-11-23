#include <chrono>
#include <assert.h>
#include <sstream>

#include "scene.h"
#include "app.h"



namespace jugiApp{



using namespace jugimap;



bool DemoApp::Init()
{

    GetEngineApp()->PreInit();


    //-------------------------------------------------------
    // fonts
    // Load one font used for error messages
    if(settings.GetEngine()==Engine::AGK){
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 20, jugimap::FontKind::TRUE_TYPE), "OpenSans-14");

    }else if(settings.GetEngine()==Engine::nCine){
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.fnt", 20, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-20");

    }else{

        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 14, jugimap::FontKind::TRUE_TYPE), "OpenSans-14");

    }

    // scenes
    demoScene = new DemoScene();
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


std::string jugimapAppName = "Sprite timeline animation";


}
