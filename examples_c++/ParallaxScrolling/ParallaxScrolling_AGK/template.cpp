#include <algorithm>
#include "jugimapAGK/jmAGK.h"
#include "jugiApp/app.h"
#include "template.h"



void app::Begin(void)
{

    std::string appName = jugiApp::jugimapAppName + " - AGK version";
    agk::SetWindowTitle(appName.c_str());
    agk::SetVirtualResolution (1300, 800);
    agk::SetWindowAllowResize(0);

    agk::UseNewDefaultFonts(1);
    agk::SetPrintSize(20);
    agk::CompleteRawJoystickDetection();


    // JUGIMAP CORE INITIALIZATION
    //---------------------------------------------------
    jugimap::settings.SetEngine(jugimap::Engine::AGK);
    jugimap::settings.SetScreenSize(jugimap::Vec2i(agk::GetVirtualWidth(),agk::GetVirtualHeight()));
    jugimap::settings.SetZOrderStep(-10);                                       // Z-order step for setting z-order for layers.
    jugimap::settings.SetMapZOrderStart(10000);                                 // 10000 is max value for AGK zOrder
    jugimap::settings.SetMapZOrderStep(-1000);
    jugimap::objectFactory = new jugimap::ObjectFactoryAGK();                   // All jugimap map elements are created via 'objectFactory' object!
    jugimap::WorldMapCamera::allowRotation = false;                             // AGK 'view' transformation does not support rotation!

    // path prefixes
    // jugimap::JugiMapBinaryLoader::pathPrefix = "";                           // No prefix needed.
    // jugimap::GraphicFile::pathPrefix = "";                                   // No prefix needed.
    // jugimap::Font::pathPrefix = "";                                          // No prefix needed.
    // jugimap::TextLibrary::pathPrefix = "";                                   // No prefix needed.

    // path where text files are stored
    jugimap::TextLibrary::path = "media/texts/";

    // Load shaders required for some jugimap sprite properties
    jugimap::shaders::LoadJugimapShaders();


    // Joystick connection
    for(int i=0; i<jugimap::joysticks.size(); i++){
        if(agk::GetRawJoystickExists(i+1)==1){
            jugimap::joysticks[i]._SetConnected(true);
            std::string name = agk::GetRawJoystickName(1);
            jugimap::joysticks[i]._SetName(name);
        }
    }

    //----
    jugiApp::application = new jugiApp::DemoApp();
    jugiApp::application->Init();

}


int app::Loop (void)
{

    //--- Manage errors
    if(jugimap::settings.GetErrorMessage() != ""){
        agk::Print(jugimap::settings.GetErrorMessage().c_str());
        agk::Sync();
        return 0;
    }

    jugiApp::application->Update(agk::GetFrameTime());

    //---
    agk::Sync();
    return 0;
}


void app::End (void)
{

    delete jugiApp::application;
    jugimap::shaders::DeleteShaders();
    jugimap::DeleteGlobalObjects();

}



app App;

