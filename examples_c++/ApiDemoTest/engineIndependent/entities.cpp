#include <assert.h>
#include "tileGrid.h"
#include "various.h"
#include "entities.h"



// Entities are objects which control sprites and give them game specific behavior.


namespace apiTestDemo{

using namespace jugimap;



bool Entities::Setup(Map *map)
{

    std::vector<Sprite*>CollectedSprites;

    CollectSpritesWithParameter(map, CollectedSprites, "entity");


    for(Sprite* s : CollectedSprites){

        std::string entity = Parameter::Value(s->GetParameters(), "entity");

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
    for(EWalkingFella &wf : EWalkingFella::templateFellas){
        Sprite *s = wf.GetControlledSprite();
        delete s;
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

    std::string animationName = Parameter::Value(sprite->GetParameters(), "frameAnimation", "");
    if(animationName != ""){
        frameAnimation = FindFrameAnimationWithName(sprite, animationName);
    }

    // ... if not choose one animation randomly
    if(frameAnimation==nullptr){
        int ind = iRand(0, sprite->GetSourceSprite()->GetFrameAnimations().size()-1);
        frameAnimation = sprite->GetSourceSprite()->GetFrameAnimations().at(ind);
    }

    //---
    animationPlayer.SetSprite(sprite);

    return true;
}


void ESimpleAnimatedObject::Start()
{
    active = true;
    animationPlayer.Play(frameAnimation);
}


void ESimpleAnimatedObject::Update()
{
    animationPlayer.Update();
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

std::vector<EWalkingFella>EWalkingFella::templateFellas;
std::vector<EWalkingFella*>EWalkingFella::deactivatedFellas;

bool EWalkingFella::Init(Sprite *_sprite)
{

    sprite = dynamic_cast<StandardSprite*>(_sprite);
    if(sprite == nullptr) return false;


    //--- data
    msWaitTime = 400;
    speed = 80.0;
    faStandLeft = FindFrameAnimationWithName(sprite, "Stand Left");
    faStandRight = FindFrameAnimationWithName(sprite, "Stand Right");
    faWalkLeft = FindFrameAnimationWithName(sprite, "Walk Left");
    faWalkRight = FindFrameAnimationWithName(sprite, "Walk Right");
    assert(faStandLeft && faStandRight && faWalkLeft && faWalkRight);
    if(!(faStandLeft && faStandRight && faWalkLeft && faWalkRight)) return false;



    //--- start states
    std::string dir = Parameter::Value(sprite->GetParameters(), "startDir");
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

    frameAnimationPlayer.SetSprite(sprite);

    direction = startDirection;
    state = startState;
    if(direction==dirRIGHT){
        frameAnimationPlayer.Play(faWalkRight);
    }else{
        frameAnimationPlayer.Play(faWalkLeft);
    }

}


void EWalkingFella::Update()
{

    if(state==stateSTAND){

        //int msPassed = glob::millisecondsPassed - msTimerStart;
        int msPassed = jugimap::time.GetPassedNetTimeMS() - msTimerStart;
        if(msPassed >= msWaitTime){
            if(direction==dirLEFT){
                direction = dirRIGHT;
                frameAnimationPlayer.Play(faWalkRight);

            }else{
                direction = dirLEFT;
                frameAnimationPlayer.Play(faWalkLeft);
            }
            state = stateWALK;
        }


    }else if(state==stateWALK){

        //DbgOutput("Fella flipX: "+ std::to_string(sprite->GetGlobalFlip().x));

        // the sprite uses probe points to detect blocking objects and slopes
        VectorShape *probeAheadV = FindProbeShapeWithProperties(sprite, 2, false, ShapeKind::SINGLE_POINT);              // dataflags = 2
        VectorShape *probeAheadDownV = FindProbeShapeWithProperties(sprite, 3, false, ShapeKind::SINGLE_POINT);          // dataflags = 3
        VectorShape *probeRayCastingPointV = FindProbeShapeWithProperties(sprite, 1, false, ShapeKind::SINGLE_POINT);     // dataflags = 1

        // these points are the same for all sprite frames (as defined in editor) so they could be initialized as class memebers

        assert(probeRayCastingPointV && probeAheadV && probeAheadDownV);
        if(probeRayCastingPointV==nullptr || probeAheadV==nullptr || probeAheadDownV==nullptr) return;


        SinglePointShape *probeAhead = static_cast<SinglePointShape*>(probeAheadV->GetGeometricShape());
        SinglePointShape *probeAheadDown = static_cast<SinglePointShape*>(probeAheadDownV->GetGeometricShape());
        SinglePointShape *probeRayCastingPoint = static_cast<SinglePointShape*>(probeRayCastingPointV->GetGeometricShape());


        // probe points are relative to handle point of the sprite active image; we need their world positions
        AffineMat3f m = MakeTransformationMatrix(sprite->GetFullPosition(), sprite->GetScale(), sprite->GetFlip(), sprite->GetRotation(), Vec2f());

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
                frameAnimationPlayer.Play(faStandLeft);
            }else{
                frameAnimationPlayer.Play(faStandRight);
            }
            //msTimerStart = glob::millisecondsPassed;            // when the way is blocked lets wait a bit before turning around
            msTimerStart = jugimap::time.GetPassedNetTimeMS();            // when the way is blocked lets wait a bit before turning around
            state = stateSTAND;

        }else{

            frameAnimationPlayer.Update();

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

}


void EWalkingFella::SetPaused(bool _paused)
{
    if(_paused){
        frameAnimationPlayer.Pause();
    }else{
        frameAnimationPlayer.Resume();
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


void EWalkingFella::CopyToTemplates()
{
    templateFellas.push_back(EWalkingFella());
    EWalkingFella &wf = templateFellas.back();

    wf = *this;
    wf.sprite = static_cast<jugimap::StandardSprite*>(sprite->MakePassiveCopy());
}



void EWalkingFella::Spawn()
{

    EWalkingFella *wf = nullptr;

    if(deactivatedFellas.empty()==false){

        // Re-use existing deactivated WalkingFella !

        wf = deactivatedFellas.back();
        deactivatedFellas.pop_back();

        StandardSprite *sStored = wf->sprite;
        *wf = *this;                    // spawned entity will have the same properties
        wf->sprite = sStored;           // ! must restore original sprite



        wf->sprite->CopyProperties(sprite, Sprite::Property::TRANSFORMATION |  Sprite::Property::APPEARANCE);
        wf->sprite->SetVisible(true);
        wf->Start();

    }else{

        // Create new WalkingFella and its sprite

        wf = new EWalkingFella(*this);
        entities.AddEntity(wf);
        wf->sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());

        wf->Start();
    }

}


//================================================================================================================



ERollingStone::~ERollingStone()
{
    // empty
}


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

    tweenStoneRotation.Init(0.0, 360.0, 2, Easing::LINEAR);

    //--- start states
    std::string dir = Parameter::Value(sprite->GetParameters(), "startDir");
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
    // Rotation is handled manually via looping tween - this will be replaced by the propeties animation once it get added to the editor.
    float pRotate = tweenStoneRotation.GetValue();
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
    std::string pathId = Parameter::Value(sprite->GetParameters(), "pathId");
    if(pathId=="") return false;


    movementPath = FindVectorShapeWithParameter(sprite->GetLayer()->GetMap(), "id", pathId);
    if(movementPath==nullptr) return false;

    movementPath->CalculatePathLength();
    speed = 300.0;
    tweenPathMovement.Init(0.0, 1.0, movementPath->GetPathLength()/speed, Easing::EASE_IN_OUT);

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

        float p = tweenPathMovement.GetValue();
        sprite->SetPosition(movementPath->GetPathPoint(p));

        //---
        if(tweenPathMovement.IsIdle()){
            state = stateMOVING_BACK;
            tweenPathMovement.Play();
        }


    }else if(state==stateMOVING_BACK){
    //-----------------------------------------------------------------------------------------------

        float p = tweenPathMovement.GetValue();
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


