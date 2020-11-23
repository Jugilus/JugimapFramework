#ifndef JUGIMAP_SPRITES_H
#define JUGIMAP_SPRITES_H

#include "jmCommon.h"
#include "jmColor.h"
#include "jmGlobal.h"



namespace jugimap {


class JugiMapBinaryLoader;
class GraphicItem;
class Layer;
class SpriteLayer;
class SourceSprite;
class ComposedSprite;
class StandardSprite;
class Collider;
struct AnimatedProperties;

//class TextLayer;
class Font;

struct SourceSpriteReplacements;



///\ingroup MapElements
///\brief The Sprite is the base sprite class.
class Sprite
{
public:
    friend class JugiMapBinaryLoader;
    friend class ComposedSprite;
    friend class SpriteLayer;
    friend class ComposedSprite;

    /// The Property enumerator provides flags for sprite properties.
    enum Property
    {
        NONE =      0,
        POSITION =  1 << 0,
        FLIP =      1 << 1,
        SCALE =     1 << 2,
        ROTATION =  1 << 3,
        HANDLE =    1 << 4,

        ALPHA =     1 << 5,
        BLEND =     1 << 6,
        OVERLAY_COLOR = 1 << 7,
        COLOR =     1 << 8,

        TEXTURE =     1 << 10,
        TEXT_STRING = 1 << 11,

        VISIBILITY =  1 << 13,

        ID =        1 << 15,
        TAG =       1 << 16,
        DATA =      1 << 17,
        LINK =      1 << 18,

        TRANSFORMATION = POSITION | FLIP | SCALE |  ROTATION | HANDLE,
        APPEARANCE = ALPHA | BLEND | OVERLAY_COLOR | COLOR,
        ALL = TRANSFORMATION | APPEARANCE | TEXTURE | TEXT_STRING | VISIBILITY |  ID | TAG | DATA | LINK
    };


    /// Constructor.
    Sprite(){}
    Sprite(const Sprite &s) = delete;
    Sprite& operator=(const Sprite &s) = delete;


    ///\brief Create a new sprite which is a passive copy of this sprite.
    ///
    /// Sprites created as passive copies does not have initialized their engine sprite and they are not added to the layer.
    /// To display a passive sprite on screen you must turn it into an active copy:
    /// \code
    /// sprite->GetLayer()->AddSprite(sprite);
    /// sprite->InitEngineSprite();
    /// \endcode
    ///
    /// You can also create an active sprite from a passive sprite by using MakeActiveCopy.
    /// \see MakeActiveCopy
    Sprite* MakePassiveCopy();


    ///\brief Create a new sprite which is an active copy of this sprite.
    ///
    /// Sprites created as an active copies are displayed on screen immediately.
    /// \see MakeActiveCopy
    Sprite* MakeActiveCopy();




    ///\brief Copy properties from the given **_sprite**.
    ///
    /// The given **copyFlags** must consists of flags defined in the Property enumerator.
    /// **Important:** This function does not copy core sprite properties:
    /// *source Sprite*, *layer*, *parentComposedSprite*;
    virtual void CopyProperties(Sprite *_sprite, int copyFlags);


    //------ MANAGE ENGINE OBJECTS


    ///\brief Initialize the engine sprite.
    ///
    /// This function creates and initialize the engine sprite.
    /// It gets called in the SpriteLayer::InitEngineLayer at the map engine initialization but we can use it also
    /// to initilaze sprites created with MakePassiveCopy.
    virtual void InitEngineObjects(){}


    ///\brief Update the engine sprite.
    ///
    /// This function updates the engine sprite when properties of the jugimap sprite change. Usually you don't need to use it as other functions call it when required.
    virtual void UpdateEngineObjects(){}


    ///\brief Draw the engine sprite.
    ///
    /// This function draws the engine sprite on screen.
    /// It is applicable only for engines which do not offer automatic drawing (rendering) of sprites.
    virtual void DrawEngineSprite(){}


