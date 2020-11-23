#ifndef JUGIMAP_OBJECT_FACTORY_H
#define JUGIMAP_OBJECT_FACTORY_H


#include <vector>
#include <string>
#include "jmSourceGraphics.h"


namespace jugimap{


class EngineApp;
class App;
class EngineScene;
class SourceGraphics;
class Scene;
class Map;
class GraphicFile;
class GraphicItem;
class SourceSprite;
class Sprite;
class StandardSprite;
class TextSprite;
class ComposedSprite;
class SkeletonSprite;
class VectorShape;
class MultiShapesCollider;
class FrameAnimation;
class SpriteLayer;
class DrawingLayer;
//class Text;
class VectorLayer;
//class TextLayer;
class Drawer;
class Font;
class BinaryStreamReader;
class TextStreamReader;
class SystemMouseCursor;



///\ingroup MapElements
///\brief The ObjectFactory is the base class for engine specific object factories.
///
/// Jugimap api supports engines by extending jugimap classes with engine specific properites.
/// For creating objects of these extended classes you must use the engine extended ObjectFactory object.
class ObjectFactory
{
public:

    ObjectFactory();
    virtual ~ObjectFactory(){}


    virtual BinaryStreamReader* NewBinaryFileStreamReader(const std::string &fileName);

    virtual TextStreamReader* NewTextFileStreamReader(const std::string &fileName);

    /// Returns a new graphic file object.
    virtual GraphicFile* NewFile();

    /// Returns a new graphic item object.
    virtual GraphicItem* NewItem();

    /// Returns a new source sprite object.
    virtual SourceSprite* NewSourceSprite();

    /// Returns a new map object.
    virtual EngineApp* NewEngineApp(App *_app);

    /// Returns a new map object.
    virtual EngineScene* NewEngineScene(Scene *_scene);

    /// Returns a new map object.
    virtual Map* NewMap(int _zOrderStart);

    /// Returns a new sprite layer object.
    virtual SpriteLayer* NewSpriteLayer();

    /// Returns a new vector layer object.
    virtual VectorLayer* NewVectorLayer();

    /// Returns a new standard sprite object.
    virtual StandardSprite *NewStandardSprite();

    /// Returns a new standard sprite object.
    //virtual StandardSprite *NewStandardSprite(SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer);

    /// Returns a new text sprite object.
    virtual TextSprite *NewTextSprite();

    /// Returns a new text sprite object.
    //virtual TextSprite* NewTextSprite(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle, SpriteLayer *_spriteLayer);

    /// Returns a new composed sprite object.
    virtual ComposedSprite* NewComposedSprite();

    /// Returns a new spine object.
    virtual SkeletonSprite *NewSpineSprite();

    /// Returns a new spriter object.
    virtual SkeletonSprite *NewSpriterSprite();

    /// Returns a new sprite object of the given *_kind*.
    Sprite* NewSprite(SpriteKind _kind);

    /// Returns a new drawer object.
    virtual Drawer* NewDrawer();

    /// Returns a new text object.
    //virtual Text* NewText(TextLayer* _textLayer, Font* _font, const std::string &_textString, Vec2f _position, ColorRGBA _color);

    /// Returns a new text layer object.
    //virtual TextLayer* NewTextLayer(const std::string &_name);

    /// Returns a new font object.
    virtual Font* NewFont(const std::string &_relativeFilePath, int _size=-1, FontKind _fontKind = FontKind::NOT_DEFINED);


};


//namespace glob{

extern ObjectFactory genericObjectFactory;

/// \ingroup Global
/// The ObjectFactory object used for creating jugimap objects.
///
/// This object must be created as an engine extended ObjectFactory.
/// \see ObjectFactory
extern ObjectFactory *objectFactory;

//}



}


#endif
