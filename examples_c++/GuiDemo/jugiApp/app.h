#ifndef DEMO_EXTENDED_APP_H
#define DEMO_EXTENDED_APP_H


#include "../jugimap/jugimap.h"



namespace jugiApp{

class DemoScene;


class DemoApp : public jugimap::App
{
public:

    //~DemoApp() override;
    bool Init() override;
    //void Start() override;
    //void Update(float delta) override;
    //void Draw() override;



private:

    DemoScene *guiDemoTestScene = nullptr;      //LINK




};


extern DemoApp *application;

extern std::string jugimapAppName;


}


#endif
