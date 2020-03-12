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





/*
void Map::SetAlignOffsetsFromMap()
{

    for(Layer *l : layers){

        if(l->linkedLayerName.empty() || l->name==l->linkedLayerName){      //--- layers which represents parallax planes

            if(l->alignOffset_obtainFromMap.x){

                //if(l->alignX==AlignX::LEFT){
                if(l->alignPosition.x==0){
                    l->alignOffset.x = l->boundingBox.min.x - boundingBox.min.x;

                //}else if(l->alignX==AlignX::RIGHT){
                }else if(l->alignPosition.x==100){
                    l->alignOffset.x = l->boundingBox.max.x - boundingBox.max.x;
                }
            }

            if(l->alignOffset_obtainFromMap.y){

                float yAlignTopOffset = l->boundingBox.min.y - boundingBox.min.y;
                float yAlignBottomOffset = l->boundingBox.max.y - boundingBox.max.y;
                if(settings.IsYCoordinateUp()){
                    yAlignTopOffset = l->boundingBox.max.y - boundingBox.max.y;
                    yAlignBottomOffset = l->boundingBox.min.y - boundingBox.min.y;
                }

                //if(l->alignY==AlignY::TOP){
                if(l->alignPosition.y==0){
                    l->alignOffset.y = yAlignTopOffset;

                //}else if(l->alignY==AlignY::BOTTOM){
                }else if(l->alignPosition.y==100){
                    l->alignOffset.y = yAlignBottomOffset;
                }

            }
        }
    }

}
*/


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
    worldMapSize = _worldMapSize;


    InitMapParameters();
    UpdateBoundingBox();
    Normalize();
    SetLayersPlanes();

    //----



    // Parallax layers must be visible at any world position.
    // If the size from the editor is not big enough we duplicate and offset sprites until we get the needed size.
    //TileLayers();

}


