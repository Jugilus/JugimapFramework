#ifndef JUGIMAP_LAYERS_H
#define JUGIMAP_LAYERS_H

#include "jmCommon.h"
#include "jmColor.h"
#include "jmGlobal.h"


namespace jugimap {


class Map;
class Sprite;
class StandardSprite;
class SourceSprite;
class TextSprite;
class Font;
class ComposedSprite;
class JugiMapBinaryLoader;
class VectorShape;




///\ingroup MapElements
///\brief The Layer class is the base class for layers.
class Layer
{
public:
    friend class JugiMapBinaryLoader;
    friend class ComposedSprite;
    friend class Map;


    /// Constructor.
    Layer(){}

    Layer(const Layer &l) = delete;
    Layer& operator=(const Layer &l) = delete;



    ///\brief  Initialize layer properties from the *parameters* set in the editor.
    ///
    /// This function should usually not be used manually as it gets called in the Map::InitMapParameters function.
    virtual void InitLayerParameters();


    ///\brief  Initialize all engine objects related to this layer and its content.
    ///
    /// This function should usually not be used manually as it gets called in the Map::InitEngineMap function.
    virtual void InitEngineObjects() {}


    ///\brief  Update all engine objects related to this layer and its content.
    ///
    /// This function should usually not be used manually as it gets called in the Map::UpdateEngineMap function.
    virtual void UpdateEngineObjects() {}


    ///\brief  Draw all engine objects related to this layer and its content.
    ///
    /// This function should usually not be used manually as it gets called in the Map::DrawEngineMap function.
    /// It is needed only for the engines which do not handle drawing (rendering) internally.
    virtual void DrawEngineObjects() {}


    ///\brief Returns a Map object where this layer is stored.
    ///
    /// If this layer is stored in a ComposedSprite it will returns the map where this composed sprite is located.
    Map* GetMap() {return map;}


    ///\brief Returns a ComposedSprite where this layer is stored or *nullptr* if it is stored in a map.
    ComposedSprite* GetParentComposedSprite() {return parentComposedSprite;}


    ///\brief Returns true if this layer is stored in a Map; if it's stored in a ComposedSprite returns false.
    bool IsMapLayer(){ return (parentComposedSprite==nullptr); }


     ///\brief Returns the kind of this layer.
    LayerKind GetKind() {return kind;}


    ///\brief Returns the name of this layer.
    std::string GetName() {return name;}


    ///\brief Returns a reference to the vector of stored paramters in this layer.
    std::vector<jugimap::Parameter>& GetParameters() {return parameters;}


    ///\brief Returns the zOrder of this layer.
    ///
    /// The zOrder is usable in engines where we set drawing order via zOrder factor.
    int GetZOrder() {return zOrder;}


    /// \brief Update the map bounding box of this layer.
    virtual void UpdateBoundingBox(){}


    /// \brief Returns the map bounding box of this layer.
    Rectf GetBoundingBox(){ return boundingBox;}


    ///\brief Returns the LayerType factor of this layer.
    ///
    /// This factor is used for layers stored in parallax maps.
    LayerType GetLayerType(){ return layerType;}


    ///\brief Returns the AlignX factor of this layer.
    ///
    /// This factor is used for layers stored in parallax maps and screen maps.
    //AlignX GetAlignX() {return alignX;}


    ///\brief Returns the AlignY factor of this layer.
    ///
    /// This factor is used for layers stored in parallax maps and screen maps.
    //AlignY GetAlignY() {return alignY;}

    StretchingVariant GetStretchingVariant(){ return stretchingVariant;}


    ///\brief Returns the allign position factor of this layer.
    ///
    /// This position is used for layers stored in parallax maps and screen maps.
    Vec2i GetAlignPosition(){ return alignPosition; }


    ///\brief Returns the allign offset factor of this layer.
    ///
    /// This factor is used for layers stored in parallax maps and screen maps.
    Vec2f GetAlignOffset(){ return alignOffset; }


