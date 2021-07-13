#include <ncine/Application.h>
#include <ncine/IFile.h>
#include <ncine/FileSystem.h>
#include <ncine/Keys.h>

#include "jugiAppNCINE.h"

#include "main.h"



nctl::UniquePtr<ncine::IAppEventHandler> createAppEventHandler()
{

    MyEventHandler *eh = new MyEventHandler();
    return nctl::UniquePtr<ncine::IAppEventHandler>(eh);
}



//==========================================================================================



void MyEventHandler::onPreInit(ncine::AppConfiguration &config)
{


#if defined(__ANDROID__)
    config.dataPath() = "asset::";
#elif defined(__EMSCRIPTEN__)
    config.dataPath() = "/";
#else
    #ifdef NCPROJECT_DEFAULT_DATA_DIR
    config.dataPath() = NCPROJECT_DEFAULT_DATA_DIR;
    #else
    config.dataPath() = "data/";
    #endif
#endif


    config.resolution.set(1300, 800);
    //config.resolution.set(1920, 1080);
    std::string appName = jugiApp::jugimapAppName + " - nCine version";
    config.windowTitle = nctl::String(appName.c_str());      // jugimapAppName must be a global variable
    config.deferShaderQueries = false;
    config.withVSync = true;
    config.isResizable = false;
    //config.withDebugOverlay = true;
    //config.withGlDebugContext = true;
}


void MyEventHandler::onInit()
{

    ncine::theApplication().setAutoSuspension(true);

    // JUGIMAP CORE INITIALIZATION
    //---------------------------------------------------

    #if !defined (__ANDROID__)
        jugimap::JugiMapBinaryLoader::pathPrefix = std::string(ncine::FileSystem::dataPath().data());
    #endif
    jugimap::GraphicFile::pathPrefix = std::string(ncine::FileSystem::dataPath().data());
    jugimap::Font::pathPrefix = std::string(ncine::FileSystem::dataPath().data());
    jugimap::TextLibrary::pathPrefix = std::string(ncine::FileSystem::dataPath().data());

    // path where text files are stored
    jugimap::TextLibrary::path = "media/texts/";

    jugimap::settings.SetEngine(jugimap::Engine::nCine);
    jugimap::settings.SetScreenSize(jugimap::Vec2i(ncine::theApplication().widthInt(), ncine::theApplication().heightInt()));
    jugimap::settings.SetYCoordinateUp(true);
    jugimap::objectFactory = new jugimap::ObjectFactoryNC();


    //---------------------------------------------------
    jugiApp::application = new jugiApp::DemoApp();
    jugiApp::sceneFactory.reset(new jugiApp::SceneFactoryNCINE());
    jugiApp::application->Init();

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

    jugiApp::application->Update(ncine::theApplication().interval());
}



//#ifdef __ANDROID__

void MyEventHandler::onTouchDown(const ncine::TouchEvent &event)
{

    int pointerID = 0;
    int pointerIndex = event.findPointerIndex(pointerID++);

    while(pointerIndex != -1){
        const ncine::TouchEvent::Pointer &pointer = event.pointers[pointerIndex];
        jugimap::touch._SetFingerState(pointerID, true, jugimap::Vec2i(pointer.x, pointer.y));

        pointerIndex = event.findPointerIndex(pointerID++);
    }


//#ifdef __ANDROID__

    jugimap::mouse._SetButtonState(jugimap::MouseButton::LEFT, true);

//#endif

}


void MyEventHandler::onTouchUp(const ncine::TouchEvent &event)
{

    int pointerID = 0;
    int pointerIndex = event.findPointerIndex(pointerID++);

    while(pointerIndex != -1){
        const ncine::TouchEvent::Pointer &pointer = event.pointers[pointerIndex];
        jugimap::touch._SetFingerState(pointerID, false, jugimap::Vec2i(pointer.x, pointer.y));

        pointerIndex = event.findPointerIndex(pointerID++);
    }


//#ifdef __ANDROID__

    jugimap::mouse._SetButtonState(jugimap::MouseButton::LEFT, false);

//#endif

}


