#ifndef JUGIMAP_ANIMATION_COMMON_H
#define JUGIMAP_ANIMATION_COMMON_H

#include <vector>
#include "jmCommonFunctions.h"

#include "jmGlobal.h"




namespace jugimap{




class GraphicItem;
class AnimationTrackState;
class AKMeta;
class SourceSprite;
class JugiMapBinaryLoader;





//==================================================================================

//Do not change content of this struct!
struct AnimationFrame
{
    GraphicItem *texture = nullptr;              // LINK
    int msTime = 100;                        // in milliseconds
    Vec2f offset;
    Vec2i flip;
    int dataFlags = 0;
};



//==================================================================================


struct AnimatedProperties
{

    Vec2f translation;
    Vec2f scale = Vec2f(1.0f, 1.0f);
    float rotation = 0.0f;
    Vec2i flip;
    bool hidden = false;
    float alpha = 1.0f;
    ColorRGBA colorOverlayRGBA = 0;
    ColorOverlayBlend colorOverlayBlend = ColorOverlayBlend::NORMAL;
    GraphicItem *graphicItem = nullptr;

    //----
    void Reset();
    void Append(AnimatedProperties &ap);

    //----
    int changeFlags = 0;

};


//==================================================================================


///\ingroup Animation
/// \brief The animation base parameters of Animation objects.
///
/// These parameters were set in the editor and should not be changed. The copy of this struct is made in AnimationInstance
/// objects which can be modified when played in AnimationPlayer objects.
struct AnimationBaseParameters
{

    int startDelay = 0;                     ///< The starting dealay of animation in milliseconds.
    int loopCount = 0;                      ///< The number of loops.
    bool loopForever = true;                ///< The animation loops forever.
    bool repeat = false;                    ///< The animation should repeats (if it doesn't loops forever).
    int repeat_DelayTimeStart = 0;          ///< The repeat delay - start point time in milliseconds.
    int repeat_DelayTimeEnd = 0;            ///< The repeat delay - end point time in milliseconds. The actual repeat delay is a random value between the start and end point time.
    bool startAtRepeatTime = true;          ///< Start the animation after the repeat delay (overrides the starting delay).

};



///\ingroup Animation
/// \brief The base animation class.
///
/// This class stores animation data and parameters as they were created in JugiMap Editor.
class  Animation
{
public:
    friend class JugiMapBinaryLoader;



    ///\brief Destructor.
    virtual ~Animation(){}


    ///\brief Returns the kind of this animation.
    AnimationKind GetKind(){ return kind; }


    ///\brief Returns the source sprite to which this animation belongs.
    SourceSprite* GetSourceSprite(){ return sourceObject; }


    ///\brief Returns the name of this animation.
    std::string GetName(){return name;}


    ///\brief Returns a reference to the base parameters of this animation.
    AnimationBaseParameters &GetBaseParameters(){ return bp;}


    ///\brief Returns a reference to the vector of stored parameters in this animation.
    std::vector<Parameter>& GetParameters(){return parameters;}


    ///\brief Returns the dataFlags factor of this frame animation.
    int GetDataFlags(){return dataFlags;}


    ///\brief Returns duration of this animation in milliseconds. This is duration of one loop.
    int GetDuration(){return duration;}



protected:
    AnimationKind kind = AnimationKind::NOT_DEFINED;
    SourceSprite * sourceObject = nullptr;                    // LINK to source sprite where this animation is stored
    std::string name;
    AnimationBaseParameters bp;
    int duration = 0;

    // custom data
    unsigned char dataFlags = 0;
    std::vector<Parameter> parameters;



    //----
    Animation(SourceSprite *_sourceObject);
    Animation(const std::string &_nameID, SourceSprite *_sourceObject);
    Animation(const Animation &mkaSrc);
    Animation& operator=(const Animation &mkaSrc);

};



///\ingroup Animation
/// \brief The base animation instance class.
///
/// This class is used for playing animations in the animation players.
class AnimationInstance
{
public:

