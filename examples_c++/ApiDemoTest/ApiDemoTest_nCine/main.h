#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <ncine/AppConfiguration.h>



class MyEventHandler : public ncine::IAppEventHandler, public ncine::IInputEventHandler
{
  public:
    ~MyEventHandler() override {}

    void onPreInit(ncine::AppConfiguration &config) override;
    void onInit() override;
    void onFrameStart() override;



#ifdef __ANDROID__
    void onTouchDown(const ncine::TouchEvent &event) override;
    void onTouchUp(const ncine::TouchEvent &event) override;
    void onTouchMove(const ncine::TouchEvent &event) override;
#endif
    void onMouseButtonPressed(const ncine::MouseEvent &event) override;
    void onMouseButtonReleased(const ncine::MouseEvent &event) override;
    void onMouseMoved(const ncine::MouseState &state) override;

    void onShutdown() override;


};



#endif
