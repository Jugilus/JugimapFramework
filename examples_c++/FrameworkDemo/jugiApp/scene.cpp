#include <chrono>
#include <assert.h>
#include <sstream>

#include "app.h"
#include "scene.h"



namespace jugiApp{



using namespace jugimap;




bool PlatformerScene::Init()
{

    GetEngineScene()->PreInit();


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


    // Important: Maps must be created in their drawing order, from the back to the front !

    //---- background
    backgroundSourceGraphics = LoadAndAddSourceGraphics(backgroundSourceGraphicsFile);
    if(backgroundSourceGraphics==nullptr){
        return false;
    }
    backgroundMap = LoadAndAddMap(backgroundMapFile, backgroundSourceGraphics, MapType::PARALLAX);
    if(backgroundMap==nullptr){
        return false;
    }

    //---- world map
    worldSourceGraphics = LoadAndAddSourceGraphics(worldMapSourceGraphicsFile);
    if(worldSourceGraphics==nullptr){
        return false;
    }
    worldMap = LoadAndAddMap(worldMapFile, worldSourceGraphics, MapType::WORLD);
    if(worldMap==nullptr){
        return false;
    }

    //---- gui
    guiSourceGraphics = LoadAndAddSourceGraphics(guiMapSourceGraphicsFile);
    if(guiSourceGraphics==nullptr){
        return false;
    }
    guiMap = LoadAndAddMap(guiMapFile, guiSourceGraphics, MapType::SCREEN);
    if(guiMap==nullptr){
        return false;
    }


    // ADD ADDITIONAL ELEMENTS TO MAPS
    //------------------------------------------------------------------------------
    // worldMap
    //worldMapTextLayer = worldMap->AddSpriteLayer("worldMapTextLayer");                // a layer for testing text sprites
    worldMapDrawingLayer = new ShapesDrawingLayer("worldMapDrawingLayer", worldMap);    // a drawing layer drawing sprite and map shapes
    worldMap->AddDrawingLayer(worldMapDrawingLayer);


    // guiMap
    AddButtonsAndLabelsToGuiMap();                                                      // manually add buttons and text labels
    guiMapDrawingLayer = new ShapesDrawingLayer("guiMapDrawingLayer", guiMap);          // a drawing layer for drawing sprite and map shapes
    guiMap->AddDrawingLayer(guiMapDrawingLayer);




    //------------------------------------------------------------------------------
    // INITIALIZE MAPS ACCORDING TO THEIR TYPE AND ASSIGN CAMERAS
    //------------------------------------------------------------------------------
    Vec2i screenSize = settings.GetScreenSize();
    // worldMap
    worldMap->InitAsWorldMap();
    worldCamera.Init(Rectf(270,0, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    worldMap->SetCamera(&worldCamera);

    // backgroundMap
    backgroundMap->InitAsParallaxMap(worldMap->GetWorldMapSize());
    backgroundMap->SetCamera(&worldCamera);                                             // Parallax maps must have the same camera as the world map!


    // guiMap
    Vec2i guiMapDesignSize(270, screenSize.y);
    Vec2f guiMapPosition = Vec2f(0,0);

    guiMap->InitAsScreenMap(guiMapDesignSize);
    guiCamera.Init(guiMapDesignSize, guiMapPosition);
    guiMap->SetCamera(&guiCamera);

    //--- Uncomment the code bellow for transformed 'guiMap' !
    //guiMapPosition = Vec2f(screenSize.x*0.75, screenSize.y*0.5);
    //guiCamera.Init(guiMapDesignSize, guiMapPosition, 0.5, 30*settings.GetRotationSignForYdir());
    //guiCamera.SetMapHandle(Vec2f(guiMapDesignSize.x/2.0, guiMapDesignSize.y/2.0));



    //------------------------------------------------------------------------------
    // MANUALLY CREATE A MAP
    //------------------------------------------------------------------------------
    // Create 'infoMap' which will have a sprite with information about JugiMap.
    infoMap = AddMap("infoMap", MapType::SCREEN);

    // Add layer
    SpriteLayer *infoLayer = infoMap->AddSpriteLayer("infoLayer");

    // Add sprite
    SourceSprite *infoSourceSprite = FindSourceSpriteWithName(guiSourceGraphics, "info");
    assert(infoSourceSprite);
    // By default sprites have handle point in the center -> we add sprite at the center of design size.
    //infoLayer->AddStandardSprite(infoSourceSprite, Vec2f(std::round(infoMapDesignSize.x/2.0), std::round(infoMapDesignSize.y/2.0)));
    infoLayer->AddStandardSprite(infoSourceSprite, Vec2f(0,0));

    // Initialize the map as Screen Map and setup camera
    Vec2i infoMapDesignSize = infoSourceSprite->GetGraphicItems().front()->GetSize();       // design size is the same size as sprite
    infoMap->InitAsScreenMap(infoMapDesignSize);

    // When shown 'infoMap' will be positioned in the center of the worldmap camera viewport.
    infoCamera.Init(infoMapDesignSize, worldCamera.GetViewport().GetCenter());
    infoCamera.SetMapHandle(Vec2f(infoMapDesignSize.x/2, infoMapDesignSize.y/2));
    // At the start 'infoMap' will not be shown. Lets move it outside the screen. It will be translated into the center when shown.
    infoCamera.SetMapPosition(Vec2f(worldCamera.GetViewport().max.x + infoMapDesignSize.x/2 + 1, worldCamera.GetViewport().GetCenter().y));
    infoMap->SetCamera(&infoCamera);

    // Tween for animating translation of 'infoMap' into the view (and out of it).
    infoBoxAniTween.Init(worldCamera.GetViewport().max.x + infoMap->GetScreenMapDesignSize().x/2 + 1,
                         worldCamera.GetViewport().GetCenter().x, 0.5, Easing::Kind::EASE_END);



    //------------------------------------------------------------------------------
    // Set Cameras for "double camera" (split screen) mode.
    worldCameraA.Init(Rectf(270, screenSize.y/2+5, screenSize.x, screenSize.y), worldMap->GetWorldMapSize());
    worldCameraB.Init(Rectf(270,0, screenSize.x, screenSize.y/2-5), worldMap->GetWorldMapSize());


    // Set camera controllers for moving camera around.
    VectorShape *cameraPath = FindVectorShapeWithParameter(worldMap, "cameraPath", "");
    if(cameraPath){
        cameraController.Init(&worldCamera, cameraPath);
        cameraControllerA.Init(&worldCameraA, cameraPath);
        cameraControllerB.Init(&worldCameraB, MakeWorldCameraOverviewPath_v1(worldCameraB), true);      // 'true' -> controller takes ownership of 'cameraPath' object

    }

    //------------------------------------------------------------------------------
    // SETUP ENTITIES
    //------------------------------------------------------------------------------
    BuildCollidersTileGrid(worldMap);           // used for collision detection of entities with tile environment
    if(entities.Setup(worldMap)==false){
        return false;
    }


    //---
    GetEngineScene()->PostInit();

    //---
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

}


void PlatformerScene::Update()
{

    jugimap::time.UpdatePassedTimeMS();

    UpdateInMapCursorPositions();

    //---
    if(GuiWidget::GetInteracted_Pressed("button")){
        GuiButton *buttonClicked = static_cast<GuiButton*>(GuiWidget::GetInteracted());

        if(buttonClicked->GetTag()==ButtonTag::SHOW_SPRITE_SHAPES){
            worldMapDrawingLayer->SetDoDrawMapSpritesCollisionShapes(buttonClicked->IsChecked());
            if(parallaxMapDrawingLayer) parallaxMapDrawingLayer->SetDoDrawMapSpritesCollisionShapes(buttonClicked->IsChecked());
            if(guiMapDrawingLayer) guiMapDrawingLayer->SetDoDrawMapSpritesCollisionShapes(buttonClicked->IsChecked());

        }else if(buttonClicked->GetTag()==ButtonTag::SHOW_MAP_SHAPES){
            worldMapDrawingLayer->SetDoDrawMapVectorShapes(buttonClicked->IsChecked());
            if(parallaxMapDrawingLayer) parallaxMapDrawingLayer->SetDoDrawMapVectorShapes(buttonClicked->IsChecked());
            if(guiMapDrawingLayer) guiMapDrawingLayer->SetDoDrawMapVectorShapes(buttonClicked->IsChecked());

        }else if(buttonClicked->GetTag()==ButtonTag::PAUSE_CAMERA){
            cameraController.SetPaused(buttonClicked->IsChecked());
            cameraControllerA.SetPaused(buttonClicked->IsChecked());
            cameraControllerB.SetPaused(buttonClicked->IsChecked());

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

}


void PlatformerScene::ManageDeactivateWalkers()
{


    for(Entity *e : entities.GetList()){
        if(e->IsActive()==false) continue;

        EWalkingFella* wf = dynamic_cast<EWalkingFella*>(e);

        if(wf){
            //if(wf->GetControlledSprite()->PointInside(sceneCursor.GetWorldPosition())){
            if(wf->GetControlledSprite()->PointInside(cursorInWorldMapPosition)){
                if(mouse.IsButtonPressed(MouseButton::LEFT)){
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
    for(EWalkingFella *wf : EWalkingFella::templateFellas){
        wf->Spawn();
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
            x = infoBoxAniTween.Update();
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
            x = infoBoxAniTween.Update();
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
    infoCamera.SetMapPosition(Vec2f(x, worldCamera.GetViewport().GetCenter().y));

}


void PlatformerScene::UpdateInMapCursorPositions()
{

    Vec2f cursorScreenPos(mouse.GetX(), mouse.GetY());

    if(guiCamera.GetViewport().Contains(cursorScreenPos)){
        cursorInGuiMapPosition = guiCamera.MapPointFromScreenPoint(cursorScreenPos);
        cursorInWorldMapPosition = Vec2f(-999999,-999999);              // big number to not pick any actual in world map position

    }else{

        if(doubleCamera){
            if(worldCameraA.GetViewport().Contains(cursorScreenPos)){
                cursorInWorldMapPosition = worldCameraA.MapPointFromScreenPoint(cursorScreenPos);

            }else if(worldCameraB.GetViewport().Contains(cursorScreenPos)){
                cursorInWorldMapPosition = worldCameraB.MapPointFromScreenPoint(cursorScreenPos);
            }

        }else{
            if(worldCamera.GetViewport().Contains(cursorScreenPos)){
                cursorInWorldMapPosition = worldCamera.MapPointFromScreenPoint(cursorScreenPos);
            }
        }
        cursorInGuiMapPosition = Vec2f(-999999,-999999);                // big number to not pick any actual in gui map position
    }

}


void PlatformerScene::AddButtonsAndLabelsToGuiMap()
{

    // We put one "template" button on 'guiMap' in the editor.
    // We will turn this button into a proper button and add additional buttons.
    GuiButton *button = nullptr;
    for(GuiWidget* w : guiMap->GetWidgets()){
        if(w->GetKind()==GuiWidgetKind::BUTTON && w->GetName()=="button"){
            button = static_cast<GuiButton*>(w);
            break;
        }
    }
    assert(button);

    // Buttons are gui objects built from a sprite (via custom parameters).
    // We can create more buttons by duplicating the button sprite.
    ComposedSprite* buttonSprite = static_cast<ComposedSprite*>(button->GetSprite());
    Vec2f spritePos = buttonSprite->GetPosition();

    // Spacing between buttons
    float buttonHeight = buttonSprite->GetBoundingBox().GetHeight();
    float dySmall = buttonHeight + 2;
    float dyBig = dySmall + 6;
    if(settings.IsYCoordinateUp()){
        dySmall = -dySmall;
        dyBig = -dyBig;
    }

    // Some engines miss some functionalities so we will skip creating buttons for them.
    std::vector<int>skippedButtons;
    if(settings.GetEngine()==Engine::Cocos2Dx){
        if(application->HasPhysicsEnabled()==false){
            skippedButtons = std::vector<int>{ButtonTag::DYNAMIC_CRYSTALS};
        }

    }else if(settings.GetEngine()==Engine::AGK){
        skippedButtons = std::vector<int>{ButtonTag::DOUBLE_CAMERA};

    }else if(settings.GetEngine()==Engine::SFML){
        skippedButtons = std::vector<int>{ButtonTag::DYNAMIC_CRYSTALS};

    }else if(settings.GetEngine()==Engine::nCine){
        skippedButtons = std::vector<int>{ButtonTag::DOUBLE_CAMERA, ButtonTag::DYNAMIC_CRYSTALS};

    }

    // SHOW SHAPES BUTTONS
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SHOW_SPRITE_SHAPES) == skippedButtons.end()){
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("SHOW SPRITE SHAPES");
        button->SetTag(ButtonTag::SHOW_SPRITE_SHAPES);
        button = nullptr;
    }

    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SHOW_MAP_SHAPES) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dySmall;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("SHOW MAP SHAPES");
        button->SetTag(ButtonTag::SHOW_MAP_SHAPES);
        button = nullptr;
    }

    // CAMERA BUTTONS
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::PAUSE_CAMERA) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dyBig;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("PAUSE CAMERA");
        button->SetTag(ButtonTag::PAUSE_CAMERA);
        button = nullptr;
    }

    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::ZANY_CAMERA) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dySmall;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("ZANY CAMERA");
        button->SetTag(ButtonTag::ZANY_CAMERA);
        button = nullptr;
    }


    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::DOUBLE_CAMERA) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dySmall;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("DOUBLE CAMERA");
        button->SetTag(ButtonTag::DOUBLE_CAMERA);
        button = nullptr;
    }


