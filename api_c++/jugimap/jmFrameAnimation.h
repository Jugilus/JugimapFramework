#ifndef JUGIMAP_FRAME_ANIMATION_H
#define JUGIMAP_FRAME_ANIMATION_H

#include <vector>
#include "jmAnimationCommon.h"



namespace jugimap{


class GraphicItem;
class SourceSprite;
class Sprite;
class StandardSprite;
class JugiMapBinaryLoader;





///\ingroup Animation
/// \brief Frame animation of standard sprites.
///
/// The FrameAnimation class represents the sprite frame animation from JugiMap Editor.
class  FrameAnimation : public Animation
{
public:
    friend class JugiMapBinaryLoader;


    ~FrameAnimation() override;


    std::vector<AnimationFrame*>&GetFrames(){ return frames; }

private:
    std::vector<AnimationFrame*>frames;                         // OWNED

    FrameAnimation(SourceSprite* _sourceObject);
    FrameAnimation(SourceSprite* _sourceObject, const std::string &_nameID);
    FrameAnimation(const FrameAnimation &fa);
    FrameAnimation& operator=(const FrameAnimation &fa);

};



//==================================================================================



///\ingroup Animation
///\brief A class for using frame animations in animation players.
class FrameAnimationInstance : public AnimationInstance
{
public:
    friend class EFrameAnimationPlayer;


    ///\brief Constructor.
    ///
    /// Create an FrameAnimationInstance object for animating the given sprite *_sprite* with the given frame animation *_animation*.
    FrameAnimationInstance(FrameAnimation *_animation, Sprite *_sprite);


    ///\brief Constructor.
    ///
    /// Create an FrameAnimationInstance object for animating the given sprites *_sprites* with the given frame animation *_animation*.
    /// The sprites must have the same SourceSprite!
    FrameAnimationInstance(FrameAnimation *_animation, std::vector<Sprite*>&_sprites);


    ///\brief Destructor.
    ~FrameAnimationInstance() override;


    ///\brief Returns the assigned sprite; if a vector of sprites has been assigned it returns nullptr!
    Sprite* GetSprite(){ return sprite;}


    ///\brief Returns a pointer to the vector of assigned sprites; if one sprite has been assigned it returns nullptr.
    ///
    /// The vector of assigned sprites object is owned by FrameAnimationInstance object! Sprites themselves are link pointers.
    std::vector<Sprite*>* GetSprites(){ return sprites;}


    ///\brief Returns the starting index in the sequence of frames of this animation.
    int GetStartingFrameIndex(){ return startingFrameIndex;}


    ///\brief Set the starting index in the sequence of frames of this animation to the given *_startingFrameIndex*.
    void SetStartingFrameIndex(int _startingFrameIndex);


    ///\brief Get a reference to the AnimatedProperties object of this animation instance.
    AnimatedProperties &GetAnimatedProperties(){ return ap; }


    int Update(int msTimePoint, int _flags) override;
    void UpdateAnimatedSprites(bool _resetSpriteAnimatedProperties) override;
    void ResetAnimatedProperties() override;
    void OnPlayingStart() override;
    void OnPlayingStop() override;



private:
    Sprite* sprite = nullptr;                                       // LINK
    std::vector<Sprite*>*sprites = nullptr;                         // vector OWNED, sprites LINKS

    AnimatedProperties ap;

    int startingFrameIndex = 0;
    int activeFrameIndex = 0;

    FrameAnimationInstance(const FrameAnimationInstance& _src) = delete ;
    FrameAnimationInstance& operator=(const FrameAnimationInstance& _src) = delete;

    void Update(AnimationFrame *_animationFrame);

};




}
#endif
