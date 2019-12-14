#include <algorithm>
#include <utility>
#include <map>
#include <assert.h>
#include "jmCommon.h"
#include "jmVectorShapes.h"
#include "jmUtilities.h"
#include "jmVectorShapesUtilities.h"
#include "jmCamera.h"
#include "jmLayers.h"
#include "jmSprites.h"
#include "jmMap.h"


namespace jugimap {



Map::~Map()
{
    DeleteContent();
}


void Map::DeleteContent()
{
    for(Layer* l : layers) delete l;
    layers.clear();
}


void Map::InitMapParameters()
{

    for(Parameter &PV : parameters){

        /*
        if(PV.name=="kind"){

            if(PV.value=="WORLD"){
                type = Type::WORLD;

            }else if(PV.value=="PARALLAX"){
                type = Type::PARALLAX;

            }else if(PV.value=="SCREEN"){
                type = Type::SCREEN;
            }
        }
        */
    }


    for(Layer *l : layers){
        l->InitLayerParameters();
    }

}


void Map::UpdateBoundingBox()
{

    boundingBox = Rectf(999999.0, 999999.0, -999999.0, -999999.0);

    DbgOutput("UpdateBoundingBox");

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            sl->UpdateBoundingBox();
            boundingBox = boundingBox.Unite(sl->boundingBox);

            DbgOutput("layer:"+ l->name + " BB min x:" + std::to_string(sl->boundingBox.min.x) + " y:" + std::to_string(sl->boundingBox.min.y)
                                       + " BB max x:" + std::to_string(sl->boundingBox.max.x) + " y:" + std::to_string(sl->boundingBox.max.y));
        }

    }

    DbgOutput("map:"+ name + " BB min x:" + std::to_string(boundingBox.min.x) + " y:" + std::to_string(boundingBox.min.y)
                           + " BB max x:" + std::to_string(boundingBox.max.x) + " y:" + std::to_string(boundingBox.max.y));

}