    // SPRITES & ENTITIES BUTTONS
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::PAUSE_SPRITES) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dyBig;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("PAUSE_SPRITES");
        button->SetTag(ButtonTag::PAUSE_SPRITES);
        button = nullptr;
    }

    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::DYNAMIC_CRYSTALS) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dySmall;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("DYNAMIC CRYSTALS");
        button->SetTag(ButtonTag::DYNAMIC_CRYSTALS);
        button = nullptr;
    }


    //---
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SPAWN_WALKERS) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dySmall;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("SPAWN WALKERS");
        button->SetTag(ButtonTag::SPAWN_WALKERS);
        button = nullptr;
    }


    // INFO BUTTON
    //---------
    if(std::find(skippedButtons.begin(), skippedButtons.end(), ButtonTag::SHOW_INFO) == skippedButtons.end()){
        if(button==nullptr){
            Sprite *sprite = buttonSprite->MakeActiveCopy();
            button = new GuiButton(sprite);
            guiMap->AddWidget(button);
            spritePos.y += dyBig;
        }
        button->GetSprite()->SetPosition(spritePos);
        button->SetTextLabel("SHOW INFO");
        button->SetTag(ButtonTag::SHOW_INFO);
        button = nullptr;
    }


    //----------------------------------------------------------------------------------------------------
    // EXTRA LABELS
    //----------------------------------------------------------------------------------------------------

    // We create several text sprites - labels for various info texts.
    // The number of these labels varies between engines.

    if(settings.GetEngine()==Engine::Cocos2Dx){
        labels.resize(7);

    }else if(settings.GetEngine()==Engine::AGK){
        labels.resize(9);

    }else if(settings.GetEngine()==Engine::SFML){
        labels.resize(6);

    }else if(settings.GetEngine()==Engine::nCine){
        labels.resize(5);
    }


    // When obtaining position from other sprites use their bounding box and not sprite position and handle.
    // The reason is that the handle of a composed sprite is not a coordinate relative to the sprite top (or bottom) left corner like it is for a standard sprite.
    // So we can not obtain x position of the left border with 'xPos - xHandle' for a composed sprite. We obtain it from 'boundingBox.min.x'

    Vec2f pos(buttonSprite->GetBoundingBox().min.x , spritePos.y);
    if(settings.IsYCoordinateUp()){
        pos.y -= 50;
    }else{
        pos.y += 50;
    }
    Font *font = fontLibrary.At(0);                                             // used font
    SpriteLayer *sl = static_cast<SpriteLayer*>(buttonSprite->GetLayer());      // we out text sprites on the same layer as buttons

    for(int i=0; i<labels.size(); i++){
        labels[i] = sl->AddTextSprite("Label", font, ColorRGBA(255,255,255,255), pos, TextHandleVariant::LEFT);
        if(settings.IsYCoordinateUp()){
            pos.y -= font->GetCommonPixelHeight() * 1.1;
        }else{
            pos.y += font->GetCommonPixelHeight()  * 1.1;
        }

    }


    // Add some texts to the 'worldMapTextLayer' for testing.
    if(worldMapTextLayer){
        std::vector<Sprite*>platformSprites;
        CollectSpritesWithSourceSpriteName(worldMap, platformSprites, "Platform 1");
        for(Sprite* s : platformSprites){
            TextSprite *t = worldMapTextLayer->AddTextSprite(s->GetSourceSprite()->GetName(), font, ColorRGBA(255,255,255,128), s->GetPosition()+Vec2f(20,0), TextHandleVariant::LEFT);
            t->SetCustomObject(new SpriteCustomObjectTest(s));
        }
        worldMapTextLayer->InitEngineObjects();
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
        for(Sprite* s : worldMapTextLayer->GetSprites()){
            if(s->GetKind()==SpriteKind::TEXT && s->GetCustomObject()){
                TextSprite *ts = static_cast<TextSprite*>(s);
                Sprite *trackedSprite = static_cast<SpriteCustomObjectTest*>(s->GetCustomObject())->sprite;
                ts->SetPosition(trackedSprite->GetGlobalPosition());
                ts->SetTextString(s->GetSourceSprite()->GetName()+" pos x:"+std::to_string(int(trackedSprite->GetGlobalPosition().x))+" y:"+std::to_string(int(trackedSprite->GetGlobalPosition().y)));
             }
        }
    }


}


}
