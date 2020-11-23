#include <string>
#include <sstream>
#include "jmObjectFactory.h"
#include "jmFont.h"
#include "jmInput.h"
#include "jmGlobal.h"



namespace jugimap {



TextHandleVariant GetTextHandleVariantFromInt(int _id)
{

    if(_id==(int)TextHandleVariant::CENTER){
        return TextHandleVariant::CENTER;

    }else if(_id==(int)TextHandleVariant::LEFT_TOP){
        return TextHandleVariant::LEFT_TOP;

    }else if(_id==(int)TextHandleVariant::TOP){
        return TextHandleVariant::TOP;

    }else if(_id==(int)TextHandleVariant::RIGHT_TOP){
        return TextHandleVariant::RIGHT_TOP;

    }else if(_id==(int)TextHandleVariant::RIGHT){
        return TextHandleVariant::RIGHT;

    }else if(_id==(int)TextHandleVariant::RIGHT_BOTTOM){
        return TextHandleVariant::RIGHT_BOTTOM;

    }else if(_id==(int)TextHandleVariant::BOTTOM){
        return TextHandleVariant::BOTTOM;

    }else if(_id==(int)TextHandleVariant::LEFT_BOTTOM){
        return TextHandleVariant::LEFT_BOTTOM;

    }else if(_id==(int)TextHandleVariant::LEFT){
        return TextHandleVariant::LEFT;
    }

    return TextHandleVariant::CENTER;

}


Vec2f GetRelativeHandleFromTextHandleVariant(TextHandleVariant _thv)
{

    switch (_thv)
    {
    case TextHandleVariant::CENTER:
        return Vec2f(0.5f, 0.5f);

    case TextHandleVariant::LEFT_TOP:
        return settings.IsYCoordinateUp()? Vec2f(0.0f, 1.0f) : Vec2f(0.0f, 0.0f);

    case TextHandleVariant::TOP:
        return settings.IsYCoordinateUp()? Vec2f(0.5f, 1.0f) : Vec2f(0.5f, 0.0f);

    case TextHandleVariant::RIGHT_TOP:
        return settings.IsYCoordinateUp()? Vec2f(1.0f, 1.0f) : Vec2f(1.0f, 0.0f);

    case TextHandleVariant::RIGHT:
        return Vec2f(1.0f, 0.5f);

    case TextHandleVariant::RIGHT_BOTTOM:
        return settings.IsYCoordinateUp()? Vec2f(1.0f, 0.0f) : Vec2f(1.0f, 1.0f);

    case TextHandleVariant::BOTTOM:
        return settings.IsYCoordinateUp()? Vec2f(0.5f, 0.0f) : Vec2f(0.5f, 1.0f);

    case TextHandleVariant::LEFT_BOTTOM:
        return settings.IsYCoordinateUp()? Vec2f(0.0f, 0.0f) : Vec2f(0.0f, 1.0f);

    case TextHandleVariant::LEFT:
        return Vec2f(0.0f, 0.5f);

    }
}




Settings settings;
Time time;



void DeleteGlobalObjects()
{

    SourceGraphics::DeleteGlobalData();
    if(objectFactory) delete objectFactory;
    fontLibrary.DeleteData();


}




}
