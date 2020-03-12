#include <assert.h>
#include "jmMapBinaryLoader.h"
#include "jmCollision.h"
#include "jmSourceGraphics.h"
#include "jmSprites.h"
#include "jmLayers.h"
#include "jmText.h"
#include "jmDrawing.h"
#include "jmMap.h"
#include "jmVectorShapes.h"
#include "jmStreams.h"
#include "jmObjectFactory.h"




namespace jugimap{



BinaryStreamReader* ObjectFactory::NewBinaryFileStreamReader(const std::string &fileName)
{
    return new StdBinaryFileStreamReader(fileName);
}



GraphicFile* ObjectFactory::NewFile()
{
    return new GraphicFile();
}


GraphicItem* ObjectFactory::NewItem()
{
    return new GraphicItem();
}


SourceSprite* ObjectFactory::NewSourceSprite()
{
    return new SourceSprite();
}


Map* ObjectFactory::NewMap()
{
    return new Map();
}


SpriteLayer* ObjectFactory::NewSpriteLayer()
{
    return new SpriteLayer();
}


VectorLayer* ObjectFactory::NewVectorLayer()
{
    return new VectorLayer();
}


StandardSprite* ObjectFactory::NewStandardSprite()
{
    return new StandardSprite();
}


ComposedSprite* ObjectFactory::NewComposedSprite()
{
    return new ComposedSprite();
}


SkeletonSprite* ObjectFactory::NewSpineSprite()
{
    return new SkeletonSprite();
}


SkeletonSprite* ObjectFactory::NewSpriterSprite()
{
    return new SkeletonSprite();
}


//MultiShapesCollider* ObjectFactory::NewCollider()
//{
//    return new MultiShapesCollider;
//}



Sprite* ObjectFactory::NewSprite(SpriteKind _kind)
{
    if(_kind==SpriteKind::STANDARD){

        return NewStandardSprite();

    }else if(_kind==SpriteKind::COMPOSED){

        return NewComposedSprite();

    }else if(_kind==SpriteKind::SPINE){

        return NewSpineSprite();

    }else if(_kind==SpriteKind::SPRITER){

        return NewSpriterSprite();

    }

    assert(false);
    return nullptr;
}


Drawer* ObjectFactory::NewDrawer()
{
    return new Drawer();

}


Text* ObjectFactory::NewText(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
{
    return new Text(_textLayer, _font, _textString, _position, _color);
}


TextLayer* ObjectFactory::NewTextLayer(const std::string &_name)
{
    return new TextLayer(_name);
}


Font* ObjectFactory::NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind)
{
    return new Font(_relativeFilePath, _size, _fontKind);
}




//namespace glob{

ObjectFactory genericObjectFactory;
ObjectFactory *objectFactory = &genericObjectFactory;

//}


}