    ///\brief Returns true if the engine sprite of this sprite has been initialized; otherwise returns false.
    ///
    /// \see InitEngineSprite
    virtual bool IsEngineSpriteInitialized() {return false;}


    //------


    ///\brief Set the visibility of this sprite.
    ///
    /// \see IsVisible
    virtual void SetVisible(bool _visible);


    ///\brief Returns *true* if this sprite is visible; otherwise returns false.
    ///
    /// \see IsVisible
    bool IsVisible(){ return visible; }


    //------        

    ///\brief Set a boolean flag which indicates if this sprite's engine sprite is used directly.
    ///
    /// By default a jugimap sprite is manipulated via Sprite interface and its engine sprite is updated via UpdateEngineSprite function.
    /// If you wish to manipulate the engine sprite directly you should set this flag to true in which case the engine sprite will no longer get updated via UpdateEngineSprite.
    /// Be aware that such sprite will also no longer have updated bounding box and colliders.
    /// An example of using engine sprites directly is when they are used as dynamic objects in physics engine.
    /// \see IsEngineSpriteUsedDirectly
    void SetDisabledEngineSpriteUpdate(bool _engineSpriteUsedDirectly){ engineSpriteUsedDirectly = _engineSpriteUsedDirectly; }


    ///\brief Returns *true* if this sprite's engine sprite is used directly.
    ///
    /// \see SetEngineSpriteUsedDirectly
    bool IsEngineSpriteUsedDirectly(){ return engineSpriteUsedDirectly; }



    ///\brief Update the map bounding box of this sprite.
    ///
    /// The dimensions of bounding box are based on the size of the active texture. If a sprite is not scaled or rotated the bounding box and the texture have the same size.
    /// Usually you don't need to use this function as other functions call it when required.
    virtual void UpdateBoundingBox() {boundingBox = Rectf(position, position);}


    /// \brief Returns the world bounding rectangle of this sprite.
    ///
    /// \see UpdateBoundingBox
    Rectf GetBoundingBox(){ return boundingBox;}


    ///\brief Capture the sprite properties, usually the position only,  required for the *lerp drawing*.
    ///
    /// \see Globals::SetUseLerpDrawing
    virtual void CaptureForLerpDrawing(){}


    //---- LINK OBJECTS


    ///\brief Returns a sprite layer where this sprite is stored.
    SpriteLayer* GetLayer() {return layer;}


    ///\brief Returns the source sprite of this sprite.
    SourceSprite* GetSourceSprite() {return sourceSprite;}


    ///\brief Returns the parent composed sprite of this sprite.
    ///
    /// If this sprite is not inside a composed sprite returns nullptr.
    ComposedSprite* GetParentComposedSprite() {return parentComposedSprite;}


    ///\brief Returns the kind of this sprite.
    ///
    /// The kind factor is the same as the kind of assigned SourceSprite object.
    SpriteKind GetKind();


    //------
    ///\brief Set the id of this sprite to the given _id.
    ///
    /// An id is an arbitrary integer number which can be assigned to a Sprite object in the editor.
    /// \see GetId
    void SetId(int _id){ id = _id; }


    ///\brief Returns the id of this sprite.
    ///
    /// \see SetId
    int GetId(){ return id;}


    ///\brief Set the name of this sprite to the given _nameID.
    ///
    /// A name is an arbitrary string which can be assigned to a Sprite object in the editor.
    /// \see GetName
    void SetName(const std::string& _nameID){ name = _nameID; }


    ///\brief Returns the name of this sprite.
    ///
    /// \see SetName
    std::string GetName(){ return name;}



    ///\brief Set the tag of this sprite to the given _tag.
    ///
    /// A tag is an arbitrary integer number which can be assigned to a Sprite object.
    /// \see GetTag
    void SetTag(int _tag){ tag = _tag; }


