#include <string>
#include <sstream>
#include "jmGlobal.h"
#include "jmSourceGraphics.h"
#include "jmFrameAnimation.h"
#include "jmLayers.h"
#include "jmMap.h"
#include "jmSprites.h"
#include "jmVectorShapes.h"
#include "jmUtilities.h"


namespace jugimap {


//--- COLLECT LAYERS

void CollectLayersWithParameter(Map *map, std::vector<Layer*>&collectedLayers,  const std::string &pName, const std::string &pValue, LayerKind layerKind)
{
    for(Layer *l : map->GetLayers()){
        if(layerKind!=LayerKind::NOT_DEFINED && layerKind!=l->GetKind()) continue;
        if(Parameter::Exists(l->GetParameters(), pName, pValue)){
            collectedLayers.push_back(l);
        }
    }
}


Layer* FindLayerWithName(Map *map, const std::string &name, LayerKind layerKind)
{
    for(Layer *l : map->GetLayers()){
        if(layerKind!=LayerKind::NOT_DEFINED && layerKind!=l->GetKind()) continue;
        if(l->GetName()==name){
            return l;
        }
    }
    return nullptr;
}



//--- COLLECT SPRITES


void CollectSpritesWithParameter(Map *map, std::vector<Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithParameter(sl, collectedSprites, pName, pValue);
        }
    }
}


void CollectSpritesWithDataFlags(Map *map, std::vector<Sprite*>&collectedSprites,  int dataFlags, bool compareDataFlagsAsBitmask)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithDataFlags(sl, collectedSprites, dataFlags, compareDataFlagsAsBitmask);
        }
    }
}


void CollectSpritesWithName(Map *map, std::vector<Sprite*>&collectedSprites,  const std::string &name)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithName(sl, collectedSprites, name);
        }
    }
}


void CollectSpritesWithParameter(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, const std::string &pName, const std::string &pValue)
{

    for(Sprite *s : layer->GetSprites()){
        if(Parameter::Exists(s->GetParameters(), pName, pValue)){
            collectedSprites.push_back(s);
        }
        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesWithParameter(sl, collectedSprites, pName, pValue);
                }
            }
        }
    }
}


void CollectSpritesWithDataFlags(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, int dataFlags, bool compareDataFlagsAsBitmask)
{
    for(Sprite *s : layer->GetSprites()){
        if(compareDataFlagsAsBitmask){
            if(s->GetDataFlags() & dataFlags){
                collectedSprites.push_back(s);
            }

        }else{
            if(s->GetDataFlags()==dataFlags){
                collectedSprites.push_back(s);
            }
        }

        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesWithDataFlags(sl, collectedSprites, dataFlags, compareDataFlagsAsBitmask);
                }
            }
        }
    }
}


void CollectSpritesWithName(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  const std::string &name)
{
    for(Sprite *s : layer->GetSprites()){
        if(s->GetSourceSprite()->GetName()==name){
            collectedSprites.push_back(s);
        }
        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesWithName(sl, collectedSprites, name);
                }
            }
        }
    }

}



//--- FIND VECTOR SHAPE


VectorShape* FindVectorShapeWithParameter(Map *map, const std::string &pName, const std::string &pValue, ShapeKind kind)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::VECTOR){
            VectorLayer * vl = static_cast<VectorLayer*>(l);
            VectorShape * vs = FindVectorShapeWithParameter(vl, pName, pValue, kind);
            if(vs) return vs;
        }
    }
    return nullptr;
}


VectorShape* FindVectorShapeWithProperties(Map *map, int probe, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::VECTOR){
            VectorLayer * vl = static_cast<VectorLayer*>(l);
            VectorShape * vs = FindVectorShapeWithProperties(vl, probe, dataFlags, compareDataFlagsAsBitmask, kind);
            if(vs) return vs;
        }
    }
    return nullptr;
}


VectorShape* FindVectorShapeWithParameter(VectorLayer * vectorLayer, const std::string &pName, const std::string &pValue, ShapeKind kind)
{
    return FindVectorShapeWithParameter(vectorLayer->GetVectorShapes(), pName, pValue, kind);
}


VectorShape* FindVectorShapeWithProperties(VectorLayer * vectorLayer, int probe, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{
    return FindVectorShapeWithProperties(vectorLayer->GetVectorShapes(), probe, dataFlags, compareDataFlagsAsBitmask, kind);
}


VectorShape* FindVectorShapeWithParameter(std::vector<VectorShape *> &vectorShapes, const std::string &pName, const std::string &pValue, ShapeKind kind)
{
    for(VectorShape * vs : vectorShapes){

        if(kind!=ShapeKind::NOT_DEFINED && vs->GetKind()!=kind){
            continue;
        }
        if(Parameter::Exists(vs->GetParameters(), pName, pValue)){
            return vs;
        }
    }
    return nullptr;
}


VectorShape* FindVectorShapeWithProperties(std::vector<VectorShape*>&vectorShapes, int probe, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{

    for(VectorShape *vs : vectorShapes){

        if(kind!=ShapeKind::NOT_DEFINED && vs->GetKind()!=kind){
            continue;
        }
        if(probe!=-1 && vs->IsProbe()!=bool(probe)){
            continue;
        }
        if(dataFlags!=-1){
            if(compareDataFlagsAsBitmask){
                if((vs->GetDataFlags() & dataFlags)==0){
                    continue;
                }
            }else{
                if(vs->GetDataFlags() != dataFlags){
                    continue;
                }
            }
        }
        return vs;
    }

    return nullptr;
}


VectorShape* FindProbeShapeWithProperties(StandardSprite * standardSprite, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{
    if(standardSprite->GetActiveImage()==nullptr) return nullptr;

    return FindVectorShapeWithProperties(standardSprite->GetActiveImage()->GetProbeShapes(), 1, dataFlags, compareDataFlagsAsBitmask, kind);
}



//--- FIND SOURCE SPRITE


SourceSprite* FindSourceSpriteWithName(SourceGraphics *sourceGraphics, const std::string &name)
{

    for(SourceSprite* ss : sourceGraphics->GetSourceSprites()){
        if(ss->GetName()==name){
            return ss;
        }
    }

    return nullptr;
}


SourceSprite* FindSourceSpriteByGraphicFilePath(SourceGraphics *sourceGraphics, const std::string &filePath)
{
    for(SourceSprite* ss : sourceGraphics->GetSourceSprites()){
        for(GraphicItem *gi : ss->GetGraphicItems()){
            if(gi->GetFile()->GetRelativeFilePath()==filePath){
                return ss;
            }
        }
    }

    return nullptr;
}



//--- //--- FIND ANIMATIONS





FrameAnimation* FindFrameAnimationWithName(StandardSprite * standardSprite, const std::string &name)
{
    return FindFrameAnimationWithName(standardSprite->GetSourceSprite()->GetFrameAnimations(), name);
}


FrameAnimation* FindFrameAnimationWithName(std::vector<FrameAnimation*>&frameAnimations, const std::string &name)
{
    for(FrameAnimation * fa : frameAnimations){
        if(fa->GetName()==name){
            return fa;
        }
    }
    return nullptr;
}


FrameAnimation* FindFrameAnimationWithParameter(std::vector<FrameAnimation*>&frameAnimations, const std::string &pName, const std::string &pValue)
{
    for(FrameAnimation * fa : frameAnimations){
        if(Parameter::Exists(fa->GetParameters(), pName, pValue)){
            return fa;
        }
    }
    return nullptr;
}


}
