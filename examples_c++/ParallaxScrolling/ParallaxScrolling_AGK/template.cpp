#include <algorithm>
#include "sceneAGK.h"
#include "template.h"



void app::Begin(void)
{

    agk::SetWindowTitle("JugiMap Parallax Scrolling - AGK version");
    agk::SetVirtualResolution (1300, 800);
    agk::SetWindowAllowResize(0);

    agk::UseNewDefaultFonts(1);
    agk::SetPrintSize(20);


    // Set required jugimap global paramaters and objects
    //---------------------------------------------------

    jugimap::settings.SetScreenSize(jugimap::Vec2i(agk::GetVirtualWidth(),agk::GetVirtualHeight()));
    jugimap::settings.SetZOrderStep(-10);                                       // Z-order step for setting z-order for layers.
    jugimap::objectFactory = new jugimap::ObjectFactoryAGK();                   // All jugimap map elements are created via 'objectFactory' object!
    jugimap::sceneManager = new jugimap::SceneManager();
    jugimap::WorldMapCamera::allowRotation = false;                             // AGK 'view' transformation does not support rotation!

    // path prefixes
    //jugimap::JugiMapBinaryLoader::pathPrefix = "";                            // No prefix needed.
    //jugimap::GraphicFile::pathPrefix = "";                                    // No prefix needed.
    //jugimap::Font::pathPrefix = "";                                           // No prefix needed.

    // Load shaders required for some jugimap sprite properties
    jugimap::shaders::colorBlend_SimpleMultiply->Load("media/shaders_AGK/spriteColorOverlay_simpleMultiply.ps");
    jugimap::shaders::colorBlend_Normal->Load("media/shaders_AGK/spriteColorOverlay_normal.ps");
    jugimap::shaders::colorBlend_Multiply->Load("media/shaders_AGK/spriteColorOverlay_multiply.ps");
    jugimap::shaders::colorBlend_LinearDodge->Load("media/shaders_AGK/spriteColorOverlay_linearDodge.ps");
    jugimap::shaders::colorBlend_Color->Load("media/shaders_AGK/spriteColorOverlay_color.ps");

    //---------------------------------------------------

    jugimap::Scene* scene = jugimap::sceneManager->AddScene(new ParallaxSceneAGK());
    jugimap::sceneManager->SetCurrentScene(scene);

}


int app::Loop (void)
{

    //--- Manage errors
    if(jugimap::settings.GetErrorMessage() != ""){
        agk::Print(jugimap::settings.GetErrorMessage().c_str());
        agk::Sync();
        return 0;
    }


    //--- Set mouse properties
    mouse.SetScreenPosition(jugimap::Vec2f(agk::GetPointerX(), agk::GetPointerY()));
    mouse.SetPressed(agk::GetPointerState());
    mouse.SetHit(agk::GetPointerPressed());


    jugimap::sceneManager->Update(agk::GetFrameTime()*1000);

    //---
    agk::Sync();
    return 0;
}


void app::End (void)
{

    jugimap::sceneManager->DeleteScenes();
    jugimap::shaders::DeleteShaders();
    jugimap::DeleteGlobalObjects();

}



app App;

