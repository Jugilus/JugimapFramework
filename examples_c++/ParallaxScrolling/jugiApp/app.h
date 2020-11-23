#ifndef JUGIMAP_SPRITE_TIMELINE_ANIMATION_H
#define JUGIMAP_SPRITE_TIMELINE_ANIMATION_H


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

    DemoScene *demoScene = nullptr;      //LINK




};


extern DemoApp *application;

extern std::string jugimapAppName;


}


#endif
