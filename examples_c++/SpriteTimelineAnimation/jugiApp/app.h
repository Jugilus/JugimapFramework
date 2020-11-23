#ifndef JUGIMAP_SPRITE_TIMELINE_ANIMATION_H
#define JUGIMAP_SPRITE_TIMELINE_ANIMATION_H


#include "../jugimap/jugimap.h"



namespace jugiApp{

class DemoScene;


class DemoApp : public jugimap::App
{
public:


    bool Init() override;


private:

    DemoScene *demoScene = nullptr;      //LINK


};


extern DemoApp *application;

extern std::string jugimapAppName;


}


#endif
