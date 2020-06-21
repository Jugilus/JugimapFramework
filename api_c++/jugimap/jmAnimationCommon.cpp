#include "jmCommonFunctions.h"
#include "jmSprites.h"
#include "jmTimelineAnimationInstance.h"
#include "jmAnimationCommon.h"


namespace jugimap{







void AnimatedProperties::Reset()
{
    translation.Set(0.0f, 0.0f);
    scale.Set(1.0f, 1.0f);
    hidden = false;
    rotation = 0.0f;
    flip.Set(0,0);
    alpha = 1.0f;
    //animationFrame = nullptr;
    //animationFrameSet = false;
    //shaderBasedPropertiesSet = false;
    colorOverlayRGBA = 0;
    colorOverlayBlend = ColorOverlayBlend::NORMAL;

    changeFlags = 0;
}


void AnimatedProperties::Append(AnimatedProperties &ap)
{

    translation += ap.translation;
    scale *= ap.scale;
    rotation += ap.rotation;
    alpha *= ap.alpha;
    if(ap.flip.x==1) flip.x = 1-flip.x;
    if(ap.flip.y==1) flip.y = 1-flip.y;
    if(ap.hidden) hidden = true;


    //if(shaderBasedPropertiesSet && ap.shaderBasedPropertiesSet){
    if(ap.changeFlags & Sprite::Property::OVERLAY_COLOR){

        if(changeFlags & Sprite::Property::OVERLAY_COLOR){
            //ColorRGBA c = shaderBasedProperties.GetColorF();
            //float a = shaderBasedProperties.GetAlpha();
            //Color c2 = ap.shaderBasedProperties.GetColorF();
            //float a2 = ap.shaderBasedProperties.GetAlpha();
            //shaderBasedProperties.SetRGB(255*(c.r+c2.r)/2.0, 255*(c.g+c2.g)/2.0, 255*(c.b+c2.b)/2.0);
            //shaderBasedProperties.SetAlpha((a + a2)/2.0);

            colorOverlayRGBA.Set(255*(colorOverlayRGBA.r + ap.colorOverlayRGBA.r)/2.0, 255*(colorOverlayRGBA.g + ap.colorOverlayRGBA.g)/2.0,
                                 255*(colorOverlayRGBA.b + ap.colorOverlayRGBA.b)/2.0, 255*(colorOverlayRGBA.a + ap.colorOverlayRGBA.a)/2.0);

        }else{
            //shaderBasedProperties = ap.shaderBasedProperties;
            colorOverlayRGBA = ap.colorOverlayRGBA;
            colorOverlayBlend = ap.colorOverlayBlend;
        }
    }

    //if(animationFrameSet==false){
    if((changeFlags & Sprite::Property::TEXTURE)==0){
        graphicItem = ap.graphicItem;
        //animationFrame = ap.animationFrame;
        //animationFrameSet = ap.animationFrameSet;
    }

    changeFlags |= ap.changeFlags;

}


//===============================================================================================================


Animation::Animation(SourceSprite *_sourceObject)
{
    sourceObject = _sourceObject;
}


Animation::Animation(const std::string &_name, SourceSprite *_sourceObject)
{
    name = _name;
    sourceObject = _sourceObject;
}


Animation::Animation(const Animation &mkaSrc)
{
    kind = mkaSrc.kind;
    sourceObject = mkaSrc.sourceObject;
    name = mkaSrc.name;
    bp = mkaSrc.bp;
    duration = mkaSrc.duration;
    parameters = mkaSrc.parameters;
    dataFlags = mkaSrc.dataFlags;

}


Animation& Animation::operator=(const Animation &mkaSrc)
{
    if(this==&mkaSrc) return *this;

    kind = mkaSrc.kind;
    sourceObject = mkaSrc.sourceObject;
    name = mkaSrc.name;
    bp = mkaSrc.bp;
    duration = mkaSrc.duration;
    parameters = mkaSrc.parameters;
    dataFlags = mkaSrc.dataFlags;

    return *this;
}


//=============================================================================================================================


int AnimationPlayer::Play(AnimationInstance *_animationInstance)
{

    assert(_animationInstance);


    int returnFlag = 0;

    //---- special case: 'dummyNoAnimationInstance'
    if(_animationInstance==&dummyNoAnimationInstance){

        if(state==AnimationPlayerState::IDLE) return returnFlag;

        //---
        Stop();
        returnFlag |= AnimationPlayerFlags::PLAYER_STOPPED;
        return returnFlag;
    }


    //----
    if(animationInstance){
        if(_animationInstance==animationInstance){
            return returnFlag;
        }else{
            Stop();
        }
    }

    animationInstance = _animationInstance;
    animationInstance->OnPlayingStart();
    msStartPlayOffset = animationInstance->GetStartPlayTimeOffset();
    bp = animationInstance->GetBaseParameters();


    //----
    msCurrentAnimationTime = msStartPlayOffset;
    countLoop = 0;

    int msCurrentTime = time.GetPassedNetTimeMS();

    if(bp.loopForever==false && bp.repeat && bp.startAtRepeatTime){
        msRepeat = GetRepeatTime(msCurrentTime);
        state = AnimationPlayerState::WAITING_TO_REPEAT;

    }else if(bp.startDelay>0){
        msRepeat = msCurrentTime+bp.startDelay;
        state = AnimationPlayerState::WAITING_TO_START;
        DbgOutput(" PLAY millisecondsPassed:" + std::to_string(msCurrentTime) + " msRepeat::" + std::to_string(msRepeat));


    }else{
        msAnimationTimeStart = msCurrentTime;
        state = AnimationPlayerState::PLAYING;

        //----
        returnFlag |= UpdateAnimationInstance();
    }

    returnFlag |= AnimationPlayerFlags::PLAYER_STARTED;

    return returnFlag;
}


int AnimationPlayer::Update()
{

    int returnFlag = 0;
    if(state==AnimationPlayerState::IDLE) return returnFlag;

    assert(animationInstance);


    int duration = animationInstance->GetAnimation()->GetDuration();
    float fSpeed = animationInstance->GetSpeedFactor();
    int msCurrentTime = time.GetPassedNetTimeMS();

    if(state==AnimationPlayerState::PLAYING){

        msCurrentAnimationTime = msCurrentTime - msAnimationTimeStart + msStartPlayOffset;
        msCurrentAnimationTime *= fSpeed;

        if(msCurrentAnimationTime >= duration){

            msCurrentAnimationTime = duration;

            returnFlag |= UpdateAnimationInstance();


            if(bp.loopForever){
                msAnimationTimeStart = msCurrentTime;
                msStartPlayOffset = 0;
                msCurrentAnimationTime = 0;

            }else{
                countLoop++;
                if(countLoop<bp.loopCount){
                    msAnimationTimeStart = msCurrentTime;
                    msStartPlayOffset = 0;
                    msCurrentAnimationTime = 0;

                }else{

                    if(bp.repeat){
                        msRepeat = GetRepeatTime(msCurrentTime);
                        state = AnimationPlayerState::WAITING_TO_REPEAT;
                        msStartPlayOffset = 0;
                        //msCurrentAnimationTime = -1;

                    }else{

                        //msRepeat = -1;
                        //msCurrentAnimationTime = -1;
                        state = AnimationPlayerState::STALLED;
                    }
                }
            }

        }else{

            returnFlag |= UpdateAnimationInstance();

        }


    }else if(state==AnimationPlayerState::WAITING_TO_REPEAT){


        if(msCurrentTime>=msRepeat){
            //msStartPlayOffset = 0;
            countLoop = 0;
            msAnimationTimeStart = msCurrentTime;
            msCurrentAnimationTime = msStartPlayOffset;
            state = AnimationPlayerState::PLAYING;

            returnFlag |= UpdateAnimationInstance();
        }

    }else if(state==AnimationPlayerState::WAITING_TO_START){


        if(msCurrentTime>=msRepeat){
            DbgOutput(" START millisecondsPassed:" + std::to_string(msCurrentTime) + " msRepeat::"+ std::to_string(msRepeat));
            //msStartPlayOffset = 0;
            countLoop = 0;
            msAnimationTimeStart = msCurrentTime;
            msCurrentAnimationTime = msStartPlayOffset;
            state = AnimationPlayerState::PLAYING;

            returnFlag |= UpdateAnimationInstance();

        }else{
            //DbgOutput(" WAITING_TO_START msCurrentTime:" + std::to_string(msCurrentTime) + " msRepeat::" + std::to_string(msRepeat));
        }
    }


    returnFlag |= AnimationPlayerFlags::PLAYER_UPDATED;

    return returnFlag;

}


int AnimationPlayer::UpdateAnimationInstance()
{

    int returnFlag = 0;

    returnFlag = animationInstance->Update(msCurrentAnimationTime, AnimationPlayerFlags::NONE);

    if(animationInstance->GetKind()==AnimationKind::TIMELINE_ANIMATION){
        TimelineAnimationInstance* tai = static_cast<TimelineAnimationInstance*>(animationInstance);
        if(tai->GetMetaAnimationTrackState()){
            if(activeControllerKey != tai->GetMetaAnimationTrackState()->GetActiveKey()){
                returnFlag |= AnimationPlayerFlags::META_KEY_CHANGED;
            }
            activeControllerKey = dynamic_cast<AKMeta*>(tai->GetMetaAnimationTrackState()->GetActiveKey());
        }
    }

    returnFlag |= AnimationPlayerFlags::ANIMATION_INSTANCE_UPDATED;

    return returnFlag;

}


void AnimationPlayer::Pause()
{

    if(state== AnimationPlayerState::IDLE) return;
    assert(animationInstance);

    if(animationInstance->GetKind()==AnimationKind::TIMELINE_ANIMATION){
        static_cast<TimelineAnimationInstance*>(animationInstance)->PauseSubPlayers();
    }

    stateStored = state;
    msTimeStored = time.GetPassedNetTimeMS();
    state = AnimationPlayerState::PAUSED;

}


void AnimationPlayer::Resume()
{

    if(state==AnimationPlayerState::IDLE) return;
    assert(animationInstance);

    if(animationInstance->GetKind()==AnimationKind::TIMELINE_ANIMATION){
        static_cast<TimelineAnimationInstance*>(animationInstance)->ResumeSubPlayers();
    }

    state = stateStored;
    msAnimationTimeStart += time.GetPassedNetTimeMS() - msTimeStored;
    state = AnimationPlayerState::PLAYING;

}


int AnimationPlayer::Stop()
{

    int returnFlag = 0;

    if(state==AnimationPlayerState::IDLE) return returnFlag;
    assert(animationInstance);

    if(animationInstance->GetKind()==AnimationKind::TIMELINE_ANIMATION){
        static_cast<TimelineAnimationInstance*>(animationInstance)->StopSubPlayers();
    }

    animationInstance->OnPlayingStop();
    animationInstance = nullptr;
    activeControllerKey = nullptr;
    state = AnimationPlayerState::IDLE;

    returnFlag |= AnimationPlayerFlags::PLAYER_STOPPED;

    return returnFlag;

}


void AnimationPlayer::SetParametersForCompletingLoop()
{
    // Modify parameters which affect that after current loop the animation will end!
    bp.loopForever = false;
    bp.repeat = false;
    countLoop = 0;
    bp.loopCount = 1;

}


int AnimationPlayer::GetRepeatTime(int _msCurrentTime)
{

    int msRepeatPeriod = bp.repeat_DelayTimeEnd-bp.repeat_DelayTimeStart;
    if(msRepeatPeriod<0) msRepeatPeriod = 0;

    double fRnd = fRand(0.0, 1.0);
    //qDebug() << "fRnd:" << fRnd; // << "simple_rand.min():" << simple_rand.min() << " simple_rand.max():" << simple_rand.max();
    int msTime = _msCurrentTime + bp.repeat_DelayTimeStart + msRepeatPeriod * fRnd;

    //DbgOutput("repeat time: " + std::to_string((bp.repeat_PeriodStart + msRepeatPeriod * fRnd)/1000.0));

    return msTime;
}


//================================================================================================


int AnimationQueuePlayer::Play(AnimationInstance *_animationInstance, int _flags)
{

    assert(_animationInstance);

    int returnFlag = 0;

    if(animationInstances.empty()==false && _animationInstance==animationInstances.back()) return returnFlag;


    if(_flags & AnimationPlayerFlags::DISCARD_ANIMATION_QUEUE){
        animationInstances.clear();
        returnFlag |= player.Stop();
    }


    //---- special case: 'dummyNoAnimationInstance'
    if(_animationInstance==&dummyNoAnimationInstance){
        // add 'dummyNoAnimationInstance' only if there is animtion running !
        if(animationInstances.empty()==false){
            animationInstances.push_back(_animationInstance);
        }else{
            return returnFlag;
        }

    }else{
        animationInstances.push_back(_animationInstance);
    }


    if(animationInstances.size()==1){
        returnFlag |= player.Play(animationInstances.front());

    }else{

        // currently played instance

        AnimationInstance *ai = player.GetAnimationInstance();     assert(ai);

        if(ai->GetCompleteLoops()){
            player.SetParametersForCompletingLoop();

        }else{
            player.Stop();
            bool removed = RemoveElementFromVector(animationInstances, ai);   assert(removed);
            returnFlag |= PlayNextAnimationInQueue();
        }
    }


    returnFlag |= AnimationPlayerFlags::PLAYER_STARTED;

    return returnFlag;

}


int AnimationQueuePlayer::Stop()
{

    int returnFlag = 0;
    if(player.GetState()==AnimationPlayerState::IDLE) return returnFlag;

    animationInstances.clear();
    player.Stop();

    returnFlag |= AnimationPlayerFlags::PLAYER_STOPPED;

    return returnFlag;

}


int AnimationQueuePlayer::Update()
{

    int returnFlag = 0;
    if(player.GetState()==AnimationPlayerState::IDLE) return returnFlag;


    AnimationInstance *ai = player.GetAnimationInstance();     assert(ai);

    returnFlag |= player.Update();

    if(player.GetState()==AnimationPlayerState::STALLED && animationInstances.size()>1){
        bool removed = RemoveElementFromVector(animationInstances, ai);   assert(removed);
        player.Stop();
        //---
        returnFlag |= PlayNextAnimationInQueue();
    }


    returnFlag |= AnimationPlayerFlags::PLAYER_UPDATED;

    return returnFlag;
}


int AnimationQueuePlayer::PlayNextAnimationInQueue()
{

    assert(player.GetState()==AnimationPlayerState::IDLE);

    int returnFlag = 0;

    if(animationInstances.empty()) return returnFlag;

    //----
    if(animationInstances.size()>5){
        RemoveElementFromVector(animationInstances, animationInstances.front());
    }


    AnimationInstance *ai = animationInstances.front();

    /*
    if(childPlayer){
        qDebug()<<"";
        qDebug()<<"A animationInstances size:"<<animationInstances.size();
        for(ETimelineAnimationInstance *i : animationInstances){
            if(i==&dummyNoAnimationInstance){
                qDebug()<<"   "<<"dummyNoAnimationInstance";
            }else{
                qDebug()<<"   "<<i->GetTimelineAnimation()->GetName();
            }
        }
    }
    */


    // play older animation in queue
    while(ai != animationInstances.back()){

        if(ai==&dummyNoAnimationInstance || ai->GetCompleteLoops()==false){
            bool removed = RemoveElementFromVector(animationInstances, ai);   assert(removed);
            ai = animationInstances.front();

        }else{
            returnFlag |= player.Play(ai);
            player.SetParametersForCompletingLoop();
            break;
        }
    }


    // play last animation in queue
    if(ai==animationInstances.back()){
        if(ai==&dummyNoAnimationInstance){
            bool removed = RemoveElementFromVector(animationInstances, ai);   assert(removed);
            ai = nullptr;
        }else{
            returnFlag |= player.Play(ai);
        }
    }

    /*
    if(childPlayer){
        qDebug()<<"B animationInstances size:"<<animationInstances.size();
        for(ETimelineAnimationInstance *i : animationInstances){
            std::string played = "";
            if(i==ai){
                played = "  PLAYED";
            }
            if(i==&dummyNoAnimationInstance){
                qDebug()<<"   "<<"dummyNoAnimationInstance"<<played;
            }else{
                qDebug()<<"   "<<i->GetTimelineAnimation()->GetName()<<played;
            }
        }
    }
    */


    return returnFlag;
}



DummyNoAnimationInstance dummyNoAnimationInstance;
AnimationPlayer *activeTimelineAnimationPlayer = nullptr;


}
