#include <assert.h>
#include "jmCommonFunctions.h"
#include "jmGlobal.h"
#include "jmSprites.h"
#include "jmFrameAnimation.h"



namespace jugimap{



FrameAnimation::FrameAnimation(const FrameAnimation &fa)
{

    name = fa.name;
    sourceSprite = fa.sourceSprite;
    loopCount = fa.loopCount;
    loopForever = fa.loopForever;
    scaleDuration = fa.scaleDuration;
    parameters = fa.parameters;
    dataFlags = fa.dataFlags;

    for(AnimationFrame *af : fa.frames){
        frames.push_back(new AnimationFrame(*af));
    }
}


FrameAnimation& FrameAnimation::operator=(const FrameAnimation &fa)
{

    if(this==&fa) return *this;

    name = fa.name;
    sourceSprite = fa.sourceSprite;
    loopCount = fa.loopCount;
    loopForever = fa.loopForever;
    scaleDuration = fa.scaleDuration;
    parameters = fa.parameters;
    dataFlags = fa.dataFlags;

    for(AnimationFrame *af : frames){
        delete af;
    }
    frames.clear();

    for(AnimationFrame *af : fa.frames){
        frames.push_back(new AnimationFrame(*af));
    }
    return *this;
}


FrameAnimation::~FrameAnimation()
{
    for(AnimationFrame *af : frames){
        delete af;
    }
}





//================================================================================================



void FrameAnimationPlayer::SetFrameAnimation(FrameAnimation * _frameAnimation)
{
    assert(state==stateIDLE);

    frameAnimation = _frameAnimation;

}


bool FrameAnimationPlayer::Play(FrameAnimation * _frameAnimation, int _indexFrame)
{
    Reset();
    frameAnimation = _frameAnimation;
    return Play(_indexFrame);
}


bool FrameAnimationPlayer::Play(int _indexFrame)
{
    if(frameAnimation==nullptr || frameAnimation->frames.empty()) return false;

    indexCurrentFrame = _indexFrame;
    countLoop = 0;

    if(frameAnimation->loopForever==false && frameAnimation->repeatAnimation && frameAnimation->startAtRepeatTime){
        msRepeat = GetRepeatTime(time.GetPassedNetTimeMS());
        state = stateWAITING_TO_REPEAT;

    }else{
        msFrameStartTime = time.GetPassedNetTimeMS();
        state = statePLAYING;
    }

    return true;
}


void FrameAnimationPlayer::Pause()
{
    if(state==stateIDLE || state==statePAUSED) return;

    stateStored = state;
    msTimeStored = time.GetPassedNetTimeMS();
    state = statePAUSED;
}


void FrameAnimationPlayer::Resume()
{
    if(state!=statePAUSED) return;

    state = stateStored;
    msFrameStartTime += time.GetPassedNetTimeMS() - msTimeStored;
}


void FrameAnimationPlayer::Stop()
{
    state = stateIDLE;
}


int FrameAnimationPlayer::Update()
{
    if(state==stateIDLE) return 0;

    assert(frameAnimation && frameAnimation->frames.empty()==false);


    int returnFlag = 0;

    if(state==statePLAYING){

        int msCurrentTime = time.GetPassedNetTimeMS();
        if(msCurrentTime-msFrameStartTime >= frameAnimation->frames[indexCurrentFrame]->duration * frameAnimation->scaleDuration){
            indexCurrentFrame ++;
            msFrameStartTime = msCurrentTime;


            if(indexCurrentFrame>=frameAnimation->frames.size()){

                if(frameAnimation->loopForever){
                    indexCurrentFrame = 0;

                }else{
                    countLoop++;
                    if(countLoop<frameAnimation->loopCount){
                        indexCurrentFrame = 0;

                    }else{
                        indexCurrentFrame--;

                        if(frameAnimation->repeatAnimation){
                            msRepeat = GetRepeatTime(msCurrentTime);
                            state = stateWAITING_TO_REPEAT;

                        }else{

                            msRepeat = -1;
                            state = stateLOOP_END;

                        }
                    }
                }
            }
            returnFlag = flagFRAME_CHANGED;
        }

    }else if(state==stateWAITING_TO_REPEAT){


        int msCurrentTime = time.GetPassedNetTimeMS();
        if(msCurrentTime>=msRepeat){
            indexCurrentFrame = 0;
            countLoop = 0;
            msFrameStartTime = msCurrentTime;
            state = statePLAYING;
            //---
            returnFlag = flagFRAME_CHANGED;
        }
    }

    return returnFlag;
}


void FrameAnimationPlayer::Reset()
{
    frameAnimation = nullptr;
    state = stateIDLE;
}


int FrameAnimationPlayer::GetRepeatTime(int _msCurrentTime)
{
    int msRepeatPeriod = frameAnimation->repeat_PeriodEnd-frameAnimation->repeat_PeriodStart;
    if(msRepeatPeriod<0) msRepeatPeriod = 0;
    //float fRnd = agk::Random(0,1000)/1000.0;
    float fRnd =  fRand(0.0, 1.0);
    int msTime = _msCurrentTime + frameAnimation->repeat_PeriodStart + msRepeatPeriod * fRnd;

    //qDebug() << "repeat time: " << (FrameAnimation->repeat_PeriodStart + msRepeatPeriod * fRnd)/1000.0;

    return msTime;
}


//==============================================================================================


void StandardSpriteFrameAnimationPlayer::OnChangedStandardSpriteAnimationFrame()
{

    AnimationFrame *frame =  frameAnimationPlayer.GetActiveFrame();

    if(frame->image){
        if(frame->flip != standardSprite->GetFlip()){
            standardSprite->SetFlip(frame->flip);
        }
        if(frame->offset.Equals(standardSprite->GetAnimationFrameOffset())==false){
            standardSprite->SetAnimationFrameOffset(frame->offset);
        }
    }

    standardSprite->SetActiveImage(frame->image);
}






}
