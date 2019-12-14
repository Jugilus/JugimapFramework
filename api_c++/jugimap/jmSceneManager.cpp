#include <algorithm>
#include <utility>
#include <assert.h>
#include "jmCommonFunctions.h"
#include "jmGlobal.h"
#include "jmScene.h"
#include "jmSceneManager.h"


namespace jugimap {




Scene * SceneManager::AddScene(Scene *_scene)
{
    scenes.push_back(_scene);
    return _scene;
}



void SceneManager::SetCurrentScene(Scene *_scene)
{
    if(_scene==currentScene) return;

    currentScene = _scene;

    //--- Initialize scene
    if(currentScene->IsInitialized()==false){
        if(currentScene->Init()==false){
            //currentScene = nullptr;
        }
    }

    //--- Start scene
    if(currentScene && currentScene->IsActive()==false){

        // reset time variables !
        passedTimeMS = passedTimeMSPrevious = time.UpdatePassedTimeMS();
        storedUpdatePeriods.clear();

        //
        currentScene->Start();
    }
}



void SceneManager::Update(float _updatePeriodMS)
{

    if(currentScene == nullptr) return;


    //--- Verify frame time for abnormal values
    _updatePeriodMS = jugimap::ClampValue(_updatePeriodMS, 0.0f, 200.0f);     // safety clamp
    float avgUpdatePeriodMS = GetAverageUpdatePeriod(_updatePeriodMS);

    //DbgOutput("updatePeriodMS:" + std::to_string(_updatePeriodMS)+ " avgUpdatePeriodMS:"+std::to_string(avgUpdatePeriodMS));

    if(_updatePeriodMS > 3 * avgUpdatePeriodMS){
        _updatePeriodMS = avgUpdatePeriodMS;
        //DbgOutput("UpdatePeriodMS too big! Clamped to average value:"+std::to_string(avgUpdatePeriodMS));
    }

    time.SetLogicTimeMS(_updatePeriodMS);


    //--- Manage abnormal pauses due to application suspension or dragging the window around in Windows OS which stops updating.
    passedTimeMS = time.UpdatePassedTimeMS();
    int deltaPassedTimeMS = passedTimeMS - passedTimeMSPrevious;
    if(deltaPassedTimeMS > 10*_updatePeriodMS){                 // Big spike in frame time indicates abnormal pause.
        time.AddSuspendedTimeMS(deltaPassedTimeMS);
        //DbgOutput("Big Spike in passed time! Added to suspendedTimeMS:"+std::to_string(deltaPassedTimeMS));
    }
    passedTimeMSPrevious = passedTimeMS;


    // Update scene logic
    //---------------------------------------
    currentScene->Update();

}



void SceneManager::UpdateViaFixedTimeStep(float _fixedTimeStepMS)
{

    if(currentScene == nullptr) return;


    time.SetLogicTimeMS(_fixedTimeStepMS);

    //----
    passedTimeMS = time.UpdatePassedTimeMS();

    double passedDeltaTimeMS = passedTimeMS - passedTimeMSPrevious;
    float frameTimeMS = jugimap::ClampValue(passedDeltaTimeMS, 0.0, 200.0);     // safety clamp
    float avgFrameTime = GetAverageUpdatePeriod(frameTimeMS);

    DbgOutput("frameTimeMS:" + std::to_string(frameTimeMS)+ " avgFrameTime:"+std::to_string(avgFrameTime));

    // manage unexpected spikes in frame rate - due to suspension or dragging the window around in Windows (stops updating)
    if(passedDeltaTimeMS > avgFrameTime*10){
        frameTimeMS = avgFrameTime;
        time.AddSuspendedTimeMS(passedDeltaTimeMS);
        DbgOutput("Big Spike in passed time! Added to suspendedTimeMS:"+std::to_string(frameTimeMS));
    }
    passedTimeMSPrevious = passedTimeMS;

    accumulatorMS += frameTimeMS;

    //---
    while(accumulatorMS >= _fixedTimeStepMS){

        currentScene->Update();

        //---
        accumulatorMS -= _fixedTimeStepMS;
    }
    jugimap::settings.SetLerpDrawingFactor(accumulatorMS / _fixedTimeStepMS);

}



void SceneManager::Draw()
{
    if(currentScene == nullptr) return;

    currentScene->Draw();
}



void SceneManager::DeleteScenes()
{

    for(Scene *s : scenes){
        delete s;
    }
    scenes.clear();

}



float SceneManager::GetAverageUpdatePeriod(float _currentUpdatePeriodMS)
{

    if(storedUpdatePeriods.size()<sizeStoredUpdatePeriods-1){
        storedUpdatePeriods.push_back(_currentUpdatePeriodMS);

    }else{
        for(int i=0; i<storedUpdatePeriods.size()-1; i++){
            storedUpdatePeriods[i] = storedUpdatePeriods[i+1];
        }
        storedUpdatePeriods[storedUpdatePeriods.size()-1] = _currentUpdatePeriodMS;

    }

    float sumFrameTime = 0.0;
    for(int i=0; i<storedUpdatePeriods.size(); i++){
        sumFrameTime += storedUpdatePeriods[i];
    }

    float avgUpdatePeriodMS = sumFrameTime/storedUpdatePeriods.size();

    return avgUpdatePeriodMS;

}



SceneManager *sceneManager = nullptr;


}