    virtual ~AnimationInstance(){}


    ///\brief Returns the kind of this animation.
    AnimationKind GetKind(){ return kind;}


    ///\brief Update the animation state for the given *msTimePoint*.
    ///
    /// This function is usually called by animation players.
    virtual int Update(int msTimePoint, int _flags) = 0;


    ///\brief Update the animated sprites.
    ///
    /// This function must be called manually when an animation player updates animation and returns PlayingAnimationFlags::ANIMATED_PROPERTIES_CHANGED flag.
    /// The parameter *_resetSpriteAnimatedProperties* must be set in the following way:
    /// - If one animation instance is used to animate a sprite the parameter is *true*.
    /// - If more animation instances (via more players) is used to animating the same sprite, the parameter is *true* for the first processed instance and *false* for the others.
    virtual void UpdateAnimatedSprites(bool _resetSpriteAnimatedProperties) = 0;


    virtual void ResetAnimatedProperties() = 0;
    virtual void OnPlayingStart(){}
    virtual void OnPlayingStop(){}


    ///\brief Returns the animation object of this animation instance.
    Animation * GetAnimation(){ return animation; }


    ///\brief Returns the base animation parameters of this animation instance.
    ///
    /// This is a copy of the parameters of the related animation object and can be changed before animation playback.
    AnimationBaseParameters &GetBaseParameters(){ return bp; }


     ///\brief Set a parameter which indicates that the animation should finish its current loop before ending.
     ///
     /// This parameter is used when the animation is played with an AnimationQueuePlayer object. If an AnimationPlayer object is used for playback it has no effect.
    void SetCompleteLoops(bool _completeLoops){ completeLoops = _completeLoops; }


    ///\brief Returns the *completeLoops* parameter of this animation instance.
    bool GetCompleteLoops(){ return completeLoops; }


    ///\brief Set the speed factor of animation playback. The value 1.0 is the default speed; higher values means faster animation
    /// and lower values means slower animation.
    void SetSpeedFactor(float _fSpeed) { fSpeed = _fSpeed; }


    ///\brief Returns the speed factor of this animation instance.
    float GetSpeedFactor() {return fSpeed;}


    ///\brief Set the animation starting time point offset in milliseconds.
    void SetStartPlayTimeOffset(int _startPlayTimeOffset){ startPlayTimeOffset = _startPlayTimeOffset; }


    ///\brief Returns the animation starting time point offset of this animation instance.
    int GetStartPlayTimeOffset(){ return startPlayTimeOffset; }


protected:

    AnimationKind kind = AnimationKind::NOT_DEFINED;
    Animation * animation = nullptr;                           // LINK to related timelineAnimation
    AnimationBaseParameters bp;


    float fSpeed = 1.0;
    bool completeLoops = true;
    int startPlayTimeOffset = 0;

};



///\ingroup Animation
/// \brief The DummyNoAnimationInstance is a special animation instance which can be used with animation players as an
/// alternative way to stop animation.
class DummyNoAnimationInstance : public AnimationInstance
{
public:

    int Update(int msTimePoint, int _flags) override { return 0;}
    void UpdateAnimatedSprites(bool _resetAnimatedProperties) override {}
    void ResetAnimatedProperties() override {}

};






///\ingroup Animation
/// \brief The standard animation player for playing animations via AnimationInstance objects.
class AnimationPlayer
{
public:

    //---

    ///\brief Returns the played animation instance.
    AnimationInstance *GetAnimationInstance() {return animationInstance;}


    ///\brief Returns the current animation time in milliseconds.
    int GetCurrentAnimationTime() { return msCurrentAnimationTime;}


    ///\brief Returns the state of this player.
    AnimationPlayerState GetState() {return state;}


