
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
/// \brief Extended BinaryStreamReader class for AGK.
class BinaryFileStreamReaderAGK : public BinaryStreamReader
{
public:

    ///\brief Constructor
    ///
    /// Creates a new BinaryFileStreamReaderAGK object and open the file *fileName* for reading.
    /// This reader utilizes AGK's file reader and the file path must follow the requirements of that reader.
    BinaryFileStreamReaderAGK(const std::string &fileName)
    {
        if(agk::GetFileExists(fileName.c_str())){
            agkId = agk::OpenToRead(fileName.c_str());
            if(agkId>0){
                opened = true;
            }
        }else{
            opened = false;
        }
    }


    bool IsOpen() override { return opened; }
    int Pos() override { return agk::GetFilePos(agkId); }
    void Seek(int _pos) override { agk::SetFilePos(agkId, _pos); }
    int Size() override { return agk::GetFileSize(agkId); }
    void Close() override { agk::CloseFile(agkId); }


    unsigned char ReadByte()  override
    {
        unsigned char value = agk::ReadByte(agkId);
        return value;
    }

    int ReadInt()  override
    {
        int value = agk::ReadInteger(agkId);
        return value;
    }

    float ReadFloat()  override
    {
        float value = agk::ReadFloat(agkId);
        return value;
    }

    std::string ReadString() override
    {
        char *str = agk::ReadString2(agkId);
        std::string value(str);
        agk::DeleteString(str);
        return value;
    }



private:
    unsigned int agkId = 0;
    bool opened = false;
};


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended TextStreamReader class for AGK.
class TextFileStreamReaderAGK : public TextStreamReader
{
public:

    TextFileStreamReaderAGK(const std::string &fileName)
    {
        if(agk::GetFileExists(fileName.c_str())){
            agkId = agk::OpenToRead(fileName.c_str());
            if(agkId>0){
                opened = true;
            }
        }else{
            opened = false;
        }
    }


    ~TextFileStreamReaderAGK() override
    {
        if(opened){
            Close();
        }
    }


    bool IsOpen() override { return opened; }
    bool Eof() override { return agk::FileEOF(agkId);}
    void Close() override { if(opened) agk::CloseFile(agkId); }


    void ReadLine(std::string &s) override
    {
        char *c = agk::ReadLine(agkId);
        s = std::string(c);
        agk::DeleteString(c);
    }


private:
    unsigned int agkId = 0;
    bool opened = false;

};



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


    ~StandardSpriteAGK () override;


    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual void SetActiveImage(GraphicItem *_image) override;
    virtual bool IsEngineSpriteInitialized() override {return agkId!=0;}
    void SetPhysicsMode(PhysicsMode _physicsMode) override;

    //----
    int GetAgkId(){return agkId;}


private:
    unsigned int agkId = 0;                                         // OWNED data  (agk sprite id)
    unsigned int shaderAgkId = 0;                                   // LINK id

    void ManageShaders_OverlayColor();
};



/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended TextSprite class for AGK.
class TextSpriteAGK : public TextSprite
{
public:
    virtual ~TextSpriteAGK();

    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual bool IsEngineSpriteInitialized() override { return (agkId!=0); }

    int GetAgkId(){ return agkId;}

private:
    int agkId = 0;              // OWNED

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
/// This class currently provides basic functionality for displaying a spriter or a spine sprite on screen.
/// For real use in games it should be extended with more functions.
class SkeletonSpriteAGK : public SkeletonSprite
{
public:
    friend class JugiMapBinaryLoader;


    SkeletonSpriteAGK(){}
    ~SkeletonSpriteAGK() override;

    void InitEngineObjects() override;
    void UpdateEngineObjects() override;
    void SetVisible(bool _visible) override;


    //---- DATA
    int GetAgkId(){return agkId;}

private:
    unsigned int agkId = 0;
};



//===================================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended SpriteLayer class for AGK.
class SpriteLayerAGK : public SpriteLayer
{
public:

    void UpdateEngineObjects() override;

};

//===================================================================================================



/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended Map class for AGK.
class MapAGK : public Map
{
public:

    MapAGK(){}
    MapAGK(int _zOrderStart) : Map(_zOrderStart) {}

    void SetVisible(bool _visible) override;

    void UpdateEngineObjects() override;

};


//===================================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended EngineScene class for AGK.
class EngineSceneAGK : public EngineScene
{
public:

    EngineSceneAGK(Scene *_scene) : EngineScene(_scene){}

    void PreUpdate() override;

};


//===================================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended EngineApp class for AGK.
class EngineAppAGK : public EngineApp
{
public:

    EngineAppAGK(App *_app) : EngineApp(_app){}

    void SetSystemMouseCursorVisible(bool _visible) override;

};


//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended Font class for AGK.
class FontAGK : public Font
{
public:

    FontAGK(const std::string &_relativeFilePath, int _size, FontKind _kind);
    ~FontAGK();

    int GetAgkId(){ return agkId; }


    virtual int GetPixelWidth(const std::string &s) override;
    virtual int GetPixelHeight(const std::string &s) override;
    virtual Vec2f GetPixelSize(const std::string &s) override;

private:
    int agkId = 0;              // OWNED
    int agkProbeTextId = 0;          // OWNED
};



//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended Drawer class for AGK.
class DrawerAGK : public Drawer
{
public:

    DrawerAGK();
    void UpdateEngineDrawer() override;

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

void LoadJugimapShaders();
void DeleteShaders();


}



//===============================================================================================



struct KeyboardConversionTable
{

    KeyboardConversionTable();

    std::vector<jugimap::KeyCode>table;

};

extern KeyboardConversionTable keyboardConversionTable;



//===============================================================================================


/// \ingroup EngineExtension_AGK-tier2
/// \brief Extended ObjectFactory class for AGK.
class ObjectFactoryAGK : public ObjectFactory
{
public:


    BinaryStreamReader* NewBinaryFileStreamReader(const std::string &fileName) override { return new BinaryFileStreamReaderAGK(fileName); }

    TextStreamReader* NewTextFileStreamReader(const std::string &fileName) override { return new TextFileStreamReaderAGK(fileName); }


    GraphicFile* NewFile() override { return new GraphicFileAGK(); }

    GraphicItem* NewItem() override { return new GraphicItemAGK(); }

    StandardSprite *NewStandardSprite() override { return new StandardSpriteAGK (); }

    TextSprite *NewTextSprite() override { return  new TextSpriteAGK(); }

    SpriteLayer* NewSpriteLayer() override { return new SpriteLayerAGK(); }

    SkeletonSprite *NewSpineSprite() override { return new SkeletonSpriteAGK(); }

    SkeletonSprite *NewSpriterSprite() override { return new SkeletonSpriteAGK(); }

    Font* NewFont(const std::string &_relativeFilePath, int _size, FontKind _fontKind) override
    {
        return new FontAGK(_relativeFilePath, _size, _fontKind);
    }

    Drawer* NewDrawer() override { return new DrawerAGK(); }

    Map * NewMap(int _zOrderStart) override
    {
        return new MapAGK(_zOrderStart);
    }

    EngineScene * NewEngineScene(Scene *_scene) override
    {
        return new EngineSceneAGK(_scene);
    }

    EngineApp * NewEngineApp(App *_app) override
    {
        return new EngineAppAGK(_app);

    }

};



}






#endif
