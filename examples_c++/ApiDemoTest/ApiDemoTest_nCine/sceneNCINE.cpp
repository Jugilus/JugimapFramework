#include <assert.h>
#include <ncine/Application.h>
#include <ncine/Vector2.h>
#include <ncine/AppConfiguration.h>
#include <nctl/String.h>
#include <ncine/Matrix4x4.h>
#include "sceneNCINE.h"



namespace apiTestDemo{


namespace jm = jugimap;




PlatformerSceneNC::~PlatformerSceneNC()
{
    if(ncNode){
        //delete ncNode;
        jm::DeleteNCNode(ncNode);
    }
}




bool PlatformerSceneNC::Init()
{

    // Set z-order starting values for maps
    zOrderBackgroundMap = 1000;
    zOrderWorldMap = 2000;
    zOrderTestParallaxMap = 3000;
    zOrderGuiMap = 4000;
    zOrderInfoMap = 5000;


    // Engine independent scene initialization
    if(PlatformerScene::Init()==false){

        //--- show error message
        if(jm::JugiMapBinaryLoader::error != ""){
            jm::settings.SetErrorMessage(jm::JugiMapBinaryLoader::error);
        }else{
            jm::settings.SetErrorMessage("Scene::Init() error!");
        }

        AddErrorMessageTextNode();
        return false;
    }


    // Initialize few remaining things that can not be done in engine independent way.

    // Create font.
    font = jm::objectFactory->NewFont("media/fonts/OpenSans-Regular.fnt", 14, jm::FontKind::NOT_DEFINED);

    // Set parameters for precise text positioning needed in 'MakeButtonsAndTexts'.
    buttonLabelOffset = jm::Vec2f(20,-23);
    extraLabelsYSpacingFactor = 1.1;
    extraLabelsYOffset = -70;
    extraLabelsCount = 5;

    // Skip buttons for features not supported by the engine.
    skippedButtons = std::vector<int>{ButtonTag::SHOW_COLLISION_SHAPES, ButtonTag::SHOW_VECTOR_SHAPES, ButtonTag::DOUBLE_CAMERA,ButtonTag::DYNAMIC_CRYSTALS};

    MakeButtonsAndTexts();


    // Build the nodes structure.
    ncNode = new ncine::SceneNode();        // node of this scene
    ncine::theApplication().rootNode().addChildNode(ncNode);

    SetEngineSceneLink(ncNode);
    ncNode->addChildNode(static_cast<jm::MapNC*>(backgroundMap)->GetMapNCNode());
    ncNode->addChildNode(static_cast<jm::MapNC*>(worldMap)->GetMapNCNode());
    if(testParallaxMap) ncNode->addChildNode(static_cast<jm::MapNC*>(testParallaxMap)->GetMapNCNode());
    ncNode->addChildNode(static_cast<jm::MapNC*>(guiMap)->GetMapNCNode());
    ncNode->addChildNode(static_cast<jm::MapNC*>(infoMap)->GetMapNCNode());

    AddErrorMessageTextNode();

    return true;
}


void PlatformerSceneNC::AddErrorMessageTextNode()
{

    if(font==nullptr){
         font = jm::objectFactory->NewFont("media/fonts/OpenSans-Regular.fnt", 14, jm::FontKind::NOT_DEFINED);
    }
    if(ncNode==nullptr){
        ncNode = new ncine::SceneNode();
        ncine::theApplication().rootNode().addChildNode(ncNode);
    }

    ncine::TextNode * textNode = new ncine::TextNode(ncNode, static_cast<jm::FontNC*>(font)->GetNCFont());
    textNode->setAnchorPoint(ncine::DrawableNode::AnchorTopLeft);
    textNode->setPosition(10, jm::settings.GetScreenSize().y-10);
    textNode->setLayer(10000);
    textNode->setString("");

    jm::errorMessageLabel = textNode;       // set global link

}


void PlatformerSceneNC::UpdateTexts()
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