void Map::SetLayersPlanes()
{

    // Parallax and screen maps can have more layers which visually form one layers plane. Layers which form a layers plane must have the same
    // parallax (or screen) parameters! The layers planes are independent from each other - in the code bellow we obtain sizes for layers planes and
    // normalize the layers relative to their plane.


    //--- layers of the same plane are obtained via 'attachToLayer' name and share the same parameters
    for(Layer *l : layers){

        if(l->linkedLayerName.empty()){
            l->linkedLayerName = l->name;

        }else{
            Layer *aLayer = FindLayerWithName(this, l->linkedLayerName);
            if(aLayer){
                l->parallaxLayerMode = aLayer->parallaxLayerMode;
                l->parallaxFactor = aLayer->parallaxFactor;
                l->screenLayerMode = aLayer->screenLayerMode;
                l->alignX = aLayer->alignX;
                l->alignY = aLayer->alignY;
                l->alignOffset = aLayer->alignOffset;
            }
        }
        /*
        if(l->attachToLayer == l->name){
            if(kind==MapKind::PARALLAX){

                if(l->GetParallaxLayerMode()==ParallaxLayerMode::NO_CHANGE ||
                   l->GetParallaxLayerMode()==ParallaxLayerMode::TILING_X ||
                   l->GetParallaxLayerMode()==ParallaxLayerMode::TILING_Y ||
                   l->GetParallaxLayerMode()==ParallaxLayerMode::TILING_XY)
                {
                    ParallaxLayersPlane plp;
                    plp.attachToLayer = l->name;
                    plp.parallaxLayerMode = l->parallaxLayerMode;
                    plp.parallaxFactor = l->parallaxFactor;
                    plp.alignX = l->alignX;
                    plp.alignY = l->alignY;
                    plp.alignOffset = l->alignOffset;
                    parallaxLayersPlanes.push_back(plp);
                }

            }else if(kind==MapKind::SCREEN){


            }
        }
        */
    }



    std::map<std::string, std::vector<Layer*>>layersPlanes;
    std::map<std::string, std::vector<Layer*>>::iterator it;

    if(type==MapType::PARALLAX){

        //--- group layers of the same plane
        for(Layer *l : layers){
            if(l->GetParallaxLayerMode()==ParallaxLayerMode::NO_CHANGE || l->GetParallaxLayerMode()==ParallaxLayerMode::TILING_X || l->GetParallaxLayerMode()==ParallaxLayerMode::TILING_Y || l->GetParallaxLayerMode()==ParallaxLayerMode::TILING_XY){
                std::pair<std::map<std::string, std::vector<Layer*>>::iterator, bool> p = layersPlanes.insert(std::pair<std::string, std::vector<Layer*>>(l->linkedLayerName, std::vector<Layer*>()));
                it = p.first;
                std::vector<Layer*> &planeLayers = it->second;
                planeLayers.push_back(l);
            }
        }

    }else if(type==MapType::SCREEN){

        //--- group layers of the same plane
        for(Layer *l : layers){
            if(l->GetScreenLayerMode()==ScreenLayerMode::NO_CHANGE){
                std::pair<std::map<std::string, std::vector<Layer*>>::iterator, bool> p = layersPlanes.insert(std::pair<std::string, std::vector<Layer*>>(l->linkedLayerName, std::vector<Layer*>()));
                it = p.first;
                std::vector<Layer*> &planeLayers = it->second;
                planeLayers.push_back(l);
            }
        }
    }



    //----- normalize layers
    for(it=layersPlanes.begin(); it!=layersPlanes.end(); it++){

        std::vector<Layer*> &planeLayers = it->second;

        //--- obtain plane bounding box
        Rectf planeBoundingBox(999999.0, 999999.0, -999999.0, -999999.0);

        for(Layer *l : planeLayers){
            if(l->GetKind()==LayerKind::SPRITE){
                SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                planeBoundingBox = planeBoundingBox.Unite(sl->boundingBox);
            }
        }

        //--- normalize layers relative to plane bounding box
        for(Layer *l : planeLayers){

            if(l->GetKind()==LayerKind::SPRITE){
            //----------------------------------------------------
                SpriteLayer * sl = static_cast<SpriteLayer*>(l);

                // normalize sprite positions
                for(Sprite *s : sl->GetSprites()){
                    Vec2f pos = s->GetPosition();
                    //pos.x -= planeBoundingBox.min.x;
                    //pos.y -= planeBoundingBox.min.y;
                    pos -= planeBoundingBox.min;
                    s->SetPosition(pos);
                }

                //sl->boundingBox.min =  sl->boundingBox.min - planeBoundingBox.min;
                //sl->boundingBox.max = sl->boundingBox.max - planeBoundingBox.min;

                sl->boundingBox.min -=  planeBoundingBox.min;
                sl->boundingBox.max -=  planeBoundingBox.min;


            }else if(l->GetKind()==LayerKind::VECTOR){
            //----------------------------------------------------
                VectorLayer * vl = static_cast<VectorLayer*>(l);

                for(VectorShape *vs : vl->vectorShapes){
                    MoveGeometricShape(vs->GetGeometricShape(), -planeBoundingBox.min);
                }
            }

            //---
            l->layersPlaneSize = Vec2f(planeBoundingBox.Width(),planeBoundingBox.Height());
        }


        /*
        //--- vector layers
        std::string attachToLayer = planeLayers[0]->attachToLayer;
        for(VectorLayer *vl : vectorLayers){
            if(vl->attachToLayer==attachToLayer){

                vl->parallaxLayerMode = planeLayers[0]->parallaxLayerMode;
                vl->parallaxFactor = planeLayers[0]->parallaxFactor;
                vl->screenLayerMode = planeLayers[0]->screenLayerMode;
                vl->alignX = planeLayers[0]->alignX;
                vl->alignY = planeLayers[0]->alignY;
                vl->alignOffset = planeLayers[0]->alignOffset;

                for(VectorShape *vs : vl->vectorShapes){
                    MoveGeometricShape(vs->GetGeometricShape(), -planeBoundingBox.min);
                }
                vl->layersPlaneSize = Vec2f(planeBoundingBox.Width(),planeBoundingBox.Height());
            }
        }
        */

        //for(ParallaxLayersPlane &plp : parallaxLayersPlanes){
        //    if(plp.attachToLayer==attachToLayer){
        //        plp.layersPlaneSize = Vec2f(planeBoundingBox.Width(),planeBoundingBox.Height());
        //        break;
        //    }
        //}

    }
}


