#include <chrono>
#include <assert.h>
#include <sstream>

#include "app.h"
#include "scene.h"



using namespace jugimap;


namespace jugiApp{




void CameraController::Init(WorldMapCamera *_cameraView, jugimap::VectorShape *_cameraPath, bool _ownCameraPath)
{
    camera = _cameraView;
    cameraPath = _cameraPath;
    ownCameraPath = _ownCameraPath;

    //----
    float speed = 125;
    float duration = cameraPath->GetPathLength() / speed;

    //----
    movingTween.Init(0.0, 1.0, duration, Easing::Kind::LINEAR);
    movingTween.SetMode(Tween::Mode::LOOP);
    movingTween.Play();

    Vec2f pos = cameraPath->GetPathPoint(0.0);   // start position
    camera->SetPointedPosition(pos, true);
}


void CameraController::Start()
{
    movingTween.Play();
}


void CameraController::Update()
{
    if(cameraPath==nullptr) return;
    //return;

    float p = movingTween.Update();
    Vec2f pos = cameraPath->GetPathPoint(p);
    camera->SetPointedPosition(pos);
}


void CameraController::SetPaused(bool _paused)
{

    if(cameraPath==nullptr) return;
    paused = _paused;

    if(paused){
        movingTween.Pause();
    }else{
        movingTween.Resume();
    }

}


//==============================================================================================


VectorShape* MakeWorldCameraOverviewPath_v1(WorldMapCamera &_camera)
{

    Vec2f min =  _camera.GetViewport().GetSize()/2.0;
    Vec2f max =  Vec2iToVec2f(_camera.GetWorldSize()) - _camera.GetViewport().GetSize()/2.0;
    if(max.x<min.x) max.x = min.x;
    if(max.y<min.y) max.y = min.y;
    if(settings.IsYCoordinateUp()){
        std::swap(min.y,max.y);
    }

    PolyLineShape *shape = new PolyLineShape();
    shape->vertices.push_back(Vec2f(min.x, max.y));
    shape->vertices.push_back(Vec2f(max.x, max.y));
    shape->vertices.push_back(Vec2f(max.x, min.y));
    shape->vertices.push_back(Vec2f(min.x, min.y));
    //shape->vertices.push_back(Vec2f(min.x, min.y));
    //shape->vertices.push_back(Vec2f(min.x, max.y));
    //shape->vertices.push_back(Vec2f(max.x, max.y));
    //shape->vertices.push_back(Vec2f(max.x, min.y));
    shape->_SetClosed(true);
    shape->RebuildPath();

    VectorShape *cameraPath = new VectorShape(shape);

    return cameraPath;
}


SpriteAnimation::~SpriteAnimation()
{

    if(aniInstance){
        delete aniInstance;
    }

}


//==============================================================================================



bool DemoScene::Init()
{

    GetEngineScene()->PreInit();


    //------------------------------------------------------------------------------
    // LOAD SOURCE GRAPHICS AND MAPS
    //------------------------------------------------------------------------------
    std::string worldMapFile = "media/maps/world.jmb";
    std::string worldSourceGraphicsFile = "media/maps/cfgWorld.sgb";
    std::string overlayMapFile = "media/maps/overlay.jmb";
    std::string overlaySourceGraphicsFile = "media/maps/cfgOverlay.sgb";



    SourceGraphics* worldSourceGraphics = LoadAndAddSourceGraphics(worldSourceGraphicsFile);
    if(worldSourceGraphics==nullptr){
        return false;
    }

    Map* worldMap = LoadAndAddMap(worldMapFile, worldSourceGraphics, MapType::WORLD);
    if(worldMap==nullptr){
        return false;
    }


    //------------------------------------------------------------------------------
    // INITIALIZE MAPS AND ASSIGN CAMERAS
    //------------------------------------------------------------------------------
    worldMap->InitAsWorldMap();
    Vec2i screenSize = settings.GetScreenSize();
    worldCamera.Init(Rectf(0,0, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    worldMap->SetCamera(&worldCamera);

    // Initialize camera controller which moves camera around the map
    cameraController.Init(&worldCamera, MakeWorldCameraOverviewPath_v1(worldCamera), true);


    //------------------------------------------------------------------------------
    // SETUP ANIMATIONS
    //-------------------------------------------------------------------------------
    // We use simple struct 'AnimatedSprite' to group animated sprites with two object required for animation playback: 'AnimationInstance' and 'AnimationPlayer'
    std::vector<Sprite*>sprites;
    CollectSpritesWithParameter(worldMap, sprites, "animation");

    for(Sprite *s: sprites){

        jugimap::TimelineAnimation *ta = FindTimelineAnimationWithName(static_cast<StandardSprite*>(s), Parameter::GetValue(s->GetParameters(), "animation"));
        if(ta){
            SpriteAnimation *as = new SpriteAnimation();
            as->aniInstance = new jugimap::TimelineAnimationInstance(ta, s);

            animatedSprites.push_back(std::unique_ptr<SpriteAnimation>(as));

            //---- if a sprite has defined "aniDelay" parameter we use it to delay animation
            if(Parameter::Exists(s->GetParameters(), "animDelay")){
                float animDelay = Parameter::GetFloatValue(s->GetParameters(), "animDelay");
                int msStartDelay = animDelay * as->aniInstance->GetAnimation()->GetDuration();       // aniDelay = 1.0 -> animation is delayed for one duration (loop) period
                as->aniInstance->GetBaseParameters().startDelay = msStartDelay;
            }


            //--- There is sprite named 'sprity' which move along a closed path. We make few more sprities and animate them.
            if(s->GetName()=="sprity"){

                for(int i=1; i<=3; i++){
                    Sprite * sExtra = s->MakeActiveCopy();              // create a copy of sprity
                    SpriteAnimation *asExtra = new SpriteAnimation();
                    asExtra->aniInstance = new jugimap::TimelineAnimationInstance(ta, sExtra);

                    // We can set custom start play time offset for animation playback.
                    // This will place sprites at their offset "in animation" position  when animation start.
                    asExtra->aniInstance->SetStartPlayTimeOffset(ta->GetDuration() * i/20.0);    // lets distribute them along the movement path close enough to each other

                    animatedSprites.push_back(std::unique_ptr<SpriteAnimation>(asExtra));
                }
            }
        }
    }


    //---
    GetEngineScene()->PostInit();

    SetInitialized(true);

    return true;
}


void DemoScene::Start()
{

    cameraController.Start();

    for(std::unique_ptr<SpriteAnimation> &as : animatedSprites){
        as->player.Play(as->aniInstance);
    }

}


void DemoScene::Update()
{

    //---- update global time variable
    jugimap::time.UpdatePassedTimeMS();


    //---- pause scene
    if(jugimap::mouse.IsButtonPressed(MouseButton::LEFT)){
        cameraController.SetPaused(!cameraController.IsPaused());
        // Uncomment the code bellow to also pause sprites
        /*
        for(auto &as : animatedSprites){
            if(cameraController.IsPaused()){
                as->player.Pause();
            }else{
                as->player.Resume();
            }
        }
        */
    }


    //---- update animations
    for(auto &as : animatedSprites){

        int flags = as->player.Update();
        if(flags & AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED){
            as->aniInstance->UpdateAnimatedSprites(true);
        }

        // animation runtime feedback via 'Meta track'
        if(as->aniInstance->GetAnimation()->GetName()=="aniRotatorA"){
            if(flags & AnimationPlayerFlags::META_KEY_CHANGED){        // Meta key has changed in the last player.Update() call !
                AKMeta* metaKey = as->player.GetActiveMetaKey();
                if(metaKey){
                    //DbgOutput("aniRotatorA  metaKey label:" + metaKey->label);     // (DbgOutput is windows only)
                }
            }
        }
    }

    cameraController.Update();


}


}

