#ifndef JUGIMAP_SOURCE_GRAPHICS_H
#define JUGIMAP_SOURCE_GRAPHICS_H


#include "jmCommon.h"
#include "jmGlobal.h"



namespace jugimap {


class VectorShape;
class GraphicFile;
class SourceSprite;
class FrameAnimation;
class TimelineAnimation;
class StandardSprite;
class ComposedSprite;
class SkeletonSprite;
class SourceGraphics;
class SpriteLayer;
class VectorLayer;
class Administrator;
class JugiMapBinaryLoader;



/// \ingroup MapElements
/// \brief Graphic item.
///
/// The GraphicItem class defines parameters related to graphic items.
class GraphicItem
{
public:
    friend class JugiMapBinaryLoader;
    friend class Administrator;
    friend class GraphicFile;
    friend class SourceSprite;
    friend class GraphicFile;
    friend class StandardSprite;
    friend class SkeletonSprite;


    /// Constructor.
    GraphicItem(){}

    GraphicItem(const GraphicItem &gi) = delete;
    GraphicItem & operator=(const GraphicItem &gi) = delete;

    /// Destructor.
    virtual ~GraphicItem();


    /// \brief Initialize this graphic item.
    ///
    /// This function is called by SourceSprite::Init and should not be called manually.
    virtual void Init();


    /// \brief Returns the graphic file of this graphic item.
    GraphicFile* GetFile() {return graphicFile;}


    /// \brief Returns the name of this graphic item.
    std::string GetName() {return name;}


    /// \brief Returns the position of sub-image if this graphic item belong to an image file.
    Vec2i GetPos() {return rect.min;}


    /// \brief Returns the width of sub-image if this graphic item belongs to an image file.
    int GetWidth() {return rect.Width();}


    /// \brief Returns the height of sub-image if this graphic item belongs to an image file.
    int GetHeight() {return rect.Height();}


    /// \brief Returns the size of sub-image if this graphic item belongs to an image file.
    Vec2i GetSize() {return rect.Size();}


    /// \brief Returns the handle of sub-image if this graphic item belongs to an image file.
    Vec2f GetHandle() {return handle;}


    /// \brief Returns the spine scale if this graphic item belongs to a Spine file.
    float GetSpineScale() {return spineScale;}


    /// \brief Returns a reference to sprite shapes if this graphic item belong to an image file.
    std::vector<VectorShape*>& GetSpriteShapes() {return spriteShapes;}


    /// \brief Returns a reference to probe shapes if this graphic item belong to an image file.
    std::vector<VectorShape*>& GetProbeShapes() {return probeShapes;}



private:
    GraphicFile *graphicFile = nullptr;             //LINK pointer to parent file.
    std::string name;

    // (sub)image rectangle
    Recti rect;

    float spineScale = 1.0;
    Vec2f handle;
    std::vector<VectorShape*>spriteShapes;          // OWNED
    std::vector<VectorShape*>probeShapes;           // OWNED
};


//=======================================================================================

/// \ingroup MapElements
/// \brief Graphic file.
///
/// The GraphicFile class defines parameters related to graphic files.
/// It stores and owns GraphicItem objects.
class GraphicFile
{
public:
    friend class JugiMapBinaryLoader;
    friend class Administrator;
    friend class GraphicItem;
    friend class StandardSprite;
    friend class SkeletonSprite;
    friend class SourceGraphics;

    static std::string pathPrefix;          ///< An optional string attached to stored file paths. It must end with slash '/'!


    /// Constructor.
    GraphicFile(){}
    GraphicFile(const GraphicFile &gf) = delete;
    GraphicFile & operator=(const GraphicFile &gf) = delete;


    /// Destructor.
    virtual ~GraphicFile();


    /// \brief Initialize this GraphicFile.
    ///
    /// This function is called by GraphicItem::Init and should not be called manually.
    virtual void Init(){}


    /// \brief Returns the kind of this file.
    FileKind GetKind() {return kind;}


    /// \brief Returns the relative file path of this file.
    std::string GetRelativeFilePath() {return pathPrefix + relativeFilePath;}


    /// \brief Returns the size of image if this file is an image.
    Vec2i GetSize() {return size;}


    /// \brief Returns a reference to the vector of stored graphic items in this file.
    std::vector<GraphicItem*>& GetItems() {return items;}


