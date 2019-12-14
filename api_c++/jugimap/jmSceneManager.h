#ifndef JUGIMAP_SCENE_MANAGER_H
#define JUGIMAP_SCENE_MANAGER_H

#include <chrono>
#include "jmCommon.h"



namespace jugimap {

class Scene;


///\ingroup MapElements
///\brief The scene manager class.
///
/// The scene manager is a class which manages scenes.
/// Currently this class is quite basic.
class SceneManager
{
public:


    ///\brief Update the active scene with the given update time *_updatePeriodMS*.
    virtual void Update(float _updatePeriodMS);

    ///\brief Update
    /// the active scene with the given fixed rate update time *_updatePeriodMS*.
    ///
    /// This function can be used with engines which allow developers to manually manage the game root loop.
    virtual void UpdateViaFixedTimeStep(float _fixedTimeStepMS);


    ///\brief Draw the active scene.
    ///
    /// This function can be used with engines which allow developers to manually manage the game root loop.
    virtual void Draw();


    ///\brief Add the given scene *_scene* to this scene manager.
    Scene *AddScene(Scene *_scene);


    ///\brief Set the given scene *_scene* to be the active scene.
    virtual void SetCurrentScene(Scene *_scene);


    ///\brief Returns the active scene.
    Scene* GetCurrentScene(){ return currentScene; }


    ///\brief Delete all scenes.
    void DeleteScenes();


private:
    std::vector<Scene*> scenes;             // OWNED
    Scene* currentScene = nullptr;          // LINK

    //---
    double passedTimeMS = 0;
    double passedTimeMSPrevious = 0;
    std::vector<float>storedUpdatePeriods;
    int sizeStoredUpdatePeriods = 8;
    float accumulatorMS = 0.0;

    float GetAverageUpdatePeriod(float _currentUpdatePeriodMS);

};


extern SceneManager *sceneManager;


}


#endif


