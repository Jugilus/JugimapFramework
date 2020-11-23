#include <chrono>
#include <assert.h>
#include "jugiAppAGK.h"



namespace jugiApp{



using namespace jugimap;



bool PlatformerSceneAGK::Init()
{

    if(PlatformerScene::Init()==false){
        return false;
    }

    //---
    agk::SetPhysicsGravity(0, 500);

    return true;
}


void PlatformerSceneAGK::SetDynamicCrystalsPhysics()
{

    if(dynamicCrystals){

        //---- turn ON static physics mode for main world tiles
        SpriteLayer *layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteAGK*>(s)->SetPhysicsMode(StandardSpriteAGK::PhysicsMode::STATIC);
            }
        }

        //---- turn ON static physics mode for characters
        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteAGK*>(s)->SetPhysicsMode(StandardSpriteAGK::PhysicsMode::KINEMATIC);      //or static
            }
        }


        //---- turn ON dynamic physics mode for crystals
        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<StandardSpriteAGK*>(s)->SetPhysicsMode(StandardSpriteAGK::PhysicsMode::DYNAMIC);
                    s->SetDisabledEngineSpriteUpdate(true);            // the sprite is no longer controlled via jugimap interface
                    int spriteAgkId = static_cast<StandardSpriteAGK*>(s)->GetAgkId();
                    agk::SetSpritePhysicsDensity(spriteAgkId, 1.0, 0);
                    agk::SetSpritePhysicsRestitution(spriteAgkId, 0.3, 0);
                    agk::SetSpritePhysicsFriction(spriteAgkId, 0.7, 0);
                }
            }
        }

    }else{


        //---- turn OFF physics for all sprites in simulation
        SpriteLayer *layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Main construction"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteAGK*>(s)->SetPhysicsMode(StandardSpriteAGK::PhysicsMode::NO_PHYSICS);
            }
        }

        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Characters"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                static_cast<StandardSpriteAGK*>(s)->SetPhysicsMode(StandardSpriteAGK::PhysicsMode::NO_PHYSICS);
            }
        }


        layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(worldMap, "Items"));
        assert(layer);

        for(Sprite* s : layer->GetSprites()){
            if(s->GetKind()==SpriteKind::STANDARD){
                if(s->GetSourceSprite()->GetName()=="Blue star" || s->GetSourceSprite()->GetName()=="Violet star" || s->GetSourceSprite()->GetName()=="Cyan star"){
                    static_cast<StandardSpriteAGK*>(s)->SetPhysicsMode(StandardSpriteAGK::PhysicsMode::NO_PHYSICS);
                    //s->SetEngineSpriteUsedDirectly(false);            // ! The sprite is again used via jugimap interface (so that we can restore it to its initial position)
                    s->SetDisabledEngineSpriteUpdate(false);
                    //--- restore transformation properties from jugimap sprite which were not changed during physics simulation
                    s->SetChangeFlags(Sprite::Property::TRANSFORMATION);
                    s->UpdateEngineObjects();
                  }
            }
        }
    }

}


void PlatformerSceneAGK::UpdateTexts()
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
