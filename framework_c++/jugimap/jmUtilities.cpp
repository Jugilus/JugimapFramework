#include <string>
#include <sstream>
#include "jmGlobal.h"
#include "jmSourceGraphics.h"
#include "jmFrameAnimation.h"
#include "jmTimelineAnimation.h"
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


void CollectSpritesWithId(Map *map, std::vector<Sprite*>&collectedSprites,  int id)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithId(sl, collectedSprites, id);
        }
    }
}


void CollectSpritesWithId(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  int id)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithId(sl, collectedSprites, id);
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


void CollectSpritesWithName(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  const std::string &name)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithName(sl, collectedSprites, name);
        }
    }
}


void CollectSpritesOfKind(Map *map, std::vector<Sprite*>&collectedSprites,  SpriteKind kind)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesOfKind(sl, collectedSprites, kind);
        }
    }
}


void CollectSpritesOfKind(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  SpriteKind kind)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesOfKind(sl, collectedSprites, kind);
        }
    }
}


void CollectSpritesWithParameter(Map *map, std::vector<Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithParameter(sl, collectedSprites, pName, pValue);
        }
    }
}


void CollectSpritesWithParameter(ComposedSprite *composedSprite, std::vector<Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithParameter(sl, collectedSprites, pName, pValue);
        }
    }
}


void CollectSpritesWithConstParameter(Map *map, std::vector<Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithConstParameter(sl, collectedSprites, pName, pValue);
        }
    }
}


void CollectSpritesWithConstParameter(ComposedSprite *composedSprite, std::vector<Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithConstParameter(sl, collectedSprites, pName, pValue);
        }
    }
}


void CollectSpritesWithOrigConstParameter(Map *map, std::vector<Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithOrigConstParameter(sl, collectedSprites, pName, pValue);
        }
    }
}


void CollectSpritesWithOrigConstParameter(ComposedSprite *composedSprite, std::vector<Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithOrigConstParameter(sl, collectedSprites, pName, pValue);
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


void CollectSpritesWithDataFlags(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  int dataFlags, bool compareDataFlagsAsBitmask)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithDataFlags(sl, collectedSprites, dataFlags, compareDataFlagsAsBitmask);
        }
    }
}


void CollectSpritesWithSourceSpriteName(Map *map, std::vector<Sprite*>&collectedSprites,  const std::string &name)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithSourceSpriteName(sl, collectedSprites, name);
        }
    }
}


void CollectSpritesWithSourceSpriteName(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  const std::string &name)
{
    for(Layer *l : composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            CollectSpritesWithSourceSpriteName(sl, collectedSprites, name);
        }
    }
}


//---------------------------------------------------------------------------



void CollectSpritesWithId(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  int id)
{

    for(Sprite *s : layer->GetSprites()){
        if(s->GetId()==id){
            collectedSprites.push_back(s);
        }
        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesWithId(sl, collectedSprites, id);
                }
            }
        }
    }

}


void CollectSpritesWithName(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  const std::string &name)
{

    for(Sprite *s : layer->GetSprites()){
        if(s->GetName()==name){
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



void CollectSpritesOfKind(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  SpriteKind kind)
{
    for(Sprite *s : layer->GetSprites()){
        if(s->GetKind()==kind){
            collectedSprites.push_back(s);
        }
        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesOfKind(sl, collectedSprites, kind);
                }
            }
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


void CollectSpritesWithConstParameter(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, const std::string &pName, const std::string &pValue)
{

    for(Sprite *s : layer->GetSprites()){
        if(Parameter::Exists(s->GetSourceSprite()->GetConstantParameters(), pName, pValue)){
            collectedSprites.push_back(s);
        }
        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesWithConstParameter(sl, collectedSprites, pName, pValue);
                }
            }
        }
    }
}


void CollectSpritesWithOrigConstParameter(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, const std::string &pName, const std::string &pValue)
{

    for(Sprite *s : layer->GetSprites()){
        if(Parameter::Exists(s->GetOriginalSourceSprite()->GetConstantParameters(), pName, pValue)){
            collectedSprites.push_back(s);
        }
        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesWithOrigConstParameter(sl, collectedSprites, pName, pValue);
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


void CollectSpritesWithSourceSpriteName(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  const std::string &name)
{
    for(Sprite *s : layer->GetSprites()){
        if(s->GetSourceSprite()->GetName()==name){
            collectedSprites.push_back(s);
        }
        if(s->GetKind()==SpriteKind::COMPOSED){
            for(Layer *l : static_cast<ComposedSprite*>(s)->GetLayers()){
                if(l->GetKind()==LayerKind::SPRITE){
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    CollectSpritesWithSourceSpriteName(sl, collectedSprites, name);
                }
            }
        }
    }

}



//--- FIND VECTOR SHAPE



VectorShape* FindVectorShapeWithName(Map *map, const std::string &name)
{

    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::VECTOR){
            VectorLayer * vl = static_cast<VectorLayer*>(l);
            VectorShape * vs = FindVectorShapeWithName(vl, name);
            if(vs) return vs;
        }
    }
    return nullptr;
}


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


