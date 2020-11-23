
//===================================================================================================
//      JugiMap API extension for SFML.
//===================================================================================================

#ifndef JUGIMAP_SFML_H
#define JUGIMAP_SFML_H

#include <codecvt>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include "../jugimap/jugimap.h"




namespace jugimap {


/// \ingroup EngineExtension_SFML
/// \brief Extended BinaryStreamReader class for SFML.
class BinaryFileStreamReaderSFML : public BinaryStreamReader
{
public:

    ///\brief Constructor
    ///
    /// Creates a new BinaryStreamReaderSFML object and open the file *fileName* for reading.
    /// This reader utilizes SFML's reader and the file path must follow the requirements of that reader.
    BinaryFileStreamReaderSFML(const std::string &fileName)
    {
        opened = fis.open(fileName);
    }


    ~BinaryFileStreamReaderSFML() override
    {
        if(opened){
            Close();
        }
    }


    bool IsOpen() override {return opened;}
    int Pos() override {return fis.tell();}
    void Seek(int _pos) override {fis.seek(_pos);}
    int Size() override {return fis.getSize();}
    void Close() override {}


    unsigned char ReadByte()  override
    {
        unsigned char value;
        fis.read(reinterpret_cast<char*>(&value), 1);
        return value;
    }

    int ReadInt()  override
    {
        int value;
        fis.read(reinterpret_cast<char*>(&value), 4);
        return value;
    }

    float ReadFloat()  override
    {
        float value;
        fis.read(reinterpret_cast<char*>(&value), 4);
        return value;
    }


    std::string ReadString() override;


private:
    sf::FileInputStream fis;
    bool opened = false;
};



//===================================================================================================



/// \ingroup EngineExtension_SFML
/// \brief Extended GraphicFile class for SFML.
class GraphicFileSFML : public GraphicFile
{
public:
    static bool textureSmoothFilter;        ///< Set true for smooth texture filtering; set false for no filtering.

    GraphicFileSFML(){}
    virtual ~GraphicFileSFML() override;

    virtual void Init() override;

    /// \brief Returns the SFML texture object.
    const sf::Texture * GetSFTexture() const {return sfTexture;}

private:
    sf::Texture *sfTexture = nullptr;
};


//===================================================================================================


/// \ingroup EngineExtension_SFML
/// \brief Extended StandardSprite class for SFML.
class StandardSpriteSFML  : public StandardSprite
{
public:

    //StandardSpriteSFML (){}
    //StandardSpriteSFML (SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer);

    virtual ~StandardSpriteSFML () override;


    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual void SetActiveImage(GraphicItem *_image) override;
    virtual void DrawEngineSprite() override;
    virtual void CaptureForLerpDrawing() override;

    /// \brief Returns the SFML sprite object.
    sf::Sprite * GetSFSprite() {return sfSprite;}

private:

    sf::Sprite *sfSprite = nullptr;                 // OWNED data!
    sf::BlendMode sfBlend;
    jugimap::Vec2f posGlobal;
    jugimap::Vec2f posGlobalPrevious;
    sf::Shader *shader = nullptr;                   // LINK pointer

    int drawChangeFlags = 0;

    void ManageShaders_OverlayColor();

};


//===================================================================================================


/// \ingroup EngineExtension_SFML
/// \brief Extended TextSprite class for SFML.
class TextSpriteSFML  : public TextSprite
{
public:

    static Vec2f posOffset;

    //TextSpriteSFML(){}
    //TextSpriteSFML(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle, SpriteLayer *_spriteLayer);
    virtual ~TextSpriteSFML();

    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual bool IsEngineSpriteInitialized() override { return (sfText!=nullptr); }
    virtual void DrawEngineSprite() override;