void Map::InitScreenMap(Vec2i _screenMapDesignSize)
{

    type = MapType::SCREEN;
    screenMapDesignSize = _screenMapDesignSize;


    InitMapParameters();
    UpdateBoundingBox();
    SetLayersPlanes();


    //--- align layer sprites
    for(Layer *l : layers){
        //if(l->GetScreenLayerMode()==ScreenLayerMode::STANDARD){
        if(l->GetLayerType()==LayerType::SCREEN){

            float xAlignLEFT = 0.0;
            float xAlignRIGHT = screenMapDesignSize.x - l->layersPlaneSize.x;
            float yAlignTOP = 0.0;
            float yAlignBOTTOM = screenMapDesignSize.y - l->layersPlaneSize.y;

            if(settings.IsYCoordinateUp()){
                yAlignTOP = screenMapDesignSize.y - l->layersPlaneSize.y;
                yAlignBOTTOM = 0.0;
            }


            Vec2f alignPos;
            alignPos.x = xAlignLEFT * (1.0 - l->alignPosition.x/100.0) +  xAlignRIGHT * (l->alignPosition.x/100.0);
            alignPos.y = yAlignTOP * (1.0 - l->alignPosition.y/100.0) +  yAlignBOTTOM * (l->alignPosition.y/100.0);


            /*
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
            */


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


void Map::Normalize()
{

    if(boundingBox.min.Equals(Vec2f(0.0, 0.0))) return;


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

    boundingBox = Rectf(0,0, boundingBox.Width(), boundingBox.Height());
}


void Map::SetLayersPlanes()
{

    // Parallax and screen maps can have more layers which visually form one layers plane. Layers which form a layers plane must have the same
    // parallax (or screen) parameters! The layers planes are independent from each other - in the code bellow we obtain sizes for layers planes and
    // normalize the layers relative to their plane.


    //--- layers of the same plane are obtained via 'attachToLayer' name and share the same parameters
    for(Layer *l : layers){
        if(type==MapType::PARALLAX && l->GetLayerType()!=LayerType::PARALLAX) continue;
        if(type==MapType::SCREEN && l->GetLayerType()!=LayerType::SCREEN) continue;


        if(l->attachToLayer.empty()){
            l->attachToLayer = l->name;

        }else{
            Layer *aLayer = FindLayerWithName(this, l->attachToLayer);
            if(aLayer){
                l->parallaxFactor = aLayer->parallaxFactor;
                l->layerType = aLayer->layerType;
                l->alignPosition = aLayer->alignPosition;
                l->alignOffset = aLayer->alignOffset;
                l->alignOffset_obtainFromMap = aLayer->alignOffset_obtainFromMap;
                l->tilingCount = aLayer->tilingCount;
                l->tilingCountAutoSpread = aLayer->tilingCountAutoSpread;
                l->tilingSpacing = aLayer->tilingSpacing;
                l->tilingSpacingDelta = aLayer->tilingSpacingDelta;
            }
        }
    }


    //----- gather layers belonging to same plane (sharing the same 'linkedLayerName')
    std::map<std::string, std::vector<Layer*>>layersPlanes;
    std::map<std::string, std::vector<Layer*>>::iterator it;

    for(Layer *l : layers){
        if(type==MapType::PARALLAX && l->GetLayerType()!=LayerType::PARALLAX) continue;
        if(type==MapType::SCREEN && l->GetLayerType()!=LayerType::SCREEN) continue;

        std::pair<std::map<std::string, std::vector<Layer*>>::iterator, bool> p = layersPlanes.insert(std::pair<std::string, std::vector<Layer*>>(l->attachToLayer, std::vector<Layer*>()));
        it = p.first;
        std::vector<Layer*> &planeLayers = it->second;
        planeLayers.push_back(l);
    }


    //-----
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

        //--- get align offsets from map if needed
        for(Layer *l : planeLayers){
            if(l->alignOffset_obtainFromMap.x){

                if(l->alignPosition.x==0){      // left
                    l->alignOffset.x = planeBoundingBox.min.x - boundingBox.min.x;

                }else if(l->alignPosition.x==100){      // right
                    l->alignOffset.x = planeBoundingBox.max.x - boundingBox.max.x;
                }
            }

            if(l->alignOffset_obtainFromMap.y){

                float yAlignTopOffset = planeBoundingBox.min.y - boundingBox.min.y;
                float yAlignBottomOffset = planeBoundingBox.max.y - boundingBox.max.y;

                if(settings.IsYCoordinateUp()){
                    yAlignTopOffset = planeBoundingBox.max.y - boundingBox.max.y;
                    yAlignBottomOffset = planeBoundingBox.min.y - boundingBox.min.y;
                }

                if(l->alignPosition.y==0){      // top
                    l->alignOffset.y = yAlignTopOffset;

                }else if(l->alignPosition.y==100){  // bottom
                    l->alignOffset.y = yAlignBottomOffset;
                }
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
                    pos -= planeBoundingBox.min;
                    s->SetPosition(pos);
                }
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

            // some safety stuff which should not happen
            if(l->layersPlaneSize.x < 1){
                l->layersPlaneSize.x = 50;
                assert(false);
            }
            if(l->layersPlaneSize.y < 1){
                l->layersPlaneSize.y = 50;
                assert(false);
            }
        }


        //--- tile layers
        Vec2f layersPlaneSize = planeLayers.front()->layersPlaneSize;
        LayerType layerType = planeLayers.front()->layerType;
        Vec2i tilingCount = planeLayers.front()->tilingCount;
        Vec2i tilingCountAutoSpread = planeLayers.front()->tilingCountAutoSpread;
        Vec2i tilingSpacing = planeLayers.front()->tilingSpacing;
        Vec2i tilingSpacingDelta = planeLayers.front()->tilingSpacingDelta;
        Vec2f parallaxFactor = planeLayers.front()->parallaxFactor;

        std::vector<std::vector<Sprite*>>origSprites(planeLayers.size());
        std::vector<std::vector<VectorShape*>>origShapes(planeLayers.size());

        if(tilingCount.x>1 || tilingCountAutoSpread.x==1){

            float widthMax = 0;
            if(layerType==LayerType::PARALLAX){
                widthMax = worldMapSize.x * parallaxFactor.x;

                // There are situations depending of the view position and zoom factor that require bigger 'widthRequired'!
                // To not complicate lets make it as big as map size!
                if(widthMax<worldMapSize.x){
                    widthMax = worldMapSize.x;
                }

            }else if(layerType==LayerType::SCREEN){
                widthMax = screenMapDesignSize.x;
            }


            float xStepDef = layersPlaneSize.x;
            float xStepCumulative = 0.0;
            int count = 1;

            while(layersPlaneSize.x<widthMax){

                float xStepVar = iRand(-tilingSpacingDelta.x/2.0, tilingSpacingDelta.x/2.0);
                float xStep = xStepDef + tilingSpacing.x + xStepVar;
                if(xStep < xStepDef/2.0){
                    xStep = xStepDef/2.0;
                }
                if(xStep < 10){
                    xStep = 10;
                }

                for(int j=0; j<planeLayers.size(); j++){
                    Layer *l = planeLayers[j];

                    if(l->GetKind()==LayerKind::SPRITE){
                    //----------------------------------------------------
                        SpriteLayer * sl = static_cast<SpriteLayer*>(l);

                        if(origSprites[j].empty()){
                            origSprites[j] = sl->GetSprites();
                        }

                        //----
                        for(Sprite *s : origSprites[j] ){
                            Vec2f pos = s->GetPosition();
                            //pos.x += xStep*i;
                            pos.x += xStepCumulative + xStep;
                            Sprite *sprCopy = s->MakePassiveCopy();
                            sprCopy->GetLayer()->AddSprite(sprCopy);        // Passive copies are not inserted into layer at creation!
                            sprCopy->SetPosition(pos);
                        }
                        l->layersPlaneSize.x += xStep;
                        l->boundingBox.max.x += xStep;


                    }else if(l->GetKind()==LayerKind::VECTOR){
                    //----------------------------------------------------
                        VectorLayer * vl = static_cast<VectorLayer*>(l);

                        if(origShapes[j].empty()){
                            origShapes[j] = vl->GetVectorShapes();
                        }

                        //----
                        for(VectorShape *vs : origShapes[j]){
                            VectorShape *vsCopy = new VectorShape(*vs);
                            vl->AddVectorShape(vsCopy);
                            //MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(xStep*i, 0.0));
                            MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(xStepCumulative + xStep, 0.0));

                        }
                        vl->layersPlaneSize.x += xStep;
                        vl->boundingBox.max.x += xStep;
                    }
                }

                layersPlaneSize.x += xStep;
                xStepCumulative += xStep;

                count++;
                if(tilingCountAutoSpread.x==0 && tilingCount.x==count){
                    break;
                }
            }

        }

        //---- clear origSprites lists before tiling in y direction
        for(int j=0; j<planeLayers.size(); j++){
            origSprites[j].clear();
            origShapes[j].clear();
        }


        //----
        if(tilingCount.y>1 || tilingCountAutoSpread.y==1){

            float heightMax = 0;
            if(layerType==LayerType::PARALLAX){
                heightMax = worldMapSize.y * parallaxFactor.y;

                // There are situations depending of the view position and zoom factor that require bigger 'widthRequired'!
                // To not complicate lets make it as big as map size!
                if(heightMax<worldMapSize.y){
                    heightMax = worldMapSize.y;
                }

            }else if(layerType==LayerType::SCREEN){
                heightMax = screenMapDesignSize.y;
            }


            float yStepDef = layersPlaneSize.y;
            float yStepCumulative = 0.0;
            int count = 1;

            while(layersPlaneSize.y<heightMax){

                float yStepVar = iRand(-tilingSpacingDelta.y/2.0, tilingSpacingDelta.y/2.0);
                float yStep = yStepDef + tilingSpacing.y + yStepVar;
                if(yStep < yStepDef/2.0){
                    yStep = yStepDef/2.0;
                }
                if(yStep < 10){
                    yStep = 10;
                }

                for(int j=0; j<planeLayers.size(); j++){
                    Layer *l = planeLayers[j];

                    if(l->GetKind()==LayerKind::SPRITE){
                    //----------------------------------------------------
                        SpriteLayer * sl = static_cast<SpriteLayer*>(l);

                        if(origSprites[j].empty()){
                            origSprites[j] = sl->GetSprites();
                        }

                        //----
                        for(Sprite *s : origSprites[j] ){
                            Vec2f pos = s->GetPosition();
                            //pos.y += xStep*i;
                            pos.y += yStepCumulative + yStep;
                            Sprite *sprCopy = s->MakePassiveCopy();
                            sprCopy->GetLayer()->AddSprite(sprCopy);        // Passive copies are not inserted into layer at creation!
                            sprCopy->SetPosition(pos);
                        }
                        l->layersPlaneSize.y += yStep;
                        l->boundingBox.max.y += yStep;


                    }else if(l->GetKind()==LayerKind::VECTOR){
                    //----------------------------------------------------
                        VectorLayer * vl = static_cast<VectorLayer*>(l);

                        if(origShapes[j].empty()){
                            origShapes[j] = vl->GetVectorShapes();
                        }

                        //----
                        for(VectorShape *vs : origShapes[j]){
                            VectorShape *vsCopy = new VectorShape(*vs);
                            vl->AddVectorShape(vsCopy);
                            //MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(xStep*i, 0.0));
                            MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(yStepCumulative + yStep, 0.0));

                        }
                        vl->layersPlaneSize.y += yStep;
                        vl->boundingBox.max.y += yStep;
                    }
                }

                layersPlaneSize.y += yStep;
                yStepCumulative += yStep;

                count++;
                if(tilingCountAutoSpread.y==0 && tilingCount.y==count){
                    break;
                }
            }

        }



    }
}