VectorShape* FindVectorShapeWithProperties(Map *map, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{
    for(Layer *l : map->GetLayers()){
        if(l->GetKind()==LayerKind::VECTOR){
            VectorLayer * vl = static_cast<VectorLayer*>(l);
            VectorShape * vs = FindVectorShapeWithProperties(vl, dataFlags, compareDataFlagsAsBitmask, kind);
            if(vs) return vs;
        }
    }
    return nullptr;
}


VectorShape* FindVectorShapeWithName(VectorLayer * vectorLayer, const std::string &name)
{
    return FindVectorShapeWithName(vectorLayer->GetVectorShapes(), name);
}


VectorShape* FindVectorShapeWithParameter(VectorLayer * vectorLayer, const std::string &pName, const std::string &pValue, ShapeKind kind)
{
    return FindVectorShapeWithParameter(vectorLayer->GetVectorShapes(), pName, pValue, kind);
}


VectorShape* FindVectorShapeWithProperties(VectorLayer * vectorLayer, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{
    return FindVectorShapeWithProperties(vectorLayer->GetVectorShapes(), dataFlags, compareDataFlagsAsBitmask, kind);
}


VectorShape* FindVectorShapeWithName(std::vector<VectorShape *> &vectorShapes, const std::string &name)
{
    for(VectorShape * vs : vectorShapes){
        if(vs->GetName()==name){
            return vs;
        }
    }
    return nullptr;
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


VectorShape* FindVectorShapeWithProperties(std::vector<VectorShape*>&vectorShapes, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{

    for(VectorShape *vs : vectorShapes){

        if(kind!=ShapeKind::NOT_DEFINED && vs->GetKind()!=kind){
            continue;
        }
        //if(probe!=-1 && vs->IsProbe()!=bool(probe)){
        //    continue;
        //}
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


/*
VectorShape* FindSpriteExtraShapeWithProperties(StandardSprite * standardSprite, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{
    if(standardSprite->GetActiveImage()==nullptr) return nullptr;

    return FindVectorShapeWithProperties(standardSprite->GetActiveImage()->GetExtraShapes(), 1, dataFlags, compareDataFlagsAsBitmask, kind);
}
*/


VectorShape* FindSpriteExtraShapeWithProperties(GraphicItem * graphicItem, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{

    if(graphicItem==nullptr) return nullptr;

    return FindVectorShapeWithProperties(graphicItem->GetExtraShapes(), dataFlags, compareDataFlagsAsBitmask, kind);

}


VectorShape* FindSpriteCollisionShapeWithProperties(GraphicItem * graphicItem, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind)
{

    if(graphicItem==nullptr) return nullptr;

    return FindVectorShapeWithProperties(graphicItem->GetSpriteShapes(), dataFlags, compareDataFlagsAsBitmask, kind);

}



//--- FIND SOURCE SPRITE


void CollectSourceSpritesWithConstParameter(SourceGraphics *sourceGraphics, std::vector<SourceSprite*>&collectedSourceSprites, const std::string &pName, const std::string &pValue)
{

    for(SourceSprite* ss : sourceGraphics->GetSourceSprites()){
        if(Parameter::Exists(ss->GetConstantParameters(), pName, pValue)){
            collectedSourceSprites.push_back(ss);
        }
    }

}


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




//--- FIND ANIMATIONS


FrameAnimation* FindFrameAnimationWithName(StandardSprite * standardSprite, const std::string &name)
{
    return FindFrameAnimationWithName(standardSprite->GetSourceSprite()->GetFrameAnimations(), name);
}


FrameAnimation* FindFrameAnimationWithName(std::vector<FrameAnimation*>&animations, const std::string &name)
{
    for(FrameAnimation * fa : animations){
        if(fa->GetName()==name){
            return fa;
        }
    }
    return nullptr;
}


FrameAnimation* FindFrameAnimationWithParameter(std::vector<FrameAnimation*>&animations, const std::string &pName, const std::string &pValue)
{
    for(FrameAnimation * fa : animations){
        if(Parameter::Exists(fa->GetParameters(), pName, pValue)){
            return fa;
        }
    }
    return nullptr;
}


//-----

TimelineAnimation* FindTimelineAnimationWithName(StandardSprite * standardSprite, const std::string &name)
{
    return FindTimelineAnimationWithName(standardSprite->GetSourceSprite()->GetTimelineAnimations(), name);
}


TimelineAnimation* FindTimelineAnimationWithName(std::vector<TimelineAnimation*>&animations, const std::string &name)
{
    for(TimelineAnimation * ta : animations){
        if(ta->GetName()==name){
            return ta;
        }
    }
    return nullptr;
}


TimelineAnimation* FindTimelineAnimationWithParameter(std::vector<TimelineAnimation*>&animations, const std::string &pName, const std::string &pValue)
{
    for(TimelineAnimation * ta : animations){
        if(Parameter::Exists(ta->GetParameters(), pName, pValue)){
            return ta;
        }
    }
    return nullptr;
}


//---



void GatherSpritesWithSetNameID(ComposedSprite *_composedSprite, std::vector<std::vector<Sprite*>>&spritesPerNameID)
{

    for(Layer* l : _composedSprite->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer* sl = static_cast<SpriteLayer*>(l);

            for(Sprite* o : sl->GetSprites()){

                if(o->GetName() != ""){

                    int index = -1;
                    for(int i=0; i<spritesPerNameID.size(); i++){
                        if(o->GetName()==spritesPerNameID[i].front()->GetName()){
                            index = i;
                            break;
                        }
                    }

                    if(index==-1){
                        spritesPerNameID.push_back(std::vector<Sprite*>());
                        index = spritesPerNameID.size()-1;
                    }

                    bool objectExist = false;
                    for(Sprite* o2 : spritesPerNameID[index]){
                        if(o2==o){
                            objectExist = true;
                            break;
                        }
                    }
                    if(objectExist==false){
                        spritesPerNameID[index].push_back(o);
                    }

                }
                if(o->GetKind()==SpriteKind::COMPOSED){
                    GatherSpritesWithSetNameID(static_cast<ComposedSprite*>(o), spritesPerNameID);
                }
            }
        }
    }
}




}
