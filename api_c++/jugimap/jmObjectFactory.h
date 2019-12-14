#ifndef JUGIMAP_OBJECT_FACTORY_H
#define JUGIMAP_OBJECT_FACTORY_H


#include <vector>
#include <string>
#include "jmSourceGraphics.h"


namespace jugimap{


class SourceGraphics;
class Map;
class GraphicFile;
class GraphicItem;
class SourceSprite;
class Sprite;
class StandardSprite;
class ComposedSprite;
class SkeletonSprite;
class VectorShape;
class MultiShapesCollider;
class FrameAnimation;
class SpriteLayer;
class DrawingLayer;
class Text;
class VectorLayer;
class TextLayer;
class Drawer;
class Font;
class BinaryStreamReader;



///\ingroup MapElements
///\brief A factory for jugimap objects.
///
/// Jugimap api supports multiple engines by extending jugimap classes with engine specific functionality.
/// For creating objects of these extended classes an engine extended ObjectFactory is used.
class ObjectFactory
{
public:

    virtual BinaryStreamReader* NewBinaryStreamReader(const std::string &fileName);

    /// Returns a new graphic file object.
    virtual GraphicFile* NewFile();

    /// Returns a new graphic item object.
    virtual GraphicItem* NewItem();

    /// Returns a new source sprite object.
    virtual SourceSprite* NewSourceSprite();

    /// Returns a new map object.
    virtual Map* NewMap();

    /// Returns a new sprite layer object.
    virtual SpriteLayer* NewSpriteLayer();

    /// Returns a new vector layer object.
    virtual VectorLayer* NewVectorLayer();

    /// Returns a new standard sprite object.
    virtual StandardSprite *NewStandardSprite();

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
    virtual Text* NewText(TextLayer* _textLayer, Font* _font, const std::string &_textString, Vec2f _position, ColorRGBA _color);

    /// Returns a new text layer object.
    virtual TextLayer* NewTextLayer(const std::string &_name);

    /// Returns a new font object.
    virtual Font* NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind);

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