/*
void Map::TileLayers()
{

    for(Layer *l : layers){
        if(type==MapType::PARALLAX && l->GetLayerType()!=LayerType::PARALLAX) continue;
        if(type==MapType::SCREEN && l->GetLayerType()!=LayerType::SCREEN) continue;


        if(l->tilingCount.x==1){


            float widthRequired = 0;

            if(l->GetLayerType()==LayerType::PARALLAX){
                widthRequired = worldMapSize.x * l->parallaxFactor.x;

                // There are situations depending of the view position and zoom factor that require bigger 'widthRequired'!
                // To not complicate lets make it as big as map size!
                if(widthRequired<worldMapSize.x){
                    widthRequired = worldMapSize.x;
                }

            }else if(l->GetLayerType()==LayerType::SCREEN){
                widthRequired = screenMapDesignSize.x;
            }



            if(l->layersPlaneSize.x < widthRequired){

                if(l->layersPlaneSize.x < 1){       //safety
                    l->layersPlaneSize.x = 100;
                    assert(false);
                }

                float xStepDef = l->layersPlaneSize.x;
                float xStepCumulative = 0.0;
                int i = 1;

                if(l->GetKind()==LayerKind::SPRITE){
                //----------------------------------------------------
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    std::vector<Sprite*>OrigSprites = sl->GetSprites();

                    i = 1;
                    while(l->layersPlaneSize.x<widthRequired){

                        float xStepVar = iRand(-l->tilingSpacingDelta.x/2.0, l->tilingSpacingDelta.x/2.0);
                        float xStep = xStepDef + l->tilingSpacing.x + xStepVar;
                        if(xStep < xStepDef/2.0){
                            xStep = xStepDef/2.0;
                        }
                        if(xStep < 10){
                            xStep = 10;
                        }

                        //----
                        for(Sprite *s : OrigSprites){
                            Vec2f pos = s->GetPosition();
                            //pos.x += xStep*i;
                            pos.x += xStepCumulative + xStep;
                            Sprite *sprCopy = s->MakePassiveCopy();
                            sprCopy->GetLayer()->AddSprite(sprCopy);        // Passive copies are not inserted into layer at creation!
                            sprCopy->SetPosition(pos);
                        }
                        l->layersPlaneSize.x += xStep;
                        sl->boundingBox.max.x += xStep;
                        xStepCumulative += xStep;
                        i++;

                        if(l->tilingCountAutoSpread.x==1 && l->tilingCountNumber.x==i){
                            break;
                        }
                    }

                }else if(l->GetKind()==LayerKind::VECTOR){
                //----------------------------------------------------
                    VectorLayer * vl = static_cast<VectorLayer*>(l);
                    std::vector<VectorShape*>OrigShapes = vl->GetVectorShapes();

                    i = 1;
                    while(vl->layersPlaneSize.x<widthRequired){

                        float xStepVar = iRand(-l->tilingSpacingDelta.x/2.0, l->tilingSpacingDelta.x/2.0);
                        float xStep = xStepDef + l->tilingSpacing.x + xStepVar;
                        if(xStep < xStepDef/2.0){
                            xStep = xStepDef/2.0;
                        }
                        if(xStep < 10){
                            xStep = 10;
                        }

                        //----
                        for(VectorShape *vs : OrigShapes){
                            VectorShape *vsCopy = new VectorShape(*vs);
                            vl->AddVectorShape(vsCopy);
                            //MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(xStep*i, 0.0));
                            MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(xStepCumulative + xStep, 0.0));

                        }
                        vl->layersPlaneSize.x += xStep;
                        vl->boundingBox.max.x += xStep;
                        xStepCumulative += xStep;
                        i++;

                        if(l->tilingCountAutoSpread.x==1 && l->tilingCountNumber.x==i){
                            break;
                        }
                    }
                }
            }

        }


        if(l->tilingCount.y==1){

            float heightRequired =0;

            if(l->GetLayerType()==LayerType::PARALLAX){
                heightRequired = worldMapSize.y * l->parallaxFactor.y;

                // There are situations depending of the view position and zoom factor that require bigger 'widthRequired'!
                // To not complicate lets make it as big as map size!
                if(heightRequired<worldMapSize.y){
                    heightRequired = worldMapSize.y;
                }

            }else if(l->GetLayerType()==LayerType::SCREEN){
                heightRequired = screenMapDesignSize.y;
            }


            if(l->layersPlaneSize.y < heightRequired){

                if(l->layersPlaneSize.y < 1){       //safety
                    l->layersPlaneSize.y = 100;
                    assert(false);
                }


                float yStepDef = l->layersPlaneSize.y;
                float yStepCumulative = 0.0;
                int i = 1;

                if(l->GetKind()==LayerKind::SPRITE){
                //----------------------------------------------------
                    SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                    std::vector<Sprite*>OrigSprites = sl->GetSprites();

                    i = 1;
                    while(l->layersPlaneSize.y<heightRequired){

                        float yStepVar = iRand(-l->tilingSpacingDelta.y/2.0, l->tilingSpacingDelta.y/2.0);
                        float yStep = yStepDef + l->tilingSpacing.y + yStepVar;
                        if(yStep < yStepDef/2.0){
                            yStep = yStepDef/2.0;
                        }
                        if(yStep < 10){
                            yStep = 10;
                        }

                        for(Sprite *s : OrigSprites){
                            Vec2f pos = s->GetPosition();
                            //pos.y += yStep*i;
                            pos.x += yStepCumulative + yStep;
                            Sprite *sprCopy = s->MakePassiveCopy();
                            sprCopy->GetLayer()->AddSprite(sprCopy);
                            sprCopy->SetPosition(pos);
                        }
                        l->layersPlaneSize.y += yStep;
                        sl->boundingBox.max.y += yStep;
                        yStepCumulative += yStep;
                        i++;

                        if(l->tilingCountAutoSpread.y==1 && l->tilingCountNumber.y==i){
                            break;
                        }
                    }

                }else if(l->GetKind()==LayerKind::VECTOR){
                //----------------------------------------------------
                    VectorLayer * vl = static_cast<VectorLayer*>(l);
                    std::vector<VectorShape*>OrigShapes = vl->GetVectorShapes();

                    i = 1;
                    while(vl->layersPlaneSize.y<heightRequired){

                        float yStepVar = iRand(-l->tilingSpacingDelta.y/2.0, l->tilingSpacingDelta.y/2.0);
                        float yStep = yStepDef + l->tilingSpacing.y + yStepVar;
                        if(yStep < yStepDef/2.0){
                            yStep = yStepDef/2.0;
                        }
                        if(yStep < 10){
                            yStep = 10;
                        }

                        for(VectorShape *vs : OrigShapes){
                            VectorShape *vsCopy = new VectorShape(*vs);
                            vl->AddVectorShape(vsCopy);
                            //MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(0.0, yStep*i));
                            MoveGeometricShape(vsCopy->GetGeometricShape(), Vec2f(0.0, yStepCumulative+yStep));
                        }
                        vl->layersPlaneSize.y += yStep;
                        vl->boundingBox.max.y += yStep;
                        yStepCumulative += yStep;
                        i++;

                        if(l->tilingCountAutoSpread.y==1 && l->tilingCountNumber.y==i){
                            break;
                        }
                    }
                }
            }
        }
    }


}
*/

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

