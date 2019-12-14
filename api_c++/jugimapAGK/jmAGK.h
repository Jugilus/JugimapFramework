
//===================================================================================================
//      JugiMap API extension for SFML.
//===================================================================================================


#ifndef JUGIMAP_AGK_H
#define JUGIMAP_AGK_H

#include <map>
#include "AGK.h"
#include "../jugimap/jugimap.h"



namespace jugimap {


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended GraphicItem class for AGK.
class GraphicItemAGK : public GraphicItem
{
public:

    GraphicItemAGK(){}
    virtual ~GraphicItemAGK() override;

    virtual void Init() override;

    int GetAgkId() const {return agkId;}

private:
    unsigned int agkId = 0;
};


//=======================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended GraphicFile class for AGK.
class GraphicFileAGK : public GraphicFile
{
public:

    GraphicFileAGK(){}
    virtual ~GraphicFileAGK() override;

    virtual void Init() override;

    int GetAgkId() const {return agkId;}

private:
    unsigned int agkId = 0;
};


//=======================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended StandardSprite class for AGK.
class StandardSpriteAGK  : public StandardSprite
{
public:
    friend class JugiMapBinaryLoader;
    friend class StandardSpriteFrameAnimationPlayer;


    StandardSpriteAGK (){}
    ~StandardSpriteAGK () override;


    virtual void InitEngineSprite() override;
    virtual void UpdateEngineSprite() override;
    virtual void SetVisible(bool _visible) override;
    virtual void SetActiveImage(GraphicItem *_image) override;
    virtual bool IsEngineSpriteInitialized() {return agkId!=0;}
    void SetPhysicsMode(PhysicsMode _physicsMode) override;

    //----
    int GetAgkId(){return agkId;}


private:
    unsigned int agkId = 0;                                         // OWNED data  (agk sprite id)
    unsigned int shaderAgkId = 0;                                   // LINK id

    void ManageShaders_OverlayColor();
};



class ColliderAGK : public Collider
{
public:

    ColliderAGK(StandardSprite* _standardSprite);

    virtual Kind GetKind(){ return Kind::SINGLE_SHAPE; }
    virtual void AddShapes() override;
    virtual void UpdateShapes(const AffineMat3f &m) override;
    virtual void ClearShapes() override;
    virtual int GetShapesCount() override;
    virtual bool PointInside(Vec2f _pos) override;
    virtual bool Overlaps(Collider *_otherCollider) override;
    virtual bool RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos) override;

private:
    int nShapes = 0;
    int agkId = 0;          // LINK ( StandardSpriteAGK agkID)

};


//=======================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended SkeletonSprite class for AGK.
///
/// This class provides basic functionality for displaying a spriter or a spine sprite on screen.
/// For real use in games it should be extended with more functions.
class SkeletonSpriteAGK : public SkeletonSprite
{
public:
    friend class JugiMapBinaryLoader;


    SkeletonSpriteAGK(){}
    ~SkeletonSpriteAGK() override;

    void InitEngineSprite() override;
    void UpdateEngineSprite() override;
    void SetVisible(bool _visible) override;


    //---- DATA
    int GetAgkId(){return agkId;}

private:
    unsigned int agkId = 0;
};



//===================================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended GraphicFile class for AGK.
class SpriteLayerAGK : public SpriteLayer
{
public:

    void UpdateEngineLayer() override;

};



//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended GraphicFile class for AGK.
class FontAGK : public Font
{
public:

    FontAGK(const std::string &_relativeFilePath, int _size, FontKind _kind);
    ~FontAGK();

    int GetAgkId(){ return agkId; }

private:
    int agkId = 0;              // OWNED

};


//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended Text class for AGK.
class TextAGK : public Text
{
public:
    TextAGK(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
        : Text(_textLayer, _font, _textString, _position, _color)
    {}

    virtual ~TextAGK();

    virtual void InitEngineText();
    virtual void UpdateEngineText();
    virtual bool IsEngineTextInitialized(){ return (agkId!=0); }

    int GetAgkId(){ return agkId;}

private:
    int agkId = 0;              // OWNED

};


//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended TextLayer class for AGK.
class TextLayerAGK : public TextLayer
{
public:

    TextLayerAGK(const std::string &_name) : TextLayer(_name) {}

    virtual void UpdateEngineLayer() override;
};


//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended Drawer class for AGK.
class DrawerAGK : public Drawer
{
public:

    DrawerAGK();

    virtual void SetOutlineColor(ColorRGBA _outlineColor) override;
    virtual void Line(jugimap::Vec2f p1, jugimap::Vec2f p2) override;
    virtual void RectangleOutline(const jugimap::Rectf &rect) override;
    virtual void EllipseOutline(jugimap::Vec2f c, jugimap::Vec2f r) override;
    virtual void Dot(jugimap::Vec2f p) override;

private:
    int agkOutlineColor = 0;

};


//===================================================================================================


class ShaderAGK_ColorBlend_SimpleMultiply;
class ShaderAGK_ColorBlend_Normal;
class ShaderAGK_ColorBlend_Multiply;
class ShaderAGK_ColorBlend_LinearDodge;
class ShaderAGK_ColorBlend_Color;


class FragmentShaderAGK_ColorBlend
{
public:

    friend class StandardSpriteAGK;
    friend class ShaderAGK_ColorBlend_SimpleMultiply;
    friend class ShaderAGK_ColorBlend_Normal;
    friend class ShaderAGK_ColorBlend_Multiply;
    friend class ShaderAGK_ColorBlend_LinearDodge;
    friend class ShaderAGK_ColorBlend_Color;


    virtual ~FragmentShaderAGK_ColorBlend();
    void Load(const std::string &_filePath);

    unsigned int GetAgkId(){ return agkId;}

private:
    unsigned int agkId = 0;

    //--- uniforms
    ColorRGBA colorRGBA{255, 255, 255, 128};
    std::string colorRGBAname = "overlayColor";
};



class ShaderAGK_ColorBlend_SimpleMultiply : public FragmentShaderAGK_ColorBlend
{};


class ShaderAGK_ColorBlend_Normal : public FragmentShaderAGK_ColorBlend
{};


class ShaderAGK_ColorBlend_Multiply : public FragmentShaderAGK_ColorBlend
{};


class ShaderAGK_ColorBlend_LinearDodge : public FragmentShaderAGK_ColorBlend
{};


class ShaderAGK_ColorBlend_Color : public FragmentShaderAGK_ColorBlend
{};


namespace shaders
{

extern ShaderAGK_ColorBlend_SimpleMultiply* colorBlend_SimpleMultiply;
extern ShaderAGK_ColorBlend_Normal* colorBlend_Normal;
extern ShaderAGK_ColorBlend_Multiply* colorBlend_Multiply;
extern ShaderAGK_ColorBlend_LinearDodge* colorBlend_LinearDodge;
extern ShaderAGK_ColorBlend_Color* colorBlend_Color;

void DeleteShaders();

}


//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended ObjectFactory class for AGK.
class ObjectFactoryAGK : public ObjectFactory
{
public:

    GraphicFile* NewFile() override { return new GraphicFileAGK(); }

    GraphicItem* NewItem() override { return new GraphicItemAGK(); }

    StandardSprite *NewStandardSprite() override { return new StandardSpriteAGK (); }

    SpriteLayer* NewSpriteLayer() override { return new SpriteLayerAGK(); }

    SkeletonSprite *NewSpineSprite() override { return new SkeletonSpriteAGK(); }

    SkeletonSprite *NewSpriterSprite() override { return new SkeletonSpriteAGK(); }


    Font* NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind) override
    {
        return new FontAGK(_relativeFilePath, _size, _fontKind);
    }

    Text* NewText(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color) override
    {
        return new TextAGK(_textLayer, _font, _textString, _position, _color);
    }

    TextLayer* NewTextLayer(const std::string &_name) override { return new TextLayerAGK(_name); }

    Drawer* NewDrawer() override { return new DrawerAGK(); }

};



}






#endif
