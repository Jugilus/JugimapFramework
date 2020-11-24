
//===================================================================================================
//      JugiMap API extension for nCIne.
//===================================================================================================

#ifndef JUGIMAP_NCINE_H
#define JUGIMAP_NCINE_H

#include <ncine/imgui.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/DrawableNode.h>
#include <ncine/SceneNode.h>
#include <ncine/TextNode.h>
#include <ncine/Font.h>
#include <ncine/IFile.h>
#include "../jugimap/jugimap.h"

#if defined(__ANDROID__)
#include <ncine/AssetFile.h>
#endif





namespace jugimap {



#if defined(__ANDROID__)


class AndroidBinaryFileStreamReaderNC : public BinaryStreamReader
{
public:

    AndroidBinaryFileStreamReaderNC(const std::string &fileName)
    {
        ncIFile = new ncine::AssetFile(fileName.c_str());
        ncIFile->open(ncine::IFile::OpenMode::READ | ncine::IFile::OpenMode::BINARY);
    }

    ~AndroidBinaryFileStreamReaderNC(){ if(ncIFile) delete ncIFile; }


    bool IsOpen() override {return ncIFile->isOpened();}
    int Pos() override {return ncIFile->tell();}
    void Seek(int _pos) override {ncIFile->seek(_pos, SEEK_SET);}
    int Size() override {return ncIFile->size();}
    void Close() override {ncIFile->close();}


    unsigned char ReadByte()  override
    {
        unsigned char value;
        ncIFile->read(reinterpret_cast<char*>(&value), 1);
        return value;
    }

    int ReadInt()  override
    {
        int value;
        ncIFile->read(reinterpret_cast<char*>(&value), 4);
        return value;
    }

    float ReadFloat()  override
    {
        float value;
        ncIFile->read(reinterpret_cast<char*>(&value), 4);
        return value;
    }


    std::string ReadString() override
    {
        int length = ReadInt();
        char *buf = new char[length];
        ncIFile->read(buf, length);
        std::string value(buf, length);
        delete[] buf;
        return value;
    }


private:
    ncine::IFile *ncIFile = nullptr;    // OWNED
};

#endif



/// \ingroup EngineExtension_nCine
/// \brief Extended GraphicFile class for nCine.
class GraphicFileNC : public GraphicFile
{
public:

    GraphicFileNC(){}
    virtual ~GraphicFileNC() override;

    virtual void Init() override;

    /// \brief Returns the nCine texture object.
    ncine::Texture *GetNCTexture() {return ncTexture;}

private:
    ncine::Texture *ncTexture = nullptr;    // OWNED
};


//===================================================================================================


/// \ingroup EngineExtension_nCine
/// \brief Extended StandardSprite class for nCine.
class StandardSpriteNC  : public StandardSprite
{
public:

    StandardSpriteNC(){}
    //StandardSpriteNC(SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer);

    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual void SetActiveImage(GraphicItem *_image) override;
    //virtual void SetVisible(bool _visible) override;
    virtual bool IsEngineSpriteInitialized() override {return (ncSprite!=nullptr);}

    /// \brief Returns the nCine sprite object.
    ncine::Sprite * GetNCSprite() {return ncSprite;}

private:

    ncine::Sprite *ncSprite = nullptr;          // LINK

    void ManageShaders_OverlayColor();

};


//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended TextSprite class for Cocos2d-x.
class TextSpriteNC  : public TextSprite
{
public:

    TextSpriteNC(){}
    //TextSpriteNC(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle, SpriteLayer *_spriteLayer);

    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual bool IsEngineSpriteInitialized() override { return (ncTextNode!=nullptr); }

    //---
    /// \brief Returns the nCine text node.
    ncine::TextNode* GetNCTextNode(){ return ncTextNode;}


private:
    ncine::TextNode* ncTextNode = nullptr;      // LINK
    bool emptyString = false;

};


//===================================================================================================


/// \ingroup EngineExtension_nCine
/// \brief Extended SpriteLayer class for nCine.
class SpriteLayerNC : public SpriteLayer
{
public:

