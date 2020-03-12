#include <assert.h>
#include <ncine/Application.h>
#include <ncine/Vector2.h>
#include <ncine/AppConfiguration.h>
#include <nctl/String.h>
#include <ncine/Matrix4x4.h>
#include "sceneNCINE.h"








ParallaxSceneNC::~ParallaxSceneNC()
{
    if(ncNode){
        //delete ncNode;
        jugimap::DeleteNCNode(ncNode);
    }
}




bool ParallaxSceneNC::Init()
{

    // Set z-order starting values for maps
    zOrderBackgroundMap = 1000;
    zOrderWorldMap = 2000;


    // Engine independent scene initialization
    if(ParallaxScene::Init()==false){

        //--- show error message
        if(jugimap::JugiMapBinaryLoader::error != ""){
            jugimap::settings.SetErrorMessage(jugimap::JugiMapBinaryLoader::error);
        }else{
            jugimap::settings.SetErrorMessage("Scene::Init() error!");
        }

        //AddErrorMessageTextNode();
        return false;
    }

    // Create font.
    //font = jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.fnt", 14, jugimap::FontKind::NOT_DEFINED);

    // Build the nodes structure.
    ncNode = new ncine::SceneNode();        // node of this scene
    ncine::theApplication().rootNode().addChildNode(ncNode);

    SetEngineSceneLink(ncNode);
    ncNode->addChildNode(static_cast<jugimap::MapNC*>(backgroundMap)->GetMapNCNode());
    ncNode->addChildNode(static_cast<jugimap::MapNC*>(worldMap)->GetMapNCNode());

    //AddErrorMessageTextNode();

    return true;
}


/*
void ParallaxSceneNC::AddErrorMessageTextNode()
{

    if(font==nullptr){
         font = jugimap::objectFactory->NewFont("media/fonts/OpenSans-Regular.fnt", 14, jugimap::FontKind::NOT_DEFINED);
    }
    if(ncNode==nullptr){
        ncNode = new ncine::SceneNode();
        ncine::theApplication().rootNode().addChildNode(ncNode);
    }

    ncine::TextNode * textNode = new ncine::TextNode(ncNode, static_cast<jugimap::FontNC*>(font)->GetNCFont());
    textNode->setAnchorPoint(ncine::DrawableNode::AnchorTopLeft);
    textNode->setPosition(10, jugimap::settings.GetScreenSize().y-10);
    textNode->setLayer(10000);
    textNode->setString("");

    jugimap::errorMessageLabel = textNode;       // set global link

}
*/




