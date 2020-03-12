#include <chrono>
#include <assert.h>
#include "various.h"
#include "entities.h"
#include "tileGrid.h"
#include "platformerScene.h"



namespace apiTestDemo{


using namespace jugimap;



PlatformerScene::~PlatformerScene()
{

    if(backgroundMap) delete backgroundMap;
    if(worldMap) delete worldMap;
    if(guiMap) delete guiMap;
    if(infoMap) delete infoMap;

    if(worldSourceGraphics) delete worldSourceGraphics;
    if(backgroundSourceGraphics) delete backgroundSourceGraphics;
    if(guiSourceGraphics) delete guiSourceGraphics;

    if(font) delete font;
    for(Button *b : buttons){
        delete b;
    }
}



bool PlatformerScene::Init()
{


    //------------------------------------------------------------------------------
    // LOAD SOURCE GRAPHICS AND MAPS
    //------------------------------------------------------------------------------
    //std::string worldMapFile = "media/maps/worldmap_test.jmb";        // map with some composed sprite dummies for testing
    std::string worldMapFile = "media/maps/worldmap.jmb";
    std::string worldMapSourceGraphicsFile = "media/maps/cfgPlatformerWorld.sgb";
    std::string backgroundMapFile = "media/maps/background.jmb";
    std::string backgroundSourceGraphicsFile = "media/maps/cfgPlatformerBackground.sgb";
    std::string guiMapFile = "media/maps/gui.jmb";
    std::string guiMapSourceGraphicsFile = "media/maps/cfgPlatformerGui.sgb";


    //---- world map
    worldSourceGraphics = new SourceGraphics();
    if(JugiMapBinaryLoader::LoadSourceGraphics(worldMapSourceGraphicsFile, worldSourceGraphics)==false){
        return false;
    }

    worldMap = objectFactory->NewMap();
    worldMap->_SetZOrderStart(zOrderWorldMap);
    if(JugiMapBinaryLoader::LoadMap(worldMapFile, worldMap, worldSourceGraphics)==false){
        return false;
    }

    //---- background
    backgroundSourceGraphics = new SourceGraphics();
    if(JugiMapBinaryLoader::LoadSourceGraphics(backgroundSourceGraphicsFile, backgroundSourceGraphics)==false){
        return false;
    }

    backgroundMap = objectFactory->NewMap();
    backgroundMap->_SetZOrderStart(zOrderBackgroundMap);
    if(JugiMapBinaryLoader::LoadMap(backgroundMapFile, backgroundMap, backgroundSourceGraphics)==false){
        return false;
    }

    //---- gui
    guiSourceGraphics = new SourceGraphics();
    if(JugiMapBinaryLoader::LoadSourceGraphics(guiMapSourceGraphicsFile, guiSourceGraphics)==false){
        return false;
    }

    guiMap = objectFactory->NewMap();
    guiMap->_SetZOrderStart(zOrderGuiMap);
    if(JugiMapBinaryLoader::LoadMap(guiMapFile, guiMap, guiSourceGraphics)==false){
        return false;
    }


    //------------------------------------------------------------------------------
    // INITIALIZE MAPS
    //------------------------------------------------------------------------------

    // Initialize maps according to their kind - make sure to call correct Init function!
    // First initialize world map as its size is needed for parallax maps!
    worldMap->InitWorldMap();
    backgroundMap->InitParallaxMap(worldMap->GetWorldMapSize());


    // 'guiMap' is a screen map and its elements will be aligned within 'guiMapDesignSize'
    //Vec2i guiMapDesignSize(270, glob::screenSize.y);
    Vec2i guiMapDesignSize(270, settings.GetScreenSize().y);
    guiMap->InitScreenMap(guiMapDesignSize);


    // Initialize engine maps - creates engine objects (textures, sprites...) and get ready everything for action
    backgroundMap->InitEngineMap();
    worldMap->InitEngineMap();
    guiMap->InitEngineMap();


    //------------------------------------------------------------------------------
    // CREATE MAP ELEMENTS MANUALLY
    //------------------------------------------------------------------------------

    // Create drawing layers for drawing sprite shapes and vector shapes on every map.
    worldMapDrawingLayer = new ShapesDrawingLayer("worldMapDrawingLayer", worldMap);
    worldMapDrawingLayer->_SetZOrder(worldMap->GetNextZOrder(JugiMapBinaryLoader::zOrderCounterStep));    // Call this before adding the layer to the map !
    worldMap->_AddLayer(worldMapDrawingLayer);
    worldMapDrawingLayer->InitEngineLayer();

    guiMapDrawingLayer = new ShapesDrawingLayer("guiMapDrawingLayer", guiMap);
    guiMapDrawingLayer->_SetZOrder(guiMap->GetNextZOrder(JugiMapBinaryLoader::zOrderCounterStep));    // Call this before adding the layer to the map !
    guiMap->_AddLayer(guiMapDrawingLayer);
    guiMapDrawingLayer->InitEngineLayer();


    //----
    // Create a text layer for 'guiMap'
    guiMapTextLayer = objectFactory->NewTextLayer("guiMapTextLayer");
    guiMapTextLayer->_SetZOrder(guiMap->GetNextZOrder(JugiMapBinaryLoader::zOrderCounterStep));  // Call this before adding the layer to the map !
    guiMap->_AddLayer(guiMapTextLayer);
    guiMapTextLayer->InitEngineLayer();


    // Create a text layer for world map - for testing.
    //worldMapTextLayer = objectFactory->NewTextLayer("worldMapTextLayer");
    //worldMapTextLayer->_SetZOrder(worldMap->GetNextZOrder(JugiMapBinaryLoader::zOrderCounterStep)); // Call this before adding the layer to the map !
    //worldMap->_AddLayer(worldMapTextLayer);
    //worldMapTextLayer->InitEngineLayer();


    //----
    // Create 'infoMap' which will display a sprite with information about JugiMap.
    infoMap = objectFactory->NewMap();
    infoMap->_SetName("infoMap");
    infoMap->_SetType(MapType::SCREEN);
    infoMap->_SetZOrderStart(zOrderInfoMap);

    SpriteLayer *infoLayer = objectFactory->NewSpriteLayer();
    infoLayer->_SetName("infoLayer");
    infoLayer->_SetKind(LayerKind::SPRITE);
    infoLayer->_SetLayerType(LayerType::SCREEN);
    infoLayer->_SetAlignPosition(Vec2i(50,50));     // middle, middle
    infoLayer->_SetZOrder(zOrderInfoMap);
    JugiMapBinaryLoader::zOrderCounterStart = zOrderWorldMap;
    infoMap->_AddLayer(infoLayer);

    StandardSprite *infoSprite = objectFactory->NewStandardSprite();
    SourceSprite *infoSourceSprite = FindSourceSpriteByGraphicFilePath(guiSourceGraphics, jugimap::GraphicFile::pathPrefix + "media/graphics/info.png");
    assert(infoSourceSprite);
    infoSprite->_SetSourceSprite(infoSourceSprite);

    infoLayer->AddSprite(infoSprite);

    infoMap->InitScreenMap(infoSourceSprite->GetGraphicItems().front()->GetSize());     // set infoMap design size to be the same size as single sprite in it
    infoMap->InitEngineMap();


    //------------------------------------------------------------------------------
    // INITIALIZE CAMERAS
    //------------------------------------------------------------------------------
    Vec2i screenSize = settings.GetScreenSize();
    worldCamera.Init(Rectf(270,0, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    worldCameraA.Init(Rectf(270, screenSize.y/2+5, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    worldCameraB.Init(Rectf(270,0, screenSize.x, screenSize.y/2-5), worldMap->GetWorldMapSize());

    Vec2f guiMapPosition = Vec2f(0,0);
    guiCamera.Init(guiMapDesignSize, guiMapPosition);

    //--- Uncomment bellow for transformed guiMap test !
    //guiMapPosition = Vec2f(settings.GetScreenSize().x*0.75, settings.GetScreenSize().y*0.5);
    //guiCamera.Init(guiMapDesignSize, guiMapPosition, 0.5, -30);     // the angle can differ between engines (either 30 or -30)
    //guiCamera.SetMapHandle(Vec2f(guiMapDesignSize.x/2.0, guiMapDesignSize.y/2.0));


    infoCamera.Init(infoMap->GetScreenMapDesignSize(), worldCamera.GetViewport().Center());
    //infoCamera.Init(infoMap->GetScreenMapDesignSize(), Vec2f(0,0));
    infoCamera.SetMapHandle(Vec2f(infoMap->GetScreenMapDesignSize().x/2, infoMap->GetScreenMapDesignSize().y/2));
    infoCamera.SetMapPosition(Vec2f(worldCamera.GetViewport().max.x + infoMap->GetScreenMapDesignSize().x/2 + 1, worldCamera.GetViewport().Center().y));
    infoBoxAniTween.Init(worldCamera.GetViewport().max.x + infoMap->GetScreenMapDesignSize().x/2 + 1,
                         worldCamera.GetViewport().Center().x, 0.5, Easing::EASE_OUT);


    VectorShape *cameraPath = FindVectorShapeWithParameter(worldMap, "cameraPath", "");
    if(cameraPath){
        cameraController.Init(&worldCamera, cameraPath);
        cameraControllerA.Init(&worldCameraA, cameraPath);
        cameraControllerB.Init(&worldCameraB, MakeWorldCameraOverviewPath_v1(worldCameraB), true);

    }
    // Alternative camera paths
    //cameraController.Init(&worldCamera, MakeWorldCameraOverviewPath_v2(worldCamera), true);         // 'true' -> controller takes ownership of 'cameraPath' object
    //cameraControllerA.Init(&worldCameraA, MakeWorldCameraOverviewPath_v2(worldCameraA), true);
    //cameraControllerB.Init(&worldCameraB, MakeWorldCameraOverviewPath_v3(worldCameraB), true);


    // Assign cameras to maps.
    backgroundMap->SetCamera(&worldCamera);
    worldMap->SetCamera(&worldCamera);
    guiMap->SetCamera(&guiCamera);
    infoMap->SetCamera(&infoCamera);


    //---- handle double/single world camera mode
    AssignCamerasToMaps();


    //------------------------------------------------------------------------------
    // SETUP ENTITIES
    //------------------------------------------------------------------------------

    BuildCollidersTileGrid(worldMap);           // used for collision detection of entities with tile environment

    if(entities.Setup(worldMap)==false){
        return false;
    }

    SetInitialized(true);

    return true;
}


void PlatformerScene::Start()
{
    //---
    cameraController.Start();
    cameraControllerA.Start();
    cameraControllerB.Start();

    entities.Start();

    SetActive(true);
}


void PlatformerScene::Update()
{


    jugimap::time.UpdatePassedTimeMS();

    UpdateProjectedMousePositions();

    //----
    Tweens::Update();     //  we use jugimap tween objects for camera movements so we need to update them every loop


    //---- GUI
    Button *buttonClicked = nullptr;
    for(Button *b : buttons){
        if(b->Update()==Button::flagCLICKED){
            buttonClicked = b;
            break;
        }
    }

    if(buttonClicked){
        if(buttonClicked->GetTag()==ButtonTag::SHOW_COLLISION_SHAPES){
            worldMapDrawingLayer->SetDoDrawMapSpritesCollisionShapes(buttonClicked->IsChecked());
            if(testParallaxMapDrawingLayer) testParallaxMapDrawingLayer->SetDoDrawMapSpritesCollisionShapes(buttonClicked->IsChecked());
            if(guiMapDrawingLayer) guiMapDrawingLayer->SetDoDrawMapSpritesCollisionShapes(buttonClicked->IsChecked());

        }else if(buttonClicked->GetTag()==ButtonTag::SHOW_VECTOR_SHAPES){
            worldMapDrawingLayer->SetDoDrawMapVectorShapes(buttonClicked->IsChecked());
            if(testParallaxMapDrawingLayer) testParallaxMapDrawingLayer->SetDoDrawMapVectorShapes(buttonClicked->IsChecked());
            if(guiMapDrawingLayer) guiMapDrawingLayer->SetDoDrawMapVectorShapes(buttonClicked->IsChecked());

        }else if(buttonClicked->GetTag()==ButtonTag::PAUSE_CAMERA){
            cameraController.SetPaused(buttonClicked->IsChecked());
            cameraControllerA.SetPaused(buttonClicked->IsChecked());
            cameraControllerB.SetPaused(buttonClicked->IsChecked());
            //jugimap::settings.SetErrorMessage("Test runtime error message!");

        }else if(buttonClicked->GetTag()==ButtonTag::ZANY_CAMERA){
            cameraController.SetZany(buttonClicked->IsChecked());
            cameraControllerA.SetZany(buttonClicked->IsChecked());
            cameraControllerB.SetZany(buttonClicked->IsChecked());

        }else if(buttonClicked->GetTag()==ButtonTag::DOUBLE_CAMERA){
            doubleCamera = buttonClicked->IsChecked();
            AssignCamerasToMaps();

        }else if(buttonClicked->GetTag()==ButtonTag::DYNAMIC_CRYSTALS){
            dynamicCrystals = buttonClicked->IsChecked();
            SetDynamicCrystalsPhysics();

        }else if(buttonClicked->GetTag()==ButtonTag::PAUSE_SPRITES){
            entities.SetPaused(buttonClicked->IsChecked());
            SetPhysicsSimulationDisabled(buttonClicked->IsChecked());

        }else if(buttonClicked->GetTag()==ButtonTag::SPAWN_WALKERS){
            TestSpawnWalkers();

        }else if(buttonClicked->GetTag()==ButtonTag::SHOW_INFO){
            showInfoBox = buttonClicked->IsChecked();

        }
    }

    //---- entities logic
    entities.Update();
    ManageDeactivateWalkers();


    //---- camera movements
    cameraController.Update();
    cameraControllerA.Update();
    cameraControllerB.Update();

    if(entities.IsPaused()){
        DummyFunction();
    }
    //----
    UpdateTexts();

    //----
    ManageInfoBox();

    //----
    UpdateEngineObjects();
}



void PlatformerScene::UpdateEngineObjects()
{
    backgroundMap->UpdateEngineMap();
    worldMap->UpdateEngineMap();
    guiMap->UpdateEngineMap();
    infoMap->UpdateEngineMap();
}


void PlatformerScene::ManageDeactivateWalkers()
{


    for(Entity *e : entities.GetList()){
        if(e->IsActive()==false) continue;

        EWalkingFella* wf = dynamic_cast<EWalkingFella*>(e);

        if(wf){
            if(wf->GetControlledSprite()->PointInside(mouse.GetWorldPosition())){
                if(mouse.IsHit()){
                    e->Deactivate();              // Deactivate entity...
                    //entities.DeleteEntity(e);       // OR delete it
                }
               break;
            }
        }
    }

}


void PlatformerScene::TestSpawnWalkers()
{
    for(EWalkingFella &wf : EWalkingFella::templateFellas){
        wf.Spawn();
    }
}


void PlatformerScene::ManageInfoBox()
{

    if(showInfoBox==false && infoBoxState==infoHIDDEN) return;

    float x = 0.0;

    if(showInfoBox){

        if(infoBoxState==infoHIDDEN){
            x = infoBoxAniTween.GetStartValue();
            infoBoxAniTween.SetMode(Tween::Mode::NORMAL);
            infoBoxAniTween.Play();
            infoBoxState = infoSHOW_ANI;

        }else if(infoBoxState==infoSHOW_ANI){
            x = infoBoxAniTween.GetValue();
            if(infoBoxAniTween.IsIdle()){
                infoBoxState = infoSHOWN;
            }

        }else if(infoBoxState==infoHIDE_ANI){
            x = infoBoxAniTween.GetEndValue();
            infoBoxState = infoSHOWN;

        }else if(infoBoxState==infoSHOWN){
            x = infoBoxAniTween.GetEndValue();
        }

    }else{

        if(infoBoxState==infoSHOWN){
            x = infoBoxAniTween.GetEndValue();
            infoBoxAniTween.SetMode(Tween::Mode::REVERSE);
            infoBoxAniTween.Play();
            infoBoxState = infoHIDE_ANI;

        }else if(infoBoxState==infoHIDE_ANI){
            x = infoBoxAniTween.GetValue();
            if(infoBoxAniTween.IsIdle()){
                infoBoxState = infoHIDDEN;
            }

        }else if(infoBoxState==infoSHOW_ANI){
            x = infoBoxAniTween.GetStartValue();
            infoBoxState = infoHIDDEN;

        }else if(infoBoxState==infoHIDDEN){
            x = infoBoxAniTween.GetStartValue();
        }
    }

    //float x = infoBoxPositionOFF.x + p*(infoBoxPositionON.x - infoBoxPositionOFF.x);
    infoCamera.SetMapPosition(Vec2f(x, worldCamera.GetViewport().Center().y));

}


void PlatformerScene::UpdateProjectedMousePositions()
{

    if(guiCamera.GetViewport().Contains(mouse.GetScreenPosition())){
        mouse.SetGuiPosition(guiCamera.MapPointFromScreenPoint(mouse.GetScreenPosition()));
        mouse.SetWorldPosition(Vec2f(-999999,-999999));     // set that mouse is not picking any world map position
    }else{

        if(doubleCamera){
            if(worldCameraA.GetViewport().Contains(mouse.GetScreenPosition())){
                mouse.SetWorldPosition(worldCameraA.MapPointFromScreenPoint(mouse.GetScreenPosition()));

            }else if(worldCameraB.GetViewport().Contains(mouse.GetScreenPosition())){
                mouse.SetWorldPosition(worldCameraB.MapPointFromScreenPoint(mouse.GetScreenPosition()));
            }

        }else{
            if(worldCamera.GetViewport().Contains(mouse.GetScreenPosition())){
                mouse.SetWorldPosition(worldCamera.MapPointFromScreenPoint(mouse.GetScreenPosition()));
            }
        }
        mouse.SetGuiPosition(Vec2f(-999999,-999999));   // set that mouse is not picking any gui map position
    }


    /*
    std::string str = "Mouse position:  screen X:";
    str = str + std::to_string(mouse.GetScreenPosition().x) + " Y:" + std::to_string(mouse.GetScreenPosition().y);
    str += "  world X:";
    str = str + std::to_string(mouse.GetWorldPosition().x) + " Y:" + std::to_string(mouse.GetWorldPosition().y);
    str += "  gui X:";
    str = str + std::to_string(mouse.GetGuiPosition().x) + " Y:" + std::to_string(mouse.GetGuiPosition().y);
    DbgOutput(str);
    */

}


void PlatformerScene::MakeButtonsAndTexts()
{

    SpriteLayer *buttonsLayer = dynamic_cast<SpriteLayer*>(FindLayerWithName(guiMap, "buttons", LayerKind::SPRITE));
    if(buttonsLayer==nullptr) return;
    if(buttonsLayer->GetSprites().empty()) return;

    SpriteLayer *logoLayer = dynamic_cast<SpriteLayer*>(FindLayerWithName(guiMap, "logo", LayerKind::SPRITE));
    if(logoLayer==nullptr) return;
    if(logoLayer->GetSprites().empty()) return;

    // 'buttonsLayer' created in editor has only one sprite; its handle point is in top-left corner.
    // We create more sprites from that sprite - one for each button.

    StandardSprite *sprite = static_cast<StandardSprite*>(buttonsLayer->GetSprites().front());
    StandardSprite *spriteOrig = sprite;
    float buttonHeight = sprite->GetActiveImage()->GetHeight();

    Vec2f spritePos(logoLayer->GetBoundingBox().min.x, logoLayer->GetBoundingBox().max.y + 10);     // starting position is bellow the logo
    ColorRGBA color(255,255,0,255);
    float dySmall = buttonHeight + 2;
    float dyBig = dySmall + 6;

    if(settings.IsYCoordinateUp()){
        spritePos = Vec2f(logoLayer->GetBoundingBox().min.x, logoLayer->GetBoundingBox().min.y - 10);
        dySmall = -dySmall;
        dyBig = -dyBig;

    }

    Text* text = nullptr;

    // SHOW SHAPES BUTTONS
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SHOW_COLLISION_SHAPES) == skippedButtons.end()){
        sprite->SetPosition(spritePos);
        //sprite->SetColorOverlayRGBA(ColorRGBA(0,0,255,64));
        //sprite->SetColorOverlayActive(true);
        text = objectFactory->NewText(guiMapTextLayer, font, "SHOW SPRITE SHAPES", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::SHOW_COLLISION_SHAPES, sprite, text, true));
    }

    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SHOW_VECTOR_SHAPES) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dySmall;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);

