#ifndef JUGIMAP_FRAME_ANIMATION_H
#define JUGIMAP_FRAME_ANIMATION_H

#include <vector>
#include "jmCommon.h"



namespace jugimap{


class GraphicItem;
class SourceSprite;
class StandardSprite;
class JugiMapBinaryLoader;


//Do not change content of this struct!
struct AnimationFrame
{
    GraphicItem *image = nullptr;              // LINK
    int duration = 100;                        // in milliseconds
    Vec2f offset;
    Vec2i flip;
    int dataFlags = 0;
};



///\ingroup Animation
/// \brief Frame animation of standard sprites.
///
/// The FrameAnimation class represents the sprite frame animation from JugiMap Editor.
class FrameAnimation
{
public:
    friend class JugiMapBinaryLoader;
    friend class FrameAnimationPlayer;

    ///\brief Constructor.
    FrameAnimation(){}


    ///\brief Copy constructor.
    FrameAnimation(const FrameAnimation &fa);


    ///\brief Assignment operator.
    FrameAnimation& operator=(const FrameAnimation &fa);


    ///\brief Destructor.
    ~FrameAnimation();


    ///\brief Returns the name of this frame animation.
    std::string GetName(){return name;}


    ///\brief Returns the source sprite *link* of this frame animation.
    SourceSprite *GetSourceSprite(){return sourceSprite;}


    ///\brief Returns a reference to the vector of stored parameters in this frame animation.
    std::vector<jugimap::Parameter> &GetParameters(){return parameters;}


    ///\brief Returns the dataFlags factor of this frame animation.
    int GetDataFlags(){return dataFlags;}



private:
    std::string name;
    SourceSprite * sourceSprite = nullptr;                    // LINK to owner
    std::vector<AnimationFrame*>frames;                       // OWNED

    int loopCount = 0;
    bool loopForever = true;
    float scaleDuration = 1.0;
    bool repeatAnimation = false;
    int repeat_PeriodStart = 0;
    int repeat_PeriodEnd = 0;
    bool startAtRepeatTime = true;

    std::vector<jugimap::Parameter> parameters;
    int dataFlags = 0;
};




class FrameAnimationPlayer
{
public:
    static const int stateIDLE = 0;
    static const int statePLAYING = 1;
    static const int statePAUSED = 2;
    static const int stateLOOP_END = 3;
    static const int stateWAITING_TO_REPEAT = 4;

    //---
    static const int flagFRAME_CHANGED = 1;


    void SetFrameAnimation(FrameAnimation * _frameAnimation);
    FrameAnimation* GetFrameAnimation(){return frameAnimation;}
    AnimationFrame* GetActiveFrame(){return (frameAnimation!=nullptr)? frameAnimation->frames[indexCurrentFrame] : nullptr;}
    int GetState(){return state;}

    bool Play(FrameAnimation * _frameAnimation, int _indexFrame=0);
    bool Play(int _indexFrame=0);
    void Pause();
    void Resume();
    void Stop();
    int Update();
    void Reset();


private:

    FrameAnimation * frameAnimation = nullptr;    // LINK
    int state = stateIDLE;
    int indexCurrentFrame = 0;
    int msFrameStartTime = -1;
    int countLoop = 0;
    int msRepeat = -1;
    int stateStored = 0;
    int msTimeStored = 0;

    int GetRepeatTime(int _msCurrentTime);
};


//================================================================================================


// Wrapper class for animating JMStandardSprite with FrameAnimationPlayer

///\ingroup Animation
/// \brief Frame animation player for standard sprites.
///
/// This class wraps the generic *FrameAnimationPlayer*.
class StandardSpriteFrameAnimationPlayer
{
public:

    ///\brief Set a standard sprite which should be animated.
    void SetSprite(StandardSprite  *_standardSprite){standardSprite = _standardSprite;}


    ///\brief Start playing the given *_frameAnimation* at frame *_indexFrame*.
    ///
    /// **Important:** The *_frameAnimation* must belong to the SourceSprite of the animated standard sprite!
    /// The *_indexFrame* must be within the vector bounds of the played animation!
    ///\see Stop
    void Play(FrameAnimation *_frameAnimation, int _indexFrame=0)
    {
        frameAnimationPlayer.Play(_frameAnimation, _indexFrame);
        OnChangedStandardSpriteAnimationFrame();
    }


    ///\brief Update the played frame animation.
    ///
    /// This function must be called every game frame.
    void Update()
    {
        if(frameAnimationPlayer.Update()==FrameAnimationPlayer::flagFRAME_CHANGED){
            OnChangedStandardSpriteAnimationFrame();
        }
    }


    ///\brief Pause the played frame animation.
    ///
    /// \see Resume
    void Pause(){frameAnimationPlayer.Pause();}


    ///\brief Resume the paused frame animation.
    ///
    /// \see Pause
    void Resume(){frameAnimationPlayer.Resume();}


    ///\brief Stop the played frame animation.
    ///
    /// \see Play
    void Stop(){frameAnimationPlayer.Stop();}


    ///\brief Reset this player .
    ///
    /// This function will stop the frame animation and set the FrameAnimation object to nullptr.
    void Reset(){frameAnimationPlayer.Reset();}



private:
    StandardSprite  *standardSprite = nullptr;             // LINK pointer to animated sprite!
    FrameAnimationPlayer frameAnimationPlayer;

    void OnChangedStandardSpriteAnimationFrame();

};




}
#endif