void Map::InitWorldMap()
{

    InitMapParameters();

    //assert(type==Type::WORLD);
    type = MapType::WORLD;

    // Normalize layer sprites and obtain worldMapSize.
    // When size of the map is defined solely by tiles then worldMapSize = tilesCount * tileSize.
    // For other situations we need to obtain it from layers bounding boxes.

    DbgOutput("InitWorldMap");
    boundingBox = Rectf(999999.0, 999999.0, -999999.0, -999999.0);
    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE ){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            boundingBox = boundingBox.Unite(sl->boundingBox);

            DbgOutput("layer:"+ l->name + " BB min x:" + std::to_string(sl->boundingBox.min.x) + " y:" + std::to_string(sl->boundingBox.min.y)
                                   + " BB max x:" + std::to_string(sl->boundingBox.max.x) + " y:" + std::to_string(sl->boundingBox.max.y));
        }
    }

    DbgOutput("map:"+ name + " BB min x:" + std::to_string(boundingBox.min.x) + " y:" + std::to_string(boundingBox.min.y)
                           + " BB max x:" + std::to_string(boundingBox.max.x) + " y:" + std::to_string(boundingBox.max.y));

    DbgOutput(" ");

    UpdateBoundingBox();

    //---
    for(Layer *l : layers){

        if(l->GetKind()==LayerKind::SPRITE){
        //----------------------------------------------------
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);

            // normalize sprite positions
            for(Sprite *s : sl->GetSprites()){
                Vec2f pos = s->GetPosition();
                //pos.x -= boundingBox.min.x;
                //pos.y -= boundingBox.min.y;
                pos -= boundingBox.min;
                s->SetPosition(pos);
            }

            //sl->boundingBox.min =  sl->boundingBox.min - boundingBox.min;
            //sl->boundingBox.max = sl->boundingBox.max - boundingBox.min;

            sl->boundingBox.min -=  boundingBox.min;
            sl->boundingBox.max -=  boundingBox.min;

        }else if(l->GetKind()==LayerKind::VECTOR){
        //----------------------------------------------------
            VectorLayer * vl = static_cast<VectorLayer*>(l);

            for(VectorShape *vs : vl->vectorShapes){
                MoveGeometricShape(vs->GetGeometricShape(), -boundingBox.min);
            }
        }
    }


    worldMapSize = Vec2i(boundingBox.Width(), boundingBox.Height());
    boundingBox = Rectf(0,0, worldMapSize.x, worldMapSize.y);
}