    //---
    /// \brief Returns the SFML text object.
    sf::Text* GetSFText(){ return sfText;}


private:
    sf::Text* sfText = nullptr;              // OWNED

};


//===================================================================================================


class ShaderSFML_ColorOverlay_SimpleMultiply;
class ShaderSFML_ColorOverlay_Normal;
class ShaderSFML_ColorOverlay_Multiply;
class ShaderSFML_ColorOverlay_LinearDodge;
class ShaderSFML_ColorOverlay_Color;

class FragmentShaderSFML_ColorOverlay
{
public:
    friend class ShaderSFML_ColorOverlay_SimpleMultiply;
    friend class ShaderSFML_ColorOverlay_Normal;
    friend class ShaderSFML_ColorOverlay_Multiply;
    friend class ShaderSFML_ColorOverlay_LinearDodge;
    friend class ShaderSFML_ColorOverlay_Color;


    virtual ~FragmentShaderSFML_ColorOverlay(){ if(shader) delete shader; }
    void Load(const std::string &_filePath);

    sf::Shader *GetShader(){ return shader; }
    ColorRGBA GetColorRGBA(){ return colorRGBA; }
    void SetColorRGBA(ColorRGBA _colorRGBA){ colorRGBA = _colorRGBA; }
    const std::string &GetColorRGBAname(){ return colorRGBAname; }


private:
    sf::Shader *shader = nullptr;

    //--- uniforms
    ColorRGBA colorRGBA{255, 255, 255, 128};
    std::string colorRGBAname = "overlayColor";
};


class ShaderSFML_ColorOverlay_SimpleMultiply : public FragmentShaderSFML_ColorOverlay
{};


class ShaderSFML_ColorOverlay_Normal : public FragmentShaderSFML_ColorOverlay
{};


class ShaderSFML_ColorOverlay_Multiply : public FragmentShaderSFML_ColorOverlay
{};


class ShaderSFML_ColorOverlay_LinearDodge : public FragmentShaderSFML_ColorOverlay
{};


class ShaderSFML_ColorOverlay_Color : public FragmentShaderSFML_ColorOverlay
{};


namespace shaders
{
    extern ShaderSFML_ColorOverlay_SimpleMultiply ColorOverlay_SimpleMultiply;
    extern ShaderSFML_ColorOverlay_Normal ColorOverlay_Normal;
    extern ShaderSFML_ColorOverlay_Multiply ColorOverlay_Multiply;
    extern ShaderSFML_ColorOverlay_LinearDodge ColorOverlay_LinearDodge;
    extern ShaderSFML_ColorOverlay_Color ColorOverlay_Color;

    void LoadJugimapShaders();

}


//===================================================================================================


/// \ingroup EngineExtension_SFML
/// \brief Extended Font class for SFML.
class FontSFML : public Font
{
public:
    static std::wstring_convert<std::codecvt_utf8<sf::Uint32>, sf::Uint32> conv;

    FontSFML(const std::string &_relativeFilePath, int _size, FontKind _kind);
    ~FontSFML();

    virtual int GetPixelWidth(const std::string &s) override;
    virtual int GetPixelHeight(const std::string &s) override;
    virtual Vec2f GetPixelSize(const std::string &s) override;

    /// \brief Returns the SFML font object.
    sf::Font* GetSFFont(){ return sfFont;}

private:
    sf::Font* sfFont = nullptr;             // OWNED
    sf::Text* sfText = nullptr;
};




/*

/// \ingroup EngineExtension_SFML
/// \brief Extended Text class for SFML.
class TextSFML : public Text
{
public:
    static std::wstring_convert<std::codecvt_utf8<sf::Uint32>, sf::Uint32> conv;

    TextSFML(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color);

    virtual ~TextSFML();

    //virtual void InitEngineText();
    virtual void UpdateEngineText();
    virtual bool IsEngineTextInitialized(){ return (sfText!=nullptr); }

    /// \brief Returns the SFML text object.
    sf::Text* GetSFText(){ return sfText;}

private:
    sf::Text* sfText = nullptr;              // OWNED

};


/// \ingroup EngineExtension_SFML
/// \brief Extended TextLayer class for SFML.
class TextLayerSFML : public TextLayer
{
public:

    TextLayerSFML(const std::string &_name) : TextLayer(_name){}

    virtual void DrawEngineLayer() override;
};

*/

//===================================================================================================


/// \ingroup EngineExtension_SFML
/// \brief Extended Drawer class for SFML.
class DrawerSFML : public Drawer
{
public:

    virtual void SetOutlineColor(ColorRGBA _outlineColor) override;
    virtual void Line(jugimap::Vec2f p1, jugimap::Vec2f p2) override;
    virtual void RectangleOutline(const jugimap::Rectf &rect) override;
    virtual void EllipseOutline(jugimap::Vec2f c, jugimap::Vec2f r) override;
    virtual void Dot(jugimap::Vec2f p) override;

private:
    sf::Color sfOutlineColor;

};


//===================================================================================================


/// \ingroup EngineExtension_SFML
/// \brief Extended Map class for SFML.
class MapSFML : public Map
{
public:

    MapSFML(){}
    MapSFML(int _zOrderStart) : Map(_zOrderStart) {}

    void DrawEngineObjects() override;

    //void UpdateAndDrawEngineMap();
};


//===================================================================================================


class EngineSceneSFML : public EngineScene
{
public:

    EngineSceneSFML(Scene *_scene) : EngineScene(_scene){}

    void PreUpdate() override;

};


//===================================================================================================


class EngineAppSFML : public EngineApp
{
public:

    EngineAppSFML(App *_app) : EngineApp(_app){}

    void SetSystemMouseCursorVisible(bool _visible) override;

};


//===================================================================================================

/*
class CommandsSFML : public Commands
{
public:

    virtual void PreAppUpdate() override;
    virtual void PostAppUpdate() override;
    virtual void SetSystemMouseCursorVisible(bool _visible) override;


};
*/

/*
class KeyboardImplSFML : public KeyboardImpl
{
public:

    KeyboardImplSFML();

    virtual bool IsKeyDown(KeyCode _keyCode) override;

//private:
    std::vector<sf::Keyboard::Key>keyCodesConversionTable;

};
*/


struct KeyboardConversionTable
{

    KeyboardConversionTable();

    std::vector<jugimap::KeyCode>table;

};


void ProcessEvents(sf::Event &event);



/*
class MouseSFML : public Mouse
{
public:

    MouseSFML(){}


    virtual bool ButtonDown(MouseButton _mouseButton) override;
    virtual bool ButtonPressed(MouseButton _mouseButton) override;
    virtual bool ButtonReleased(MouseButton _mouseButton) override;

};

*/


//===================================================================================================



/// \ingroup EngineExtension_SFML
/// \brief Extended ObjectFactory class for SFML.
class ObjectFactorySFML : public ObjectFactory
{
public:


    virtual BinaryStreamReader* NewBinaryFileStreamReader(const std::string &fileName) override { return new BinaryFileStreamReaderSFML(fileName);}
    virtual GraphicFile* NewFile() override { return new GraphicFileSFML(); }
    virtual Map* NewMap(int _zOrderStart) override { return new MapSFML(_zOrderStart); }
    virtual StandardSprite *NewStandardSprite() override { return new StandardSpriteSFML (); }
    //virtual StandardSprite *NewStandardSprite(SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer) override
    //{
    //    return new StandardSpriteSFML(_sourceSprite, _position, _spriteLayer);
    //}
    virtual TextSprite *NewTextSprite() override { return new TextSpriteSFML(); }
    //virtual TextSprite *NewTextSprite(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle, SpriteLayer *_spriteLayer) override
    //{
    //    return new TextSpriteSFML (_textString, _font, _color, _position, _textHandle, _spriteLayer);
    //}
    virtual EngineScene * NewEngineScene(Scene *_scene) override { return new EngineSceneSFML(_scene); }
    virtual EngineApp * NewEngineApp(App *_app) override { return new EngineAppSFML(_app); }

    virtual Font* NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind) override
    {
        return new FontSFML(_relativeFilePath, _size, _fontKind);
    }

    virtual Drawer* NewDrawer() override { return new DrawerSFML(); }

};



namespace globSFML{


extern sf::RenderTarget *CurrentRenderTarget;
extern sf::Window *CurrentWindow;

extern int countSpriteDrawCalls;
extern int countDrawnSprites;
extern int countDrawnFrames;

}


}






#endif
