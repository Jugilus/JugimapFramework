
#include "app.h"
#include "scene.h"




namespace jugiApp{



using namespace jugimap;





void CameraController::Init(WorldMapCamera *_cameraView, jugimap::VectorShape *_cameraPath, bool _ownCameraPath)
{
    camera = _cameraView;
    cameraPath = _cameraPath;
    ownCameraPath = _ownCameraPath;

    //----
    float speed = 200;
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
    std::string backgroundMapFile = "media/maps/background.jmb";
    std::string backgroundSourceGraphicsFile = "media/maps/cfgBackground.sgb";
    std::string testScreenMapFile = "media/maps/testScreenMap.jmb";


    // IMPORTANT: Maps must be added in the correct order as displayed, from the back to the front.

    //--- background
    SourceGraphics* backgroundSourceGraphics = LoadAndAddSourceGraphics(backgroundSourceGraphicsFile);
    if(backgroundSourceGraphics==nullptr){
        return false;
    }

    Map* backgroundMap = LoadAndAddMap(backgroundMapFile, backgroundSourceGraphics, MapType::PARALLAX);
    if(backgroundMap==nullptr){
        return false;
    }


    //--- world
    SourceGraphics* worldSourceGraphics = LoadAndAddSourceGraphics(worldSourceGraphicsFile);
    if(worldSourceGraphics==nullptr){
        return false;
    }

    Map* worldMap = LoadAndAddMap(worldMapFile, worldSourceGraphics, MapType::WORLD);
    if(worldMap==nullptr){
        return false;
    }


    bool includeTestScreenMap = false;          // set to true to include dummy testing screen map

    //---- screen map
    Map* testScreenMap = nullptr;
    if(includeTestScreenMap){
        testScreenMap = LoadAndAddMap(testScreenMapFile, backgroundSourceGraphics, MapType::SCREEN);
        if(testScreenMap==nullptr){
            return false;
        }
    }



    //------------------------------------------------------------------------------
    // INITIALIZE MAPS AND ASSIGN CAMERAS
    //------------------------------------------------------------------------------
    Vec2i screenSize = settings.GetScreenSize();

    worldMap->InitAsWorldMap();
    if(testScreenMap){
        worldCamera.Init(Rectf(200,0, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    }else{
        worldCamera.Init(Rectf(0,0, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    }
    worldMap->SetCamera(&worldCamera);

    //---
    backgroundMap->InitAsParallaxMap(worldMap->GetWorldMapSize());

    // The parallax map has fixed height (no tiling) and y parallax factors set in the editor are valid for design size 1300x800.
    // If screen (viewport) size has different height we need to adjust these factors, otherwise parallax layers are not positioned correctly in y direction.
    backgroundMap->ModifyYParallaxFactorsForFixedMapHeight(Vec2i(1300,800), settings.GetScreenSize());

    //---
    backgroundMap->SetCamera(&worldCamera);             // Parallax map must have the same camera as the world map!


    //---
    if(testScreenMap){
        Vec2i designSize = Vec2i(200, settings.GetScreenSize().y);
        testScreenMap->InitAsScreenMap(designSize);
        screenCamera.Init(designSize, Vec2f(0,0));
        testScreenMap->SetCamera(&screenCamera);
    }

    // Initialize camera controller which moves camera around the map
    cameraController.Init(&worldCamera, MakeWorldCameraOverviewPath_v1(worldCamera), true);


    //------------------------------------------------------------------------------
    // SETUP ANIMATIONS
    //-------------------------------------------------------------------------------
    // There are some animated sprites in 'backgroundMap' (waterfall) and we need to setup their animations.
    std::vector<Sprite*>sprites;
    std::vector<std::string>animatedSourceSpriteNames({"waterAniA", "waterAniB", "waterAniC"});
    for(const std::string &name : animatedSourceSpriteNames){
        sprites.clear();

        //collect animated sprites
        CollectSpritesWithSourceSpriteName(backgroundMap, sprites, name);
        assert(sprites.empty()==false);
        assert(sprites.front()->GetSourceSprite()->GetFrameAnimations().empty()==false);

        animatedSprites.push_back(std::unique_ptr<SpriteAnimation>(new SpriteAnimation()));
        if(sprites.size()==1){
            animatedSprites.back()->aniInstance = new FrameAnimationInstance(sprites.front()->GetSourceSprite()->GetFrameAnimations().front(), sprites.front());
        }else{
            animatedSprites.back()->aniInstance  = new FrameAnimationInstance(sprites.front()->GetSourceSprite()->GetFrameAnimations().front(), sprites);
        }
    }


    //---
    GetEngineScene()->PostInit();


    //---
    SetInitialized(true);

    return true;

}


void DemoScene::Start()
{

    cameraController.Start();

    for(auto &as : animatedSprites){
        if(as->player.Play(as->aniInstance) & AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED){
            as->aniInstance->UpdateAnimatedSprites(true);
        }
    }


}


void DemoScene::Update()
{

    //---- update global time variable
    jugimap::time.UpdatePassedTimeMS();


    //---- pause scene
    if(jugimap::mouse.IsButtonPressed(MouseButton::LEFT)){
        cameraController.SetPaused(!cameraController.IsPaused());
    }


    //---- update animations
    for(auto &as : animatedSprites){
        if(as->player.Update() & AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED){
            as->aniInstance->UpdateAnimatedSprites(true);
        }
    }

    //---- update camera controller
    cameraController.Update();


}


}


