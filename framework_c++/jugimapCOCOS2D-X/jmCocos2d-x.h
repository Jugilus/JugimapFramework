
//===================================================================================================
//      JugiMap API extension for Cocos2d-x.
//===================================================================================================


#ifndef JUGIMAP_COCOS2D_H
#define JUGIMAP_COCOS2D_H

#include "cocos2d.h"
#include "../jugimap/jugimap.h"



namespace jugimap {




/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended BinaryStreamReader class for Cocos2d-x.
class BinaryFileStreamReaderCC : public BinaryBufferStreamReader
{
public:

    ///\brief Constructor
    ///
    /// Creates a new BinaryFileStreamReaderCC object and open the file *fileName* for reading.
    /// This reader utilizes Cocos2d-x's reader and the file path must follow the requirements of that reader.
    BinaryFileStreamReaderCC(const std::string &fileName);

};


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended TextStreamReader class for Cocos2d-x.
class TextFileStreamReaderCC : public TextStreamReader
{
public:


    TextFileStreamReaderCC(const std::string &fileName);

    bool IsOpen() override;
    void ReadLine(std::string &s) override;
    bool Eof() override;
    void Close() override;

private:
    std::vector<std::string>lines;
    int index = 0;
};


//===================================================================================================


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

    StandardSpriteCC(){}

    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual void SetActiveImage(GraphicItem *_image) override;
    virtual bool IsEngineSpriteInitialized() override { return (ccSprite!=nullptr); }
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


    void ManageShaders_OverlayColor();

};


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended TextSprite class for Cocos2d-x.
class TextSpriteCC  : public TextSprite
{
public:

    TextSpriteCC(){}

    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual bool IsEngineSpriteInitialized() override { return (ccLabel!=nullptr); }

    //---
    /// \brief Returns the Cocos2d-x label object.
    cocos2d::Label * GetCCLabel() {return ccLabel;}


private:
    cocos2d::Label* ccLabel = nullptr;

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
/// \brief Extended Font class for Cocos2d-x.
class FontCC : public Font
{
public:

    FontCC(const std::string &_relativeFilePath, int _loadFontSize, FontKind _kind);
    virtual ~FontCC() override;
    //virtual void InitEngineObjects() override;

    //virtual int GetPixelWidth(int unicode) override;
    virtual int GetPixelWidth(const std::string &s) override;
    virtual int GetPixelHeight(const std::string &s) override;
    virtual Vec2f GetPixelSize(const std::string &s) override;

private:

    cocos2d::Label* ccLabel = nullptr;

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


    MapCC(int _zOrderStart) : Map(_zOrderStart){}

    virtual void InitEngineObjects(MapType _mapType) override;
    virtual void UpdateEngineObjects() override;

    /// \brief Returns the MapCCNode object.
    cocos2d::Node * GetMapNode(){return ccMapNode;}

    /// \brief Set Camera objects for this map.
    ///
    /// Use this function if you need to display this map with more cameras (ei. for split screen).
    /// If you use only one camera it is enough to set it with Map::SetCamera.
    void SetCameras(const std::vector<Camera*> &_cameras) {cameras = _cameras;}


    void SetVisible(bool _visible) override;


    /// \brief Returns a reference to the vector with stored cameras.
    std::vector<Camera*> & GetCameras(){ return cameras; }


private:
    MapCCNode * ccMapNode = nullptr;

    std::vector<Camera*>cameras;                        // LINKs

};



//===================================================================================================


class EngineSceneCC;


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended EngineScene class for Cocos2d-x.
class EngineSceneCC : public EngineScene
{
public:

    EngineSceneCC(Scene *_scene);
    void PostInit() override;
    void PostUpdate() override;

    /// \brief Returns the Cocos2d-x node object.
    cocos2d::Node* GetEngineNode(){ return ccNode; }

private:
    cocos2d::Node *ccNode = nullptr;
};



class EngineAppCC;


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended cocos2d::Scene class for use with jugimap::EngineAppCC objects.
class AppCCNode : public cocos2d::Scene
{
public:
    friend class EngineAppCC;

