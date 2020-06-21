#ifndef JUGIMAP_TIMELINE_ANIMATION_INSTANCE_H
#define JUGIMAP_TIMELINE_ANIMATION_INSTANCE_H


#include "jmMap.h"
#include "jmTimelineAnimation.h"



namespace jugimap{




class SourceSprite;
class Sprite;
class VectorShape;


class TimelineAnimation;
class AnimationQueuePlayer;
class FrameAnimationInstance;
class AnimationMemberState;
class TimelineAnimationInstance;




class AnimationTrackState
{
public:

    AnimationTrackState(AnimationTrack* _tAnimation, AnimationMemberState* _tAnimationsStatesSet);
    virtual ~AnimationTrackState();


    bool Empty(){ return (activeStartKey==nullptr && activeEndKey==nullptr); }
    AnimationTrackKind GetKind();

    AnimationTrack* GetAnimationTrack(){ return animationTrack; }
    AnimationKey* GetStateKey(){ return stateKey; }
    AnimationKey* GetActiveStartKey(){ return activeStartKey; }
    AnimationKey* GetActiveEndKey(){ return activeEndKey; }
    AnimationKey* GetActiveKey(){ return  (activeStartKey!=nullptr)? activeStartKey : activeEndKey;}

    VectorShape * GetPath(){ return path; }
    AnimationQueuePlayer* GetTimelineAnimationQueuePlayer(){ return timelineAnimationPlayer; }
    AnimationQueuePlayer* GetFrameAnimationPlayer(){ return frameAnimationPlayer; }
    AnimationTrackParameters &GetTrackParameters(){ return tp; };

    bool IsDisabled(){ return disabled; }
    void SetDisabled(bool _disabled){ disabled = _disabled; }

    int Update(int _animationTime, int _flags);

    void _ClearLinkPointers(VectorShape* _shape);

private:
    AnimationTrackKind kind = AnimationTrackKind::NOT_DEFINED;

    AnimationMemberState * animationMemberState = nullptr;                          // LINK to owner
    AnimationTrack* animationTrack = nullptr;                                       // LINK to related tAnimation object
    AnimationKey* activeStartKey = nullptr;                                         // LINK to a key in tAnimation
    AnimationKey* activeEndKey = nullptr;                                           // LINK to a key in tAnimation

    //---
    AnimationKey *stateKey = nullptr;                                               // OWNED

    AnimationQueuePlayer *frameAnimationPlayer = nullptr;                           // OWNED, sub-player
    std::vector<FrameAnimationInstance*>*frameAnimationInstances = nullptr;        // OWNED, instances for sub-player
    //EFrameAnimationInstance* activeFrameAnimationInstance = nullptr;

    AnimationQueuePlayer* timelineAnimationPlayer = nullptr;                        // OWNED, sub-player
    std::vector<TimelineAnimationInstance*>*timelineAnimationInstances = nullptr;   // OWNED, instances for sub-player
    //ETimelineAnimationInstance* activeTimelineAnimationInstance = nullptr;        // LINK

    AnimationTrackParameters tp;                                                    // OWNED

    //---
    VectorShape *path = nullptr;                                                    // LINK, stored pointer for quicker access
    SourceSprite* sourceObject = nullptr;                                           // LINK, stored pointer for quicker access

    bool disabled = false;


    void SetActiveKeys(int _animationTime);
    int ManageSubTimelineAnimation();
    int ManageSubFrameAnimation();
    void FindSourceObject();
    Map* FindMap();

    TimelineAnimationInstance* FindTimelineAnimationInstance(const std::string& _name);
    TimelineAnimationInstance* CreateAndStoreTimelineAnimationInstance(const std::string& _name);

    FrameAnimationInstance* FindFrameAnimationInstance(const std::string& _name);
    FrameAnimationInstance* CreateAndStoreFrameAnimationInstance(const std::string& _name);

};


//================================================================


class AnimationMemberState
{
public:
    friend class AnimationTrackState;
    friend class TimelineAnimationInstance;


    AnimationMember *GetAnimationMember(){ return animationMember; }
    std::vector<AnimationTrackState*> &GetAnimationTrackStates(){ return animationsTrackStates; }
    Sprite* _GetSprite(){ return sprite;}
    std::vector<Sprite*>* _GetSprites(){ return sprites;}

