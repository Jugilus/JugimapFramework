#ifndef JUGIMAP_APP_H
#define JUGIMAP_APP_H

#include <chrono>
#include "jmCommon.h"



namespace jugimap {


class Scene;
class SceneManager;
class EngineApp;





///\ingroup MapElements
///\brief The App is the base class of the application.
class App
{
public:

    ///\brief Constructor.
    App();

    ///\brief Destructor.
    virtual ~App();


    ///\brief Initialize the application.
    virtual bool Init() = 0;


    ///\brief Update the application logic. The given *_framePeriod* is a period of one frame in seconds.
    virtual void Update(float _framePeriod);


    ///\brief Execute drawing of the application graphics. This function is available for engines which does not handle drawing automatically
    /// as part of the engine update.
    virtual void Draw();


    //virtual void PreUpdate(){}

    //virtual void PostUpdate(){}


    ///\brief Set the physics enabled flag.
    ///
    /// You can use this flag as an indicator if physics are used in this app.
    void SetPhysicsEnabled(bool _physicsEnabled){ physicsEnabled = _physicsEnabled; }


    ///\brief Returns the physics enabled flag.
    bool HasPhysicsEnabled(){ return physicsEnabled; }


    ///\brief Returns the engine app object of this application.
    EngineApp* GetEngineApp(){ return engineApp; }


    ///\brief Add the given *_scene* to this application. The application takes over the ownership of the scene!
    void AddScene(Scene *_scene){ scenes.push_back(_scene);}


    ///\brief Returns a reference to the vector of stored scenes.
    std::vector<Scene*>& GetScenes(){ return scenes; }


    ///\brief Set the active scene.
    virtual void SetActiveScene(Scene *_scene);


    ///\brief Returns the active scene or nullptr if none.
    Scene* GetActiveScene(){ return activeScene; }


protected:

    virtual void ManageSuspendedTime(float _updatePeriodMS);


    ///\brief Update
    /// the active scene with the given fixed rate update time *_updatePeriodMS*.
    ///
    /// This function can be used with engines which allow developers to manually manage the game root loop.
    //virtual void UpdateViaFixedTimeStep(float _fixedTimeStepMS);


private:
    bool physicsEnabled = false;

    EngineApp* engineApp = nullptr;                 // OWNED
    std::vector<Scene*>scenes;                      // OWNED
    Scene* activeScene = nullptr;                  // LINK

    //---
    double passedTimeMS = 0;
    double passedTimeMSPrevious = 0;
    std::vector<float>storedUpdatePeriods;
    int sizeStoredUpdatePeriods = 8;
    float accumulatorMS = 0.0;


    float GetAverageUpdatePeriod(float _currentUpdatePeriodMS);

};


///\ingroup MapElements
///\brief The EngineApp class defines engine specific application properties.
class EngineApp
{
public:

    EngineApp(App *_app) : app(_app){}
    virtual ~EngineApp(){}


    ///\brief Interface function for running engine specific code before the application initialization.
    virtual void PreInit(){}


    ///\brief Interface function for running engine specific code after the application initialization.
    virtual void PostInit(){}


    ///\brief Set the visibility of the system mouse cursor.
    virtual void SetSystemMouseCursorVisible(bool _visible){}


    ///\brief Returns the application object.
    App* GetApp(){ return app; }


protected:
    App *app = nullptr;             // LINK

};



}

#endif


