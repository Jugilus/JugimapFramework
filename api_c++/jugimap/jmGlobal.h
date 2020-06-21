#ifndef JUGIMAP_GLOBAL_H
#define JUGIMAP_GLOBAL_H

#include <chrono>
#include "jmCommon.h"



namespace jugimap {


/// \addtogroup Globals
///  @{


/// The kinds of file.
enum class FileKind
{
    SINGLE_IMAGE,                   ///< Single image file.
    TILE_SHEET_IMAGE,               ///< Tile sheet image file.
    SPRITE_SHEET_IMAGE,             ///< %Sprite sheet image file.
    SPINE_FILE,                     ///< Spine file.
    SPRITER_FILE,                   ///< Spriter file.
    NOT_DEFINED                     ///< The file kind not defined.
};


/// The kinds of sprite. Used as identifier for Sprite objects.
enum class SpriteKind
{
    STANDARD,                       ///< Standard sprite - StandardSprite object.
    COMPOSED,                       ///< Composed sprite - ComposedSprite object.
    SPINE,                          ///< Spine sprite - SkeletonSprite object.
    SPRITER,                        ///< Spriter sprite - SkeletonSprite object.
    NOT_DEFINED                     ///< The sprite kind not defined.
};


/// The types of map.
enum class MapType
{
    WORLD,                  ///< World map.
    PARALLAX,               ///< Parallax map.
    SCREEN,                 ///< Screen map.
    NOT_DEFINED             ///< The map type not defined.
};


/// The kinds of layer. Used as identifier for Layer objects.
enum class LayerKind
{
    SPRITE,                     ///< %Sprite layer - SpriteLayer object.
    VECTOR,                     ///< Vector layer - VectorLayer object.
    DRAWING,                    ///< Drawing layer - DrawingLayer object.
    TEXT,                       ///< %Text layer - TextLayer object.
    CUSTOM,                     ///< Custom layer.
    NOT_DEFINED                 ///< The layer kind not defined.
};



/// The types of layer. Used to distinguish layers for world, parallax and screen maps.
enum class LayerType
{
    WORLD,
    PARALLAX,
    PARALLAX_STRETCHING_SINGLE_SPRITE,
    SCREEN,
    SCREEN_STRETCHING_SINGLE_SPRITE
};



/// The stretching variants.
enum class StretchingVariant
{
    XY_TO_WORLD_SIZE,
    XY_TO_VIEWPORT_SIZE
};


/// The align factors for the X direction.
enum class AlignX
{
    LEFT,                               ///< Align left.
    MIDDLE,                             ///< Align to middle.
    RIGHT                               ///< Align right.
};

/// The align factors for the Y direction.
enum class AlignY
{
    TOP,                                ///< Align top.
    MIDDLE,                             ///< Align to middle.
    BOTTOM                              ///< Align bottom.
};


/// The blend modes for the openGL pixel blending.
enum class Blend
{
    SOLID,                              ///< Solid (cover) blend.
    ALPHA,                              ///< Alpha blend.
    MULTIPLY,                           ///< Multiply (darken) blend.
    ADDITIVE,                           ///< Additive (lighten) blend.
    NOT_DEFINED                         ///< The blend not defined.
};


/// The blend modes for the shader based pixel blending which simulate photoshop blending modes.
enum class ColorOverlayBlend
{
    SIMPLE_MULTIPLY,                    ///< Simple multiply (not a real blending).
    NORMAL,                             ///< Normal mode.
    MULTIPLY,                           ///< Multiply mode.
    LINEAR_DODGE,                       ///< Linear dodge mode.
    COLOR,                              ///< Color mode.
    NOT_DEFINED                         ///< The blend not defined.
};


/// The kinds of font.
enum class FontKind
{
    TRUE_TYPE,                          ///< True type font.
    NOT_DEFINED                         ///< The font kind not defined.
};


/// The kinds of geometric shape. Used as identifier for GeometricShape and VectorShape objects.
enum class ShapeKind
{
    POLYLINE,                   ///< Polyline - PolyLineShape object.
    BEZIER_POLYCURVE,           ///< Bezier polycurve - BezierShape object.
    ELLIPSE,                    ///< Ellipse - EllipseShape object.
    SINGLE_POINT,               ///< Single point - SinglePointShape object.
    NOT_DEFINED                 ///< The shape kind not defined.
};



/// The kinds of animation. Used as identifier for Animation objects.
enum class AnimationKind
{
    FRAME_ANIMATION = 0,        ///< Frame animation - FrameAnimation object.
    TIMELINE_ANIMATION = 1,     ///< Timeline animation - TimelineAnimation object.
    NOT_DEFINED = -1
};


//==================================================================================


/// The kinds of animation track. Used as identifier for AnimationTrack and AnimationKey classes.
enum class AnimationTrackKind
{
    TRANSLATION,                ///< Translation.
    SCALING,                    ///< Scaling.
    ROTATION,                   ///< Rotation.
    ALPHA_CHANGE,               ///< Alpha change.
    OVERLAY_COLOR_CHANGE,       ///< Overlay color change.
    PATH_MOVEMENT,              ///< Path movement.
    FRAME_CHANGE,               ///< Frame change.
    FRAME_ANIMATION,            ///< Frame animation.
    TIMELINE_ANIMATION,         ///< Timeline animation.
    FLIP_HIDE,                  ///< Flip, hide.
    META,                       ///< Meta.
    NOT_DEFINED                 ///< Not defined.

};




/// The states of animation player.
enum class AnimationPlayerState
{
    IDLE,                       ///< The player is idle and has assigned no animation instance.
    PLAYING,                    ///< The player is playing and updating animation instance.
    STALLED,                    ///< The player is playing but animation instance is not being updated. This a case when an animation is not being looped and remains in its end state.
    PAUSED,                     ///< The player has been paused.
    WAITING_TO_START,           ///< The player is waiting to start animation due to the *startDelay* parameter.
    WAITING_TO_REPEAT           ///< The player is waiting to repeat animation due to the *repeat* parameter.
};



///\ingroup Animation
/// \brief The bit-mask flags of animation player.
struct AnimationPlayerFlags
{

