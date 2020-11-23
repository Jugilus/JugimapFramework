#include <assert.h>
#include "tileGrid.h"
#include "various.h"
#include "entities.h"



// Entities are objects which control sprites and give them game specific behavior.


namespace jugiApp{

using namespace jugimap;



bool Entities::Setup(Map *map)
{

    std::vector<Sprite*>CollectedSprites;

    CollectSpritesWithParameter(map, CollectedSprites, "entity");


    for(Sprite* s : CollectedSprites){

        std::string entity = Parameter::GetValue(s->GetParameters(), "entity");

        Entity *e = nullptr;

        if(entity=="simpleAnimatedObject"){
            e = new ESimpleAnimatedObject();

        }else if(entity=="walkingFella"){
            e = new EWalkingFella();

        }else if(entity=="rollingStone"){
            e = new ERollingStone();

        }else if(entity=="movingPlatform"){
            e = new EMovingPlatform();

        }else{
            continue;
        }

        //---
        if(e->Init(s)==false){
            //glob::errorMessage = "SetupEntities error! Failed to initialize '"+entity+"' for sprite:" + s->GetSourceSprite()->GetName();
            settings.SetErrorMessage("SetupEntities error! Failed to initialize '"+entity+"' for sprite:" + s->GetSourceSprite()->GetName());
            return false;
        }
        list.push_back(e);

    }


    // Create entity templates from all WalkingFellas (for testing spawning).
    for(Entity *e : list){
        EWalkingFella* wf = dynamic_cast<EWalkingFella*>(e);
        if(wf){
            wf->CopyToTemplates();
        }
    }

    return true;
}


void Entities::Start()
{
    for(Entity *e : list){
        e->Start();
    }
}


void Entities::Update()
{
    if(paused) return;

    for(Entity *e : list){
        if(e->IsActive()==false) continue;
        e->Update();
    }
}


void Entities::SetPaused(bool _paused)
{
    paused = _paused;

    for(Entity *e : list){
        e->SetPaused(paused);
    }
}


void Entities::Delete()
{
    for(Entity *e : list){
        delete e;
    }
    list.clear();


    //--- template entities
    for(EWalkingFella *wf : EWalkingFella::templateFellas){
        Sprite *s = wf->GetControlledSprite();          // sprites of templates are not stored in a map so we need to delete them manually
        s->GetLayer()->RemoveAndDeleteSprite(s);
        delete wf;
    }
}


void Entities::DeleteEntity(Entity *e)
{

    int j = 0;
    for(int i=0; i<list.size(); i++) {
      if(list[i] != e){
          list[j++] = list[i];
      }else{
          list[i]->OnDeleteFromScene();
          delete list[i];
      }
    }
    list.resize(j);
}


Entities entities;


//=====================================================================================


bool ESimpleAnimatedObject::Init(Sprite *_sprite)
{

    sprite = dynamic_cast<StandardSprite*>(_sprite);

    if(sprite == nullptr) return false;

    if(sprite->GetSourceSprite()->GetFrameAnimations().empty()) return false;

    jugimap::FrameAnimation * frameAnimation = nullptr;
    std::string animationName = Parameter::GetValue(sprite->GetParameters(), "frameAnimation", "");
    if(animationName != ""){
        frameAnimation = FindFrameAnimationWithName(sprite, animationName);
    }

    // ... if not choose one animation randomly
    if(frameAnimation ==nullptr){
        int ind = iRand(0, sprite->GetSourceSprite()->GetFrameAnimations().size()-1);
        frameAnimation = sprite->GetSourceSprite()->GetFrameAnimations().at(ind);
    }

    if(frameAnimation){
        aniDefault.reset(new FrameAnimationInstance(frameAnimation, sprite));
    }


    return true;
}


void ESimpleAnimatedObject::Start()
{
    active = true;

    if(aniDefault.get()){
        if(sprite->GetSourceSprite()->GetName()=="Shine"){      // a little hack to start with the sprite 'hidden' -> will need to add an option in the editor to set the starting frame (or none) of a sprite
            sprite->SetActiveImage(nullptr);
        }
        animationPlayer.Play(aniDefault.get());
    }

}


void ESimpleAnimatedObject::Update()
{

    if(animationPlayer.Update() & AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED){
        aniDefault->UpdateAnimatedSprites(true);
    }
}


void ESimpleAnimatedObject::SetPaused(bool _paused)
{

    if(_paused){
        animationPlayer.Pause();
    }else{
        animationPlayer.Resume();
    }

}


//==================================================================================================

std::vector<EWalkingFella*>EWalkingFella::templateFellas;
std::vector<EWalkingFella*>EWalkingFella::deactivatedFellas;



bool EWalkingFella::Init(Sprite *_sprite)
{

    sprite = dynamic_cast<StandardSprite*>(_sprite);
    if(sprite == nullptr) return false;


    //--- data
    msWaitTime = 400;
    speed = 80.0;
    jugimap::FrameAnimation * faStandLeft = FindFrameAnimationWithName(sprite, "Stand Left");
    jugimap::FrameAnimation * faStandRight = FindFrameAnimationWithName(sprite, "Stand Right");
    jugimap::FrameAnimation * faWalkLeft = FindFrameAnimationWithName(sprite, "Walk Left");
    jugimap::FrameAnimation * faWalkRight = FindFrameAnimationWithName(sprite, "Walk Right");

    assert(faStandLeft && faStandRight && faWalkLeft && faWalkRight);

    if(!(faStandLeft && faStandRight && faWalkLeft && faWalkRight)) return false;

    aniStandLeft.reset(new FrameAnimationInstance(faStandLeft, sprite));
    aniStandRight.reset(new FrameAnimationInstance(faStandRight, sprite));
    aniWalkLeft.reset(new FrameAnimationInstance(faWalkLeft, sprite));
    aniWalkRight.reset(new FrameAnimationInstance(faWalkRight, sprite));


    //--- start states
    std::string dir = Parameter::GetValue(sprite->GetParameters(), "startDir");
    if(dir=="right"){
        startDirection = dirRIGHT;
    }else{
        startDirection = dirLEFT;
    }
    startState = stateWALK;

    return true;
}


void EWalkingFella::Start()
{
    active = true;


    direction = startDirection;
    state = startState;

    if(direction==dirRIGHT){
        aniActive = aniWalkRight.get();
    }else{
        aniActive = aniWalkLeft.get();

    }

    if(animationPlayer.Play(aniActive) & AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED){
        aniActive->UpdateAnimatedSprites(true);
    }

}


void EWalkingFella::Update()
{

    if(state==stateSTAND){

        int msPassed = jugimap::time.GetPassedNetTimeMS() - msTimerStart;
        if(msPassed >= msWaitTime){
            if(direction==dirLEFT){
                direction = dirRIGHT;
                aniActive = aniWalkRight.get();

            }else{
                direction = dirLEFT;
                aniActive = aniWalkLeft.get();
            }
            state = stateWALK;
        }


    }else if(state==stateWALK){

        //DbgOutput("Fella flipX: "+ std::to_string(sprite->GetGlobalFlip().x));

        // the sprite uses probe points to detect blocking objects and slopes
        VectorShape *probeAheadV = FindSpriteExtraShapeWithProperties(sprite->GetActiveImage(), 2, false, ShapeKind::SINGLE_POINT);              // dataflags = 2
        VectorShape *probeAheadDownV = FindSpriteExtraShapeWithProperties(sprite->GetActiveImage(), 3, false, ShapeKind::SINGLE_POINT);          // dataflags = 3

        //VectorShape *probeAheadDownV = FindProbeShapeWithProperties(sprite, 3, false, ShapeKind::SINGLE_POINT);          // dataflags = 3
        VectorShape *probeRayCastingPointV = FindSpriteExtraShapeWithProperties(sprite->GetActiveImage(), 1, false, ShapeKind::SINGLE_POINT);     // dataflags = 1

        // these points are the same for all sprite frames (as defined in editor) so they could be initialized as class memebers

        assert(probeRayCastingPointV && probeAheadV && probeAheadDownV);
        if(probeRayCastingPointV==nullptr || probeAheadV==nullptr || probeAheadDownV==nullptr) return;


        SinglePointShape *probeAhead = static_cast<SinglePointShape*>(probeAheadV->GetGeometricShape());
        SinglePointShape *probeAheadDown = static_cast<SinglePointShape*>(probeAheadDownV->GetGeometricShape());
        SinglePointShape *probeRayCastingPoint = static_cast<SinglePointShape*>(probeRayCastingPointV->GetGeometricShape());


        // probe points are relative to handle point of the sprite active image; we need their world positions
        AffineMat3f m = MakeTransformationMatrix(sprite->GetPosition(), sprite->GetScale(), sprite->GetFlip(), sprite->GetRotation(), Vec2f());

        //---- check if direction is blocked by 'wall'
        Vec2f pointAhead = m.Transform(probeAhead->point.x, probeAhead->point.y);
        bool movementBlocked = collidersTileGrid.PointInTile(pointAhead);

        //---- check if direction is blocked by 'hole'
        if(movementBlocked==false){
            Vec2f pointAheadDown = m.Transform(probeAheadDown->point.x, probeAheadDown->point.y);
            movementBlocked = !collidersTileGrid.PointInTile(pointAheadDown);
        }

        if(movementBlocked){
            if(direction==dirLEFT){
                aniActive = aniStandLeft.get();
            }else{
                aniActive = aniStandRight.get();
            }
            msTimerStart = jugimap::time.GetPassedNetTimeMS();            // when the way is blocked lets wait a bit before turning around
            state = stateSTAND;

        }else{

            //---
            // Sprite moves over sloping grounds so we need to obtain a ground point ahead of sprite;
            // This point is obtained as intersection between the ground collision shape and 'vertical ray' casted downward

            Vec2f rayCastingPoint = m.Transform(probeRayCastingPoint->point.x, probeRayCastingPoint->point.y);
            Vec2f pointGround(rayCastingPoint.x, sprite->GetPosition().y);       // default point is at the same y position as sprite

            for(int i=0; i<2; i++){      // the ground point can be found in one of two tiles according to map design

                // to make this code engine independent we distinguish beetwen y coordinate up and down
                if(settings.IsYCoordinateUp()){

                    Sprite *s = collidersTileGrid.GetSprite(Vec2f(rayCastingPoint.x, rayCastingPoint.y-i*collidersTileGrid.GetTileSize().y));
                    if(s && s->RaycastHit(rayCastingPoint, rayCastingPoint-Vec2f(0.0, collidersTileGrid.GetTileSize().y*2), pointGround)){
                        break;
                    }

                }else{
                    Sprite *s = collidersTileGrid.GetSprite(Vec2f(rayCastingPoint.x, rayCastingPoint.y+i*collidersTileGrid.GetTileSize().y));
                    if(s && s->RaycastHit(rayCastingPoint, rayCastingPoint+Vec2f(0.0, collidersTileGrid.GetTileSize().y*2), pointGround)){
                        break;
                    }
                }
            }

           //DbgOutput("Direction:"+std::to_string(direction)+"  Sprite x: "+std::to_string(sprite->GetPos().x) +" y:"+std::to_string(sprite->GetPos().y)+
           //           "   pointAheadGround x:"+std::to_string(pointGround.x)+" y:"+std::to_string(pointGround.y));



            float moveDistance = speed *  jugimap::time.GetLogicTimeMS()/1000.0;   // * agk::GetFrameTime();
            Vec2f posNew = MakeVec2fAtDistanceFromP1(sprite->GetPosition(), pointGround, moveDistance);

            //DbgOutput("moveDistance: "+std::to_string(moveDistance) +"  posNew x:"+std::to_string(posNew.x)+" y:"+std::to_string(posNew.y));

            sprite->SetPosition(posNew);
        }
    }


    //---- manage animation player

    int aniPlayerFlags = 0;
    if(animationPlayer.GetAnimationInstance()!=aniActive){
        aniPlayerFlags = animationPlayer.Play(aniActive);
     }else{
        aniPlayerFlags = animationPlayer.Update();
    }

    if(aniPlayerFlags & AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED){
        aniActive->UpdateAnimatedSprites(true);
    }

}


void EWalkingFella::SetPaused(bool _paused)
{
    if(_paused){
        animationPlayer.Pause();
    }else{
        animationPlayer.Resume();
    }
}


void EWalkingFella::Deactivate()
{
    active = false;
    sprite->SetVisible(false);

    deactivatedFellas.push_back(this);
}


void EWalkingFella::OnDeleteFromScene()
{
    if(active){
        sprite->GetLayer()->RemoveAndDeleteSprite(sprite);
        sprite = nullptr;
    }
}


void EWalkingFella::CopyPropertiesFrom(EWalkingFella* _src)
{

    speed = _src->speed;
    msTimerStart = _src->msTimerStart;
    msWaitTime = _src->msWaitTime;
    state = _src->state;
    direction = _src->direction;
    startState = _src->startState;
    startDirection = _src->startDirection;
}


void EWalkingFella::CopyToTemplates()
{

    EWalkingFella *wf = new EWalkingFella();
    wf->CopyPropertiesFrom(this);
    wf->sprite = static_cast<jugimap::StandardSprite*>(sprite->MakePassiveCopy());      // copy sprite as passive copy

    templateFellas.push_back(wf);
}


void EWalkingFella::Spawn()
{

    EWalkingFella *wf = nullptr;


    if(deactivatedFellas.empty()==false){

        // Re-use existing deactivated WalkingFella !

        wf = deactivatedFellas.back();
        deactivatedFellas.pop_back();

        wf->CopyPropertiesFrom(this);     // spawned entity will have the same properties
        wf->sprite->CopyProperties(sprite, Sprite::Property::TRANSFORMATION |  Sprite::Property::APPEARANCE);
        wf->sprite->SetVisible(true);
        wf->Start();

    }else{

        // Create new WalkingFella and its sprite

        wf = new EWalkingFella();
        wf->Init(static_cast<StandardSprite*>(sprite->MakeActiveCopy()));
        wf->CopyPropertiesFrom(this);                                       // spawned entity will have the same properties
        wf->Start();

        entities.AddEntity(wf);
    }


}


//================================================================================================================



bool ERollingStone::Init(Sprite *_sprite)
{
    sprite = dynamic_cast<StandardSprite *>(_sprite);
    if(sprite == nullptr) return false;


    //--- data
    speed = 150.0;
    std::vector<VectorShape*>shapes = sprite->GetActiveImage()->GetSpriteShapes();
    if(shapes.size()!=1) return false;
    if(dynamic_cast<EllipseShape*>(shapes.front()->GetGeometricShape())==nullptr) return false;
    radius = static_cast<EllipseShape*>(shapes.front()->GetGeometricShape())->a;

    tweenStoneRotation.Init(0.0, 360.0, 2, Easing::Kind::LINEAR);

    //--- start states
    std::string dir = Parameter::GetValue(sprite->GetParameters(), "startDir");
    if(dir=="right"){
        startDirection = dirRIGHT;
    }else{
        startDirection = dirLEFT;
    }

    return true;
}


void ERollingStone::Start()
{
    active = true;
    direction = startDirection;
    tweenStoneRotation.Play();
}


void ERollingStone::Update()
{

    // sprite uses probe points to detect blocking objects and slopes

    // ROTATION
    //------------------------
    float pRotate = tweenStoneRotation.Update();
    if(direction==dirRIGHT) pRotate = -pRotate;
    if(settings.IsYCoordinateUp()){
        pRotate = -pRotate;
    }

    sprite->SetRotation(pRotate);

    if(tweenStoneRotation.IsIdle()){
        tweenStoneRotation.Play();
    }


    // MOVEMENT
    //------------------------
    // Direction blocking and moving along the slopes is handled similar as in the WalkingFella.
    // The diference is that here we obtain probe points from the geometry - circle with the handle point in the center!

    Vec2f pointAhead(sprite->GetPosition().x + radius, sprite->GetPosition().y);
    if(direction==dirLEFT){
        pointAhead = Vec2f(sprite->GetPosition().x - radius, sprite->GetPosition().y);
    }

    bool movementBlocked = collidersTileGrid.PointInTile(pointAhead);
    if(movementBlocked){

        direction = (direction==dirRIGHT)? dirLEFT : dirRIGHT;

    }else{

        Vec2f pointGround(pointAhead.x, sprite->GetPosition().y+radius);       // default pos (Sprite handle point is in the circle centre)
        if(settings.IsYCoordinateUp()){
             pointGround.y = sprite->GetPosition().y-radius;
        }

        for(int i=0; i<2; i++)      // the ground point can be found in one of two tiles
        {

            if(settings.IsYCoordinateUp()){

                Sprite  *s = collidersTileGrid.GetSprite(Vec2f(pointAhead.x, pointAhead.y-i*collidersTileGrid.GetTileSize().y));
                if(s && s->RaycastHit(pointAhead, pointAhead-Vec2f(0.0, collidersTileGrid.GetTileSize().y*2), pointGround)){
                    break;
                }

            }else{

                Sprite  *s = collidersTileGrid.GetSprite(Vec2f(pointAhead.x, pointAhead.y+i*collidersTileGrid.GetTileSize().y));
                if(s && s->RaycastHit(pointAhead, pointAhead+Vec2f(0.0, collidersTileGrid.GetTileSize().y*2), pointGround)){
                    break;
                }
            }
        }
        Vec2f pointCircleCenter(pointGround.x, pointGround.y-radius);
        if(settings.IsYCoordinateUp()){
             pointCircleCenter.y = pointGround.y+radius;
        }
        float moveDistance = speed * jugimap::time.GetLogicTimeMS()/1000.0;
        Vec2f posNew = MakeVec2fAtDistanceFromP1(sprite->GetPosition(), pointCircleCenter, moveDistance);

        //DbgOutput("moveDistance: "+std::to_string(moveDistance) +"  posNew x:"+std::to_string(posNew.x)+" y:"+std::to_string(posNew.y));

        sprite->SetPosition(posNew);
    }
}



void ERollingStone::SetPaused(bool _paused)
{
    if(_paused){
        tweenStoneRotation.Pause();
    }else{
        tweenStoneRotation.Resume();
    }
}


void ERollingStone::OnDeleteFromScene()
{
    if(active){
        sprite->GetLayer()->RemoveAndDeleteSprite(sprite);
        sprite = nullptr;
    }
}

//================================================================================================================


EMovingPlatform::~EMovingPlatform()
{
   // empty

}


bool EMovingPlatform::Init(Sprite *_sprite)
{

    sprite = dynamic_cast<StandardSprite*>(_sprite);
    if(sprite == nullptr) return false;


    //---- movement path
    std::string pathId = Parameter::GetValue(sprite->GetParameters(), "pathId");
    if(pathId=="") return false;


    movementPath = FindVectorShapeWithParameter(sprite->GetLayer()->GetMap(), "id", pathId);
    if(movementPath==nullptr) return false;

    speed = 300.0;
    tweenPathMovement.Init(0.0, 1.0, movementPath->GetPathLength()/speed, Easing::Kind::EASE_START_END);

    //----
    startState = stateMOVING_FORWARD;

    return true;
}


void EMovingPlatform::Start()
{
    active = true;
    state = startState;
    sprite->SetPosition(movementPath->GetPathPoint(0.0));
    tweenPathMovement.Play();

}


void EMovingPlatform::Update()
{

    if(state==stateMOVING_FORWARD){
    //-----------------------------------------------------------------------------------------------

        float p = tweenPathMovement.Update();
        sprite->SetPosition(movementPath->GetPathPoint(p));

        //---
        if(tweenPathMovement.IsIdle()){
            state = stateMOVING_BACK;
            tweenPathMovement.Play();
        }


    }else if(state==stateMOVING_BACK){
    //-----------------------------------------------------------------------------------------------

        float p = tweenPathMovement.Update();
        sprite->SetPosition(movementPath->GetPathPoint(1.0-p));

        //---
        if(tweenPathMovement.IsIdle()){
            state = stateMOVING_FORWARD;
            tweenPathMovement.Play();
        }
    }

}


void EMovingPlatform::SetPaused(bool _paused)
{
    if(_paused){
        tweenPathMovement.Pause();
    }else{
        tweenPathMovement.Resume();
    }
}


void EMovingPlatform::OnDeleteFromScene()
{
    if(active){
        sprite->GetLayer()->RemoveAndDeleteSprite(sprite);
        sprite = nullptr;
    }
}


}


