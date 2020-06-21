#ifndef API_DEMO_TEST__SCENE_H
#define API_DEMO_TEST__SCENE_H


#include "../jugimap/jugimap.h"



// A simple mouse class which provides engine independent mouse interface
class Mouse
{
public:

    jugimap::Vec2f GetScreenPosition(){ return screenPosition; }
    jugimap::Vec2f GetWorldPosition(){ return worldPosition; }
    jugimap::Vec2f GetGuiPosition(){ return guiPosition; }
    bool IsPressed(){ return pressed;}
    bool IsHit(){ bool hitStored = hit; hit = false; return hitStored;}


    //--- functions bellow should be called only by the engine mouse events code
    void SetScreenPosition(jugimap::Vec2f _pos){ screenPosition = _pos;}
    void SetWorldPosition(jugimap::Vec2f _pos){ worldPosition = _pos;}
    void SetGuiPosition(jugimap::Vec2f _pos){ guiPosition = _pos;}
    void SetHit(bool _hit){ hit = _hit;}
    void SetPressed(bool _pressed){ pressed = _pressed;}

private:

    jugimap::Vec2f screenPosition;
    jugimap::Vec2f worldPosition;
    jugimap::Vec2f guiPosition;

    bool hit = false;
    bool pressed = false;

};

extern Mouse mouse;



//==============================================================================================


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



struct AnimatedSprite
{

    jugimap::Sprite *sprite = nullptr;                      // LINK
    jugimap::AnimationInstance* aniInstance = nullptr;      // OWNED
    jugimap::AnimationPlayer player;

    ~AnimatedSprite();

};



class DemoScene : public jugimap::Scene
{
public:

    ~DemoScene() override;
    bool Init() override;
    void Start() override;
    void Update() override;


protected:

    jugimap::SourceGraphics *worldSourceGraphics = nullptr;             // OWNED
    jugimap::Map* worldMap = nullptr;                                   // OWNED
    jugimap::WorldMapCamera worldCamera;
    CameraController cameraController;

    //--- starting zOrder value for map layering (for engines which handle sprites drawing order by the 'z-order' parameter)
    int zOrderWorldMap = 1000;

    std::vector<AnimatedSprite*>animatedSprites;


    virtual void UpdateEngineObjects();

};



#endif
