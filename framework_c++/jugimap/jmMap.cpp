#include <algorithm>
#include <utility>
#include <map>
#include <assert.h>
#include "jmCommon.h"
#include "jmVectorShapes.h"
#include "jmUtilities.h"
#include "jmVectorShapesUtilities.h"
#include "jmCamera.h"
#include "jmGuiWidgetsA.h"
#include "jmGuiText.h"
#include "jmGuiTextWidgets.h"
#include "jmLayers.h"
#include "jmDrawing.h"
#include "jmSprites.h"
#include "jmMap.h"


namespace jugimap {



Map::~Map()
{
    DeleteContent();
}


void Map::DeleteContent()
{

    for(GuiWidget * w : widgets){
        if(w==GuiWidget::GetInteracted()){
            GuiWidget::_SetInteractedWidget(nullptr);
        }
        delete w;
    }
    widgets.clear();


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
void Map::Init(MapType _mapType)
{


    type = _mapType;

    // set layer types according to mapType
    if(type==MapType::SCREEN){
        for(Layer *l : layers){
            if(l->GetLayerType()!=LayerType::SCREEN && l->GetLayerType()!=LayerType::SCREEN_STRETCHING_SINGLE_SPRITE){
                l->_SetLayerType(LayerType::SCREEN);
            }
        }

    }else if(type==MapType::PARALLAX){
        for(Layer *l : layers){
            if(l->GetLayerType()!=LayerType::PARALLAX && l->GetLayerType()!=LayerType::PARALLAX_STRETCHING_SINGLE_SPRITE){
                l->_SetLayerType(LayerType::PARALLAX);
            }
        }

    }else if(type==MapType::WORLD){
        for(Layer *l : layers){
            l->_SetLayerType(LayerType::WORLD);         // Not really needed for world maps but lets make it correct as loaded map has layers defined as parallax or screen layers.
        }
    }

    //----
    InitMapParameters();


    // init engine objects
    InitEngineObjects();

    UpdateBoundingBox();

    if(type==MapType::WORLD){
        Normalize();
        worldMapSize = Vec2i(boundingBox.Width(), boundingBox.Height());

    }else if(type==MapType::PARALLAX){
        Normalize();
    }


    //----
    RebuildWidgets();
    SetWidgetsToInitialState();

}
*/


void Map::InitEngineObjects(MapType _mapType)
{

    type = _mapType;
    for(Layer *l : layers){
        l->InitEngineObjects();
    }

    UpdateBoundingBox();

    //----
    RebuildWidgets();
    SetWidgetsToInitialState();

}



void Map::InitAsWorldMap()
{

    assert(type==MapType::WORLD);
    //type = MapType::WORLD;
    for(Layer *l : layers){
        l->_SetLayerType(LayerType::WORLD);         // Not really needed for world maps but lets make it correct as loaded map has layers defined as parallax or screen layers.
    }

    //----
    InitMapParameters();
    UpdateBoundingBox();
    Normalize();
    worldMapSize = Vec2i(boundingBox.GetWidth(), boundingBox.GetHeight());

}



void Map::InitAsParallaxMap(Vec2i _worldMapSize)
{

    //assert(type==MapType::PARALLAX);
    //type = MapType::PARALLAX;
    for(Layer *l : layers){
        if(l->GetLayerType()!=LayerType::PARALLAX && l->GetLayerType()!=LayerType::PARALLAX_STRETCHING_SINGLE_SPRITE){
            l->_SetLayerType(LayerType::PARALLAX);
        }
    }

    worldMapSize = _worldMapSize;

    InitMapParameters();
    UpdateBoundingBox();
    Normalize();
    SetLayersPlanes();

}


void Map::InitAsScreenMap(Vec2i _screenMapDesignSize)
{

    assert(type==MapType::SCREEN);
    //type = MapType::SCREEN;
    for(Layer *l : layers){
        if(l->GetLayerType()!=LayerType::SCREEN && l->GetLayerType()!=LayerType::SCREEN_STRETCHING_SINGLE_SPRITE){
            l->_SetLayerType(LayerType::SCREEN);
        }
    }
    screenMapDesignSize = _screenMapDesignSize;


    InitMapParameters();
    UpdateBoundingBox();
    Normalize();
    SetLayersPlanes();

    //--- align layer sprites
    for(Layer *l : layers){
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
            alignPos += l->alignOffset;


            if(l->GetKind()==LayerKind::SPRITE){
            //----------------------------------------------------
                SpriteLayer * sl = static_cast<SpriteLayer*>(l);

                for(Sprite *s : sl->GetSprites()){
                    Vec2f pos = s->GetPosition();
                    pos += alignPos;
                    s->SetPosition(pos);
                }
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

            // empty - we handle stretch modes at layer update

        }
    }

}


void Map::UpdateBoundingBox()
{

    boundingBox = Rectf(999999.0, 999999.0, -999999.0, -999999.0);

    DbgOutput("UpdateBoundingBox map:" + name);

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            sl->UpdateBoundingBox();
            boundingBox = boundingBox.Unite(sl->boundingBox);

            DbgOutput("layer:"+ l->name + " BB min x:" + std::to_string(sl->boundingBox.min.x) + " y:" + std::to_string(sl->boundingBox.min.y)
                                       + " BB max x:" + std::to_string(sl->boundingBox.max.x) + " y:" + std::to_string(sl->boundingBox.max.y));
        }

    }

