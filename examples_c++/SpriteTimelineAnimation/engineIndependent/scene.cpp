#include <chrono>
#include <assert.h>
#include "scene.h"



using namespace jugimap;





Mouse mouse;



//==============================================================================================


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

    Vec2f min =  _camera.GetViewport().Size()/2.0;
    Vec2f max =  Vec2iToVec2f(_camera.GetWorldSize()) - _camera.GetViewport().Size()/2.0;
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


AnimatedSprite::~AnimatedSprite()
{

    if(aniInstance){
        delete aniInstance;
    }

}


//==============================================================================================


DemoScene::~DemoScene()
{

    if(worldMap) delete worldMap;
    if(worldSourceGraphics) delete worldSourceGraphics;

    for(AnimatedSprite * as : animatedSprites){
        delete as;
    }

}


bool DemoScene::Init()
{


    //------------------------------------------------------------------------------
    // LOAD SOURCE GRAPHICS AND MAPS
    //------------------------------------------------------------------------------

    std::string worldMapFile = "media/maps/world.jmb";
    std::string worldSourceGraphicsFile = "media/maps/cfgWorld.sgb";


    //---- world map
    worldSourceGraphics = new SourceGraphics();
    if(JugiMapBinaryLoader::LoadSourceGraphics(worldSourceGraphicsFile, worldSourceGraphics)==false){
        return false;
    }

    worldMap = objectFactory->NewMap();
    worldMap->_SetZOrderStart(zOrderWorldMap);
    //JugiMapBinaryLoader::zOrderCounterStart = zOrderWorldMap;
    if(JugiMapBinaryLoader::LoadMap(worldMapFile, worldMap, worldSourceGraphics)==false){
        return false;
    }


    //------------------------------------------------------------------------------
    // INITIALIZE MAPS
    //------------------------------------------------------------------------------

    // Initialize maps according to their kind - make sure to call correct Init function!
    // First initialize world map as its size is needed for parallax maps!
    worldMap->InitWorldMap();

    // Initialize engine objects - creates engine objects (textures, sprites...) and get ready everything for action
    worldMap->InitEngineMap();


    //------------------------------------------------------------------------------
    // INITIALIZE CAMERAS
    //------------------------------------------------------------------------------
    Vec2i screenSize = settings.GetScreenSize();
    worldCamera.Init(Rectf(0,0, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    worldMap->SetCamera(&worldCamera);


    // Initialize camera controller which moves camera around the map
    cameraController.Init(&worldCamera, MakeWorldCameraOverviewPath_v1(worldCamera), true);


    //------------------------------------------------------------------------------
    // PREPARE ANIMATIONS
    //-------------------------------------------------------------------------------

    // We use simple struct 'AnimatedSprite' to group animated sprites with two object required for animation playback: 'AnimationInstance' and 'AnimationPlayer'
    std::vector<Sprite*>sprites;
    CollectSpritesWithParameter(worldMap, sprites, "animation");

    for(Sprite *s: sprites){
        if(s->GetName()=="testbat"){
            DummyFunction();
        }
        jugimap::TimelineAnimation *ta = FindTimelineAnimationWithName(static_cast<StandardSprite*>(s), Parameter::Value(s->GetParameters(), "animation"));
        if(ta){
            AnimatedSprite *as = new AnimatedSprite();
            as->sprite = s;
            as->aniInstance = new jugimap::TimelineAnimationInstance(ta, s);

            animatedSprites.push_back(as);

            if(s->GetName()=="testbat"){
                as->aniInstance->GetBaseParameters().loopForever = false;
            }

            //---- if a sprite has defined "aniDelay" parameter we use it to delay animation
            if(Parameter::Exists(as->sprite->GetParameters(), "animDelay")){
                float animDelay = Parameter::FloatValue(as->sprite->GetParameters(), "animDelay");
                int msStartDelay = animDelay * as->aniInstance->GetAnimation()->GetDuration();       // aniDelay = 1.0 -> animation is delayed for one duration (loop) period
                as->aniInstance->GetBaseParameters().startDelay = msStartDelay;
            }


            //--- There is sprite named 'sprity' which move along a closed path. We make few more sprities and animate them.
            if(s->GetName()=="sprity"){

                for(int i=1; i<=3; i++){
                    Sprite * sExtra = s->MakeActiveCopy();              // create a copy of sprity
                    AnimatedSprite *asExtra = new AnimatedSprite();
                    asExtra->sprite = sExtra;
                    asExtra->aniInstance = new jugimap::TimelineAnimationInstance(ta, sExtra);

                    // We can set custom start play time offset for animation playback.
                    // This will place sprites at their "in animation" position  when animation start.
                    asExtra->aniInstance->SetStartPlayTimeOffset(ta->GetDuration() * i/20.0);    // lets distribute them along the movement path close enough to each other

                    animatedSprites.push_back(asExtra);
                }
            }
        }
    }


    SetInitialized(true);

    return true;
}


void DemoScene::Start()
{

    cameraController.Start();

    for(AnimatedSprite *as : animatedSprites){
        as->player.Play(as->aniInstance);
    }

}


void DemoScene::Update()
{


    //---- update global time variable
    jugimap::time.UpdatePassedTimeMS();


    //---- scene logic
    if(mouse.IsHit()){
        cameraController.SetPaused(!cameraController.IsPaused());


        // Uncomment the code bellow to also pause sprites
        /*
        for(AnimatedSprite *as : animatedSprites){
            if(cameraController.IsPaused()){
                as->player.Pause();
            }else{
                as->player.Resume();
            }
        }
        */
    }


    //---- update animations
    for(AnimatedSprite *as : animatedSprites){

        if(as->sprite->GetName()=="testbat"){
            DummyFunction();
        }

        int flags = as->player.Update();
        if(flags & AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED){
            as->aniInstance->UpdateAnimatedSprites(true);
        }

        // animation runtime feedback via 'Meta track'
        if(as->aniInstance->GetAnimation()->GetName()=="aniRotatorA"){
            if(flags & AnimationPlayerFlags::META_KEY_CHANGED){        // Meta key has changed in the last player.Update() call !
                AKMeta* metaKey = as->player.GetActiveMetaKey();
                if(metaKey){
                    DbgOutput("aniRotatorA  metaKey label:" + metaKey->label);     // (DbgOutput is windows only)
                }
            }
        }
    }

    cameraController.Update();

    //---- update engine sprites
    UpdateEngineObjects();
}



void DemoScene::UpdateEngineObjects()
{

    worldMap->UpdateEngineMap();

}








