#include <chrono>
#include <assert.h>
#include <ncine/Application.h>
#include <ncine/Vector2.h>
#include <ncine/AppConfiguration.h>
#include <nctl/String.h>
#include <ncine/Matrix4x4.h>
#include "jugiAppNCINE.h"



namespace jugiApp{



using namespace jugimap;




void PlatformerSceneNCINE::UpdateTexts()
{

    PlatformerScene::UpdateTexts();


    //---- FPS
    static int frameRate = 0;
    static int secondsCounter = 0;
    static int numFramesStored = 0;

    //---- Obtain frame rate
    if(1.0 + jugimap::time.GetPassedNetTimeMS()/1000.0 > secondsCounter){
        secondsCounter++;
        frameRate = ncine::theApplication().numFrames() - numFramesStored;
        numFramesStored = ncine::theApplication().numFrames();
    }
    std::string fpsString = " FPS: " + std::to_string(frameRate);
    if(fpsString != labels[4]->GetTextString()) labels[4]->SetTextString(fpsString);

}



}