void Map::InitParallaxMap(Vec2i _worldMapSize)
{

    type = MapType::PARALLAX;

    InitMapParameters();
    UpdateBoundingBox();
    SetLayersPlanes();

    //----
    worldMapSize = _worldMapSize;


    // Parallax layers must be visible at any world position.
    // If the size from the editor is not big enough we duplicate and offset sprites until we get the needed size.

    for(Layer *l : layers){

        if(l->parallaxLayerMode==ParallaxLayerMode::TILING_X || l->parallaxLayerMode==ParallaxLayerMode::TILING_XY){

            float widthRequired = worldMapSize.x * l->parallaxFactor.x;
            // There are situations depending of the view position and zoom factor that require bigger 'widthRequired'!
            // To not complicate lets make it as big as map size!
            if(widthRequired<worldMapSize.x){
                widthRequired = worldMapSize.x;
            }

            if(l->layersPlaneSize.x < widthRequired){

                if(l->layersPlaneSize.x < 1){       //safety
                    l->layersPlaneSize.x = 100;
                    assert(false);
                }

                float widthStep = l->layersPlaneSize.x;
                int i = 1;

                if(l->GetKind()==LayerKind::SPRITE){
                //----------------------------------------------------
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    std::vector<Sprite*>OrigSprites = sl->GetSprites();

                    i = 1;
                    while(l->layersPlaneSize.x<widthRequired){

                        for(Sprite *s : OrigSprites){
                            Vec2f pos = s->GetPosition();
                            pos.x += widthStep*i;
                            Sprite *sprCopy = s->MakePassiveCopy();
                            sprCopy->GetLayer()->AddSprite(sprCopy);        // Passive copies are not inserted into layer at creation!
                            sprCopy->SetPosition(pos);
                        }
                        l->layersPlaneSize.x += widthStep;
                        sl->boundingBox.max.x += widthStep;
                        i++;
                    }

                }else if(l->GetKind()==LayerKind::VECTOR){
                //----------------------------------------------------
                    VectorLayer * vl = static_cast<VectorLayer*>(l);
                    std::vector<VectorShape*>OrigShapes = vl->GetVectorShapes();

                    i = 1;
                    while(vl->layersPlaneSize.x<widthRequired){

                        for(VectorShape *vs : OrigShapes){
                            VectorShape *vsCopy = new VectorShape(*vs);
                            vl->AddVectorShape(vsCopy);
                            MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(widthStep*i, 0.0));

                        }
                        vl->layersPlaneSize.x += widthStep;
                        //vl->boundingBox.max.x += widthStep;
                        i++;
                    }
                }
            }
        }

        if(l->parallaxLayerMode==ParallaxLayerMode::TILING_Y || l->parallaxLayerMode==ParallaxLayerMode::TILING_XY){

            float heightRequired = worldMapSize.y * l->parallaxFactor.y;
            if(heightRequired<worldMapSize.y){
                heightRequired = worldMapSize.y;
            }

            if(l->layersPlaneSize.y < heightRequired){

                if(l->layersPlaneSize.y < 1){       //safety
                    l->layersPlaneSize.y = 100;
                    assert(false);
                }

                float heightStep = l->layersPlaneSize.y;
                int i = 1;

                if(l->GetKind()==LayerKind::SPRITE){
                //----------------------------------------------------
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    std::vector<Sprite*>OrigSprites = sl->GetSprites();

                    i = 1;
                    while(l->layersPlaneSize.y<heightRequired){

                        for(Sprite *s : OrigSprites){
                            Vec2f pos = s->GetPosition();
                            pos.y += heightStep*i;
                            Sprite *sprCopy = s->MakePassiveCopy();
                            sprCopy->GetLayer()->AddSprite(sprCopy);
                            sprCopy->SetPosition(pos);
                        }
                        l->layersPlaneSize.y += heightStep;
                        sl->boundingBox.max.y += heightStep;
                        i++;
                    }

                }else if(l->GetKind()==LayerKind::VECTOR){
                //----------------------------------------------------
                    VectorLayer * vl = static_cast<VectorLayer*>(l);
                    std::vector<VectorShape*>OrigShapes = vl->GetVectorShapes();

                    i = 1;
                    while(vl->layersPlaneSize.y<heightRequired){

                        for(VectorShape *vs : OrigShapes){
                            VectorShape *vsCopy = new VectorShape(*vs);
                            vl->AddVectorShape(vsCopy);
                            MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(0.0, heightStep*i));
                        }
                        vl->layersPlaneSize.y += heightStep;
                        //vl->boundingBox.max.y += heightStep;
                        i++;
                    }
                }
            }
        }
    }
}