    /// \brief Returns the relative file path of the spine atlas if this file is a Spine file.
    std::string GetSpineAtlasRelativeFilePath() {return pathPrefix + spineAtlasRelativeFilePath;}


private:
    FileKind kind = FileKind::NOT_DEFINED;
    std::string relativeFilePath;
    Vec2i size;
    std::vector<GraphicItem*>items;                 // OWNED objects
    std::string spineAtlasRelativeFilePath;
};


//=======================================================================================





/// \ingroup MapElements
/// \brief Source sprite.
///
/// The SourceSprite class represents source sprites from JugiMap Editor.
/// It defines common sprite data and parameters shared by all Sprite objects with the same SourceSprite object.
/// For StandardSprite objects these data includes textures, shapes, animations etc.
class SourceSprite
{
public:
    friend class JugiMapBinaryLoader;
    friend class Administrator;
    friend class JMSprite;
    friend class StandardSprite;
    friend class ComposedSprite;
    friend class SkeletonSprite;


    /// The status flags for standard source sprites (SpriteKind::STANDARD).
    enum Status{
        HAS_IMAGES_OF_DIFFERENT_SIZE = 1,                       ///< Has images of different size.
        HAS_IMAGES_WITH_SPRITE_SHAPES = 2,                      ///< Has image(s) with defined sprite shapes.
        HAS_IMAGES_WITH_DIFFERENT_SPRITE_SHAPES = 4,         ///< Has more images and they do not have all the same sprite shapes.
        HAS_MULTIPLE_SPRITE_SHAPES_IN_IMAGE = 8                 ///< The source sprite has image(s) with multiple sprite shapes.
    };


    /// Constructor.
    SourceSprite(){}

    SourceSprite(const SourceSprite &ss) = delete;
    SourceSprite & operator=(const SourceSprite &ss) = delete;

    /// Destructor.
    ~SourceSprite();


    /// \brief Initialize this source sprite.
    ///
    /// This function is called by Sprite::InitEngineSprite and should not be called manually.
    void Init();


    /// \brief Returns the kind of this source sprite.
    SpriteKind GetKind() {return kind;}


    /// \brief Returns the name of this source sprite.
    std::string GetName() {return name;}


    /// \brief Returns a reference to the vector of stored constant parameters in this source sprite.
    std::vector<Parameter>& GetConstantParameters(){return constantParameters;}


    /// \brief Returns a reference to the vector of graphic items in this source sprite.
    std::vector<GraphicItem*>& GetGraphicItems(){return graphicItems;}


    /// \brief Returns a reference to the vector of stored frame animations in this source sprite.
    std::vector<FrameAnimation*>& GetFrameAnimations(){return frameAnimations;}


    /// \brief Returns a reference to the vector of stored timeline animations in this source sprite.
    std::vector<TimelineAnimation*>& GetTimelineAnimations(){return timelineAnimations;}


    /// \brief Returns the status flags.
    int GetStatusFlags() {return statusFlags;}


    /// \brief Returns a pointer to the owned source composed sprite or nullptr if there is none.
    ComposedSprite* GetSourceComposedSprite(){return sourceComposedSprite;}


private:
    SpriteKind kind = SpriteKind::NOT_DEFINED;
    std::string name;
    std::vector<Parameter> constantParameters;
    std::vector<GraphicItem*>graphicItems;                     // LINKs
    std::vector<FrameAnimation*>frameAnimations;               // OWNED
    std::vector<TimelineAnimation*>timelineAnimations;          // OWNED

    std::vector<Parameter> parametersTMP;

    int statusFlags = 0;
    bool initialized = false;

    ComposedSprite * sourceComposedSprite = nullptr;          // OWNED
};



//=======================================================================================

class SourceGraphics
{
public:
    friend class JugiMapBinaryLoader;

    /// Constructor.
    SourceGraphics(){}


    /// Destructor.
    ~SourceGraphics();


    ///\brief Delete stored SourceGraphics objects and GraphicFile objects.
    void DeleteContent();


    ///\brief Returns a reference to the vector of stored graphic files.
    std::vector<GraphicFile*>& GetFiles(){return files;}


    ///\brief Returns a reference to the vector of stored source sprites.
    std::vector<SourceSprite*>& GetSourceSprites(){return sourceSprites;}



private:
    std::vector<GraphicFile*>files;               //OWNED
    std::vector<SourceSprite*>sourceSprites;      //OWNED

};


/// \ingroup MapElements
/// \class SourceGraphics
/// \brief The graphics storage.
///
/// The SourceGraphics class represents source graphics elements from JugiMap Editor.
/// It stores and owns SourceSprite objects and GraphicFile objects.
///
/// The following code will create a SourceGraphics object and load data from a file into it.
/// \code
///    SourceGraphics* sourceGraphics = new SourceGraphics();
///    if(JugiMapBinaryLoader::LoadSourceGraphics(sourceGraphicsFilePath, sourceGraphics)){
///        // loading successful !
///    }
/// \endcode
///
/// **Important:** The JugiMapBinaryLoader::LoadSourceGraphics function does **not** load any actual images or other files.
/// It only loads from the source graphics file various parameters and smaller bits of data. Actual loading of graphic data is
/// carried out at the initialization of engine objects during map initialization.


}






#endif