    ///\brief Returns the layers plane size of this layer.
    ///
    /// This factor is used for layers stored in parallax maps and screen maps.
    Vec2f GetLayersPlaneSize(){return layersPlaneSize;}


    ///\brief Returns the ParallaxLayerMode factor of this layer.
    ///
    /// This factor is used for layers stored in parallax maps.
    //ParallaxLayerMode GetParallaxLayerMode(){ return parallaxLayerMode;}





    ///\brief Returns the parallax factor of this layer.
    ///
    /// This factor is used for layers stored in parallax maps.
    Vec2f GetParallaxFactor() {return parallaxFactor;}


    ///\brief Returns the parallax offset of this layer.
    ///
    /// The parallax offset is an extra offset added to the sprites position.
    /// This factor is used for layers stored in parallax maps.
    Vec2f GetParallaxOffset(){return parallaxOffset;}


    ///\brief Returns the ScreenLayerMode factor of this layer.
    ///
    /// This factor is used for layers stored in screen maps.
    //ScreenLayerMode GetScreenLayerMode(){ return screenLayerMode;}


    //-------

    ///\brief Sets the name of this layer to the given *_name*.
    ///
    /// This function is available for the cases where we manually create layers.
    void _SetName(const std::string &_name){ name = _name; }


    ///\brief Sets the kind of this layer to the given *_kind*.
    ///
    /// This function is available for the cases where we manually create layers.
    void _SetKind(LayerKind _kind){ kind = _kind; }


    ///\brief Sets the layer mode of this layer to the given *_layerType*.
    ///
    /// This function is available for the cases where we manually create layers.
    void _SetLayerType(LayerType _layerType){ layerType = _layerType; }


    ///\brief Sets the parallax factor of this layer to the given *_parallaxFactor*.
    ///
    /// The parallax factor is used only for layers of LayerType::PARALLAX
    /// This function is available for the cases where we manually create layers.
    void _SetParallaxFactor(Vec2f _parallaxFactor){ parallaxFactor = _parallaxFactor; }


    ///\brief Sets the align position of this layer to the given *_alignPosition*.
    ///
    /// The align position is used only for layers of LayerType::PARALLAX and LayerType::SCREEN
    /// This function is available for the cases where we manually create layers.
    void _SetAlignPosition(Vec2i _alignPosition){ alignPosition = _alignPosition; }


    ///\brief Sets the align offset of this layer to the given *_alignOffset*.
    ///
    /// The align offset is used only for layers of LayerType::PARALLAX and LayerType::SCREEN
    /// This function is available for the cases where we manually create layers.
    void _SetAlignOffset(Vec2f _alignOffset){ alignOffset = _alignOffset; }


    ///\brief Sets the 'attachToLayer'  of this layer to the given *_attachToLayer*.
    ///
    /// The 'attachToLayer' is used only for layers of LayerType::PARALLAX and LayerType::SCREEN
    /// This function is available for the cases where we manually create layers.
    void _SetAttachToLayer(std::string _attachToLayer){ attachToLayer = _attachToLayer; }


    ///\brief Sets the stretching variant of this layer to the given *_stretchingVariant*.
    ///
    /// The stretching variant is used only for layer of LayerType::PARALLAX_STRETCHING_SINGLE_SPRITE.
    /// This function is available for the cases where we manually create layers.
    void _SetStretchingVariant(StretchingVariant _stretchingVariant){ stretchingVariant = _stretchingVariant;}


    ///\brief Sets the z-order factor of this layer to the given *_zOrder*.
    ///
    /// This function is available for the cases where we manually create layers.
    void _SetZOrder(int _zOrder){ zOrder = _zOrder; }


protected:

    /// Destructor.
    virtual ~Layer(){}

    bool UpdateParallaxOffset();

    //---
    Rectf boundingBox;

private:

    Map *map = nullptr;                               // LINK to map
    ComposedSprite *parentComposedSprite = nullptr;   // LINK to parent composed sprite (or nullptr if parent is map).

