#include "../utf8cpp/utf8.h"
#include "jmSprites.h"
#include "jmFont.h"



namespace jugimap {



std::string Font::pathPrefix;
std::string Font::textForHeight = "ABCXYZgqj";




FontsLibrary::~FontsLibrary()
{
    DeleteData();
}


void FontsLibrary::DeleteData()
{
    for(auto f : fonts){
        delete f.second;
    }
    fonts.clear();
}


Font* FontsLibrary::Find(const std::string &_name)
{

    for(auto f : fonts){
        if(f.first == _name){
            return f.second;
        }
    }

    if(fonts.empty()==false){
        return fonts.front().second;
    }

    return nullptr;;
}


Font* FontsLibrary::At(int _index)
{

    if(_index>=0 && _index<fonts.size()){
        return fonts[_index].second;
    }

    if(fonts.empty()==false){
        return fonts.front().second;
    }

    return nullptr;
}



//std::vector<Font*>fontLibrary;

FontsLibrary fontLibrary;




}



