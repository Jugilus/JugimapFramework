#include <string>
#include <sstream>
#include "jmCommonFunctions.h"
#include "jmCommon.h"
#include "jmGlobal.h"



namespace jugimap {



bool Parameter::Exists(const std::vector<Parameter> &parameters, const std::string &name, const std::string &value)
{
    for(const Parameter &P : parameters){
        if(P.name == name){
            if(value=="" || P.value==value){
                return true;
            }
        }
    }
    return false;
}


std::string Parameter::GetValue(const std::vector<Parameter> &parameters, const std::string &name, const std::string &defaultValue)
{

    for(const Parameter &P : parameters){
        if(P.name == name){
            return P.value;
        }
    }
    return defaultValue;

}


int Parameter::GetIntValue(const std::vector<Parameter> &parameters, const std::string &name, int defaultValue)
{
    for(const Parameter &P : parameters){
        if(P.name == name){
            return std::stoi(P.value);
        }
    }
    return defaultValue;
}


float Parameter::GetFloatValue(const std::vector<Parameter> &parameters, const std::string &name, float defaultValue)
{
    for(const Parameter &P : parameters){
        if(P.name == name){
            return std::stof(P.value);
        }
    }
    return defaultValue;

}


Parameter Parameter::Parse(const std::string &s)
{

    Parameter p;

    std::size_t pos = s.find(":");

    if(pos != std::string::npos){

        p.name = s.substr(0, pos);
        p.value = s.substr(pos+1);

    }else{
        p.name = s;
    }

    RemoveStringWhiteSpacesOnStartAndEnd(p.name);
    RemoveStringWhiteSpacesOnStartAndEnd(p.value);

    return p;

}



//=============================================================================================



float Easing::GetValue(float p)
{

    switch (kind)
    {
    case Kind::LINEAR:
        return p;

    case Kind::EASE_START:
        return p*p;

    case Kind::EASE_END:
        return  1.0-(1.0-p)*(1.0-p);

    case Kind::EASE_START_END:
        return p < 0.5 ? 2*p*p : 1.0 - (2.0-2.0*p)*(2.0-2.0*p)/2.0;

    case Kind::CONSTANT:
        return 0;
    }

    return p;
}


//=============================================================================================



void Tween::Init(float _valueStart, float _valueEnd, float _durationS, Easing::Kind _easingKind)
{

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



}
