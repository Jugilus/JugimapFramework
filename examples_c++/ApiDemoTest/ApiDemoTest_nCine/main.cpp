#include <ncine/Application.h>
#include <ncine/IFile.h>
#include <ncine/FileSystem.h>
#include "main.h"
#include "sceneNCINE.h"



ncine::IAppEventHandler *createAppEventHandler()
{
    return new MyEventHandler;
}

//==========================================================================================



void MyEventHandler::onPreInit(ncine::AppConfiguration &config)
{


#if defined(__ANDROID__)
    config.dataPath() = "asset::";
#elif defined(__EMSCRIPTEN__)
    config.dataPath() = "/";
#else
    #ifdef PACKAGE_DEFAULT_DATA_DIR
    config.dataPath() = PACKAGE_DEFAULT_DATA_DIR;
    #else
    config.dataPath() = "data/";
    #endif
#endif


    config.resolution.set(1300, 800);
    config.windowTitle = "JugiMap API Demo Test - nCine version";
    config.deferShaderQueries = false;
    config.withVSync = true;
    config.isResizable = false;
    //config.withDebugOverlay = true;
    //config.withGlDebugContext = true;
}


void MyEventHandler::onInit()
{

    ncine::theApplication().setAutoSuspension(true);


    // Set required jugimap global paramaters and objects
    //---------------------------------------------------

    #if !defined (__ANDROID__)
        jugimap::JugiMapBinaryLoader::pathPrefix = std::string(ncine::FileSystem::dataPath().data());
    #endif
    jugimap::GraphicFile::pathPrefix = std::string(ncine::FileSystem::dataPath().data());
    jugimap::Font::pathPrefix = std::string(ncine::FileSystem::dataPath().data());


    jugimap::settings.SetScreenSize(jugimap::Vec2i(ncine::theApplication().widthInt(), ncine::theApplication().heightInt()));
    jugimap::settings.SetYCoordinateUp(true);
    jugimap::objectFactory = new jugimap::ObjectFactoryNC();        // All map elements are created via 'objectFactory'.
    jugimap::sceneManager = new jugimap::SceneManager();

    jugimap::deleteNCNodesSpecial = true;                           // otherwise it crashes in QCreator in debug mode

    //---------------------------------------------------

    jugimap::Scene* scene = jugimap::sceneManager->AddScene(new apiTestDemo::PlatformerSceneNC());
    jugimap::sceneManager->SetCurrentScene(scene);
}


void MyEventHandler::onFrameStart()
{

    //--- Manage errors
    if(jugimap::settings.GetErrorMessage() != ""){
        if(jugimap::errorMessageLabel){
            jugimap::errorMessageLabel->setString(jugimap::settings.GetErrorMessage().c_str());
            jugimap::errorMessageLabel->setAnchorPoint(ncine::DrawableNode::AnchorTopLeft);
        }
        return ;
    }

    jugimap::sceneManager->Update(ncine::theApplication().interval() * 1000);
}



//#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const ncine::TouchEvent &event)
{
    apiTestDemo::mouse.SetPressed(true);
    apiTestDemo::mouse.SetHit(true);
    apiTestDemo::mouse.SetScreenPosition(jugimap::Vec2f(event.pointers[0].x, event.pointers[0].y));
}

void MyEventHandler::onTouchUp(const ncine::TouchEvent &event)
{
    apiTestDemo::mouse.SetPressed(false);
    apiTestDemo::mouse.SetHit(false);
}

void MyEventHandler::onTouchMove(const ncine::TouchEvent &event)
{
    apiTestDemo::mouse.SetScreenPosition(jugimap::Vec2f(event.pointers[0].x, event.pointers[0].y));
}
//#endif



void MyEventHandler::onMouseButtonPressed(const ncine::MouseEvent &event)
{
    if (event.isLeftButton()) {
        apiTestDemo::mouse.SetPressed(true);
        apiTestDemo::mouse.SetHit(true);
    }
}

void MyEventHandler::onMouseButtonReleased(const ncine::MouseEvent &event)
{
    if (event.isLeftButton()) {
        apiTestDemo::mouse.SetPressed(false);
        apiTestDemo::mouse.SetHit(false);
    }
}

void MyEventHandler::onMouseMoved(const ncine::MouseState &state)
{
    apiTestDemo::mouse.SetScreenPosition(jugimap::Vec2f(state.x, state.y));
}


void MyEventHandler::onShutdown()
{
    apiTestDemo::entities.Delete();
    jugimap::sceneManager->DeleteScenes();
    jugimap::DeleteGlobalObjects();
}


