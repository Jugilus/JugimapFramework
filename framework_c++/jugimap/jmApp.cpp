#include <string>
#include <sstream>
#include "jmObjectFactory.h"
#include "jmMapBinaryLoader.h"
#include "jmSourceGraphics.h"
#include "jmScene.h"
#include "jmSprites.h"
#include "jmFont.h"
#include "jmInput.h"
#include "jmCamera.h"
#include "jmGuiCommon.h"

#include "jmApp.h"



namespace jugimap {




App::App()
{
    engineApp = objectFactory->NewEngineApp(this);
}


App::~App()
{

    delete engineApp;

    for(Scene* s : scenes){
        delete s;
    }

}


void App::Update(float _framePeriod)
{

    if(activeScene==nullptr || activeScene->IsInitialized()==false) return;

    ManageSuspendedTime(_framePeriod*1000);

    //---
    activeScene->PreUpdate();
    activeScene->Update();
    activeScene->PostUpdate();
}


void App::Draw()
{
    if(activeScene==nullptr || activeScene->IsInitialized()==false) return;

    activeScene->Draw();
}



void App::SetActiveScene(Scene *_scene)
{

    if(_scene==activeScene) return;

    activeScene = _scene;

    //--- Initialize scene
    if(activeScene->IsInitialized()==false){
        if(activeScene->Init()==false){
            //activeScene = nullptr;
        }
    }

    //--- Start scene
    if(activeScene && activeScene->IsInitialized()){

        // reset time variables !
        passedTimeMS = passedTimeMSPrevious = time.UpdatePassedTimeMS();
        storedUpdatePeriods.clear();

        //
        activeScene->Start();
    }
}



void App::ManageSuspendedTime(float _updatePeriodMS)
{

    //if(currentScene == nullptr) return;


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
    //currentScene->PreUpdate();
    //currentScene->Update();
    //currentScene->PostUpdate();

}


/*
void App::UpdateViaFixedTimeStep(float _fixedTimeStepMS)
{

    if(currentScene == nullptr) return;


    time.SetLogicTimeMS(_fixedTimeStepMS);

    //----
    passedTimeMS = time.UpdatePassedTimeMS();

    double passedDeltaTimeMS = passedTimeMS - passedTimeMSPrevious;
    float frameTimeMS = jugimap::ClampValue(passedDeltaTimeMS, 0.0, 200.0);     // safety clamp
    float avgFrameTime = GetAverageUpdatePeriod(frameTimeMS);

    //DbgOutput("frameTimeMS:" + std::to_string(frameTimeMS)+ " avgFrameTime:"+std::to_string(avgFrameTime));

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

        currentScene->PreUpdate();
        currentScene->Update();
        currentScene->PostUpdate();

        //---
        accumulatorMS -= _fixedTimeStepMS;
    }
    jugimap::settings.SetLerpDrawingFactor(accumulatorMS / _fixedTimeStepMS);

}
*/


float App::GetAverageUpdatePeriod(float _currentUpdatePeriodMS)
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



}