    ///\brief Returns the tag of this sprite.
    ///
    /// \see SetTag
    int GetTag(){ return tag;}


    //------

    ///\brief Assign a custom object of this sprite to the given _customObject.
    ///
    /// A custom object is an arbitrary object which can be assigned to a sprite.
    /// **Important!** The sprite takes ownership over the custom object.
    /// \see GetCustomObject
    void SetCustomObject(CustomObject* _customObject){ customObject = _customObject; }


    ///\brief Returns the custom object of this sprite if exists; otherwise returns nullptr.
    ///
    /// \see SetCustomObject
    CustomObject* GetCustomObject(){ return customObject; }


    //------


    ///\brief Returns a reference to the vector of stored parameters in this sprite.
    ///
    /// These are the parameters defined in Jugimap Editor.
    std::vector<jugimap::Parameter> &GetParameters(){return parameters;}


    ///\brief Returns the dataFlags of this sprite.
    ///
    /// This is an integer defined in JugiMap Editor.
    int GetDataFlags() {return dataFlags;}


    // TRANSFORM PROPERTIES
    //-------------------------------------------------------------------------------

    ///\brief Set the position of this sprite to the given **_position**.
    ///
    /// \see GetPosition, GetGlobalPosition, GetFullPosition, GetFullGlobalPosition
    void SetPosition(Vec2f _position);


    ///\brief Returns the position of this sprite.
    ///
    /// This is a local position which is for sprites inside the map layers also a global position.
    /// For sprites in layers of a composed sprite this position is relative to the composed sprite coordinate system.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetGlobalPosition, GetFullPosition, GetFullGlobalPosition, SetPosition
    Vec2f GetPosition(bool _includeAnimatedProperties = true);


    ///\brief Returns the global position of this sprite.
    ///
    /// For sprites in map layers this position matches their (local) position.
    /// For sprites in layers of a composed sprite this position is their local position converted to global map coordinate system.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetPosition, GetFullPosition, GetFullGlobalPosition, SetPosition
    Vec2f GetGlobalPosition(bool _includeAnimatedProperties = true);


    //------


    ///\brief Set the horizontal and vertical scale factor of this sprite to the given **_scale**.
    ///
    ///\see GetScale, GetGlobalScale
    void SetScale(Vec2f _scale);


    ///\brief Returns the horizontal and vertical scale factor of this sprite.
    ///
    /// This is a local scale factor which is for sprites inside the map layers also a global scale factor.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetGlobalScale, SetScale
    Vec2f GetScale(bool _includeAnimatedProperties = true);


    ///\brief Returns the global scale factor of this sprite.
    ///
    /// For sprites in map layers this scale matches the (local) scale.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetScale, SetScale
    Vec2f GetGlobalScale(bool _includeAnimatedProperties = true);


    //------


    ///\brief Set the rotation of this sprite to the given **_rotation**.
    ///
    /// The rotation is in degrees.
    ///\see GetRotation, GetGlobalRotation
    void SetRotation(float _rotation);


    ///\brief Returns the rotation of this sprite in degrees.
    ///
    /// This is a local rotation which is for sprites inside the map layers also a global rotation.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetGlobalRotation, SetRotation
    float GetRotation(bool _includeAnimatedProperties = true);


    ///\brief Returns the global rotation of this sprite.
    ///
    /// For sprites in map layers this rotation matches the (local) rotation.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetRotation, SetRotation
    float GetGlobalRotation(bool _includeAnimatedProperties = true);


    //------


    ///\brief Set the horizontal and vertical flip factor of this sprite to the given **_flip**.
    ///
    ///\see GetFlip, GetGlobalFlip
    void SetFlip(Vec2i _flip);


    ///\brief Returns the horizontal and vertical flip factor of this sprite.
    ///
    /// The flip value 1 means that a sprite is *flipped*, the value 0 means that it is not.
    /// This is a local flip factor which is for sprites inside the map layers also a global flip factor.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetGlobalFlip, SetFlip
    Vec2i GetFlip(bool _includeAnimatedProperties = true);


