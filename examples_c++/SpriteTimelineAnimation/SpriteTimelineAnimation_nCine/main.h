#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <ncine/AppConfiguration.h>
#include "jugimapNCINE/jmNCine.h"


class MyEventHandler : public ncine::IAppEventHandler, public ncine::IInputEventHandler
{
  public:
    MyEventHandler();
    ~MyEventHandler() override {}

    void onPreInit(ncine::AppConfiguration &config) override;
    void onInit() override;
    void onFrameStart() override;



//#ifdef __ANDROID__
    void onTouchDown(const ncine::TouchEvent &event) override;
    void onTouchUp(const ncine::TouchEvent &event) override;
    void onTouchMove(const ncine::TouchEvent &event) override;
//#endif

    void onMouseButtonPressed(const ncine::MouseEvent &event) override;
    void onMouseButtonReleased(const ncine::MouseEvent &event) override;
    void onMouseMoved(const ncine::MouseState &state) override;
    void onScrollInput(const ncine::ScrollEvent &event) override;

    void onKeyPressed(const ncine::KeyboardEvent &event) override;
    void onKeyReleased(const ncine::KeyboardEvent &event) override;

    void onJoyButtonPressed(const ncine::JoyButtonEvent &event) override;
    void onJoyButtonReleased(const ncine::JoyButtonEvent &event) override;
    void onJoyHatMoved(const ncine::JoyHatEvent &event) override;
    void onJoyAxisMoved(const ncine::JoyAxisEvent &event) override;
    //void onJoyMappedButtonPressed(const ncine::JoyMappedButtonEvent &event) override;
    //void onJoyMappedButtonReleased(const ncine::JoyMappedButtonEvent &event) override;
    //void onJoyMappedAxisMoved(const ncine::JoyMappedAxisEvent &event) override;
    void onJoyConnected(const ncine::JoyConnectionEvent &event) override;
    void onJoyDisconnected(const ncine::JoyConnectionEvent &event) override;


    void onShutdown() override;


private:

    std::vector<jugimap::KeyCode>keysConversionTable;

};



#endif
