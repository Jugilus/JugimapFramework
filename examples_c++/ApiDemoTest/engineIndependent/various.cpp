#include "various.h"


using namespace jugimap;


namespace apiTestDemo{


Mouse mouse;



//==============================================================================================


void CameraController::Init(WorldMapCamera *_cameraView, jugimap::VectorShape *_cameraPath, bool _ownCameraPath)
{
    camera = _cameraView;
    cameraPath = _cameraPath;
    ownCameraPath = _ownCameraPath;

    //----
    //cameraPath->CalculatePathLength();
    float speed = 200;                                       // 50 pixels per second
    float duration = cameraPath->GetPathLength() / speed;


    //----
    movingTween.Init(0.0, 1.0, duration, Easing::Kind::LINEAR);
    movingTween.SetMode(Tween::Mode::LOOP);
    movingTween.Play();

    zanyTween.Init(0.0, 1.0, 7, Easing::Kind::EASE_START_END);
    zanyTween.SetMode(Tween::Mode::LOOP_REVERSE);

    Vec2f pos = cameraPath->GetPathPoint(0.0);   // start position
    camera->SetPointedPosition(pos, true);

    //camera->SetPointedPosition(Vec2f(600, 100));
}


void CameraController::Start()
{
    movingTween.Play();
    if(zany){
        zanyTween.Play();
    }
}


void CameraController::Update()
{
    if(cameraPath==nullptr) return;

    //camera->SetPointedPosition(Vec2f(1100, 850));

    //camera->SetPointedPosition(mouse.GetWorldPosition());
    //return;

    // move camera pointed point along the path
    float p = movingTween.GetValue();
    Vec2f pos = cameraPath->GetPathPoint(p);
    camera->SetPointedPosition(pos);
    DbgOutput("CameraController::Update() p:" +std::to_string(p)+ "  pos x:"+std::to_string(pos.x)+ " y:"+std::to_string(pos.y));

    if(zany){
        float z = zanyTween.GetValue();
        float scale = 0.8 + (0.4*z);
        camera->SetScale(scale);
        float rotation = -15 + 30*z;
        camera->SetRotation(rotation);
        //DbgOutput("zany scale:" +std::to_string(scale)+ "  rotation:"+std::to_string(rotation));
    }

}


void CameraController::SetPaused(bool _paused)
{
    if(cameraPath==nullptr) return;
    paused = _paused;

    if(paused){
        movingTween.Pause();
        zanyTween.Pause();

    }else{
        movingTween.Resume();
        zanyTween.Resume();
    }

}


void CameraController::SetZany(bool _zany)
{
    if(_zany==zany) return;

    zany=_zany;

    if(zany){
        zanyTween.Play();
    }else{
        zanyTween.Stop();
        camera->SetRotation(0);
        camera->SetScale(1.0);
    }

}


//==============================================================================================


VectorShape* MakeWorldCameraOverviewPath_v1(WorldMapCamera &_camera)
{

    Vec2f min =  _camera.GetViewport().Size()/2.0;
    Vec2f max =  Vec2iToVec2f(_camera.GetWorldSize()) - _camera.GetViewport().Size()/2.0;
    if(max.x<min.x) max.x = min.x;
    if(max.y<min.y) max.y = min.y;

    PolyLineShape *shape = new PolyLineShape();
    shape->vertices.push_back(min);
    shape->vertices.push_back(max);
    shape->vertices.push_back(Vec2f(max.x, min.y));
    shape->vertices.push_back(Vec2f(min.x, max.y));
    shape->vertices.push_back(min);
    shape->RebuildPath();

    VectorShape *cameraPath = new VectorShape(shape);

    return cameraPath;
}


VectorShape* MakeWorldCameraOverviewPath_v2(WorldMapCamera &_camera)
{

    Vec2f min =  _camera.GetViewport().Size()/2.0;
    Vec2f max =  Vec2iToVec2f(_camera.GetWorldSize()) - _camera.GetViewport().Size()/2.0;
    if(max.x<min.x) max.x = min.x;
    if(max.y<min.y) max.y = min.y;

    PolyLineShape *shape = new PolyLineShape();
    shape->vertices.push_back(min);
    shape->vertices.push_back(Vec2f(max.x, min.y));
    shape->vertices.push_back(max);
    shape->vertices.push_back(Vec2f(min.x, max.y));
    shape->vertices.push_back(min);
    shape->RebuildPath();

    VectorShape *cameraPath = new VectorShape(shape);

    return cameraPath;
}


VectorShape* MakeWorldCameraOverviewPath_v3(WorldMapCamera &_camera)
{

    Vec2f min =  _camera.GetViewport().Size()/2.0;
    Vec2f max =  Vec2iToVec2f(_camera.GetWorldSize()) - _camera.GetViewport().Size()/2.0;
    if(max.x<min.x) max.x = min.x;
    if(max.y<min.y) max.y = min.y;

    PolyLineShape *shape = new PolyLineShape();
    shape->vertices.push_back(Vec2f(min.x, max.y));
    shape->vertices.push_back(Vec2f(max.x, min.y));
    shape->vertices.push_back(max);
    shape->vertices.push_back(min);
    shape->vertices.push_back(Vec2f(min.x, max.y));
    shape->RebuildPath();

    VectorShape *cameraPath = new VectorShape(shape);

    return cameraPath;
}


//==============================================================================================


void Button::SetDisabled(bool _disabled)
{
    if(_disabled == disabled) return;
    disabled = _disabled;

    if(disabled){
        spriteFrame = frameNORMAL;
        sprite->SetActiveImage(sprite->GetSourceSprite()->GetGraphicItems()[spriteFrame]);    // !!! Button sprite must have 3 frames for 3 button states - normal, mouse over and active !!!
        sprite->SetAlpha(0.35);
        labelText->SetAlpha(0.35);

    }else{
        sprite->SetAlpha(1.00);
        labelText->SetAlpha(1.0);
    }
}



int Button::Update()
{

    if(disabled) return flagNONE;

    int spriteFrameCurrent = frameNORMAL;
    int returnFlag = flagNONE;

    mouseOver = sprite->PointInside(mouse.GetGuiPosition());

    //---
    if(mouseOver){

        if(mouse.IsHit()){
            returnFlag = flagCLICKED;
            if(checkable){
                checked = !checked;
            }
        }

        if(mouse.IsPressed()){
            spriteFrameCurrent = frameACTIVE;

        }else{
            if(checkable && checked){
                spriteFrameCurrent = frameACTIVE;
            }else{
                spriteFrameCurrent = frameMOUSE_OVER;
            }
        }

    }else{
        if(checkable && checked){
            spriteFrameCurrent = frameACTIVE;
        }else{
            spriteFrameCurrent = frameNORMAL;
        }
    }

    //---
    if(spriteFrameCurrent != spriteFrame){
        spriteFrame = spriteFrameCurrent;
        sprite->SetActiveImage(sprite->GetSourceSprite()->GetGraphicItems()[spriteFrame]);    // !!! Button sprite must have 3 frames for 3 button states - normal, mouse over and active !!!
    }

    return returnFlag;
}


}