    ///\brief Returns the horizontal and vertical world flip factor of this sprite.
    ///
    /// The flip value 1 means that a sprite is *flipped*, the value 0 means that it is not.
    /// For sprites in map layers this flip factor matches the (local) flip factor.
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see GetFlip, SetFlip
    Vec2i GetGlobalFlip(bool _includeAnimatedProperties = true);


    //------

    ///\brief Set the handle point of this sprite to the given **_handle** point.
    ///
    /// The handle point is a point in the sprite local coordinates, relative to which the sprite transformations are being applied.
    /// This point is automatically assigned to the handle point of the active image (for the standard sprites).
    /// You can manually use this function to set an arbitrary handle point but be aware that changing
    /// the active image will set the sprite handle point again to that of the image. The sprite shapes
    /// are also relative to the image handle point and will no longer be correct if you set a different handle point.
    virtual void SetHandle(Vec2f _handle);


    ///\brief Returns the handle point of this sprite.
    Vec2f GetHandle() {return handle;}


    // APPEARANCE PROPERTIES
    //-------------------------------------------------------------------------------

    ///\brief Set the alpha of this sprite to the given **_alpha**.
    ///
    /// The alpha is an opacity factor in the range of [0.0 - 1.0].
    ///\see GetAlpha
    void SetAlpha(float _alpha);


    ///\brief Returns the alpha of this sprite.
    ///
    /// \see SetAlpha
    //float GetAlpha(){return alpha;}


    ///\brief Returns the alpha of this sprite.
    ///
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see SetAlpha
    float GetAlpha(bool _includeAnimatedProperties = true);


    //------

    ///\brief Set the own blend of this sprite to the given **_blend**.
    ///
    /// The own blend is a Blend factor assigned to a sprite. If the own blend is not defined (Blend::NOT_DEFINED)
    /// then the Blend factor of the sprite layer will be used.
    ///\see GetOwnBlend
    void SetOwnBlend(Blend _blend);


    ///\brief Returns the own blend of this sprite.
    ///
    ///\see SetOwnBlend
    Blend GetOwnBlend(){return ownBlend;}


    //------

    ///\brief Set to *true* to make the color overlay active; set to *false* to disable the color overlay.
    ///
    /// The color overlay is a color overlayed over a sprite.
    ///\see IsOverlayColorActive, SetColorOverlayRGBA, SetColorOverlayBlend
    void SetColorOverlayActive(bool _colorOverlayActive);


    ///\brief Returns true if the overlay color of this sprite is active; otherwise returns false.
    ///
    ///\see SetColorOverlayActive, SetColorOverlayRGBA, SetColorOverlayBlend
    bool IsOverlayColorActive();



    ///\brief Set the color overlay of this sprite to the given  **_colorOverlayRGBA**.
    ///
    ///\see GetColorOverlayRGBA, SetColorOverlayActive, SetColorOverlayBlend
    void SetColorOverlayRGBA(ColorRGBA _colorOverlayRGBA);


    ///\brief Returns the overlay color of this sprite.
    ///
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    ///\see SetColorOverlayRGBA,
    ColorRGBA GetOverlayColorRGBA(bool _includeAnimatedProperties = true);


    ///\brief Set the color overlay blend of this sprite to the given  **_colorOverlayBlend**.
    ///
    /// This blending works the same as blending in painting applications when you overlay layers.
    ///\see GetColorOverlayBlend, SetColorOverlayActive, SetColorOverlayRGBA
    void SetColorOverlayBlend(ColorOverlayBlend _colorOverlayBlend);


    ///\brief Returns the overlay color blend of this sprite.
    ///
    /// If the given *_includeAnimatedProperties* is *true*, the animated properties are included in the returned value.
    ///
    /// \see SetColorOverlayBlend
    ColorOverlayBlend GetOverlayColorBlend(bool _includeAnimatedProperties = true);


