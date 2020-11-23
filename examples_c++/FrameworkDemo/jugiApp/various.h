#ifndef API_DEMO_TEST__VARIOUS_H
#define API_DEMO_TEST__VARIOUS_H


#include "../jugimap/jugimap.h"


namespace jugiApp{



/*
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

extern Mouse sceneCursor;

*/

//==============================================================================================


class CameraController
{
public:

    ~CameraController(){ if(ownCameraPath && cameraPath) delete cameraPath; }
    void Init(jugimap::WorldMapCamera *_cameraView, jugimap::VectorShape *_cameraPath, bool _ownCameraPath = false);
    void Start();
    void Update();

    bool GetPaused(){return paused;}
    bool GetZany(){return zany;}


    void SetZany(bool _zany);

    void SetPaused(bool _paused);
    bool IsPaused(){ return paused; }


private:
    jugimap::WorldMapCamera *camera = nullptr;              // LINK
    jugimap::VectorShape *cameraPath = nullptr;                     // LINK or OWNED
    bool ownCameraPath = false;
    jugimap::Tween movingTween;
    bool zany = false;
    jugimap::Tween zanyTween;

    bool paused = false;

};


//==============================================================================================


jugimap::VectorShape* MakeWorldCameraOverviewPath_v1(jugimap::WorldMapCamera &_camera);
jugimap::VectorShape* MakeWorldCameraOverviewPath_v2(jugimap::WorldMapCamera &_camera);
jugimap::VectorShape* MakeWorldCameraOverviewPath_v3(jugimap::WorldMapCamera &_camera);


//==============================================================================================


class SpriteCustomObjectTest : public jugimap::CustomObject
{
public:

    SpriteCustomObjectTest(jugimap::Sprite* _sprite) : sprite(_sprite){}

    jugimap::Sprite* sprite = nullptr;            // link
};



/*

class Button
{
public:

    static const int frameNORMAL = 0;
    static const int frameMOUSE_OVER = 1;
    static const int frameACTIVE = 2;

    static const int flagNONE = 0;
    static const int flagCLICKED = 1;


    Button(int _tag, jugimap::StandardSprite *_sprite,  jugimap::TextSprite *_stringLabel, bool _checkable):
        tag(_tag),
        sprite(_sprite),
        labelText(_stringLabel),
        checkable(_checkable)
    {}


    int GetTag(){ return tag;}
    bool IsChecked(){ return checked;}
    int Update();


    void SetDisabled(bool _disabled);
    bool IsDisabled(){ return disabled; }
    jugimap::StandardSprite* GetSprite(){ return sprite; }
    jugimap::TextSprite* GetStringLabel(){ return labelText; }


private:
    int tag = 0;
    jugimap::StandardSprite *sprite = nullptr;                   // LINK
    jugimap::TextSprite *labelText = nullptr;                        // LINK

    bool disabled = false;
    bool checkable = true;
    bool checked = false;
    bool mouseOver = false;
    int spriteFrame = frameNORMAL;

};

*/

}

#endif
