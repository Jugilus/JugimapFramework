#include <string>
#include <sstream>
#include "jmCommon.h"



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


std::string Parameter::Value(const std::vector<Parameter> &parameters, const std::string &name, const std::string &defaultValue)
{

    for(const Parameter &P : parameters){
        if(P.name == name){
            return P.value;
        }
    }
    return defaultValue;

}


int Parameter::IntValue(const std::vector<Parameter> &parameters, const std::string &name, int defaultValue)
{
    for(const Parameter &P : parameters){
        if(P.name == name){
            return std::stoi(P.value);
        }
    }
    return defaultValue;
}


float Parameter::FloatValue(const std::vector<Parameter> &parameters, const std::string &name, float defaultValue)
{
    for(const Parameter &P : parameters){
        if(P.name == name){
            return std::stof(P.value);
        }
    }
    return defaultValue;

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








}
