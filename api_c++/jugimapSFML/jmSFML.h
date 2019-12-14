
//===================================================================================================
//      JugiMap API extension for SFML.
//===================================================================================================

#ifndef JUGIMAP_SFML_H
#define JUGIMAP_SFML_H


#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include "../jugimap/jugimap.h"




namespace jugimap {



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

    StandardSpriteSFML (){}
    virtual ~StandardSpriteSFML () override;


    virtual void InitEngineSprite() override;
    virtual void UpdateEngineSprite() override;
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

}


//===================================================================================================


/// \ingroup EngineExtension_SFML
/// \brief Extended Font class for SFML.
class FontSFML : public Font
{
public:

    FontSFML(const std::string &_relativeFilePath, int _size, FontKind _kind);
    ~FontSFML();

    /// \brief Returns the SFML font object.
    sf::Font* GetSFFont(){ return sfFont;}

private:
    sf::Font* sfFont = nullptr;             // OWNED

};



/// \ingroup EngineExtension_SFML
/// \brief Extended Text class for SFML.
class TextSFML : public Text
{
public:

    TextSFML(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
        : Text(_textLayer, _font, _textString, _position, _color)
    {}

    virtual ~TextSFML();

    virtual void InitEngineText();
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


    void UpdateAndDrawEngineMap();
};


//===================================================================================================



/// \ingroup EngineExtension_SFML
/// \brief Extended ObjectFactory class for SFML.
class ObjectFactorySFML : public ObjectFactory
{
public:

    virtual GraphicFile* NewFile() override { return new GraphicFileSFML(); }

    virtual Map* NewMap() override { return new MapSFML(); }

    virtual StandardSprite *NewStandardSprite() override { return new StandardSpriteSFML (); }

    virtual Font* NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind) override
    {
        return new FontSFML(_relativeFilePath, _size, _fontKind);
    }

    virtual Text* NewText(TextLayer *_textLayer, Font* _font, const std::string &_textString, Vec2f _position, ColorRGBA _color) override
    {
        return new TextSFML(_textLayer, _font, _textString, _position, _color);
    }

    virtual TextLayer* NewTextLayer(const std::string &_name) override { return new TextLayerSFML(_name); }

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