void MyEventHandler::onTouchMove(const ncine::TouchEvent &event)
{

    int pointerID = 0;
    int pointerIndex = event.findPointerIndex(pointerID++);

    while(pointerIndex != -1){
        const ncine::TouchEvent::Pointer &pointer = event.pointers[pointerIndex];
        jugimap::touch._SetFingerState(pointerID, true, jugimap::Vec2i(pointer.x, pointer.y));

        pointerIndex = event.findPointerIndex(pointerID++);
    }


//#ifdef __ANDROID__

    jugimap::mouse._SetPosition(jugimap::Vec2i(event.pointers[0].x, event.pointers[0].y));

//#endif

}



void MyEventHandler::onMouseButtonPressed(const ncine::MouseEvent &event)
{
    if(event.isLeftButton()){
        jugimap::mouse._SetButtonState(jugimap::MouseButton::LEFT, true);

    }else if(event.isMiddleButton()){
        jugimap::mouse._SetButtonState(jugimap::MouseButton::MIDDLE, true);

    }else if(event.isRightButton()){
        jugimap::mouse._SetButtonState(jugimap::MouseButton::RIGHT, true);

    }
}


void MyEventHandler::onMouseButtonReleased(const ncine::MouseEvent &event)
{
    if(event.isLeftButton()){
        jugimap::mouse._SetButtonState(jugimap::MouseButton::LEFT, false);

    }else if(event.isMiddleButton()){
        jugimap::mouse._SetButtonState(jugimap::MouseButton::MIDDLE, false);

    }else if(event.isRightButton()){
        jugimap::mouse._SetButtonState(jugimap::MouseButton::RIGHT, false);
    }
}


void MyEventHandler::onMouseMoved(const ncine::MouseState &state)
{
    jugimap::mouse._SetPosition(jugimap::Vec2i(state.x, state.y));
}


void MyEventHandler::onScrollInput(const ncine::ScrollEvent &event)
{
    jugimap::mouse._SetWheel(jugimap::Vec2i(event.x, event.y));

}


void MyEventHandler::onKeyPressed(const ncine::KeyboardEvent &event)
{

    if(event.sym==ncine::KeySym::UNKNOWN) return;

    jugimap::KeyCode k = keysConversionTable[(int)event.sym];
    jugimap::keyboard._SetKeyState(k, true);

}


void MyEventHandler::onKeyReleased(const ncine::KeyboardEvent &event)
{

    if(event.sym==ncine::KeySym::UNKNOWN) return;

    jugimap::KeyCode k = keysConversionTable[(int)event.sym];
    jugimap::keyboard._SetKeyState(k, false);
}


void MyEventHandler::onJoyButtonPressed(const ncine::JoyButtonEvent &event)
{

    if(event.joyId<0 || event.joyId>jugimap::joysticks.size()) return;

    jugimap::joysticks[event.joyId]._SetButtonState(event.buttonId, true);

}


void MyEventHandler::onJoyButtonReleased(const ncine::JoyButtonEvent &event)
{

    if(event.joyId<0 || event.joyId>jugimap::joysticks.size()) return;

    jugimap::joysticks[event.joyId]._SetButtonState(event.buttonId, false);

}


void MyEventHandler::onJoyHatMoved(const ncine::JoyHatEvent &event)
{

    if(event.joyId<0 || event.joyId>jugimap::joysticks.size()) return;


    if(event.hatId==0){

        if(event.hatState==ncine::HatState::CENTERED){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::NONE);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::NONE);

        }else if(event.hatState==ncine::HatState::UP){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::NONE);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::UP);

        }else if(event.hatState==ncine::HatState::RIGHT_UP){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::RIGHT);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::UP);

        }else if(event.hatState==ncine::HatState::RIGHT){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::RIGHT);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::NONE);

        }else if(event.hatState==ncine::HatState::RIGHT_DOWN){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::RIGHT);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::DOWN);

        }else if(event.hatState==ncine::HatState::DOWN){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::NONE);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::DOWN);

        }else if(event.hatState==ncine::HatState::LEFT_DOWN){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::LEFT);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::DOWN);

        }else if(event.hatState==ncine::HatState::LEFT){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::LEFT);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::NONE);

        }else if(event.hatState==ncine::HatState::LEFT_UP){
            jugimap::joysticks[event.joyId]._SetPOV_X(jugimap::Joystick::POV_X::LEFT);
            jugimap::joysticks[event.joyId]._SetPOV_Y(jugimap::Joystick::POV_Y::UP);

        }
    }
}