        text = objectFactory->NewText(guiMapTextLayer, font, "SHOW MAP SHAPES", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::SHOW_VECTOR_SHAPES, sprite, text, true));
    }

    // CAMERA BUTTONS
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::PAUSE_CAMERA) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dyBig;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);
        text = objectFactory->NewText(guiMapTextLayer, font, "PAUSE CAMERA", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::PAUSE_CAMERA, sprite, text, true));
    }

    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::ZANY_CAMERA) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dySmall;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);
        text = objectFactory->NewText(guiMapTextLayer, font, "ZANY CAMERA", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::ZANY_CAMERA, sprite, text, true));
    }


    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::DOUBLE_CAMERA) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dySmall;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);
        text = objectFactory->NewText(guiMapTextLayer, font, "DOUBLE CAMERA", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::DOUBLE_CAMERA, sprite, text, true));
    }


    // SPRITES & ENTITIES BUTTONS
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::PAUSE_SPRITES) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dyBig;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);
        //sprite->SetColorOverlayRGBA(ColorRGBA(255,0,0,64));
        //sprite->SetColorOverlayActive(true);
        text = objectFactory->NewText(guiMapTextLayer, font, "PAUSE SPRITES", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::PAUSE_SPRITES, sprite, text, true));
    }

    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::DYNAMIC_CRYSTALS) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dySmall;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);
        text = objectFactory->NewText(guiMapTextLayer, font, "DYNAMIC CRYSTALS", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::DYNAMIC_CRYSTALS, sprite, text, true));
    }


    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SPAWN_WALKERS) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dySmall;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);
        text = objectFactory->NewText(guiMapTextLayer, font, "SPAWN WALKERS", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::SPAWN_WALKERS, sprite, text, false));
    }

    // INFO BUTTON
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SHOW_INFO) == skippedButtons.end()){
        if(buttons.empty()==false){
            spritePos.y += dyBig;
            sprite = static_cast<StandardSprite*>(sprite->MakeActiveCopy());
        }
        sprite->SetPosition(spritePos);
        //sprite->SetColorOverlayRGBA(ColorRGBA(255,0,0,64));
        //sprite->SetColorOverlayActive(true);
        text = objectFactory->NewText(guiMapTextLayer, font, "SHOW INFO", spritePos+buttonLabelOffset, color);
        buttons.push_back(new Button(ButtonTag::SHOW_INFO, sprite, text, true));
    }

    //----
    buttonsLayer->UpdateBoundingBox();     // we will need new size with added buttons


    //----------------------------------------------------------------------------------------------------
    // EXTRA LABELS
    //----------------------------------------------------------------------------------------------------

    int yPos = spritePos.y + extraLabelsYOffset;

    labels.resize(extraLabelsCount);

    for(int i=0; i<labels.size(); i++){
        labels[i] = jugimap::objectFactory->NewText(guiMapTextLayer, font, "Label", Vec2f(10,yPos), ColorRGBA(255,255,255,255));
        if(settings.IsYCoordinateUp()){
            //yPos -= labels[i]->GetHeight() * extraLabelsYSpacingFactor;   // GetHeight() should not be used as text is not yet initialized
            yPos -= font->GetSize() * extraLabelsYSpacingFactor;
        }else{
            //yPos += labels[i]->GetHeight() * extraLabelsYSpacingFactor;
            yPos += font->GetSize() * extraLabelsYSpacingFactor;
        }

    }

    guiMapTextLayer->InitEngineLayer();


    // add some texts to 'worldMapTextLayer' for testing
    if(worldMapTextLayer){
        std::vector<Sprite*>platformSprites;
        CollectSpritesWithName(worldMap, platformSprites, "Platform 1");
        for(Sprite* s : platformSprites){
            Text *t = objectFactory->NewText(worldMapTextLayer, font, s->GetSourceSprite()->GetName(), s->GetPosition()+Vec2f(20,0), ColorRGBA(255,255,255,128));
            t->SetLinkObject(s);
        }
        worldMapTextLayer->InitEngineLayer();
    }

    UpdateTexts();

}


