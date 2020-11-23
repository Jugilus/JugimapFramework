#ifndef JUGIMAP_SPRITE_TIMELINE_ANIMATION__DEMO_SCENE_H
#define JUGIMAP_SPRITE_TIMELINE_ANIMATION__DEMO_SCENE_H


#include "../jugimap/jugimap.h"



namespace jugiApp{


class CameraController
{
public:

    ~CameraController(){ if(ownCameraPath && cameraPath) delete cameraPath; }
    void Init(jugimap::WorldMapCamera *_cameraView, jugimap::VectorShape *_cameraPath, bool _ownCameraPath = false);
    void Start();
    void Update();

    void SetPaused(bool _paused);
    bool IsPaused(){ return paused; }

private:
    jugimap::WorldMapCamera *camera = nullptr;              // LINK
    jugimap::VectorShape *cameraPath = nullptr;             // LINK or OWNED
    bool ownCameraPath = false;
    jugimap::Tween movingTween;

    bool paused = false;

};


//==============================================================================================


jugimap::VectorShape* MakeWorldCameraOverviewPath_v1(jugimap::WorldMapCamera &_camera);



struct SpriteAnimation
{

    jugimap::AnimationInstance* aniInstance = nullptr;      // OWNING
    jugimap::AnimationPlayer player;

    ~SpriteAnimation();

};




class DemoScene : public jugimap::Scene
{
public:

    bool Init() override;
    void Start() override;
    void Update() override;


protected:
    jugimap::WorldMapCamera worldCamera;
    CameraController cameraController;

    std::vector<std::unique_ptr<SpriteAnimation>>animatedSprites;

};


}




#endif
