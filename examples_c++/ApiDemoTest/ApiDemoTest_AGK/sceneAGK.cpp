#include <chrono>
#include <assert.h>
#include "sceneAGK.h"



namespace apiTestDemo{


namespace jm = jugimap;



bool PlatformerSceneSceneAGK::Init()
{


    // Set starting z-order values for the maps in the scene.
    zOrderBackgroundMap = 5000;
    zOrderWorldMap = 4000;
    zOrderTestParallaxMap = 3000;
    zOrderGuiMap = 2000;
    zOrderInfoMap = 1000;


    // Engine indpenedent scene initialization
    if(PlatformerScene::Init()==false){

        if(jm::JugiMapBinaryLoader::error !=""){
            jm::settings.SetErrorMessage(jm::JugiMapBinaryLoader::error);
        }else{
            jm::settings.SetErrorMessage("SetupDemo() error!");
        }

        return false;
    }


    // Initialize few remaining things that can not be done in engine independent way.

    // Create font (its size may vary between engines).
    font = jm::objectFactory->NewFont("media/fonts/OpenSans-Regular.ttf", 19, jm::FontKind::TRUE_TYPE);

    // Set few parameters regarding precise text positioning needed in 'MakeButtonsAndTexts'
    buttonLabelOffset = jm::Vec2f(20,8);
    extraLabelsYSpacingFactor = 1.2;
    extraLabelsCount = 8;
    extraLabelsYOffset = 50;

    // Skip buttons for features not supported by the engine.
    skippedButtons = std::vector<int>{ButtonTag::DOUBLE_CAMERA};

    MakeButtonsAndTexts();


    //--- set physics world properties
    agk::SetPhysicsGravity(0, 500);
    //agk::SetPhysicsDebugOn();

    return true;
}




void PlatformerSceneSceneAGK::UpdateEngineObjects()
{

    PlatformerScene::UpdateEngineObjects();     // update agk objects from jugimap objects


    // Prepare objects for rendering
    //---------------------------------------

    // Convert world map camera to AGK 'view'.
    jm::Vec2f offset = worldCamera.GetProjectedMapPosition();
    offset = offset / worldCamera.GetScale();                           // this gives correct result
    agk::SetViewOffset(-offset.x, -offset.y);
    agk::SetViewZoom(worldCamera.GetScale());

    // Draw drawing layers. AGK always draws primitives on top so layers z-order has no meaning for them.
    worldMapDrawingLayer->DrawEngineLayer();
    if(testParallaxMapDrawingLayer) testParallaxMapDrawingLayer->DrawEngineLayer();
    guiMapDrawingLayer->DrawEngineLayer();


    // Reset camera change flags
    worldCamera.ClearChangeFlags();
    guiCamera.ClearChangeFlags();
    infoCamera.ClearChangeFlags();
}



void PlatformerSceneSceneAGK::SetDynamicCrystalsPhysics()
{


    if(dynamicCrystals){

        //---- turn ON static physics mode for main world tiles
        jm::SpriteLayer *layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteAGK*>(s)->SetPhysicsMode(jm::StandardSpriteAGK::PhysicsMode::STATIC);
            }
        }

        //---- turn ON static physics mode for characters
        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteAGK*>(s)->SetPhysicsMode(jm::StandardSpriteAGK::PhysicsMode::KINEMATIC);      //or static
            }
        }


        //---- turn ON dynamic physics mode for crystals
        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<jm::StandardSpriteAGK*>(s)->SetPhysicsMode(jm::StandardSpriteAGK::PhysicsMode::DYNAMIC);
                    s->SetEngineSpriteUsedDirectly(true);            // the sprite is no longer controlled via jugimap interface
                    int spriteAgkId = static_cast<jm::StandardSpriteAGK*>(s)->GetAgkId();
                    agk::SetSpritePhysicsDensity(spriteAgkId, 1.0, 0);
                    agk::SetSpritePhysicsRestitution(spriteAgkId, 0.3, 0);
                    agk::SetSpritePhysicsFriction(spriteAgkId, 0.7, 0);
                }
            }
        }

    }else{


        //---- turn OFF physics for all sprites in simulation
        jm::SpriteLayer *layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteAGK*>(s)->SetPhysicsMode(jm::StandardSpriteAGK::PhysicsMode::NO_PHYSICS);
            }
        }

        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                static_cast<jm::StandardSpriteAGK*>(s)->SetPhysicsMode(jm::StandardSpriteAGK::PhysicsMode::NO_PHYSICS);
            }
        }


        layer = dynamic_cast<jm::SpriteLayer*>(jm::FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(jm::Sprite* s : layer->GetSprites()){
            if(s->GetKind()==jm::SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<jm::StandardSpriteAGK*>(s)->SetPhysicsMode(jm::StandardSpriteAGK::PhysicsMode::NO_PHYSICS);
                    s->SetEngineSpriteUsedDirectly(false);            // ! The sprite is again used via jugimap interface (so that we can restore it to its initial position)
                    //--- restore transformation properties from jugimap sprite which were not changed during physics simulation
                    s->SetChangeFlags(jm::Sprite::Property::TRANSFORMATION);
                    s->UpdateEngineSprite();
                  }
            }
        }
    }

}



void PlatformerSceneSceneAGK::UpdateTexts()
{

    PlatformerScene::UpdateTexts();

    //----
    std::string str = "FPS: " + std::to_string(int(std::roundf(agk::ScreenFPS())));
    if(str != labels[4]->GetTextString()) labels[4]->SetTextString(str);

    //----
    str = "Managed Sprite Count: " + std::to_string(agk::GetManagedSpriteCount());
    if(str != labels[5]->GetTextString()) labels[5]->SetTextString(str);

    //----
    str = "Managed Sprite Draw Calls: " + std::to_string(agk::GetManagedSpriteDrawCalls());
    if(str != labels[6]->GetTextString()) labels[6]->SetTextString(str);

    //----
    str = "Managed Sprite Drawn Count: " + std::to_string(agk::GetManagedSpriteDrawnCount());
    if(str != labels[7]->GetTextString()) labels[7]->SetTextString(str);


}





}