void MyEventHandler::onJoyAxisMoved(const ncine::JoyAxisEvent &event)
{

    if(event.joyId<0 || event.joyId>jugimap::joysticks.size()) return;

    if(event.axisId==0){
        jugimap::joysticks[event.joyId]._SetXaxis(event.normValue);

    }else if(event.axisId==1){
        jugimap::joysticks[event.joyId]._SetYaxis(event.normValue);

    }else if(event.axisId==2){
        jugimap::joysticks[event.joyId]._SetZaxis(event.normValue);
    }

}


void MyEventHandler::onJoyConnected(const ncine::JoyConnectionEvent &event)
{

    if(event.joyId<0 || event.joyId>jugimap::joysticks.size()) return;

    jugimap::joysticks[event.joyId]._SetConnected(true);

}


void MyEventHandler::onJoyDisconnected(const ncine::JoyConnectionEvent &event)
{

    if(event.joyId<0 || event.joyId>jugimap::joysticks.size()) return;

    jugimap::joysticks[event.joyId]._SetConnected(false);

}


void MyEventHandler::onShutdown()
{
    jugimap::settings.SetAppTerminated(true);
    //jugimap::sceneManager->DeleteScenes();
    delete jugiApp::application;
    jugimap::DeleteGlobalObjects();
}