void Map::InitScreenMap(Vec2i _screenMapDesignSize)
{

    type = MapType::SCREEN;

    InitMapParameters();
    UpdateBoundingBox();
    SetLayersPlanes();

    screenMapDesignSize = _screenMapDesignSize;

    //--- align layer sprites
    for(Layer *l : layers){
        if(l->GetScreenLayerMode()==ScreenLayerMode::NO_CHANGE){

            Vec2f alignPos;

            if(l->alignX==AlignX::LEFT){
                alignPos.x = 0.0;

            }else if(l->alignX==AlignX::MIDDLE){
                alignPos.x = (screenMapDesignSize.x - l->layersPlaneSize.x)/2.0;

            }else if(l->alignX==AlignX::RIGHT){
                alignPos.x = screenMapDesignSize.x - l->layersPlaneSize.x;
            }

            if(l->alignY==AlignY::BOTTOM){
                if(settings.IsYCoordinateUp()){
                    alignPos.y = 0.0;
                }else{
                    alignPos.y = screenMapDesignSize.y - l->layersPlaneSize.y;
                }

            }else if(l->alignY==AlignY::MIDDLE){
                if(settings.IsYCoordinateUp()){
                    alignPos.y = (screenMapDesignSize.y - l->layersPlaneSize.y)/2.0;
                }else{
                    alignPos.y = (screenMapDesignSize.y - l->layersPlaneSize.y)/2.0;
                }


            }else if(l->alignY==AlignY::TOP){
                if(settings.IsYCoordinateUp()){
                    alignPos.y = screenMapDesignSize.y - l->layersPlaneSize.y;
                }else{
                    alignPos.y = 0.0;
                }
            }

            alignPos = alignPos + l->alignOffset;



            if(l->GetKind()==LayerKind::SPRITE){
            //----------------------------------------------------
                SpriteLayer * sl = static_cast<SpriteLayer*>(l);

                for(Sprite *s : sl->GetSprites()){
                    Vec2f pos = s->GetPosition();
                    //pos = pos + alignPos;
                    pos += alignPos;
                    s->SetPosition(pos);
                }
                //sl->boundingBox.min =  sl->boundingBox.min + alignPos;
                //sl->boundingBox.max = sl->boundingBox.max + alignPos;

                sl->boundingBox.min += alignPos;
                sl->boundingBox.max += alignPos;


            }else if(l->GetKind()==LayerKind::VECTOR){
            //----------------------------------------------------
                VectorLayer * vl = static_cast<VectorLayer*>(l);

                for(VectorShape *vs : vl->vectorShapes){
                    MoveGeometricShape(vs->GetGeometricShape(), alignPos);
                }

            }



        }else{

            // empty - we handle stretch modes uring layer update

        }
    }

}


void Map::InitEngineMap()
{
    for(Layer *l : layers){
        l->InitEngineLayer();
    }
}


void Map::UpdateEngineMap()
{
    for(Layer *l : layers){
        l->UpdateEngineLayer();
    }
}


void Map::DrawEngineMap()
{
    for(Layer *l : layers){
        l->DrawEngineLayer();
    }
}


void Map::_AddLayer(Layer *_layer, int _index)
{
    if(_index<0 || _index>=layers.size()){
        layers.push_back(_layer);
    }else{
        layers.insert(layers.begin()+_index, _layer);
    }

    _layer->map = this;
}


void Map::CaptureForLerpDrawing()
{

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            static_cast<SpriteLayer*>(l)->CaptureForLerpDrawing();
        }
    }
}


int Map::GetNextZOrder(int _zOrderStep)
{

    int zOrder = zOrderStart;

    for(Layer* l : layers){
        if(l->GetKind()==LayerKind::VECTOR) continue;
        zOrder = l->GetZOrder()  +  _zOrderStep;
    }

    return zOrder;

}




}