    LayerKind kind = LayerKind::NOT_DEFINED;
    std::string name = "Layer";
    std::vector<jugimap::Parameter> parameters;            // Custom parameters from JugiMap editor.

    //---
    int zOrder = 0;                                      // Drawing order of layer sprites (used by some engines)



    //---- properties of layers of the parallax maps
    //ParallaxLayerMode parallaxLayerMode = ParallaxLayerMode::STANDARD;
    LayerType layerType = LayerType::WORLD;
    Vec2i alignPosition;
    Vec2f alignOffset;
    Vec2f parallaxFactor;
    Vec2i tilingCount;
    Vec2i tilingSpacing;
    Vec2i tilingSpacingDelta;
    Vec2i tilingCountAutoSpread;                                                    //boolean Flag
    StretchingVariant stretchingVariant = StretchingVariant::XY_TO_WORLD_SIZE;

    Vec2f parallaxOffset;

    //---- properties of layers of the screen maps
    //ScreenLayerMode screenLayerMode = ScreenLayerMode::STANDARD;

    //---- properties of layers of the parallax and screen maps
    //AlignX alignX = AlignX::MIDDLE;
    //AlignY alignY = AlignY::MIDDLE;


    //bool alignOffsetX_obtainFromMap = false;
    //bool alignOffsetY_obtainFromMap = false;
    Vec2i alignOffset_obtainFromMap;                                                //boolean Flag

    std::string attachToLayer;
    Vec2f layersPlaneSize;





};



//==================================================================================================


///\ingroup MapElements
///\brief The SpriteLayer class defines the sprite and tile layers from JugiMap Editor.
///
/// A sprite layer stores Sprite objects and is their owner.
class SpriteLayer : public Layer
{
public:
    friend class JugiMapBinaryLoader;
    friend class ComposedSprite;
    friend class Map;


    /// Constructor.
    SpriteLayer();


    //----
    virtual void InitEngineObjects() override;
    virtual void InitLayerParameters() override;
    virtual void UpdateEngineObjects() override;
    virtual void DrawEngineObjects()override;


    /// \brief Returns a reference to the vector of stored sprites in this sprite layer.
    std::vector<Sprite*>& GetSprites() {return sprites;}


    /// \brief Update the map bounding box of this sprite layer.
    virtual void UpdateBoundingBox() override;


    /// \brief Sets the alpha factor of this sprite layer to the given *_alpha*.
    ///
    /// The alpha (opacity) factor is in the scale of [0.0 - 1.0].
    /// \see GetAlpha
    void SetAlpha(float _alpha);


    /// \brief Returns the alpha factor of this sprite layer.
    ///
    /// \see SetAlpha
    float GetAlpha();


    /// \brief Sets the blend factor of this sprite layer to the given *_blend*.
    ///
    /// \see GetBlend
    void SetBlend(Blend _blend);


    /// \brief Returns the blend factor of this sprite layer.
    ///
    /// \see SetBlend
    Blend GetBlend() {return blend;}


    /// \brief Adds a new sprite to this sprite layer.
    ///
    /// **Important:** This sprite layer will take over the ownership of the added sprite.
    void AddSprite(Sprite* _sprite);


    StandardSprite * AddStandardSprite(SourceSprite *_sourceSprite, Vec2f _position);

    TextSprite * AddTextSprite(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle);


    /// \brief Remove and delete the given sprite *_sprite* from this sprite layer.
    ///
    /// If the sprite belong to other layer it will not be deleted.
    virtual void RemoveAndDeleteSprite(Sprite* _sprite);


    /// \brief Capture the sprite properties, usually the position only,  required for the *lerp drawing*.
    ///
    /// \see Globals::SetUseLerpDrawing
    void CaptureForLerpDrawing();


    int FindLastZOrder();

    //-------