    //------ ANIMATED PROPERTIES


    ///\brief Returns the pre-animation properties.
    AnimatedProperties* GetAnimatedProperties(){ return ap; }


    ///\brief Store current properties for animation purposes.
    ///
    /// This function is usually called from animation objects.
    ///\see Collider
    virtual void CreateAnimatedPropertiesIfNone();


    ///\brief Set sprite properties to pre-animation state.
    ///
    /// This function is usually called from animation objects.
    ///\see Collider
    virtual void ResetAnimatedProperties();


    ///\brief Set sprite properties from the given **_ap** object.
    ///
    /// This function is usually called from animation objects.
    ///\see Collider
    virtual void AppendAnimatedProperties(AnimatedProperties& _ap);



    //------ COLLIDER


    ///\brief Returns true if this sprite has assigned a collider; otherwise returns false.
    ///
    /// A collider is assigned to sprites which have SourceSprite with defined sprite shapes.
    ///\see Collider
    virtual bool HasCollider() { return false; }


    ///\brief Returns true if the given point **_pos** is inside the shape(s) of this sprite; otherwise returns false.
    ///
    /// The given point must be in global coordinate system. This function uses a sprite collider to obtain the result. It returns *false* if this sprite has no collider assigned.
    ///\see Collider
    virtual bool PointInside(Vec2f _pos){ return false; }


    ///\brief Returns true if the shapes of this and the given **_sprite** overelaps; otherwise returns false.
    ///
    /// This function uses sprite colliders to obtain the result. It returns *false* if one or both of sprites is without a collider.
    ///\see Collider
    virtual bool Overlaps(Sprite *_sprite){  return false; }


    ///\brief Returns true if a ray (line segment) from **_rayStart** to **_rayEnd** 'hits' at the shape(s) of this sprite; otherwise return false.
    ///
    /// If the 'hit' occurs its position get stored in the given **_hitPos**.
    /// This function uses sprite colliders to obtain the result. It returns *false* if this sprite has no collider assigned.
    ///\see Collider
    virtual bool RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos){  return false; }



    //-------- CHANGE FLAGS


    ///\brief Set the change flags of this sprite to the given *_changeFlags*.
    ///
    /// The *_changeFlags* must be a value or a bitwise combination of values defined in the Property enumerator.
    /// \see AppendToChangeFlags, GetChangeFlags
    void SetChangeFlags(int _changeFlags);


    ///\brief Append the given *_changeFlags* to the change flags of this sprite with a biwise operation.
    /// ///
    /// The *_changeFlags* must be a value or a bitwise combination of values defined in the Property enumerator.
    /// \see SetChangeFlags, GetChangeFlags
    void AppendToChangeFlags(int _changeFlags);


    ///\brief Returns the *change flags* of this sprite.
    ///
    /// \see SetChangeFlags, AppendToChangeFlags
    int GetChangeFlags() {return changeFlags;}


    //--------


    ///\brief Set the source sprite of this sprite to the given **_sourceSprite**.
    ///
    /// This function must never be used on already created sprites.
    /// It should be used only for purposes of manual sprite creation.
    void _SetSourceSprite(SourceSprite *_sourceSprite){ sourceSprite = _sourceSprite; }


    ///\brief Set the layer of this sprite to the given **_sourceSprite**.
    ///
    /// This function must never be used on already created sprites.
    /// It should be used only for purposes of manual sprite creation.
    void _SetLayer(SpriteLayer *_spriteLayer){ layer = _spriteLayer; }



    SourceSpriteReplacements* GetSourceSpriteReplacements(){ return ssr;}

    SourceSprite* GetOriginalSourceSprite();


protected:

    void SetBoundingBox(const Rectf &_boundingBox) { boundingBox = _boundingBox;}


