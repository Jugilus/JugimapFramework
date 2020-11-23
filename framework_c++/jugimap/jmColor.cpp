#include "../utf8cpp/utf8.h"
#include "jmSprites.h"
#include "jmCommonFunctions.h"
#include "jmColor.h"



namespace jugimap {





ColorRGBA ColorRGBA::ParseFromHexString(std::string _hexColor)
{

    RemoveStringWhiteSpacesOnStartAndEnd(_hexColor);

    std::string hexR = "FF";
    std::string hexG = "FF";
    std::string hexB = "FF";
    std::string hexA = "FF";

    if(_hexColor.length()>=6){
        hexR = _hexColor.substr(0,2);
        hexG = _hexColor.substr(2,2);
        hexB = _hexColor.substr(4,2);

    }
    if(_hexColor.length()==8){
        hexA = _hexColor.substr(6,2);

    }

    int r = std::stoi(hexR, nullptr, 16);
    int g = std::stoi(hexG, nullptr, 16);
    int b = std::stoi(hexB, nullptr, 16);
    int a = std::stoi(hexA, nullptr, 16);

    int rgba = r | (g << 8) | (b << 16) | (a << 24);

    return ColorRGBA(rgba);

}



bool ColorRGBAf::operator ==(ColorRGBAf _colorRGBA)
{
    return AreEqual(r,_colorRGBA.r) && AreEqual(g,_colorRGBA.g) && AreEqual(b,_colorRGBA.b) && AreEqual(a,_colorRGBA.a);
}



ColorRGBA ColorsLibrary::Find(const std::string &_name)
{

    for(auto p : colors){
        if(p.first == _name){
            return p.second;
        }
    }

    return ColorRGBA();
}


ColorRGBA ColorsLibrary::At(int _index)
{

    if(_index>=0 && _index<colors.size()){
        return colors[_index].second;
    }


    return ColorRGBA();
}



ColorsLibrary textColorsLibrary;


}



