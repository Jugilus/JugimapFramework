#include "jmSprites.h"
#include "jmText.h"



namespace jugimap {



std::string Font::pathPrefix;


//==================================================================================================


Text::Text(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
    : textLayer(_textLayer),
      font(_font),
      textString(_textString),
      position(_position),
      color(_color)
{
    textLayer->GetTexts().push_back(this);
}


Text::~Text()
{
}


Vec2f Text::GetGlobalPosition()
{
    return (GetLayer()->GetParentComposedSprite()!=nullptr)? GetLayer()->GetParentComposedSprite()->ConvertToWorldPos(position) : position;
}


void Text::SetChangeFlags(int _changeFlags)
{
    changeFlags = _changeFlags;
    if(changeFlags !=0){
        textLayer->RequireEngineLayerUpdate(true);
    }
}


void Text::AppendToChangeFlags(int _changeFlags)
{
    changeFlags |= _changeFlags;
    if(changeFlags !=0 ){
        textLayer->RequireEngineLayerUpdate(true);
    }
}


//==================================================================================================


TextLayer::TextLayer(const std::string &_name)
{
    _SetName(_name);
    _SetKind(LayerKind::TEXT);
}


TextLayer::~TextLayer()
{
    for(Text *t :texts){
        delete t;
    }
}


void TextLayer::InitEngineLayer()
{
    for(Text *t : texts){
        t->InitEngineText();
    }
}


void TextLayer::UpdateEngineLayer()
{
    if(engineLayerUpdateRequired){
        for(Text *t : texts){
            t->UpdateEngineText();
        }
    }
    engineLayerUpdateRequired = false;
}


void TextLayer::DrawEngineLayer()
{
    for(Text *t : texts){
        t->DrawEngineText();
    }
}


void TextLayer::RemoveAndDeleteText(Text *_text)
{

    if(_text->GetLayer()!=this) return;

    std::vector<Text*>::iterator i = texts.begin();
    while (i != texts.end())
    {
        if(_text == *i){
            i = texts.erase(i);
        }else{
            i++;
        }
    }
    delete _text;
}



}



