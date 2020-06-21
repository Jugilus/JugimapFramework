#include "jmCommon.h"
#include "jmGlobal.h"
#include "jmTween.h"



namespace jugimap {



//==============================================================================


Tween::~Tween()
{
     Tweens::RemoveTween(this);
}


void Tween::Init(float _valueStart, float _valueEnd, float _durationS, Easing::Kind _easingKind)
{
    if(state==stateNEVER_INITIALIZED){
        Tweens::vTweens.push_back(this);
    }
    valueStart = _valueStart;
    valueEnd = _valueEnd;
    durationMS = _durationS * 1000;
    timeStartMS = time.GetPassedNetTimeMS();
    timeEndMS = timeStartMS + durationMS;
    easing.kind = _easingKind;

    state = stateIDLE;
}


void Tween::Play()
{
    if(state==stateNEVER_INITIALIZED) return;

    timeStartMS = time.GetPassedNetTimeMS();
    timeEndMS = timeStartMS + durationMS;
    state = statePLAYING;
    reverse = false;
}


void Tween::Stop()
{
    state = stateIDLE;
}


void Tween::Pause()
{
    if(state!=statePLAYING) return;

    stateStored = state;
    msTimeStored = time.GetPassedNetTimeMS();
    state = statePAUSED;
}


void Tween::Resume()
{
    if(state!=statePAUSED) return;

    state = stateStored;
    timeStartMS += time.GetPassedNetTimeMS() - msTimeStored;
    timeEndMS += time.GetPassedNetTimeMS() - msTimeStored;
}


float Tween::Update()
{

    if(state==statePLAYING){

        float p = (time.GetPassedNetTimeMS()-timeStartMS)/float(timeEndMS-timeStartMS);

        if(p>1.0f){

            p = 0.0f;

            if(mode==Mode::LOOP){
                timeStartMS = time.GetPassedNetTimeMS();
                timeEndMS = timeStartMS + durationMS;

            }else if(mode==Mode::LOOP_REVERSE){
                timeStartMS = time.GetPassedNetTimeMS();
                timeEndMS = timeStartMS + durationMS;
                reverse = !reverse;

            }else{
                value = valueEnd;
                if(mode==Mode::REVERSE){
                    value = valueStart;
                }
                state = stateIDLE;
                return value;
            }
        }

        if(reverse){
            p = 1.0f - p;
        }

        if(mode==Mode::REVERSE){
            p = 1.0f - p;
        }

        p = easing.GetValue(p);
        value = valueStart + p*(valueEnd - valueStart);

    }

    return value;
}


//==============================================================================

std::vector<Tween*> Tweens::vTweens;


void Tweens::Update()
{
    for(Tween *t : vTweens){
        t->Update();
    }
}


void Tweens::RemoveTween(Tween * _tween)
{
    if(vTweens.empty()) return;

    for(int i = int(vTweens.size())-1; i>=0; i-- ){
        if(vTweens[i]==_tween){
            vTweens.erase(vTweens.begin()+i);
        }
    }
}





}