    bool IsDisabled(){ return disabled; }
    void SetDisabled(bool _disabled){ disabled = _disabled; }


private:

    AnimationMemberState(AnimationMember *_animationMember);
    AnimationMemberState(const AnimationMember &_animationMember) = delete;
    AnimationMemberState& operator=(const AnimationMember &_animationMember) = delete;
    ~AnimationMemberState();


    int Update(int msTimePoint, int _flags);
    void StopSubPlayers();
    void PauseSubPlayers();
    void ResumeSubPlayers();
    void ResetAnimatedProperties();


    AnimationMember * animationMember = nullptr;                     //LINK
    std::vector<AnimationTrackState*>animationsTrackStates;

    Sprite* sprite = nullptr;                                       // LINK
    std::vector<Sprite*>*sprites = nullptr;                         // vector OWNED, sprites LINKS
    AnimatedProperties ap;
    bool disabled = false;

    TimelineAnimationInstance* animationInstance = nullptr;         // LINK


    int UpdateAnimatedProperties(AnimationTrackState & _ats);
    void ResetSpritesAnimatedProperties();
    void UpdateAnimatedSprites();

};



//================================================================


///\ingroup Animation
///\brief A class for using timeline animations in animation players.
class TimelineAnimationInstance : public AnimationInstance
{
public:

    //friend class AnimationQueuePlayer;


    ///\brief Constructor.
    ///
    /// Create an TimelineAnimationInstance object for animating the given sprite *_sprite* with the given timeline animation *_animation*.
    TimelineAnimationInstance(TimelineAnimation *_timelineAnimation, Sprite *_sprite);


    ///\brief Constructor.
    ///
    /// Create an TimelineAnimationInstance object for animating the given sprites *_sprites* with the given timeline animation *_animation*.
    /// The sprites must have the same SourceSprite!
    TimelineAnimationInstance(TimelineAnimation *_timelineAnimation, std::vector<Sprite*>&_sprites);


    ///\brief Destructor.
    ~TimelineAnimationInstance() override;


    ///\brief Returns the assigned sprite; if a vector of sprites has been assigned it returns nullptr!
    Sprite* GetSprite(){ return animationsMemberStates.front()->_GetSprite();}


    ///\brief Returns a pointer to the vector of assigned sprites; if one sprite has been assigned it returns nullptr.
    ///
    /// The vector of assigned sprites object is owned by FrameAnimationInstance object! Sprites themselves are link pointers.
    std::vector<Sprite*>* GetSprites(){ return animationsMemberStates.front()->_GetSprites();}


    ///\brief Returns a reference to the vector of animation members.
    std::vector<AnimationMemberState*>& GetAnimationsMemberStates(){ return animationsMemberStates;}


    ///\brief Returns a pointer to meta track state. If the animation has no meta track returns nullptr.
    AnimationTrackState* GetMetaAnimationTrackState(){ return metaAnimationTrackState;}



    int Update(int msTimePoint, int _flags) override;
    void StopSubPlayers();
    void PauseSubPlayers();
    void ResumeSubPlayers();

    void UpdateAnimatedSprites(bool _resetSpriteAnimatedProperties) override;
    void ResetAnimatedProperties() override;

    void ClearLinkPointers(VectorShape* _shape);


    ///\brief Returns a pointer to animation member state object of the given *_animationMember*. If the animation member is not found it returns nullptr.
    AnimationMemberState* FindAnimationMemberState(AnimationMember* _animationMember);


    ///\brief Returns a pointer to animation track state object of the given *AnimationTrackKind* within the member with the given *_memberName*.
    /// If the track state was not found it returns nullptr.
    AnimationTrackState* FindAnimationTrackState(AnimationTrackKind _animationTrackKind, const std::string &_memberName="");


    ///\brief Returns a pointer to animation track state object of the given *_animationTrack*. If the animation track was not found it returns nullptr.
    AnimationTrackState* FindAnimationTrackState(AnimationTrack* _animationTrack);


private:
    std::vector<AnimationMemberState*>animationsMemberStates;                   // OWNED
    AnimationTrackState* metaAnimationTrackState = nullptr;                // OWNED

    TimelineAnimationInstance(){};
    TimelineAnimationInstance(const TimelineAnimationInstance &src) = delete;
    TimelineAnimationInstance& operator=(const TimelineAnimationInstance &src) = delete;
};





}




#endif