    if(boundingBox.min.x > boundingBox.max.x ){
        boundingBox.min.Set(0,0);
        boundingBox.max.Set(0,0);
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

    boundingBox = Rectf(0,0, boundingBox.GetWidth(), boundingBox.GetHeight());



    boundingBox = Rectf(999999.0, 999999.0, -999999.0, -999999.0);

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            boundingBox = boundingBox.Unite(sl->boundingBox);

            //DbgOutput("layer:"+ l->name + " BB min x:" + std::to_string(sl->boundingBox.min.x) + " y:" + std::to_string(sl->boundingBox.min.y)
            //                           + " BB max x:" + std::to_string(sl->boundingBox.max.x) + " y:" + std::to_string(sl->boundingBox.max.y));
        }
    }
    DbgOutput("normalized map:"+ name + " BB min x:" + std::to_string(boundingBox.min.x) + " y:" + std::to_string(boundingBox.min.y)
                           + " BB max x:" + std::to_string(boundingBox.max.x) + " y:" + std::to_string(boundingBox.max.y));



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

        DbgOutput("Layer: "+ planeLayers.front()->GetName() + "  planeBoundingBox BB min x:" + std::to_string(planeBoundingBox.min.x) + " y:" + std::to_string(planeBoundingBox.min.y)
                               + " BB max x:" + std::to_string(planeBoundingBox.max.x) + " y:" + std::to_string(planeBoundingBox.max.y));
        DbgOutput("Layer: "+ planeLayers.front()->GetName() + "  planeBoundingBox BB width:" + std::to_string(planeBoundingBox.GetWidth()) + " height:" + std::to_string(planeBoundingBox.GetHeight()));


        if(planeBoundingBox.IsValid()==false){
            continue;
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

            DbgOutput("alignOffset layer: "+ l->GetName() + " alignOffset x:" + std::to_string(l->alignOffset.x) + " y:" + std::to_string(l->alignOffset.y));
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
                vl->boundingBox.min -=  planeBoundingBox.min;
                vl->boundingBox.max -=  planeBoundingBox.min;
            }

            DbgOutput("Layer: " + l->GetName()+ "  BB after normalize  min x:" + std::to_string(l->boundingBox.min.x) + " y:" + std::to_string(l->boundingBox.min.y)
                                   + " BB max x:" + std::to_string(l->boundingBox.max.x) + " y:" + std::to_string(l->boundingBox.max.y));


            //---
            l->layersPlaneSize = Vec2f(planeBoundingBox.GetWidth(),planeBoundingBox.GetHeight());

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
                            //Sprite *sprCopy = s->MakePassiveCopy();
                            Sprite *sprCopy = s->MakeActiveCopy();
                            //sprCopy->GetLayer()->AddSprite(sprCopy);        // Passive copies are not inserted into layer at creation!
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
                            //Sprite *sprCopy = s->MakePassiveCopy();
                            //sprCopy->GetLayer()->AddSprite(sprCopy);        // Passive copies are not inserted into layer at creation!
                            Sprite *sprCopy = s->MakeActiveCopy();
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


void Map::ModifyXParallaxFactorsForFixedMapWidth(Vec2i _designViewport, Vec2i _newViewport)
{

    if(_designViewport.x==_newViewport.x) return;
    if(worldMapSize.x <= _designViewport.x) return;

    // boundingBox is original from editor and normalized (not modified by SetLayersPlanes) !
    float fXstoredDef = (boundingBox.GetWidth() - _designViewport.x)/float(worldMapSize.x - _designViewport.x);

    float fXnew = 0;
    if(boundingBox.GetWidth()>_newViewport.x  &&  worldMapSize.x > _newViewport.x){
        fXnew = (boundingBox.GetWidth() - _newViewport.x)/float(worldMapSize.x - _newViewport.x);
    }

    for(Layer *l : layers){
        if(l->GetLayerType()!=LayerType::PARALLAX) continue;

        float delta = l->GetParallaxFactor().x/fXstoredDef;
        Vec2f f(fXnew * delta, l->GetParallaxFactor().y);
        l->_SetParallaxFactor(f);
    }

}


void Map::ModifyYParallaxFactorsForFixedMapHeight(Vec2i _designViewport, Vec2i _newViewport)
{

    if(_designViewport.y==_newViewport.y) return;
    if(worldMapSize.y <= _designViewport.y) return;

    float fYstoredDef = (boundingBox.GetHeight() - _designViewport.y)/float(worldMapSize.y - _designViewport.y);

    float fYnew = 0;
    if(boundingBox.GetHeight()>_newViewport.y  &&  worldMapSize.y > _newViewport.y){
        fYnew = (boundingBox.GetHeight() - _newViewport.y)/float(worldMapSize.y - _newViewport.y);
    }

    for(Layer *l : layers){
        if(l->GetLayerType()!=LayerType::PARALLAX) continue;

        float delta = l->GetParallaxFactor().y/fYstoredDef;
        Vec2f f(l->GetParallaxFactor().x, fYnew * delta);
        l->_SetParallaxFactor(f);
    }

}