    /// \brief Destructor.
    ///
    /// **Important:** Do not delete sprites directly. Use SpriteLayer::RemoveAndDeleteSprite command:
    /// \code
    /// sprite->GetLayer()->RemoveAndDeleteSprite(sprite);
    /// \endcode
    virtual ~Sprite();


private:
    SpriteLayer *layer = nullptr;                           // LINK pointer to a layer where the sprite is stored.
    SourceSprite * sourceSprite= nullptr;                   // LINK pointer to the belonging SourceSprite.
    ComposedSprite * parentComposedSprite = nullptr;        // LINK pointer to parent composed sprite if sprite is child of a composed sprite.

    //---
    int id = 0;
    std::string name;
    int tag = 0;
    CustomObject* customObject = nullptr;

    //---
    bool visible = true;
    bool engineSpriteUsedDirectly = false;


    //--- TRANSFORMATION PROPERTIES
    Vec2f position;
    //Vec2f animationFrameOffset;
    Vec2f scale = Vec2f(1.0, 1.0);
    float rotation = 0.0;
    Vec2i flip;
    Vec2f handle;

    Rectf boundingBox;


    //--- APPEARANCE PROPERTIES
    float alpha = 1.0;                                      // in scale [0 - 1.0]
    Blend ownBlend = Blend::NOT_DEFINED;
    bool colorOverlayActive = false;
    ColorRGBA colorOverlayRGBA = 0;
    ColorOverlayBlend colorOverlayBlend = ColorOverlayBlend::NORMAL;


    //---
    AnimatedProperties* ap = nullptr;


    //--- DATA
    std::vector<Parameter> parameters;
    int dataFlags = 0;


    SourceSpriteReplacements* ssr = nullptr;
    SourceSpriteReplacements* ssrChildren = nullptr;

    //----
    int changeFlags = 0;

};


//=======================================================================================


///\ingroup MapElements
///\brief The ComposedSprite class defines the composed sprite from JugiMap Editor.
///
/// A composed sprite stores Layer objects and is their owner.
class ComposedSprite : public Sprite
{
public:
    friend class JugiMapBinaryLoader;
    friend class Sprite;
    friend class StandardSprite;
    friend class SkeletonSprite;

    /// Constructor.
    ComposedSprite(){}


    virtual void InitEngineObjects() override;
    virtual void UpdateEngineObjects() override;
    virtual void DrawEngineSprite() override;
    virtual void SetVisible(bool _visible) override;
    virtual bool HasCollider() override;
    virtual bool PointInside(Vec2f _pos) override;
    virtual bool Overlaps(Sprite *_sprite) override;
    virtual bool RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos) override;
    virtual void UpdateBoundingBox() override;
    virtual void CaptureForLerpDrawing() override;
    virtual void ResetAnimatedProperties() override;


    /// \brief Returns a position in the map system from the given *_pos* in the system of this composed sprite.
    Vec2f ConvertToWorldPos(const Vec2f &_pos)
    {
        return cTransform.Transform(_pos);
    }

    /// \brief Returns a cumulative scale in the map system from the given *_scale* in the system of this composed sprite.
    Vec2f ConvertToWorldScale(const Vec2f &_scale)
    {
        return Vec2f(cScale * _scale.x, cScale * _scale.y);
    }

    /// \brief Returns a cumulative rotation in the map system from the given *_rotation* in the system of this composed sprite.
    float ConvertToWorldRotation(float _rotation)
    {
        return (cFlip.y + cFlip.x == 1)?  cRotation - _rotation : cRotation + _rotation;    // one (and only one) flip changes direction of rotation !
    }

    /// \brief Returns a cumulative flip in the map system from the given *_flip* in the system of this composed sprite.
    Vec2i ConvertToWorldFlip(const Vec2i &_flip)
    {
        return Vec2i((_flip.x==1)? 1 - cFlip.x : cFlip.x , (_flip.y==1)? 1 - cFlip.y : cFlip.y);
    }


    Vec2f GetLocalPositionFromInMapPosition(const Vec2f &_pos) { return cTransform.Invert().Transform(_pos); }


    /// \brief Returns a reference to the vector of stored layers in this composed sprite.
    std::vector<Layer*> & GetLayers()  {return layers; }


    void SetChildrenSpritesChanged(bool _childrenSpritesChanged);



    //----
    AffineMat3f GetCTransform(){ return cTransform; }
    Vec2i GetCFlip(){ return cFlip; }
    float GetCScale(){ return cScale; }
    float GetCRotation(){ return cRotation; }


