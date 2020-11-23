#include <assert.h>
#include "jmMapBinaryLoader.h"
#include "jmCollision.h"
#include "jmSourceGraphics.h"
#include "jmSprites.h"
#include "jmLayers.h"
#include "jmFont.h"
#include "jmDrawing.h"
#include "jmMap.h"
#include "jmApp.h"
#include "jmScene.h"
#include "jmVectorShapes.h"
#include "jmStreams.h"
#include "jmInput.h"
#include "jmObjectFactory.h"




namespace jugimap{



ObjectFactory::ObjectFactory()
{

    //mouseSystemCursor = new SystemMouseCursor();

}


BinaryStreamReader* ObjectFactory::NewBinaryFileStreamReader(const std::string &fileName)
{
    return new StdBinaryFileStreamReader(fileName);
}


TextStreamReader* ObjectFactory::NewTextFileStreamReader(const std::string &fileName)
{
    return new StdTextFileStreamReader(fileName);
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


EngineApp* ObjectFactory::NewEngineApp(App *_app)
{
    return new EngineApp(_app);
}


EngineScene* ObjectFactory::NewEngineScene(Scene *_scene)
{
    return new EngineScene(_scene);
}


Map* ObjectFactory::NewMap(int _zOrderStart)
{
    return new Map(_zOrderStart);
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


//StandardSprite* ObjectFactory::NewStandardSprite(SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer)
//{
//    return new StandardSprite(_sourceSprite, _position, _spriteLayer);
//}


TextSprite* ObjectFactory::NewTextSprite()
{
    return new TextSprite();
}


//TextSprite* ObjectFactory::NewTextSprite(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle, SpriteLayer *_spriteLayer)
//{
//    return new TextSprite(_textString, _font, _color, _position, _textHandle, _spriteLayer);
//}


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

    }else if(_kind==SpriteKind::TEXT){

        return NewTextSprite();

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


/*
Text* ObjectFactory::NewText(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
{
    return new Text(_textLayer, _font, _textString, _position, _color);
}


TextLayer* ObjectFactory::NewTextLayer(const std::string &_name)
{
    return new TextLayer(_name);
}
*/


Font* ObjectFactory::NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind)
{
    return new Font(_relativeFilePath, _size, _fontKind);
}






//namespace glob{

ObjectFactory genericObjectFactory;
ObjectFactory *objectFactory = &genericObjectFactory;

//}


}
