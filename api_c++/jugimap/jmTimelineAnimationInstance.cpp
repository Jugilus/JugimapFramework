#include "jmSourceGraphics.h"
#include "jmLayers.h"
#include "jmSprites.h"
#include "jmVectorShapes.h"
#include "jmFrameAnimation.h"
#include "jmUtilities.h"
#include "jmTimelineAnimationInstance.h"



namespace jugimap{




AnimationTrackState::AnimationTrackState(AnimationTrack* _animation, AnimationMemberState *_tAnimationsStatesSet)
{

    animationTrack = _animation;
    tp = animationTrack->GetTrackParameters();
    animationMemberState = _tAnimationsStatesSet;
    kind = animationTrack->GetKind();
    stateKey = CreateAnimationKey(animationTrack);
    disabled = animationTrack->IsDisabled();    // initial disabled

}


AnimationTrackState::~AnimationTrackState()
{

    if(stateKey){
        delete stateKey;
        stateKey = nullptr;
    }

    if(frameAnimationPlayer){
        delete frameAnimationPlayer;
        frameAnimationPlayer = nullptr;
    }
    if(frameAnimationInstances){
        for(FrameAnimationInstance* fai : *frameAnimationInstances){
            delete fai;
        }
        delete frameAnimationInstances;
    }

    if(timelineAnimationPlayer){
        delete timelineAnimationPlayer;
        timelineAnimationPlayer = nullptr;
    }
    if(timelineAnimationInstances){
        for(TimelineAnimationInstance* tai : *timelineAnimationInstances){
            delete tai;
        }
        delete timelineAnimationInstances;
    }


}


AnimationTrackKind AnimationTrackState::GetKind()
{
    assert(animationTrack);

    return animationTrack->GetKind();
}


int AnimationTrackState::Update(int _animationTime, int _flags)
{

    int returnFlag = 0;

    assert(stateKey);

    //if(animationTrack->IsDisabled()){
    if(disabled){
        if(frameAnimationPlayer && frameAnimationPlayer->GetState()!=AnimationPlayerState::IDLE){
            AnimationInstance *ai = frameAnimationPlayer->GetAnimationInstance();   assert(ai);
            ai->ResetAnimatedProperties();
            frameAnimationPlayer->Stop();

        }else if(timelineAnimationPlayer && timelineAnimationPlayer->GetState()!=AnimationPlayerState::IDLE){
            AnimationInstance *ai = timelineAnimationPlayer->GetAnimationInstance();   assert(ai);
            ai->ResetAnimatedProperties();
            timelineAnimationPlayer->Stop();

        }
        return returnFlag;
    }

    SetActiveKeys(_animationTime);


    if(activeStartKey && activeEndKey==nullptr){
        stateKey->CopyFrom(activeStartKey);

    }else if(activeStartKey==nullptr && activeEndKey){
        stateKey->CopyFrom(activeEndKey);

    }else if(activeStartKey && activeEndKey){

        double p = (_animationTime- activeStartKey->GetTime())/double(activeEndKey->GetTime() - activeStartKey->GetTime());
        p = activeStartKey->GetEasing().GetValue(p);

        stateKey->CopyFrom(activeStartKey, activeEndKey, p);

    }else{
        return returnFlag;
    }


    if(animationMemberState==nullptr) return returnFlag;      // kind==KeyAnimationKind::CONTROLLER



    if (kind==AnimationTrackKind::PATH_MOVEMENT){

        if(path && path->GetName()!=tp.pathNameID){
            path = nullptr;
        }
        if(path==nullptr && tp.pathNameID != ""){
            //path = CurrentMap->FindVectorShapeOfNameID(dpCurrent.pathNameID);

            Map *map = nullptr;
            if(animationMemberState->sprite){
                map = animationMemberState->sprite->GetLayer()->GetMap();

            }else if(animationMemberState->sprites){
                map = animationMemberState->sprites->front()->GetLayer()->GetMap();
            }
            assert(map);

            path = FindVectorShapeWithName(map, tp.pathNameID);
        }


    }else if (kind==AnimationTrackKind::FRAME_CHANGE){

        if(sourceObject==nullptr){
            FindSourceObject();
        }

    }else if (kind==AnimationTrackKind::FRAME_ANIMATION){

        if(sourceObject==nullptr){
            FindSourceObject();
        }

        if(_flags & AnimationPlayerFlags::SKIP_SUBPLAYER_UPDATING){
            if(frameAnimationPlayer &&  frameAnimationPlayer->GetAnimationInstance()){
                frameAnimationPlayer->GetAnimationInstance()->Update(0, _flags);
            }

        }else{
            returnFlag |= ManageSubFrameAnimation();
        }

    }else if (kind==AnimationTrackKind::TIMELINE_ANIMATION){

        if(sourceObject==nullptr){
            FindSourceObject();
        }

        if(_flags & AnimationPlayerFlags::SKIP_SUBPLAYER_UPDATING){

            if(timelineAnimationPlayer &&  timelineAnimationPlayer->GetAnimationInstance()){
                timelineAnimationPlayer->GetAnimationInstance()->Update(0, _flags);

                //timelineAnimationPlayer->GetMainAnimationInstance()->Update(0);
            }

        }else{

            returnFlag |= ManageSubTimelineAnimation();
        }

    }else if (kind==AnimationTrackKind::META){

        // empty
    }


    return returnFlag;

}


void AnimationTrackState::SetActiveKeys(int _animationTime)
{

    std::vector<AnimationKey*> &keys = animationTrack->GetAnimationKeys();

    activeStartKey = nullptr;
    activeEndKey = nullptr;

    if(keys.empty()) return;


    if(keys.size()==1){
        activeStartKey = keys[0];
        activeEndKey = nullptr;

    }else{

        if(_animationTime <= keys[0]->GetTime()){
            activeStartKey = keys[0];
            activeEndKey = nullptr;

        }else if(_animationTime >= keys[keys.size()-1]->GetTime()){
            activeStartKey = nullptr;
            activeEndKey = keys[keys.size()-1];

        }else{

            for(int i=0; i<keys.size()-1; i++){
                if(_animationTime >= keys[i]->GetTime()  && _animationTime <= keys[i+1]->GetTime()){
                    activeStartKey = keys[i];
                    activeEndKey = keys[i+1];
                }
            }
        }
    }
}


int AnimationTrackState::ManageSubTimelineAnimation()
{

    int returnFlag = 0;

    if(timelineAnimationPlayer==nullptr){
        timelineAnimationPlayer = new AnimationQueuePlayer();
         FindSourceObject();

         //---- create animation instances
         timelineAnimationInstances = new std::vector<TimelineAnimationInstance*>();
         for(AnimationKey * tak : animationTrack->GetAnimationKeys()){
             AKTimelineAnimation * ta = static_cast<AKTimelineAnimation*>(tak);

             if(ta->animationName!=""){

                 if(FindTimelineAnimationInstance(ta->animationName)==nullptr){
                    CreateAndStoreTimelineAnimationInstance(ta->animationName);
                 }
             }
         }
    }


    AKTimelineAnimation * k = static_cast<AKTimelineAnimation*>(stateKey);


    if(k->animationName==""){

        int flags = ( k->discardAnimationsQueue)? AnimationPlayerFlags::DISCARD_ANIMATION_QUEUE : 0;
        returnFlag |= timelineAnimationPlayer->Play(&dummyNoAnimationInstance, flags);


    }else if(timelineAnimationPlayer->GetAnimationInstance()==nullptr ||
             timelineAnimationPlayer->GetAnimationInstance()->GetAnimation()->GetName() != k->animationName)
    {

        TimelineAnimationInstance* ai = FindTimelineAnimationInstance(k->animationName);
        if(ai==nullptr){               // this happens in editor when we add keys with new animations, otherwise not
            ai = CreateAndStoreTimelineAnimationInstance(k->animationName);
        }

        assert(ai);

        ai->SetCompleteLoops(k->completeLoops);
        ai->SetSpeedFactor(k->fSpeed);
        int flags = ( k->discardAnimationsQueue)? AnimationPlayerFlags::DISCARD_ANIMATION_QUEUE : 0;

        returnFlag |= timelineAnimationPlayer->Play(ai, flags);
    }

    //----
    returnFlag |= timelineAnimationPlayer->Update();


    return returnFlag;
}


int AnimationTrackState::ManageSubFrameAnimation()
{

    int returnFlag = 0;


    if(frameAnimationPlayer==nullptr){
        frameAnimationPlayer = new AnimationQueuePlayer();
        FindSourceObject();

        //---- create animation instances
        frameAnimationInstances = new std::vector<FrameAnimationInstance*>();
        for(AnimationKey * tak : animationTrack->GetAnimationKeys()){
            AKFrameAnimation * ta = static_cast<AKFrameAnimation*>(tak);

            if(ta->animationName!=""){

                if(FindFrameAnimationInstance(ta->animationName)==nullptr){
                    CreateAndStoreFrameAnimationInstance(ta->animationName);
                }
            }
        }
    }

    AKFrameAnimation * k = static_cast<AKFrameAnimation*>(stateKey);


    if(k->animationName==""){
        frameAnimationPlayer->Stop();
        int flags = ( k->discardAnimationsQueue)? AnimationPlayerFlags::DISCARD_ANIMATION_QUEUE : 0;
        returnFlag |= frameAnimationPlayer->Play(&dummyNoAnimationInstance, flags);


    }else if(frameAnimationPlayer->GetAnimationInstance()==nullptr ||
             frameAnimationPlayer->GetAnimationInstance()->GetAnimation()->GetName() != k->animationName)
    {

        frameAnimationPlayer->Stop();
        FrameAnimationInstance * ai = FindFrameAnimationInstance(k->animationName);
        if(ai==nullptr){               // this happens in editor when we add keys with new animations, otherwise not
            ai = CreateAndStoreFrameAnimationInstance(k->animationName);
        }
        assert(ai);

        ai->SetCompleteLoops(k->completeLoops);
        ai->SetSpeedFactor(k->fSpeed);
        int flags = ( k->discardAnimationsQueue)? AnimationPlayerFlags::DISCARD_ANIMATION_QUEUE : 0;

        returnFlag |= frameAnimationPlayer->Play(ai, flags);
    }

    returnFlag |= frameAnimationPlayer->Update();

    return returnFlag;

}


void AnimationTrackState::FindSourceObject()
{

    if(animationMemberState->sprite){
        sourceObject = animationMemberState->sprite->GetSourceSprite();

    }else if(animationMemberState->sprites){
        sourceObject = animationMemberState->sprites->front()->GetSourceSprite();
    }

}


Map* AnimationTrackState::FindMap()
{

    if(animationMemberState->sprite){
        return animationMemberState->sprite->GetLayer()->GetMap();

    }else if(animationMemberState->sprites){
        return animationMemberState->sprites->front()->GetLayer()->GetMap();
    }
	return nullptr;
}



TimelineAnimationInstance* AnimationTrackState::FindTimelineAnimationInstance(const std::string& _name)
{
    for(TimelineAnimationInstance* tai : *timelineAnimationInstances){
        if(tai->GetAnimation()->GetName()==_name){
            return tai;
        }
    }

    return nullptr;
}


TimelineAnimationInstance* AnimationTrackState::CreateAndStoreTimelineAnimationInstance(const std::string& _name)
{


    //--- find timeline animation
    TimelineAnimation * animation = nullptr;
    for(int i=0; i<sourceObject->GetTimelineAnimations().size(); i++){
        if(sourceObject->GetTimelineAnimations().at(i)->GetName() == _name){
            animation = sourceObject->GetTimelineAnimations().at(i);
        }
    }

    TimelineAnimationInstance* animationInstance = nullptr;

    if(animation){
       // EAnimatedJugiSprites *as = static_cast<EAnimatedJugiSprites *>(animationMemberState->animatedSprites);
        if(animationMemberState->sprite){
            animationInstance = new TimelineAnimationInstance(animation, animationMemberState->sprite);
        }else if(animationMemberState->sprites){
            animationInstance = new TimelineAnimationInstance(animation, *animationMemberState->sprites);
        }else{
            assert(false);
        }
        timelineAnimationInstances->push_back(animationInstance);
    }

    return animationInstance;

}


FrameAnimationInstance* AnimationTrackState::FindFrameAnimationInstance(const std::string& _name)
{

    for(FrameAnimationInstance* fai : *frameAnimationInstances){
        if(fai->GetAnimation()->GetName()==_name){
            return fai;
        }
    }

    return nullptr;

}


FrameAnimationInstance* AnimationTrackState::CreateAndStoreFrameAnimationInstance(const std::string& _name)
{

    FrameAnimation * animation = nullptr;
    for(int i=0; i<sourceObject->GetFrameAnimations().size(); i++){
        if(sourceObject->GetFrameAnimations().at(i)->GetName() == _name){
            animation = sourceObject->GetFrameAnimations().at(i);
            break;
        }
    }


    FrameAnimationInstance* animationInstance = nullptr;

    if(animation){

        if(animationMemberState->sprite){
            animationInstance = new FrameAnimationInstance(animation, animationMemberState->sprite);

        }else if(animationMemberState->sprites){
            animationInstance = new FrameAnimationInstance(animation, *animationMemberState->sprites);

        }
        frameAnimationInstances->push_back(animationInstance);
    }

    return animationInstance;

}



void AnimationTrackState::_ClearLinkPointers(VectorShape *_shape)
{

    if(path==_shape){
        path = nullptr;
    }

}


//============================================================================================================


AnimationMemberState::AnimationMemberState(AnimationMember *_animationMember)
{

    animationMember = _animationMember;

    for(AnimationTrack *ka : animationMember->GetAnimationTracks()){
        if(ka->GetKind()==AnimationTrackKind::META) continue;

        animationsTrackStates.push_back(new AnimationTrackState(ka, this));
    }

    disabled = _animationMember->IsDisabled();

}


AnimationMemberState::~AnimationMemberState()
{
    for(AnimationTrackState* kas : animationsTrackStates){
        delete kas;
    }
    animationsTrackStates.clear();

    if(sprites){
        delete sprites;
    }
}


int AnimationMemberState::Update(int msTimePoint, int _flags)
{

    int returnFLag = 0;

    ap.Reset();
    if(disabled){
        return returnFLag;
    }

    for(AnimationTrackState *ats : animationsTrackStates){
        returnFLag |= ats->Update(msTimePoint, _flags);
        returnFLag |= UpdateAnimatedProperties(*ats);
    }

    return returnFLag;
}


void AnimationMemberState::ResetAnimatedProperties()
{
    ap.Reset();
}


int AnimationMemberState::UpdateAnimatedProperties(AnimationTrackState & _ats)
{

    if(_ats.IsDisabled()) return 0;

    AnimationTrackKind kind = _ats.GetKind();
    AnimationKey *stateKey = _ats.GetStateKey();       assert(stateKey);
    AnimationTrackParameters &tp = _ats.GetTrackParameters();


    if(kind==AnimationTrackKind::TRANSLATION){
        ap.translation += static_cast<AKTranslation*>(stateKey)->position;
        ap.changeFlags |= Sprite::Property::POSITION;

    }else if (kind==AnimationTrackKind::SCALING){
        ap.scale *= static_cast<AKScaling*>(stateKey)->scale;
        ap.changeFlags |= Sprite::Property::SCALE;

    }else if (kind==AnimationTrackKind::FLIP_HIDE){
        ap.hidden = static_cast<AKFlipHide*>(stateKey)->hidden;
        ap.flip = static_cast<AKFlipHide*>(stateKey)->flip;
        ap.changeFlags |= Sprite::Property::FLIP;

    }else if (kind==AnimationTrackKind::ROTATION){
        ap.rotation += static_cast<AKRotation*>(stateKey)->rotation;
        ap.changeFlags |= Sprite::Property::ROTATION;

    }else if (kind==AnimationTrackKind::ALPHA_CHANGE){
        ap.alpha *= static_cast<AKAlphaChange*>(stateKey)->alpha;
        ap.changeFlags |= Sprite::Property::ALPHA;

    }else if (kind==AnimationTrackKind::OVERLAY_COLOR_CHANGE){

        /*
        ap.shaderBasedProperties.SetKind(ap.shaderBasedProperties.GetKind() | TShaderBasedProperties::kindOVERLAY_COLOR);
        ap.shaderBasedProperties.SetBlend(tp.colorBlend);

        AKOverlayColorChange* k = static_cast<AKOverlayColorChange*>(stateKey);
        ap.shaderBasedProperties.SetRGB(k->color.r * 255, k->color.g*255, k->color.b*255);
        ap.shaderBasedProperties.SetAlpha(k->color.a);
        */

        AKOverlayColorChange* k = static_cast<AKOverlayColorChange*>(stateKey);
        ap.colorOverlayBlend = tp.colorBlend;
        ap.colorOverlayRGBA.Set(k->color.r * 255, k->color.g*255, k->color.b*255, k->color.a*255);

        ap.changeFlags |= Sprite::Property::OVERLAY_COLOR;

    }else if (kind==AnimationTrackKind::PATH_MOVEMENT){

        VectorShape* path = _ats.GetPath();

        if(path){

            AKPathMovement* k = static_cast<AKPathMovement*>(stateKey);

            //----
            float p = k->relDistance;

            if(path->IsClosed() && tp.reverseDirectionOnClosedShape){
                p = 1.0f - p;
            }

            p += tp.pathRelDistanceOffset;
            if(p > 1.0f){
                p = p - 1.0f;
            }

            //----
            Vec2f pathPoint;

            if(tp.pathDirectionOrientation){
                float directionAngle = 0;
                pathPoint = path->GetPathPoint(p, directionAngle);
                ap.rotation -= directionAngle;

                ap.changeFlags |= Sprite::Property::ROTATION;

            }else{
                pathPoint = path->GetPathPoint(p);
            }

            //---
            //Vec2f pathStartPoint(path->Points[0].x, path->Points[0].y);         //
            Vec2f pathStartPoint = path->GetPath().front();

            //Vec2f posPathAni(pathPoint.x - GetX(), pathPoint.y - GetY());
            Vec2f posPathAni = pathPoint - pathStartPoint;

            //if(path->GetName()=="pathSprity"){
            //    DbgOutput("p:"+std::to_string(p)+ "  pathPoint x:" + std::to_string(pathPoint.x) + " y:" + std::to_string(pathPoint.y));
            //}

            ap.translation += posPathAni;

            ap.changeFlags |= Sprite::Property::POSITION;
        }


    }else if (kind==AnimationTrackKind::FRAME_CHANGE){

        SourceSprite *sourceObject = nullptr;
        if(sprite){
            sourceObject = sprite->GetSourceSprite();
        }else if(sprites){
            sourceObject = sprites->front()->GetSourceSprite();
        }

        AKFrameChange* k = static_cast<AKFrameChange*>(stateKey);

        if(k->frameImageIndex>=0 && k->frameImageIndex< sourceObject->GetGraphicItems().size()){
            k->animationFrame.texture = sourceObject->GetGraphicItems()[k->frameImageIndex];
            ap.graphicItem = sourceObject->GetGraphicItems()[k->frameImageIndex];
            ap.changeFlags |= Sprite::Property::TEXTURE;
        }
    }


    if(ap.changeFlags!=0){
        return AnimationPlayerFlags::ANIMATED_PROPERTIES_CHANGED;
    }
    return 0;

}


void AnimationMemberState::ResetSpritesAnimatedProperties()
{

    if(sprite){
        sprite->ResetAnimatedProperties();

    }else if(sprites){

        for(Sprite *o : *sprites){
            o->ResetAnimatedProperties();
        }
    }

}


void AnimationMemberState::UpdateAnimatedSprites()
{

    if(sprite){
        sprite->AppendAnimatedProperties(ap);


    }else if(sprites){
        for(Sprite *o : *sprites){
            o->AppendAnimatedProperties(ap);
        }
    }


    //---
    for(AnimationTrackState *ats : animationsTrackStates){
        if(ats->GetFrameAnimationPlayer()){
            if(ats->GetFrameAnimationPlayer()->GetAnimationInstance()){
                ats->GetFrameAnimationPlayer()->GetAnimationInstance()->UpdateAnimatedSprites(false);
            }

        }else if(ats->GetTimelineAnimationQueuePlayer()){
            if(ats->GetTimelineAnimationQueuePlayer()->GetAnimationInstance()){
                ats->GetTimelineAnimationQueuePlayer()->GetAnimationInstance()->UpdateAnimatedSprites(false);
            }
        }
    }

}


void AnimationMemberState::StopSubPlayers()
{

    for(AnimationTrackState *ats : animationsTrackStates){
        if(ats->GetFrameAnimationPlayer()){
            ats->GetFrameAnimationPlayer()->Stop();

        }else if(ats->GetTimelineAnimationQueuePlayer()){
            ats->GetTimelineAnimationQueuePlayer()->Stop();
        }
    }

}


void AnimationMemberState::PauseSubPlayers()
{

    for(AnimationTrackState *ats : animationsTrackStates){
        if(ats->GetFrameAnimationPlayer()){
            ats->GetFrameAnimationPlayer()->Pause();

        }else if(ats->GetTimelineAnimationQueuePlayer()){
            ats->GetTimelineAnimationQueuePlayer()->Pause();

        }
    }

}


void AnimationMemberState::ResumeSubPlayers()
{

    for(AnimationTrackState *ats : animationsTrackStates){
        if(ats->GetFrameAnimationPlayer()){
            ats->GetFrameAnimationPlayer()->Resume();

        }else if(ats->GetTimelineAnimationQueuePlayer()){
            ats->GetTimelineAnimationQueuePlayer()->Resume();

        }
    }

}


//============================================================================================================


TimelineAnimationInstance::TimelineAnimationInstance(TimelineAnimation *_timelineAnimation, Sprite *_sprite)
{

    assert(_timelineAnimation->GetSourceSprite()==_sprite->GetSourceSprite());

    kind = AnimationKind::TIMELINE_ANIMATION;
    animation = _timelineAnimation;
    bp = animation->GetBaseParameters();


    //--- create instances of member animations
    for(AnimationMember *kas : _timelineAnimation->GetAnimationMembers()){
        animationsMemberStates.push_back(new AnimationMemberState(kas));
        animationsMemberStates.back()->animationInstance = this;
    }
    if(_timelineAnimation->GetMetaAnimationTrack()){
        metaAnimationTrackState = new AnimationTrackState(_timelineAnimation->GetMetaAnimationTrack(), nullptr);
    }

    //--- set root member
    if(animationsMemberStates.empty()==false){           // root sprite
        animationsMemberStates.front()->sprite = _sprite;
        _sprite->CreateAnimatedPropertiesIfNone();
    }

    //--- set child members
    if(animationsMemberStates.size()>1){      // child sprites

        //assert(_sprite->GetSourceSprite()->GetSourceComposedSprite());
        assert(_sprite->GetKind()==SpriteKind::COMPOSED);

        //----
        std::vector<std::vector<Sprite*>>childrenPerNameID;
        GatherSpritesWithSetNameID(static_cast<ComposedSprite*>(_sprite), childrenPerNameID);

        //zbrani children so lahko isti objekti (blade) kar moram popraviti

        for(int i=1; i<animationsMemberStates.size(); i++){

            AnimationMemberState* ams = animationsMemberStates[i];

            for(std::vector<Sprite*>& childSprites : childrenPerNameID){
                assert(childSprites.empty()==false);

                if(childSprites.front()->GetName() == ams->animationMember->GetNameID()){

                    if(childSprites.size()==1){
                        ams->sprite = childSprites.front();
                        ams->sprite->CreateAnimatedPropertiesIfNone();

                    }else if(childSprites.size()>1){
                        ams->sprites = new std::vector<Sprite*>();
                        *ams->sprites = childSprites;
                        for(Sprite*o : *ams->sprites){
                            o->CreateAnimatedPropertiesIfNone();
                        }
                    }
                    break;
                }
            }
        }
    }

}


TimelineAnimationInstance::TimelineAnimationInstance(TimelineAnimation *_timelineAnimation, std::vector<Sprite*>&_sprites)
{

    assert(_sprites.empty()==false);
    for(Sprite *o : _sprites){
        assert(_timelineAnimation->GetSourceSprite()==o->GetSourceSprite());
    }

    kind = AnimationKind::TIMELINE_ANIMATION;
    animation = _timelineAnimation;
    bp = animation->GetBaseParameters();


    //--- create instances of member animations
    for(AnimationMember *kas : _timelineAnimation->GetAnimationMembers()){
        animationsMemberStates.push_back(new AnimationMemberState(kas));
        animationsMemberStates.back()->animationInstance = this;
    }
    if(_timelineAnimation->GetMetaAnimationTrack()){
        metaAnimationTrackState = new AnimationTrackState(_timelineAnimation->GetMetaAnimationTrack(), nullptr);
    }


    //--- set root members
    if(animationsMemberStates.empty()==false){
        animationsMemberStates.front()->sprites = new std::vector<Sprite*>();
        *animationsMemberStates.front()->sprites = _sprites;
        for(Sprite*o : _sprites){
            o->CreateAnimatedPropertiesIfNone();
        }
    }

    //--- set child members
    if(animationsMemberStates.size()>1){

        //----
        std::vector<std::vector<Sprite*>>childrenPerNameID;

        for(Sprite* s : _sprites){
            assert(s->GetKind()==SpriteKind::COMPOSED);
            GatherSpritesWithSetNameID(static_cast<ComposedSprite*>(s), childrenPerNameID);
        }

        for(int i=1; i<animationsMemberStates.size(); i++){

            AnimationMemberState* ams = animationsMemberStates[i];

            for(std::vector<Sprite*>& childSprites : childrenPerNameID){
                assert(childSprites.empty()==false);

                if(childSprites.front()->GetName() == ams->animationMember->GetNameID()){

                    if(childSprites.size()==1){
                        ams->sprite = childSprites.front();
                        ams->sprite->CreateAnimatedPropertiesIfNone();

                    }else if(childSprites.size()>1){
                        ams->sprites = new std::vector<Sprite*>();
                        *ams->sprites = childSprites;
                        for(Sprite*o : *ams->sprites){
                            o->CreateAnimatedPropertiesIfNone();
                        }
                    }
                    break;
                }
            }
        }
    }

}


TimelineAnimationInstance::~TimelineAnimationInstance()
{

    for(AnimationMemberState *kads : animationsMemberStates){
        delete kads;
    }
    animationsMemberStates.clear();

    if(metaAnimationTrackState){
        delete metaAnimationTrackState;
    }

}


void TimelineAnimationInstance::UpdateAnimatedSprites(bool _resetSpriteAnimatedProperties)
{

    if(_resetSpriteAnimatedProperties){
        // We reset only for root sprites as any child sprites are also affected in the proccess !!!
        animationsMemberStates.front()->ResetSpritesAnimatedProperties();
    }

    for(AnimationMemberState* set : animationsMemberStates){
        set->UpdateAnimatedSprites();
    }
}


int TimelineAnimationInstance::Update(int msTimePoint, int _flags)
{

    int returnFlag = 0;

    for(AnimationMemberState* set : animationsMemberStates){
        returnFlag |= set->Update(msTimePoint, _flags);
    }

    //----
    if(metaAnimationTrackState){
        metaAnimationTrackState->Update(msTimePoint, _flags);
    }

    return returnFlag;
}


void TimelineAnimationInstance::ResetAnimatedProperties()
{
    for(AnimationMemberState* set : animationsMemberStates){
        set->ResetAnimatedProperties();
    }
}


void TimelineAnimationInstance::StopSubPlayers()
{

    for(AnimationMemberState* kads : animationsMemberStates){
        kads->StopSubPlayers();
    }

}


void TimelineAnimationInstance::PauseSubPlayers()
{

    for(AnimationMemberState* kads : animationsMemberStates){
        kads->PauseSubPlayers();
    }

}


void TimelineAnimationInstance::ResumeSubPlayers()
{

    for(AnimationMemberState* kads : animationsMemberStates){
        kads->ResumeSubPlayers();
    }

}


void TimelineAnimationInstance::ClearLinkPointers(VectorShape* _shape)
{

    for(AnimationMemberState* kads : animationsMemberStates){
        for(AnimationTrackState *kad : kads->animationsTrackStates){
            kad->_ClearLinkPointers(_shape);
        }
    }

}


AnimationMemberState* TimelineAnimationInstance::FindAnimationMemberState(AnimationMember* _animationMember)
{

    for(AnimationMemberState* set : animationsMemberStates){
        if(set->GetAnimationMember() ==_animationMember){
            return set;
        }
    }

    return nullptr;

}


AnimationTrackState* TimelineAnimationInstance::FindAnimationTrackState(AnimationTrackKind _animationTrackKind, const std::string &_memberName)
{

    for(AnimationMemberState* set : animationsMemberStates){

        if(set->GetAnimationMember()->GetNameID()==_memberName){
            for(AnimationTrackState *tas : set->GetAnimationTrackStates()){
                if(tas->GetKind()==_animationTrackKind){
                    return tas;
                }
            }
        }
    }

    return nullptr;

}


AnimationTrackState* TimelineAnimationInstance::FindAnimationTrackState(AnimationTrack *_animationTrack)
{

    for(AnimationMemberState* set : animationsMemberStates){
        for(AnimationTrackState *tas : set->GetAnimationTrackStates()){
            if(tas->GetAnimationTrack() == _animationTrack){
                return tas;
            }
        }
    }

    return nullptr;

}







}








