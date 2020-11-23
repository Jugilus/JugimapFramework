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


    // colors
    jugimap::textColorsLibrary.Add(jugimap::ColorRGBA(0xFFCCCCCC), "greyLight");    // A B G R (color components are written backward)
    jugimap::textColorsLibrary.Add(jugimap::ColorRGBA(0xFFB987B1), "violet");
    jugimap::textColorsLibrary.Add(jugimap::ColorRGBA(0xFFD9A577), "blue");
    jugimap::textColorsLibrary.Add(jugimap::ColorRGBA(0xFF56D4B6), "green");
    jugimap::textColorsLibrary.Add(jugimap::ColorRGBA(0xFF52374E), "darkViolet");
    jugimap::textColorsLibrary.Add(jugimap::ColorRGBA(0xFF38388F), "darkRed");


    // fonts
    if(settings.GetEngine()==Engine::AGK){
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 23, jugimap::FontKind::TRUE_TYPE), "OpenSans-17");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 26, jugimap::FontKind::TRUE_TYPE), "OpenSans-19");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Italic.ttf", 28, jugimap::FontKind::TRUE_TYPE), "OpenSans-italic-20");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold.ttf", 26, jugimap::FontKind::TRUE_TYPE), "OpenSans-bold-19");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold.ttf", 31, jugimap::FontKind::TRUE_TYPE), "OpenSans-bold-23");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold.ttf", 45, jugimap::FontKind::TRUE_TYPE), "OpenSans-bold-31");

    }else if(settings.GetEngine()==Engine::nCine){
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular-17.fnt", 17, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-17");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular-19.fnt", 19, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-19");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Italic-20.fnt", 20, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-italic-20");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold-19.fnt", 19, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-bold-19");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold-23.fnt", 23, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-bold-23");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold-31.fnt", 31, jugimap::FontKind::BITMAP_FONT__FNT), "OpenSans-bold-31");

    }else{
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 17, jugimap::FontKind::TRUE_TYPE), "OpenSans-17");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 19, jugimap::FontKind::TRUE_TYPE), "OpenSans-19");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Italic.ttf", 20, jugimap::FontKind::TRUE_TYPE), "OpenSans-italic-20");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold.ttf", 19, jugimap::FontKind::TRUE_TYPE), "OpenSans-bold-19");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold.ttf", 23, jugimap::FontKind::TRUE_TYPE), "OpenSans-bold-23");
        jugimap::fontLibrary.Add(jugimap::objectFactory->NewFont("media/fonts/OpenSans-Bold.ttf", 31, jugimap::FontKind::TRUE_TYPE), "OpenSans-bold-31");

    }


    guiDemoTestScene = new DemoScene();
    AddScene(guiDemoTestScene);

    GetEngineApp()->PostInit();


    //-------------------------------------------------------
    SetActiveScene(guiDemoTestScene);


    return true;
}





DemoApp *application = nullptr;


std::string jugimapAppName = "Gui demo";


}