    ///\brief Returns the active meta key of the played animation or nullptr if none.
    AKMeta* GetActiveMetaKey() { return activeControllerKey; }


    ///\brief Play the given *_animationInstance*.
    int Play(AnimationInstance * _animationInstance);


    ///\brief Pause this player.
    void Pause();


    ///\brief Resume this player.
    void Resume();


    ///\brief Stop the played animation and put the player into the idle state.
    ///
    /// To change the played animation there is no need to call this command. The Play command will stop any played animation before starting a new one.
    int Stop();


    ///\brief Update the player. This command must be called in every update loop of the game logic.
    int Update();


    ///\brief This command modify the player parameters so that it will go into the AnimationPlayerState:STALLED state after the currently played loop.
    ///
    /// This command is used by the AnimationQueuePlayer.
    void SetParametersForCompletingLoop();


    ///\brief Returns a reference to the animation base parameters.
    ///
    /// Animation base parameters stored in the player are copy of parameters from animation instance.
    AnimationBaseParameters& GetAnimationBaseParameters(){ return bp;}


private:
    AnimationPlayerState state = AnimationPlayerState::IDLE;

    AnimationBaseParameters bp;

    int msCurrentAnimationTime = -1;
    int msStartPlayOffset = 0;
    int msAnimationTimeStart = -1;
    int countLoop = 0;
    int msRepeat = -1;
    AnimationPlayerState stateStored = AnimationPlayerState::IDLE;
    int msTimeStored = 0;

    AnimationInstance *animationInstance = nullptr;            // LINK
    AKMeta* activeControllerKey = nullptr;                              // LINK


    int UpdateAnimationInstance();
    int GetRepeatTime(int _msCurrentTime);
};



///\ingroup Animation
/// \brief The queue animation player for playing animations via animationInstance objects.
///
/// The queue player can delay a new animation in order for the old animation to complete its loop.
/// There can be up to five animations in queue; if there are more the oldest waiting will be removed.
/// The behavior that an animation complete its loop before ending is set by AnimationInstance::SetCompleteLoops command.
/// The goal of this behavior is to allow designing smooth transition beetwen animations.
/// The AnimationQueuePlayer objects are used for playing sub-animations.
class AnimationQueuePlayer
{
public:


    ///\brief Returns the wrapped AnimationPlayer object.
    AnimationPlayer & GetPlayer(){ return player; }


    ///\brief Returns the played animation instance.
    AnimationInstance *GetAnimationInstance() { return player.GetAnimationInstance();}


    ///\brief Returns the current animation time in milliseconds.
    int GetCurrentAnimationTime() { return player.GetCurrentAnimationTime();}


    ///\brief Returns the state of this player.
    AnimationPlayerState GetState() {return player.GetState(); }


    ///\brief Returns the active meta key of the played animation or nullptr if none.
    AKMeta* GetActiveMetaKey() { return player.GetActiveMetaKey(); }


    ///\brief Play the given *_animationInstance*.
    ///
    /// The flag can be AnimationPlayerFlags::DISCARD_ANIMATION_QUEUE to stop and clear any waiting animations in queue.
    int Play(AnimationInstance *_animationInstance, int _flags=0);


    ///\brief Update the player. This command must be called in every update loop of the game logic.
    int Update();


    ///\brief Pause this player.
    void Pause() { player.Pause(); }


    ///\brief Resume this player.
    void Resume() { player.Resume(); }


    ///\brief Stop the played animation, clear the queue and put the player into the idle state.
    ///
    /// To change the played animation there is no need to call this command. The Play command will stop any played animation before starting a new one.
    int Stop();


private:
    AnimationPlayer player;
    std::vector<AnimationInstance*>animationInstances;

    int PlayNextAnimationInQueue();
};


extern DummyNoAnimationInstance dummyNoAnimationInstance;
extern AnimationPlayer *activeTimelineAnimationPlayer;


}




#endif // EANIMATIONCOMMON_H