    virtual void RemoveAndDeleteSprite(Sprite* _sprite) override;

};



//===================================================================================================

/// \ingroup EngineExtension_nCine
/// \brief Extended Font class for nCine.
class FontNC : public Font
{
public:

    FontNC(const std::string &_relativeFilePath, int _size, FontKind _kind);
    virtual ~FontNC();

    /// \brief Returns the nCine font object.
    ncine::Font* GetNCFont(){ return ncFont;}

    virtual int GetPixelWidth(const std::string &s) override;
    virtual int GetPixelHeight(const std::string &s) override;
    virtual Vec2f GetPixelSize(const std::string &s) override;

private:
    ncine::Font* ncFont = nullptr;             // OWNED
    ncine::TextNode* ncTextNode = nullptr;      // OWNED

    ncine::SceneNode *dummySceneNode = nullptr;
    std::string text;
};


/*
/// \ingroup EngineExtension_nCine
/// \brief Extended Text class for nCine.
class TextNC : public Text
{
public:

    TextNC(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color);


    virtual void UpdateEngineText();
    virtual bool IsEngineTextInitialized(){ return (ncTextNode!=nullptr); }

    /// \brief Returns the nCine text object.
    ncine::TextNode* GetNCTextNode(){ return ncTextNode;}


private:
    ncine::TextNode* ncTextNode = nullptr;      // LINK
    bool emptyString = false;
};


//===================================================================================================


/// \ingroup EngineExtension_nCine
/// \brief Extended TextLayer class for nCine.
class TextLayerNC : public TextLayer
{
public:

    TextLayerNC(const std::string &_name) : TextLayer(_name) {}

    virtual void RemoveAndDeleteText(Text* _text) override;

};
*/

//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended Drawer class for Cocos2d-x.
class DrawerNC : public Drawer
{
public:

    virtual void InitEngineDrawer() override;
    virtual void UpdateEngineDrawer() override;

    virtual void Clear() override;

    virtual void SetOutlineColor(ColorRGBA _outlineColor) override;
    virtual void Line(Vec2f p1, Vec2f p2) override;
    virtual void RectangleOutline(const Rectf &rect) override;
    virtual void EllipseOutline(Vec2f c, Vec2f r) override;
    virtual void Dot(Vec2f p) override;


private:
    //cocos2d::Color4F outlineColor;
    //cocos2d::DrawNode * drawNode = nullptr;         // LINK   ( Cocos2d handles ownership of engine objects. )

    ImDrawList* drawList = nullptr;
    ImColor outlineColor;
};


//===================================================================================================


class MapNC;

/// \ingroup EngineExtension_nCine
/// \brief Extended ncine::SceneNode class for use with MapNC objects.
class MapNCNode : public ncine::SceneNode
{
public:
    explicit MapNCNode(MapNC * _map) : SceneNode() { map = _map;}

    virtual void visit(ncine::RenderQueue &renderQueue) override;

private:
    MapNC * map = nullptr;                  // LINK
};



//===================================================================================================


/// \ingroup EngineExtension_nCine
/// \brief Extended Map class for nCine.
class MapNC : public Map
{
public:

    MapNC(int _zOrderStart) : Map(_zOrderStart){}
    virtual void InitEngineObjects(MapType _mapType) override;
    virtual void UpdateEngineObjects() override;
    virtual void SetVisible(bool _visible) override;

    /// \brief Returns the MapNCNode object.
    MapNCNode * GetMapNCNode(){return mMapNCNode;}

private:
    MapNCNode * mMapNCNode = nullptr;                       // LINK
};



//===================================================================================================


class EngineSceneCC;


class EngineSceneNC : public EngineScene
{
public:

    EngineSceneNC(Scene *_scene);
    void PostInit() override;
   // void SetSystemMouseCursorVisible(bool _visible) override;
    //SceneCCNode* GetSceneNode(){ return ccSceneNode; }
    ncine::SceneNode* GetEngineNode(){ return ncNode; }

private:
    ncine::SceneNode *ncNode = nullptr;                     // LINK
};


/*
class EngineAppCC;


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended cocos2d::Scene class for use with jugimap::Scene objects.
class AppNCNode : public ncine::SceneNode
{
public:
    friend class EngineAppCC;


    ///\brief Create SceneCCNode object with the given _jmScene object.
    ///
    /// This function should be called inside extended jugimap::Scene initialization.
    AppNCNode(jugimap::App* _jmApp) : SceneNode() { jmApp = _jmApp;}

    ///\brief Destructor.
    virtual ~AppNCNode();

    ///\brief Initialize this object.
    ///
    /// This function is called by SceneCCNode::Create.
    //virtual bool init() override;

    ///\brief Update this scene object.
    virtual void update(float delta) override;

    virtual void visit(ncine::RenderQueue &renderQueue) override;

    ///\brief Get jugimap::Scene object link.
    jugimap::App * GetJMApp(){ return jmApp; }


private:
    jugimap::App *jmApp = nullptr;                      // LINK
    ncine::TextNode *errorMessageLabel = nullptr;
};

*/


class EngineAppNC : public EngineApp
{
public:

    EngineAppNC(App *_app);
    void PostInit() override;
    void SetSystemMouseCursorVisible(bool _visible) override;
    ncine::SceneNode * GetAppNode(){ return appNCNode; }


private:
    ncine::SceneNode * appNCNode = nullptr;             // LINK
    ncine::TextNode *errorMessageLabel = nullptr;       // LINK
};

//===================================================================================================


/*
class CommandsNC : public Commands
{
public:

    virtual void PreAppUpdate() override;
    virtual void PostAppUpdate() override;
    virtual void SetSystemMouseCursorVisible(bool _visible) override;

};
*/


//===================================================================================================


/// \ingroup EngineExtension_nCine
/// \brief Extended ObjectFactory class for nCine.
class ObjectFactoryNC : public ObjectFactory
{
public:


#if defined(__ANDROID__)
    virtual BinaryStreamReader* NewBinaryFileStreamReader(const std::string &fileName) override
    {
        return new AndroidBinaryFileStreamReaderNC(fileName);
    }
#endif

    virtual EngineApp* NewEngineApp(App* _app) override { return new EngineAppNC(_app); }
    virtual EngineScene* NewEngineScene(Scene *_scene) override { return new EngineSceneNC(_scene); }
    virtual GraphicFile* NewFile() override { return new GraphicFileNC(); }
    virtual Map* NewMap(int _zOrderStart) override { return new MapNC(_zOrderStart); }
    virtual SpriteLayer *NewSpriteLayer() override { return new SpriteLayerNC(); }
    virtual StandardSprite *NewStandardSprite() override { return new StandardSpriteNC (); }
    //virtual StandardSprite *NewStandardSprite(SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer) override
    //{
    //    return new StandardSpriteNC(_sourceSprite, _position, _spriteLayer);
    //}
    virtual TextSprite *NewTextSprite() override { return new TextSpriteNC(); }
    //virtual TextSprite *NewTextSprite(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle, SpriteLayer *_spriteLayer) override
    //{
    //    return new TextSpriteNC (_textString, _font, _color, _position, _textHandle, _spriteLayer);
    //}
    virtual Font* NewFont(const std::string &_relativeFilePath, int _size=-1, FontKind _fontKind = FontKind::NOT_DEFINED) override
    {
        return new FontNC(_relativeFilePath, _size, _fontKind);
    }
    virtual Drawer* NewDrawer() override { return new DrawerNC(); }


    //virtual TextLayer* NewTextLayer(const std::string &_name) override { return new TextLayerNC(_name); }

};


//===================================================================================================


//void DeleteNCNode(ncine::SceneNode* _node);
//extern bool deleteNCNodesSpecial;               // Use different way for deleting nodes with children nodes as 'delete node' causes crash in QCreator in debug mode (at least on my comp).

extern ncine::TextNode * errorMessageLabel;     // LINK


}




#endif