    static int glDrawnBatchesString;
    static int glDrawnVerticesString;

    ///\brief Create SceneCCNode object with the given _jmScene object.
    ///
    /// This function should be called inside extended jugimap::Scene initialization.
    static AppCCNode* Create(jugimap::App* _jmApp);

    ///\brief Destructor.
    virtual ~AppCCNode();

    ///\brief Initialize this object.
    ///
    /// This function is called by SceneCCNode::Create.
    virtual bool init() override;

    ///\brief Update this scene object.
    virtual void update(float delta) override;

    virtual void render(cocos2d::Renderer *renderer, const cocos2d::Mat4 &eyeTransform, const cocos2d::Mat4 *eyeProjection) override;

    ///\brief Get jugimap::App object link.
    jugimap::App * GetJMApp(){ return jmApp; }


private:
    jugimap::App *jmApp = nullptr;                      // LINK
    cocos2d::Label* errorMessageLabel = nullptr;

};


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended EngineApp class for Cocos2d-x.
class EngineAppCC : public EngineApp
{
public:

    EngineAppCC(App *_app);
    void PreInit() override;
    void PostInit() override;
    void SetSystemMouseCursorVisible(bool _visible) override;
    void Update();

    /// \brief Returns the AppCCNode object.
    AppCCNode* GetAppNode(){ return appCCNode; }


private:
    AppCCNode * appCCNode = nullptr;
};



//=====================================================================================


class EventsLayer : public cocos2d::Layer
{
public:
    static EventsLayer * Create();
    bool Init();

private:
    void OnMouseDown(cocos2d::Event *_event);
    void OnMouseUp(cocos2d::Event *_event);
    void OnMouseMove(cocos2d::Event *_event);
    void OnMouseScroll(cocos2d::Event *_event);

    void OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    bool OnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void OnTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void OnTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);


    // Jugimap's joystick class currently does not work with Cocos2d engine.
    // The problem is that key mapping from Controller::Key does not fit the actual ids from the events for my generic gamepad on Windows.
    // That wouldn't be so much of a problem if POV direction keys wouldn't be lumped together with other buttons so
    // they may have different ids on game controllers with different number of buttons.
    void OnJoystickConnected(cocos2d::Controller* controller, cocos2d::Event* event);
    void OnJoystickDisconnected(cocos2d::Controller* controller, cocos2d::Event* event);
    void OnJoystickButtonDown(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event);
    void OnJoystickButtonUp(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event);
    void OnJoystickAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event);



    std::vector<jugimap::KeyCode>keysConversionTable;

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

    void LoadJugimapShaders();
    void DeleteShaders();

}


//===================================================================================================


/// \ingroup EngineExtension_Cocos2d-x
/// \brief Extended ObjectFactory class for Cocos2d-x.
class ObjectFactoryCC : public ObjectFactory
{
public:

    virtual BinaryStreamReader* NewBinaryFileStreamReader(const std::string &filePath) override { return new BinaryFileStreamReaderCC(filePath);}
    virtual TextStreamReader* NewTextFileStreamReader(const std::string &filePath) override { return new TextFileStreamReaderCC(filePath);}
    virtual GraphicFile* NewFile() override { return new GraphicFileCC(); }
    virtual EngineApp* NewEngineApp(App* _app) override { return new EngineAppCC(_app); }
    virtual EngineScene* NewEngineScene(Scene *_scene) override { return new EngineSceneCC(_scene); }
    virtual Map* NewMap(int _zOrderStart) override { return new MapCC(_zOrderStart); }
    virtual SpriteLayer *NewSpriteLayer() override { return new SpriteLayerCC(); }
    virtual StandardSprite *NewStandardSprite() override { return new StandardSpriteCC(); }
    virtual TextSprite *NewTextSprite() override { return new TextSpriteCC(); }
    virtual Drawer* NewDrawer() override { return new DrawerCC(); }
    virtual Font * NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind) override { return new FontCC(_relativeFilePath, _size, _fontKind); }

};




}






#endif
