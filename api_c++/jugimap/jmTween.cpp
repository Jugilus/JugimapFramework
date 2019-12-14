#include "jmCommon.h"
#include "jmGlobal.h"
#include "jmTween.h"



namespace jugimap {




float Easing::GetValue(float p){

    switch (kind)
    {
    case LINEAR:
        return p;

    case EASE_IN:
        return p*p;

    case EASE_OUT:
        return  1.0-(1.0-p)*(1.0-p);

    case EASE_IN_OUT:
        return p < 0.5 ? 2*p*p : 1.0 - (2.0-2.0*p)*(2.0-2.0*p)/2.0;
    }

    return p;
}


//==============================================================================


Tween::~Tween()
{
     Tweens::RemoveTween(this);
}


void Tween::Init(float _valueStart, float _valueEnd, float _durationS, int _easingKind)
{
    if(state==stateNEVER_INITIALIZED){
        Tweens::tweensVector.push_back(this);
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


void Tween::Update()
{

    if(state==statePLAYING){

        float p = (time.GetPassedNetTimeMS()-timeStartMS)/float(timeEndMS-timeStartMS);

        if(p>1.0){

            p = 0.0;

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
                return;
            }
        }

        if(reverse){
            p = 1.0 - p;
        }

        if(mode==Mode::REVERSE){
            p = 1.0 - p;
        }

        p = easing.GetValue(p);
        value = valueStart + p*(valueEnd - valueStart);

    }
}


//==============================================================================

std::vector<Tween*> Tweens::tweensVector;


void Tweens::Update()
{
    for(Tween *t : tweensVector){
        t->Update();
    }
}


void Tweens::RemoveTween(Tween * _tween)
{
    if(tweensVector.empty()) return;

    for(int i = int(tweensVector.size())-1; i>=0; i-- ){
        if(tweensVector[i]==_tween){
            tweensVector.erase(tweensVector.begin()+i);
        }
    }
}





}