    static const int NONE = 0;                          ///< None.

    // Play
    static const int DISCARD_ANIMATION_QUEUE = 1;       ///< Stop and discard animations in queue and start the new animation immediately. This is an input flag for AnimationQueuePlayer::Play.
    static const int SKIP_SUBPLAYER_UPDATING = 2;       ///< Not available.



    // Return flags
    static const int PLAYER_STARTED = 1;                ///< The return flag for AnimationQueuePlayer::Play command. If the player already plays the given animation this flag is not returned.
    static const int PLAYER_UPDATED = 2;                ///< The return flag for AnimationQueuePlayer::Update command. If the player is in the in the idle state this flag is not returned.
    static const int PLAYER_STOPPED = 4;                ///< The return flag for AnimationQueuePlayer::Stop command. If the player is already in the idle state this flag is not returned.
    static const int ANIMATION_INSTANCE_UPDATED = 8;    ///< The return flag for AnimationQueuePlayer::Play and AnimationQueuePlayer::Update commands when the animation update has been performed.
    static const int ANIMATED_PROPERTIES_CHANGED = 16;  ///< The return flag for AnimationQueuePlayer::Play and AnimationQueuePlayer::Update commands when the animated properties have been changed.
    static const int META_KEY_CHANGED = 32;             ///< The return flag for AnimationQueuePlayer::Play and AnimationQueuePlayer::Update commands when the meta animation key changes.

};





///\brief Settings
///
/// The Settings class stores jugimap configuration parameters.
class Settings
{
public:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Set the screen size to the given *_screenSize*.
    ///
    /// This function must be called at the game initialization before loading any maps.
    ///
    /// \see GetScreenSize
    void SetScreenSize(Vec2i _screenSize){ screenSize = _screenSize; }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns the screen size.
    ///
    /// \see SetScreenSize
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Vec2i GetScreenSize(){ return screenSize; }

    //---------------

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Set to *true* if the y coordinate in the used game engine points up.
    ///
    /// This function must be called at the game initialization before loading any maps.
    /// The default value is *false*.
    ///
    ///\see IsYCoordinateUp
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetYCoordinateUp(bool _yCoordinateUp){ yCoordinateUp = _yCoordinateUp;}


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns true if the y coordinate points up; if it points down returns false.
    ///
    /// \see SetYCoordinateUp
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsYCoordinateUp(){ return yCoordinateUp; }

    //---------------

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Set to *true* if the Collider objects should be used for the sprites collision detection.
    ///
    /// This option is usable only for engines which provide collision system.
    /// The function must be called at the game initialization before loading any maps.
    /// Collider objects are used for the Sprite interface functions for collision detection. Some engines offer a collision system which is not
    /// available via colliders. In that case you can disable colliders and use solely the engine methods for all collision
    /// detection tasks in your game.
    ///
    /// The default value is *true*.
    ///
    /// \see SpriteCollidersEnabled
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void EnableSpriteColliders(bool _spriteCollidersEnabled){ spriteCollidersEnabled = _spriteCollidersEnabled; }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns true if the Collider objects are used for the sprites collision detection; otherwise returns false.
    ///
    /// \see EnableSpriteColliders
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool SpriteCollidersEnabled(){ return spriteCollidersEnabled; }

    //---------------

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Set to *false* if the engine physics are available in the used engine but disabled.
    ///
    /// This parameter has no effect with engines which does not provide a physics simulation or it can not be disabled.
    ///
    /// The default value is *true*.
    ///
    /// \see EnginePhysicsEnabled
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void EnableEnginePhysics(bool _enginePhysicsEnabled){ enginePhysicsEnabled = _enginePhysicsEnabled; }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns *true* if the engine physics system is enabled; otherwise returns *false*.
    ///
    /// \see SetUseSpriteColliders
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool EnginePhysicsEnabled(){ return enginePhysicsEnabled; }