    /// \brief Set the *sprites changed* flag which indicates that this sprite layer has sprites with changed properites.
    ///
    /// Set to *true* whenever a sprite has a property change which require an update of the engine sprite.
    /// Set to *false* after engine sprites has been updated.
    /// This function should usually not be used manually as it is called by functions which change sprite properties
    /// and the UpdateEngineLayer function.
    ///
    /// \see IsSpritesChanged
    void SetSpritesChanged(bool _spritesChanged){ spritesChanged = _spritesChanged;}


    /// \brief Returns 'true' if this sprite layer has sprites with changed properties; otherwise returns false.
    ///
    /// \see SetSpritesChanged
    bool IsSpritesChanged(){ return spritesChanged;}


    /// \brief Set the *sprites changes* flag to the given **_changedFlags**.
    ///
    /// The *sprites changes* flag affects all stored sprites. It must be a value or
    /// a bitwise combination of values defined in the Sprite::Property enumerator.
    /// This function should usually not be used manually as it is called by functions which change sprite properties.
    ///
    /// \see AppendToSpritesChangeFlag, GetSpritesChangeFlag
    void SetSpritesChangeFlag(int _spritesChangeFlag)
    {
        spritesChangeFlag = _spritesChangeFlag;
        if(spritesChangeFlag!=0){
            spritesChanged = true;
        }
    }


    /// \brief Append the given *_spritesChangeFlag* to the current *sprites changes* flag using bitwise operation.
    ///
    /// \see SetSpritesChangeFlag, GetSpritesChangeFlag
    void AppendToSpritesChangeFlag(int _spritesChangeFlag)
    {
        spritesChangeFlag |= _spritesChangeFlag;
        if(spritesChangeFlag!=0){
            spritesChanged = true;
        }
    }

    /// \brief Returns the *sprites changes* flag.
    ///
    /// \see SetSpritesChangeFlag, AppendToSpritesChangeFlag
    int GetSpritesChangeFlag(){ return spritesChangeFlag; }


    //-------


    /// \brief Returns *true* if this layer was a *tile layer* in JugiMap Editor; if it was a *sprite layer* returns false.
    ///
    /// The SpriteLayer makes no distinction between the tile layers and sprite layers from the editor. However an extended class for
    /// a particlar game engine may found that information useful.
    bool IsEditorTileLayer(){ return editorTileLayer;}


protected:

    /// Destructor
    virtual ~SpriteLayer() override;


private:


    float alpha = 1.0;                                  // Transparency for sprites: 0 - 1.0
    Blend blend = Blend::ALPHA;                         // Blending mode for sprites: SOLID, ALPHA, ADDITIVE

    //---
    std::vector<Sprite*>sprites;                        // OWNED sprites.
    bool editorTileLayer = false;
    bool spritesChanged = false;
    int spritesChangeFlag = 0;




    void UpdateSingleSpriteStretch(Vec2i _designSize, bool _stretchX, bool _stretchY);
    bool UpdateStretchingSingleSpriteLayer();

};


//==================================================================================================


///\ingroup MapElements
///\brief The VectorLayer class defines the vector layer from JugiMap Editor.
///
/// A vector layer stores VectorShape objects and is their owner.
class VectorLayer : public Layer
{
public:
    friend class JugiMapBinaryLoader;
    friend class ComposedSprite;
    friend class Map;


    /// Constructor.
    VectorLayer();


    virtual void InitLayerParameters() override;
    virtual void UpdateEngineObjects() override;

    virtual void UpdateBoundingBox() override;


    /// \brief Returns a reference to the vector of stored shapes in this vector layer.
    std::vector<VectorShape*>& GetVectorShapes() {return vectorShapes;}


    /// \brief Adds a new vector shape to this vector layer.
    ///
    /// **Important:** This vector layer will take over the ownership of the added vector shape.
    void AddVectorShape(VectorShape* _vs){vectorShapes.push_back(_vs);}


protected:

    /// Destructor.
    virtual ~VectorLayer() override;


private:

    std::vector<VectorShape*>vectorShapes;             //OWNED

};



}


#endif