protected:

    /// Destructor.
    virtual ~ComposedSprite() override;


private:
    std::vector<Layer*>layers;                         // OWNED

    // cumulative tranform properties
    AffineMat3f cTransform;
    Vec2i cFlip;
    float cScale = 1.0;
    float cRotation = 0.0;

    // misc - for construction purposes
    Vec2i tileSize;
    Vec2i nTiles;
    Vec2i size;

    bool childrenSpritesChanged = false;





    void UpdateTransform();
    //static void CopyLayers(ComposedSprite *csSource, ComposedSprite *csDest, JugiMapBinaryLoader *loader=nullptr);
    static void CopyLayers(ComposedSprite *csSource, ComposedSprite *csDest, int &zOrder);
};


//=======================================================================================


///\ingroup MapElements
///\brief The StandardSprite class defines the sprite from JugiMap Editor.
class StandardSprite  : public Sprite
{
public:
    friend class JugiMapBinaryLoader;


    /// The physics mode of the standard sprite.
    enum PhysicsMode{
        NO_PHYSICS=0,       ///< Not included in the physics simulation.
        STATIC=1,           ///< Included in the physics simulation as a static object.
        DYNAMIC=2,          ///< Included in the physics simulation as a dynamic object.
        KINEMATIC=3         ///< Included in the physics simulation as a kinematic object.
    };


    /// Constructor.
    StandardSprite(){}


    void UpdateBoundingBox() override;
    virtual bool HasCollider() override;
    virtual bool PointInside(Vec2f _pos) override;
    virtual bool Overlaps(Sprite *_sprite) override;
    virtual bool RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos) override;


    /// \brief Returns the active image.
    GraphicItem * GetActiveImage() {return activeImage;}


    /// \brief Set the active image to the given *_image*.
    ///
    /// **Important:** The *_image* must be one of the images within the source sprite's graphic items.
    /// The *_image* can also be nullptr in which case the sprite will not be visible.
    virtual void SetActiveImage(GraphicItem *_image);


    /// \brief Returns the index of the active image in the source sprite's graphic items vector.
    ///
    /// If the active image is nullptr it returns -1.
    int GetActiveImageFrameIndex();


    /// \brief Set the active image to the image at the given *_frameIndex* in the source sprite's graphic items vector.
    ///
    /// The index must be inside the vector's bounds!
    void SetActiveImageByFrameIndex(int _frameIndex);


   // bool GetPreserveShapesOnNullActiveImage(){ return preserveShapesOnNullActiveImage;}


    /// \brief Set collider to the given *_collider*.
    ///
    /// This function is called at the sprite initialization.
    void SetCollider(Collider *_collider){ collider = _collider; }


    /// \brief Returns the collider or nullptr if none.
    Collider * GetCollider(){ return collider; }


    /// \brief Returns the physics mode.
    ///
    /// This function can be used with engines which provides a physics simulation engine.
    virtual PhysicsMode GetPhysicsMode(){ return physicsMode; }


    /// \brief Set the physics mode to the given *_physicsMode*.
    ///
    /// This function can be used with engines which provides a physics simulation engine.
    virtual void SetPhysicsMode(PhysicsMode _physicsMode){ physicsMode = _physicsMode;}


    virtual void AppendAnimatedProperties(AnimatedProperties& _ap) override;



protected:

    /// Destructor.
    virtual ~StandardSprite();

    void SetColliderShapes();
    void UpdateColliderAndBoundingBox();