    //---------------

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief Set to *true* to enable the *lerp drawing* technique; otherwsie set to false.
    ///
    /// This option can be used with engines which update game logic with fixed time step.
    ///
    /// *Lerp drawing* describes a technique for reducing the stutter in scrolling or sprite movements which occurs
    /// when the game logic rate and the game drawing rate are not synchronized.

    /// The *lerp drawing* means that we obtain the actual drawing position of sprites as an interpolated point between
    /// the previous and the current position. The in-between point is calculated via the **lerpFactor** which should be set in the fixed rate logic
    /// of the main game loop.
    ///
    /// An important function related to *lerp drawing* is a function called **CaptureForLerpDrawing*. It is available for maps, layers and sprites.
    /// Capture for *lerp drawing* means that you set the sprite's previous position variable to the same value as the current position.
    /// This function must be called at initialization and whenever a sprite changes
    /// its position abruptly - the change is not a part of animated (smooth) movement.
    ///
    /// If the *CaptureForLerpDrawing* is not called correctly there may be weird sprite placements in the game.
    /// It may be a good idea to temporary disable *useLerpDrawing* during development and when everything works as intended
    /// we activate it to improve smoothness.
    ///
    /// The default value is *false*.
    ///
    /// \see LerpDrawingEnabled
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void EnableLerpDrawing(bool _lerpDrawingEnabled){ lerpDrawingEnabled = _lerpDrawingEnabled; }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns *true* if the *lerp drawing* technique is used; otherwise returns *false*.
    ///
    /// see EnableLerpDrawing
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool LerpDrawingEnabled(){ return lerpDrawingEnabled; }

    //---------------

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Set the interpolating factor for *lerp drawing*.
    ///
    /// The *lerp drawing factor* is needed if *lerp drawing* is active and the engine supports it.
    /// **Important:** This function must be called in the game update function in order to regularly provide the current factor.
    ///
    /// \see GetLerpDrawingFactor
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLerpDrawingFactor(float _lerpDrawingFactor){ lerpDrawingFactor = _lerpDrawingFactor; }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns the interpolating factor for *Lerp Drawing*.
    ///
    ///\see SetLerpDrawingFactor
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    float GetLerpDrawingFactor(){ return lerpDrawingFactor; }



    //---------------

    ///\brief Set the global error message.
    void SetErrorMessage(const std::string &_errorMessage){ errorMessage = _errorMessage; }


    ///\brief Returns the global error message.
    std::string &GetErrorMessage(){ return errorMessage; }



    //---------------
    ///\brief Set the *z-order* step. Used by engines which use a z-order factor for drawing order of sprites.
    void SetZOrderStep(int _zOrderStep){ zOrderStep = _zOrderStep; }

    ///\brief Returns the *z-order* step.
    int GetZOrderStep(){ return zOrderStep; }




private:

    Vec2i screenSize;
    bool yCoordinateUp;
    bool spriteCollidersEnabled = true;
    bool lerpDrawingEnabled = false;
    bool enginePhysicsEnabled = true;
    std::string errorMessage;
    //long int millisecondsPassed = 0;
    float lerpDrawingFactor = 1.0;
    //float logicTimeStepMS;
    int zOrderStep = 10;

};


/// The Settings object.
extern Settings settings;


///\brief Time
///
/// The Time class stores required time parameters.
class Time
{
public:


    ///\brief Updates and returns the passed time in milliseconds.
    ///
    /// This function must called at the begining of logic update.
    /// \see GetPassedNetTimeMS
    double UpdatePassedTimeMS()
    {
        if(started==false){
            initTimePoint = std::chrono::high_resolution_clock::now();
            started = true;
        }
        passedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - initTimePoint).count();
        return passedMicroseconds/1000.0;
    }


    ///\brief Add suspended time in milliseconds.
    ///
    /// Suspended time is time when application is suspended. This function is called from SceneManager::Update and should not be called manually.
    void AddSuspendedTimeMS(int _suspendedTimeMS)
    {
        suspendedTimeMS += _suspendedTimeMS;
    }



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns passed net time in milliseconds.
    ///
    /// This function returns the net passed time which is the full passed time with deducted suspended time.
    ///\see UpdatePassedTimeMS()
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int GetPassedNetTimeMS(){ return passedMicroseconds/1000.0 - suspendedTimeMS; }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Set the logic time step in milliseconds.
    ///
    /// This function must called at the begining of logic update.
    /// Depending of the engine this time step can be the frame time or fixed logic time step.
    ///\see GetLogicTimeMS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLogicTimeMS(float _logicTimeStepMS){ logicTimeStepMS = _logicTimeStepMS; }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///\brief Returns the frame time in milliseconds.
    ///
    ///\see SetLogicTimeMS
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    float GetLogicTimeMS(){ return logicTimeStepMS; }




private:
    std::chrono::high_resolution_clock::time_point initTimePoint;
    unsigned long long passedMicroseconds = 0;

    int suspendedTimeMS = 0;
    bool started = false;
    float logicTimeStepMS;

};

extern Time time;




/// Delete global jugimap objects.
///
/// This function must be called at the application exits.
void DeleteGlobalObjects();



/// @}*    //end of group Globals

}

#endif


