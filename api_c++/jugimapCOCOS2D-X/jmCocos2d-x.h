
//===================================================================================================
//      JugiMap API extension for Cocos2d-x.
//===================================================================================================


#ifndef JUGIMAP_COCOS2D_H
#define JUGIMAP_COCOS2D_H

#include "cocos2d.h"
#include "../jugimap/jugimap.h"



namespace jugimap {


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended GraphicFile class for Cocos2d-x.
class GraphicFileCC : public GraphicFile
{
public:
    static int textureSmoothFilter;

    GraphicFileCC(){}
    virtual ~GraphicFileCC() override;

    virtual void Init() override;

    /// \brief Returns the Cocos2d-x texture object.
    cocos2d::Texture2D * GetTexture() {return texture;}


private:
    cocos2d::Texture2D *texture = nullptr;
};


//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended StandardSprite class for Cocos2d-x.
class StandardSpriteCC  : public StandardSprite
{
public:

    virtual void InitEngineSprite() override;
    virtual void UpdateEngineSprite() override;
    virtual void SetActiveImage(GraphicItem *_image) override;
    virtual void SetVisible(bool _visible) override;
    virtual bool IsEngineSpriteInitialized() {return (ccSprite!=nullptr);}
    virtual void SetPhysicsMode(PhysicsMode _physicsMode) override;

    virtual void CopyProperties(Sprite *_sprite, int copyFlags) override;

    //---
    /// \brief Returns the Cocos2d-x sprite object.
    cocos2d::Sprite * GetCCSprite() {return ccSprite;}

    /// \brief Set the given *_physicsMaterial* to the Cocos2d-x sprite intended for use in physics simulation.
    void SetPhysicsMaterial(cocos2d::PhysicsMaterial _physicsMaterial);


private:
    cocos2d::Sprite *ccSprite = nullptr;
    cocos2d::GLProgramState *glProgramState = nullptr;
    cocos2d::PhysicsMaterial physicsMaterial =  cocos2d::PhysicsMaterial(1.0, 0.3, 0.7);

    int drawChangeFlag = 0;


    void ManageShaders_OverlayColor();

};


//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended SpriteLayer class for Cocos2d-x.
class SpriteLayerCC : public SpriteLayer
{
public:

    virtual void RemoveAndDeleteSprite(Sprite* _sprite) override;

};


//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended Drawer class for Cocos2d-x.
class DrawerCC : public Drawer
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
    cocos2d::Color4F outlineColor;
    cocos2d::DrawNode * drawNode = nullptr;         // LINK   ( Cocos2d handles ownership of engine objects. )

};


//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended Text class for Cocos2d-x.
class TextCC : public Text
{
public:

    TextCC(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
        : Text(_textLayer, _font, _textString, _position, _color)
    {}

    virtual void InitEngineText();
    virtual void UpdateEngineText();
    virtual bool IsEngineTextInitialized(){ return (ccLabel!=nullptr); }


    /// \brief Returns the Cocos2d-x label object.
    cocos2d::Label* GeCCLabel(){ return ccLabel;}

private:
    cocos2d::Label* ccLabel = nullptr;
};



/// \ingroup EngineExtension_nCine
/// \brief Extended TextLayer class for nCine.
class TextLayerCC : public TextLayer
{
public:

    TextLayerCC(const std::string &_name) : TextLayer(_name) {}

    virtual void RemoveAndDeleteText(Text* _text) override;
};


//===================================================================================================


class MapCC;


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended cocos2d::Node class for use with MapCC objects.
class MapCCNode : public cocos2d::Node
{
public:
    static MapCCNode* Create(MapCC * _map);

    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

private:
    MapCC * map = nullptr;
};


//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended Map class for Cocos2d-x.
class MapCC : public Map
{
public:
    static jugimap::Camera * currentCamera;


    virtual void InitEngineMap() override;
    virtual void UpdateEngineMap() override;

    /// \brief Returns the MapCCNode object.
    cocos2d::Node * GetMapNode(){return ccMapNode;}

    /// \brief Set Camera objects for this map.
    ///
    /// Use this function if you need to display this map with more cameras (ei. for split screen).
    /// If you use only one camera it is enough to set it with Map::SetCamera.
    void SetCameras(const std::vector<Camera*> &_cameras) {cameras = _cameras;}

    /// \brief Returns a reference to the vector with stored cameras.
    std::vector<Camera*> & GetCameras(){ return cameras; }


private:
    MapCCNode * ccMapNode = nullptr;

    std::vector<Camera*>cameras;                        // LINKs

};



//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended cocos2d::Scene class for use with jugimap::Scene objects.
class SceneCCNode : public cocos2d::Scene
{
public:
    static int glDrawnBatchesString;
    static int glDrawnVerticesString;

    ///\brief Create SceneCCNode object with the given _jmScene object.
    ///
    /// This function should be called inside extended jugimap::Scene initialization.
    static SceneCCNode* Create(jugimap::Scene* _jmScene);

    ///\brief Destructor.
    virtual ~SceneCCNode();

    ///\brief Initialize this object.
    ///
    /// This function is called by SceneCCNode::Create.
    virtual bool init() override;

    ///\brief Update this scene object.
    virtual void update(float delta) override;

    virtual void render(cocos2d::Renderer *renderer, const cocos2d::Mat4 &eyeTransform, const cocos2d::Mat4 *eyeProjection);

    ///\brief Get jugimap::Scene object link.
    jugimap::Scene * GetJMScene(){ return jmScene; }


    void _SetErrorMessageLabel(cocos2d::Label* _errorMessageLabel){ errorMessageLabel = _errorMessageLabel; }

private:
    jugimap::Scene *jmScene = nullptr;             // LINK

    cocos2d::Label* errorMessageLabel = nullptr;

};



namespace shaders
{

    extern cocos2d::GLProgram *glpColorOverlay_blendSimpleMultiply;
    extern cocos2d::GLProgram *glpColorOverlay_blendNormal;
    extern cocos2d::GLProgram *glpColorOverlay_blendMultiply;
    extern cocos2d::GLProgram *glpColorOverlay_blendLinearDodge;
    extern cocos2d::GLProgram *glpColorOverlay_blendColor;


    // uniform names
    extern std::string nameOverlayColor;

    void DeleteShaders();

}



//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended ObjectFactory class for Cocos2d-x.
class ObjectFactoryCC : public ObjectFactory
{
public:

    virtual GraphicFile* NewFile() override { return new GraphicFileCC(); }
    virtual Map* NewMap() override { return new MapCC(); }
    virtual SpriteLayer *NewSpriteLayer() override { return new SpriteLayerCC(); }
    virtual StandardSprite *NewStandardSprite() override { return new StandardSpriteCC (); }
    virtual Drawer* NewDrawer() override { return new DrawerCC(); }
    virtual Text* NewText(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color) override
    {
        return new TextCC(_textLayer, _font, _textString, _position, _color);
    }
    virtual TextLayer* NewTextLayer(const std::string &_name){ return new TextLayerCC(_name); }

};




}






#endif
