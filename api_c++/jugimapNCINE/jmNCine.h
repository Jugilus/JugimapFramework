
//===================================================================================================
//      JugiMap API extension for nCIne.
//===================================================================================================

#ifndef JUGIMAP_NCINE_H
#define JUGIMAP_NCINE_H

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

    virtual void InitEngineSprite() override;
    virtual void UpdateEngineSprite() override;
    virtual void SetActiveImage(GraphicItem *_image) override;
    virtual void SetVisible(bool _visible) override;
    virtual bool IsEngineSpriteInitialized() override {return (ncSprite!=nullptr);}

    /// \brief Returns the nCine sprite object.
    ncine::Sprite * GetNCSprite() {return ncSprite;}

private:

    ncine::Sprite *ncSprite = nullptr;          // LINK

    void ManageShaders_OverlayColor();

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

private:
    ncine::Font* ncFont = nullptr;             // OWNED

};


/// \ingroup EngineExtension_nCine
/// \brief Extended Text class for nCine.
class TextNC : public Text
{
public:

    TextNC(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
        : Text(_textLayer, _font, _textString, _position, _color)
    {}

    virtual void InitEngineText();
    virtual void UpdateEngineText();
    virtual bool IsEngineTextInitialized(){ return (ncTextNode!=nullptr); }

    /// \brief Returns the nCine text object.
    ncine::TextNode* GetNCTextNode(){ return ncTextNode;}


private:
    ncine::TextNode* ncTextNode = nullptr;      // LINK
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

    virtual void InitEngineMap() override;
    virtual void UpdateEngineMap() override;

    /// \brief Returns the MapNCNode object.
    MapNCNode * GetMapNCNode(){return mMapNCNode;}

private:
    MapNCNode * mMapNCNode = nullptr;       // OWNED
};


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

    virtual GraphicFile* NewFile() override { return new GraphicFileNC(); }

    virtual Map* NewMap() override { return new MapNC(); }

    virtual SpriteLayer *NewSpriteLayer() override { return new SpriteLayerNC(); }

    virtual StandardSprite *NewStandardSprite() override { return new StandardSpriteNC (); }

    virtual Text* NewText(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color) override
    {
        return new TextNC(_textLayer, _font, _textString, _position, _color);
    }

    virtual Font* NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind) override
    {
        return new FontNC(_relativeFilePath, _size, _fontKind);
    }

    virtual TextLayer* NewTextLayer(const std::string &_name) override { return new TextLayerNC(_name); }

};


//===================================================================================================


void DeleteNCNode(ncine::SceneNode* _node);
extern bool deleteNCNodesSpecial;               // Use different way for deleting nodes with children nodes as 'delete node' causes crash in QCreator in debug mode (at least on my comp).

extern ncine::TextNode * errorMessageLabel;     // LINK


}




#endif