void Map::UpdateEngineObjects()
{

    if(visible==false) return;

    for(Layer *l : layers){
        l->UpdateEngineObjects();
    }
}


void Map::DrawEngineObjects()
{

    if(visible==false) return;

    for(Layer *l : layers){
        l->DrawEngineObjects();
    }
}


SpriteLayer* Map::AddSpriteLayer(const std::string &_name)
{

    SpriteLayer *sl = objectFactory->NewSpriteLayer();
    if(type==MapType::SCREEN){
        sl->_SetLayerType(LayerType::SCREEN);
    }else if(type==MapType::PARALLAX){
        sl->_SetLayerType(LayerType::PARALLAX);
    }else if(type==MapType::WORLD){
        sl->_SetLayerType(LayerType::WORLD);
    }
    sl->name = _name;
    sl->map = this;

    int zOrder = zOrderStart;
    for(Layer* l : layers){
        if(l->GetKind()==LayerKind::VECTOR) continue;
        if(l->GetKind()==LayerKind::SPRITE){
            zOrder = static_cast<SpriteLayer*>(l)->FindLastZOrder();
        }else if(l->GetKind()==LayerKind::DRAWING){
            zOrder = static_cast<DrawingLayer*>(l)->GetZOrder();
        }
    }
    sl->zOrder = zOrder + settings.GetZOrderStep();
    layers.push_back(sl);


    sl->InitEngineObjects();

    return sl;
}


jugimap::VectorLayer* Map::AddVectorLayer(const std::string &_name)
{

    VectorLayer *vl = new VectorLayer();
    vl->name = _name;
    vl->map = this;
    layers.push_back(vl);

    vl->InitEngineObjects();

    return vl;

}


void Map::AddDrawingLayer(DrawingLayer *_drawingLayer)
{

    _drawingLayer->map = this;

    int zOrder = zOrderStart;
    for(Layer* l : layers){
        if(l->GetKind()==LayerKind::VECTOR) continue;
        if(l->GetKind()==LayerKind::SPRITE){
            zOrder = static_cast<SpriteLayer*>(l)->FindLastZOrder();
        }else if(l->GetKind()==LayerKind::DRAWING){
            zOrder = static_cast<DrawingLayer*>(l)->GetZOrder();
        }
    }
    _drawingLayer->zOrder = zOrder + settings.GetZOrderStep();
    layers.push_back(_drawingLayer);

    _drawingLayer->InitEngineObjects();

}



void Map::CaptureForLerpDrawing()
{

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            static_cast<SpriteLayer*>(l)->CaptureForLerpDrawing();
        }
    }
}



//-------------------------------------------------------------------------

void Map::RebuildWidgets()
{

    //---- delete widgets
    for(GuiWidget * w : widgets){
        if(w==GuiWidget::GetInteracted()){
            GuiWidget::_SetInteractedWidget(nullptr);
        }
        delete w;
    }
    widgets.clear();


    //----
    std::vector<Sprite*>sprites;
    CollectSpritesWithConstParameter(this, sprites, "widget");

    if(sprites.empty()==false){

        for(Sprite *s : sprites){

            std::string widget = Parameter::GetValue(s->GetSourceSprite()->GetConstantParameters(), "widget");

            if(widget=="button"){
                widgets.push_back(new GuiButton(s));

            }else if(widget=="slider" && s->GetKind()==SpriteKind::COMPOSED){
                widgets.push_back(new GuiSlider(static_cast<ComposedSprite*>(s)));

            }else if(widget=="bar"){
                widgets.push_back(new GuiBar(s));

            }else if(widget=="textInput" && s->GetKind()==SpriteKind::COMPOSED){
                widgets.push_back(new GuiTextInput(static_cast<ComposedSprite*>(s)));

            }
        }
    }


    //----
    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::VECTOR){
            VectorLayer * vl = static_cast<VectorLayer*>(l);
            for(VectorShape * vs : vl->GetVectorShapes()){
                if(Parameter::GetValue(vs->GetParameters(), "widget")=="textField"){
                    widgets.push_back(new GuiTextField(vs, this));
                }
            }
        }
    }


}



void Map::SetWidgetsToInitialState()
{

    for(GuiWidget * w : widgets){
        w->SetToInitialState();
    }
}


void Map::UpdateWidgets()
{

    if(widgetsActive==false) return;

    for(GuiWidget * w : widgets){
        w->Update();
    }

}


GuiWidget* Map::FindWidget(const std::string &_name)
{

    for(GuiWidget * w : widgets){
        if(w->GetName()==_name){
            return w;
        }
    }

    return nullptr;
}


GuiWidget* Map::FindWidget(int _tag)
{

    for(GuiWidget * w : widgets){
        if(w->GetTag()==_tag){
            return w;
        }
    }

    return nullptr;

}


}