void PlatformerScene::UpdateTexts()
{

    //----
    std::string str = " Click on walkers to remove them.";
    if(str != labels[0]->GetTextString()) labels[0]->SetTextString(str);


    //----
    int nActiveFellas = 0;
    int nUnusedFellas = 0;
    for(Entity *e : entities.GetList()){
        if(dynamic_cast<EWalkingFella*>(e)){
            if(e->IsActive()){
                nActiveFellas++;
            }else{
                nUnusedFellas++;
            }
        }
    }

    //----
    str = " Active walkers: " + std::to_string(nActiveFellas);
    if(str != labels[1]->GetTextString()) labels[1]->SetTextString(str);

    //----
    str = " Deactivated walkers: " + std::to_string(nUnusedFellas);
    if(str != labels[2]->GetTextString()) labels[2]->SetTextString(str);

    //----
    str = " ";
    if(str != labels[3]->GetTextString()) labels[3]->SetTextString(str);


    //--- update texts for worldMapTextLayer
    if(worldMapTextLayer){
        for(Text* t : worldMapTextLayer->GetTexts()){
            if(t->GetLinkObject()){
                Sprite *s = static_cast<Sprite*>(t->GetLinkObject());
                t->SetPosition(s->GetGlobalPosition());
                t->SetTextString(s->GetSourceSprite()->GetName()+" pos x:"+std::to_string(int(s->GetGlobalPosition().x))+" y:"+std::to_string(int(s->GetGlobalPosition().y)));
            }
        }
    }

}


}