private:
    GraphicItem *activeImage = nullptr;                       // LINK to currently active (displayed) image from the source sprite 'graphicItems' list.
    Collider *collider = nullptr;
    bool preserveShapesOnNullActiveImage = true;
    PhysicsMode physicsMode = PhysicsMode::NO_PHYSICS;




};



///\ingroup MapElements
///\brief The TextSprite class defines the text sprite from JugiMap Editor.
class TextSprite : public Sprite
{
public:


    ///\brief Constructor
    TextSprite(){};


    virtual void InitEngineObjects() override;
    virtual void CopyProperties(Sprite *_sprite, int copyFlags) override;
    virtual void UpdateBoundingBox() override;
    virtual bool PointInside(Vec2f _pos) override;

    //------

    /// Returns the font of this text sprite.
    Font* GetFont(){ return font; }


    ///\brief Set the text string of this text sprite to the given _textString.
    ///
    /// \see GetTextString
    void SetTextString(const std::string &_textString);


    ///\brief Returns the string of this text sprite.
    ///
    /// \see SetTextString
    std::string GetTextString(){ return textString; }


    ///\brief Set the color of this text sprite to the given _color.
    ///
    /// Only red, green and blue componenet will be used for the text color. Alpha component is set with *SetAlpha*.
    /// \see GetColor, SetAlpha
    void SetColor(ColorRGBA _color);


    ///\brief Returns the color of this text sprite.
    ///
    /// \see SetColor
    ColorRGBA GetColor(){ return color; }


    ///\brief Returns the height of this text sprite.
    ///
    /// This function require initialized engine text.
    /// \see GetSize, GetWidth
    float GetHeight();


    ///\brief Returns the width of this text sprite.
    ///
    /// This function require initialized engine text.
    /// \see GetSize, GetHeight
    float GetWidth();


    ///\brief Returns the size of this text sprite.
    ///
    /// This function require initialized engine text.
    /// \see GetWidth, GetHeight
    Vec2f GetSize();


    ///\brief Returns the relative handle of this text sprite.
    ///
    /// The relative handle has coordinates in range from 0.0 to 1.0.
    Vec2f GetRelativeHandle(){ return relHandle; }


    ///\brief Set this text sprite to be clickable.
    ///
    /// If a text sprite is clickable you can check A clickable tex sprite can
    //void SetClickable(bool _clickable) { clickable = _clickable; }

    //bool IsClickable(){ return clickable; }

    //virtual void ObtainSize() {}


    void _SetSize(Vec2f _size);
    void _SetFont(Font *_font){ font = _font; }
    /*
    ///\brief Set the relative handle of this text sprite.
    ///
    /// The relative handle has coordinates in range from 0.0 to 1.0. The (absolute) handle is obtained
    /// from relative handle by multiplying it with the sprite size. For text sprites we always specify relative handle as this ensure
    /// that text preserve its allignement if the text string changes.
    /// This function is called by other functions and should not be called directly.
    /// \see GetRelativeHandle
    */
    void _SetRelativeHandle(Vec2f _relHandle){ relHandle = _relHandle; }



protected:


private:
    std::string textString;
    Font* font = nullptr;                       // LINK pointer
    ColorRGBA color{255,255,255,255};
    Vec2f size;

    Vec2f relHandle{0.5, 0.5};

    //bool clickable = false;


    void AdjustRelativeHandleToRoundedSize();

};




//=======================================================================================

///\ingroup MapElements
///\brief The SkeletonSprite class defines the skeleton sprite from JugiMap Editor.
class SkeletonSprite : public Sprite
{
public:
    friend class JugiMapBinaryLoader;

    /// Constructor.
    SkeletonSprite(){}

    virtual ~SkeletonSprite() override {}

    virtual void InitEngineObjects() override;

};



}




#endif  // JUGIMAP_SPRITES_H
