#include <chrono>
#include <assert.h>
#include "various.h"
#include "entities.h"
#include "tileGrid.h"
#include "shapesDrawingLayer.h"


using namespace jugimap;



namespace apiTestDemo{



ShapesDrawingLayer::ShapesDrawingLayer(const std::string &_name, jugimap::Map *_map) : DrawingLayer(_name)
{
    map = _map;
    colorDrawVectorShape = jugimap::ColorRGBA(255,255,255,255);
    colorDrawMouseOverSpriteShape = jugimap::ColorRGBA(200,0,0,255);
}



void ShapesDrawingLayer::DrawEngineLayer()
{
    if(drawer==nullptr) return;

    drawer->Clear();
    if(doDrawMapSpritesCollisionShapes){
        DrawMapSpritesCollisionShapes();
    }
    if(doDrawMapVectorShapes){
        DrawMapVectorShapes();
    }
}



void ShapesDrawingLayer::DrawMapVectorShapes()
{

    drawer->SetOutlineColor(colorDrawVectorShape);

    for(Layer *l : map->GetLayers()){

        if(l->GetKind()==LayerKind::VECTOR){
            VectorLayer * vl = static_cast<VectorLayer*>(l);

            for(VectorShape *vs : vl->GetVectorShapes()){
                DrawGeometricShape(GetDrawer(), vs->GetGeometricShape(), vl->GetParallaxOffset());
            }
        }
    }
}


void ShapesDrawingLayer::DrawMapSpritesCollisionShapes()
{


    Sprite *SpriteMouseOver = nullptr;
    drawer->SetOutlineColor(colorDrawVectorShape);


    int nAll = 0;
    int countDrawn = 0;

    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);

            for(Sprite *s : sl->GetSprites()){

                Sprite *spriteMouseOverCurrent = DrawSpriteCollisionShapes(s, countDrawn);
                nAll++;

                if(spriteMouseOverCurrent){
                    SpriteMouseOver = spriteMouseOverCurrent;
                }
            }
        }
    }

   drawer->SetOutlineColor(colorDrawMouseOverSpriteShape);

    if(SpriteMouseOver){
        DrawSpriteCollisionShapes(SpriteMouseOver, countDrawn);
        nAll++;
    }
}


Sprite * ShapesDrawingLayer::DrawSpriteCollisionShapes(Sprite *sprite, int &countDrawn)
{

    Sprite *spriteMouseOver = nullptr;


    if(sprite->GetKind()==SpriteKind::STANDARD){

        StandardSprite  *standardSprite = static_cast<StandardSprite*>(sprite);

        //---- check all situations where drawing shapes is not needed
        if(standardSprite->IsVisible()==false) return nullptr;
        if(standardSprite->GetPhysicsMode()==StandardSprite::PhysicsMode::DYNAMIC) return nullptr;
        if(standardSprite->IsEngineSpriteUsedDirectly()) return nullptr;
        if(standardSprite->GetActiveImage()==nullptr) return nullptr;
        //if(glob::currentCamera->GetMapVisibleRect().Intersects(standardSprite->GetBoundingBox())==false) return nullptr;
        if(GetMap()->GetCamera()->GetMapVisibleRect().Intersects(standardSprite->GetBoundingBox())==false) return nullptr;

        std::vector<VectorShape*>allShapes = standardSprite->GetActiveImage()->GetSpriteShapes();
        for(VectorShape * vs : standardSprite->GetActiveImage()->GetProbeShapes()) allShapes.push_back(vs);
        if(allShapes.empty()) return nullptr;

        //----
        //if(standardSprite->PointInside(glob::currentCamera->MapPointFromScreenPoint(mouse.GetScreenPosition()))){
        if(standardSprite->PointInside(GetMap()->GetCamera()->MapPointFromScreenPoint(mouse.GetScreenPosition()))){
            spriteMouseOver = sprite;
        }

        // transform vector shapes to world positions of their sprites
        AffineMat3f m = MakeTransformationMatrix(standardSprite->GetFullGlobalPosition(), sprite->GetGlobalScale(), sprite->GetGlobalFlip(), sprite->GetGlobalRotation(), Vec2f());
        float scale = standardSprite->GetGlobalScale().x;

        for(VectorShape *vs : allShapes){
            DrawTransformedGeometricShape(GetDrawer(), vs->GetGeometricShape(), m, scale);
        }

        countDrawn++;

    }else if(sprite->GetKind()==SpriteKind::COMPOSED){

        ComposedSprite *composedSprite = static_cast<ComposedSprite*>(sprite);

        for(Layer *l : composedSprite->GetLayers()){
            if(l->GetKind()==LayerKind::SPRITE){
                SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                for(Sprite *s : sl->GetSprites()){
                    Sprite * sMouseOver = DrawSpriteCollisionShapes(s, countDrawn);
                    if(sMouseOver) spriteMouseOver = sMouseOver;
                }
            }
        }
    }

    return spriteMouseOver;
}



}