MyEventHandler::MyEventHandler()
{

    keysConversionTable.resize((int)ncine::KeySym::COUNT, jugimap::KeyCode::UNKNOWN);

    keysConversionTable[(int)ncine::KeySym::BACKSPACE] = jugimap::KeyCode::BACKSPACE;
    keysConversionTable[(int)ncine::KeySym::TAB] = jugimap::KeyCode::TAB;
    keysConversionTable[(int)ncine::KeySym::RETURN] = jugimap::KeyCode::ENTER;
    keysConversionTable[(int)ncine::KeySym::PAUSE] = jugimap::KeyCode::PAUSE;
    keysConversionTable[(int)ncine::KeySym::ESCAPE] = jugimap::KeyCode::ESCAPE;
    keysConversionTable[(int)ncine::KeySym::SPACE] = jugimap::KeyCode::SPACE;

    keysConversionTable[(int)ncine::KeySym::PAGEUP] = jugimap::KeyCode::PAGEUP;
    keysConversionTable[(int)ncine::KeySym::PAGEDOWN] = jugimap::KeyCode::PAGEDOWN;
    keysConversionTable[(int)ncine::KeySym::END] = jugimap::KeyCode::END;
    keysConversionTable[(int)ncine::KeySym::HOME] = jugimap::KeyCode::HOME;
    keysConversionTable[(int)ncine::KeySym::LEFT] = jugimap::KeyCode::LEFT;
    keysConversionTable[(int)ncine::KeySym::UP] = jugimap::KeyCode::UP;
    keysConversionTable[(int)ncine::KeySym::RIGHT] = jugimap::KeyCode::RIGHT;
    keysConversionTable[(int)ncine::KeySym::DOWN] = jugimap::KeyCode::DOWN;
    keysConversionTable[(int)ncine::KeySym::PRINTSCREEN] = jugimap::KeyCode::PRINT;
    keysConversionTable[(int)ncine::KeySym::INSERT] = jugimap::KeyCode::INSERT;
    keysConversionTable[(int)ncine::KeySym::DELETE] = jugimap::KeyCode::DELETEkey;

    keysConversionTable[(int)ncine::KeySym::N0] = jugimap::KeyCode::NUM_0;
    keysConversionTable[(int)ncine::KeySym::N1] = jugimap::KeyCode::NUM_1;
    keysConversionTable[(int)ncine::KeySym::N2] = jugimap::KeyCode::NUM_2;
    keysConversionTable[(int)ncine::KeySym::N3] = jugimap::KeyCode::NUM_3;
    keysConversionTable[(int)ncine::KeySym::N4] = jugimap::KeyCode::NUM_4;
    keysConversionTable[(int)ncine::KeySym::N5] = jugimap::KeyCode::NUM_5;
    keysConversionTable[(int)ncine::KeySym::N6] = jugimap::KeyCode::NUM_6;
    keysConversionTable[(int)ncine::KeySym::N7] = jugimap::KeyCode::NUM_7;
    keysConversionTable[(int)ncine::KeySym::N8] = jugimap::KeyCode::NUM_8;
    keysConversionTable[(int)ncine::KeySym::N9] = jugimap::KeyCode::NUM_9;

    keysConversionTable[(int)ncine::KeySym::KP0] = jugimap::KeyCode::NUM_0;
    keysConversionTable[(int)ncine::KeySym::KP1] = jugimap::KeyCode::NUM_1;
    keysConversionTable[(int)ncine::KeySym::KP2] = jugimap::KeyCode::NUM_2;
    keysConversionTable[(int)ncine::KeySym::KP3] = jugimap::KeyCode::NUM_3;
    keysConversionTable[(int)ncine::KeySym::KP4] = jugimap::KeyCode::NUM_4;
    keysConversionTable[(int)ncine::KeySym::KP5] = jugimap::KeyCode::NUM_5;
    keysConversionTable[(int)ncine::KeySym::KP6] = jugimap::KeyCode::NUM_6;
    keysConversionTable[(int)ncine::KeySym::KP7] = jugimap::KeyCode::NUM_7;
    keysConversionTable[(int)ncine::KeySym::KP8] = jugimap::KeyCode::NUM_8;
    keysConversionTable[(int)ncine::KeySym::KP9] = jugimap::KeyCode::NUM_9;
    keysConversionTable[(int)ncine::KeySym::KP_ENTER] = jugimap::KeyCode::ENTER;

    keysConversionTable[(int)ncine::KeySym::A] = jugimap::KeyCode::A;
    keysConversionTable[(int)ncine::KeySym::B] = jugimap::KeyCode::B;
    keysConversionTable[(int)ncine::KeySym::C] = jugimap::KeyCode::C;
    keysConversionTable[(int)ncine::KeySym::D] = jugimap::KeyCode::D;
    keysConversionTable[(int)ncine::KeySym::E] = jugimap::KeyCode::E;
    keysConversionTable[(int)ncine::KeySym::F] = jugimap::KeyCode::F;
    keysConversionTable[(int)ncine::KeySym::G] = jugimap::KeyCode::G;
    keysConversionTable[(int)ncine::KeySym::H] = jugimap::KeyCode::H;
    keysConversionTable[(int)ncine::KeySym::I] = jugimap::KeyCode::I;
    keysConversionTable[(int)ncine::KeySym::J] = jugimap::KeyCode::J;
    keysConversionTable[(int)ncine::KeySym::K] = jugimap::KeyCode::K;
    keysConversionTable[(int)ncine::KeySym::L] = jugimap::KeyCode::L;
    keysConversionTable[(int)ncine::KeySym::M] = jugimap::KeyCode::M;
    keysConversionTable[(int)ncine::KeySym::N] = jugimap::KeyCode::N;
    keysConversionTable[(int)ncine::KeySym::O] = jugimap::KeyCode::O;
    keysConversionTable[(int)ncine::KeySym::P] = jugimap::KeyCode::P;
    keysConversionTable[(int)ncine::KeySym::Q] = jugimap::KeyCode::Q;
    keysConversionTable[(int)ncine::KeySym::R] = jugimap::KeyCode::R;
    keysConversionTable[(int)ncine::KeySym::S] = jugimap::KeyCode::S;
    keysConversionTable[(int)ncine::KeySym::T] = jugimap::KeyCode::T;
    keysConversionTable[(int)ncine::KeySym::U] = jugimap::KeyCode::U;
    keysConversionTable[(int)ncine::KeySym::V] = jugimap::KeyCode::V;
    keysConversionTable[(int)ncine::KeySym::W] = jugimap::KeyCode::W;
    keysConversionTable[(int)ncine::KeySym::X] = jugimap::KeyCode::X;
    keysConversionTable[(int)ncine::KeySym::Y] = jugimap::KeyCode::Y;
    keysConversionTable[(int)ncine::KeySym::Z] = jugimap::KeyCode::Z;

    keysConversionTable[(int)ncine::KeySym::F1] = jugimap::KeyCode::F1;
    keysConversionTable[(int)ncine::KeySym::F2] = jugimap::KeyCode::F2;
    keysConversionTable[(int)ncine::KeySym::F3] = jugimap::KeyCode::F3;
    keysConversionTable[(int)ncine::KeySym::F4] = jugimap::KeyCode::F4;
    keysConversionTable[(int)ncine::KeySym::F5] = jugimap::KeyCode::F5;
    keysConversionTable[(int)ncine::KeySym::F6] = jugimap::KeyCode::F6;
    keysConversionTable[(int)ncine::KeySym::F7] = jugimap::KeyCode::F7;
    keysConversionTable[(int)ncine::KeySym::F8] = jugimap::KeyCode::F8;
    keysConversionTable[(int)ncine::KeySym::F9] = jugimap::KeyCode::F9;
    keysConversionTable[(int)ncine::KeySym::F10] = jugimap::KeyCode::F10;
    keysConversionTable[(int)ncine::KeySym::F11] = jugimap::KeyCode::F11;
    keysConversionTable[(int)ncine::KeySym::F12] = jugimap::KeyCode::F12;

    //keysConversionTable[(int)ncine::KeySym::TILDE] = jugimap::KeyCode::KEY_TILDE;                     // ~
    //keysConversionTable[(int)ncine::KeySym::] = jugimap::KeyCode::KEY_HYPHEN;                    // -
    keysConversionTable[(int)ncine::KeySym::EQUALS] = jugimap::KeyCode::EQUAL;                    // =
    keysConversionTable[(int)ncine::KeySym::LEFTBRACKET] = jugimap::KeyCode::LEFT_BRACKET;
    keysConversionTable[(int)ncine::KeySym::RIGHTBRACKET] = jugimap::KeyCode::RIGHT_BRACKET;
    keysConversionTable[(int)ncine::KeySym::BACKSLASH] = jugimap::KeyCode::BACKSLASH;
    keysConversionTable[(int)ncine::KeySym::SEMICOLON] = jugimap::KeyCode::SEMICOLON;
    keysConversionTable[(int)ncine::KeySym::QUOTE] = jugimap::KeyCode::QUOTE;
    keysConversionTable[(int)ncine::KeySym::COMMA] = jugimap::KeyCode::COMMA;
    keysConversionTable[(int)ncine::KeySym::PERIOD] = jugimap::KeyCode::PERIOD;
    keysConversionTable[(int)ncine::KeySym::SLASH] = jugimap::KeyCode::SLASH;

    keysConversionTable[(int)ncine::KeySym::LSHIFT] = jugimap::KeyCode::LEFT_SHIFT;
    keysConversionTable[(int)ncine::KeySym::RSHIFT] = jugimap::KeyCode::RIGHT_SHIFT;
    keysConversionTable[(int)ncine::KeySym::LCTRL] = jugimap::KeyCode::LEFT_CONTROL;
    keysConversionTable[(int)ncine::KeySym::RCTRL] = jugimap::KeyCode::RIGHT_CONTROL;
    keysConversionTable[(int)ncine::KeySym::LALT] = jugimap::KeyCode::LEFT_ALT;
    keysConversionTable[(int)ncine::KeySym::RALT] = jugimap::KeyCode::RIGHT_ALT;
    //keysConversionTable[(int)ncine::KeySym::LSUPER] = jugimap::KeyCode::KEY_LEFT_SYS;
    //keysConversionTable[(int)ncine::KeySym::RSUPER] = jugimap::KeyCode::KEY_RIGHT_SYS;

}


