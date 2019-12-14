#ifndef JUGIMAP_SCENE_H
#define JUGIMAP_SCENE_H

#include <chrono>
#include "jmCommon.h"



namespace jugimap {



///\ingroup MapElements
///\brief The scene class.
///
/// The Scene class is a base interface class for game scenes.
class Scene
{
public:

    virtual ~Scene(){}

    virtual bool Init() = 0;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Draw(){}


    ///\brief Set a flag that indicated if this scene has been initialized.
    void SetInitialized(bool _initialized){ initialized = _initialized;}


    ///\brief Returns true if this scene has been initialized; otherwise returns false.
    bool IsInitialized(){ return initialized;}


    ///\brief Set a flag that indicated if this scene is active.
    void SetActive(bool _active){ active = _active; }


    ///\brief Returns true if this scene is active; otherwise returns false.
    bool IsActive(){ return active;}


    ///\brief Set the tag of this scene to the given _tag.
    ///
    /// A tag is an arbitrary integer number which can be assigned to a Scene object.
    /// \see GetTag
    void SetTag(int _tag){ tag = _tag;}


    ///\brief Returns the tag of this scene.
    ///
    /// \see SetTag
    int GetTag(){ return tag; }


    ///\brief Set the engine scene link of this scene to the given _engineSceneLink.
    ///
    /// An engine scene link allows connecting a scene with a related engine scene object if needed.
    /// \see GetEngineSceneLink
    void SetEngineSceneLink(void* _engineSceneLink) { engineSceneLink = _engineSceneLink; }


    ///\brief Returns the engine scene link object.
    void* GetEngineSceneLink(){ return engineSceneLink; }


    ///\brief Set the physics enabled flag.
    ///
    /// You can use this flag as an indicator if physics are used in this scene.
    void SetPhysicsEnabled(bool _physicsEnabled){ physicsEnabled = _physicsEnabled; }


    ///\brief Returns the physics enabled flag.
    bool HasPhysicsEnabled(){ return physicsEnabled; }


private:
    bool initialized = false;
    bool active = false;

    int tag = 0;
    void* engineSceneLink = nullptr;
    bool physicsEnabled = false;

};


}

#endif


