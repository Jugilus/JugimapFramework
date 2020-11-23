#include <chrono>
#include <assert.h>
#include <sstream>

#include "app.h"
#include "scene.h"



namespace jugiApp{

using namespace jugimap;



jugimap::Vec2f InventorySlot::itemSpritePos;
jugimap::Vec2f InventorySlot::amountSpritePos;
jugimap::ColorRGBA InventorySlot::amountColorRGBA;
jugimap::ColorRGBA InventorySlot::amountColorRGBA_selectedItem;



bool DemoScene::Init()
{

    GetEngineScene()->PreInit();

    //------------------------------------------------------------------------------
    // LOAD SOURCE GRAPHICS AND MAPS
    //------------------------------------------------------------------------------

    std::string sourceGraphicsFile = "media/maps/cfgGui.sgb";
    std::string headerMapFile = "media/maps/header.jmb";
    std::string commonWidgetsMapFile = "media/maps/commonWidgets.jmb";
    std::string textWidgetsMapFile = "media/maps/textWidgets.jmb";
    std::string composedDesignsMapFile = "media/maps/composedDesigns.jmb";

    //----
    sourceGraphics = LoadAndAddSourceGraphics(sourceGraphicsFile);
    if(sourceGraphics==nullptr){
        return false;
    }

    headerMap = LoadAndAddMap(headerMapFile, sourceGraphics, MapType::SCREEN);
    if(headerMap==nullptr){
        return false;
    }

    commonWidgetsMap = LoadAndAddMap(commonWidgetsMapFile, sourceGraphics, MapType::SCREEN);
    if(commonWidgetsMap==nullptr){
        return false;
    }

    textWidgetsMap = LoadAndAddMap(textWidgetsMapFile, sourceGraphics, MapType::SCREEN);
    if(textWidgetsMap==nullptr){
        return false;
    }

    composedDesignsMap = LoadAndAddMap(composedDesignsMapFile, sourceGraphics, MapType::SCREEN );
    if(composedDesignsMap==nullptr){
        return false;
    }


    //------------------------------------------------------------------------------
    // INITIALIZE MAPS AND ASSIGN CAMERAS
    //------------------------------------------------------------------------------
    Vec2i screenSize = settings.GetScreenSize();
    Vec2i contentMapsDesignSize = commonWidgetsMap->GetSize();   // commonWidgetsMap, textWidgetsMap, composedDesignsMap were created with the same size
    Vec2i headerMapDesignSize(Vec2i(contentMapsDesignSize.x, headerMap->GetSize().y));


    //--- header map
    headerMap->InitAsScreenMap(headerMapDesignSize);
    //---
    Vec2f headerPos((screenSize.x - headerMapDesignSize.x)/2.0, 0);
    if(settings.IsYCoordinateUp()){
        headerPos.y = screenSize.y - headerMapDesignSize.y;
    }
    headerCamera.Init(screenSize, headerPos);
    //---
    headerMap->SetCamera(&headerCamera);


    //--- content maps (all content maps have the same size and have the same camera)
    commonWidgetsMap->InitAsScreenMap(contentMapsDesignSize);
    textWidgetsMap->InitAsScreenMap(contentMapsDesignSize);
    composedDesignsMap->InitAsScreenMap(contentMapsDesignSize);
    //---
    Vec2f commonWidgetsPos((screenSize.x-headerMap->GetScreenMapDesignSize().x)/2.0, headerMap->GetScreenMapDesignSize().y);
    if(settings.IsYCoordinateUp()){
        commonWidgetsPos.y = screenSize.y-headerMap->GetScreenMapDesignSize().y-commonWidgetsMap->GetScreenMapDesignSize().y;
    }
    commonCamera.Init(commonWidgetsMap->GetScreenMapDesignSize(), commonWidgetsPos);
    //---
    commonWidgetsMap->SetCamera(&commonCamera);
    textWidgetsMap->SetCamera(&commonCamera);
    composedDesignsMap->SetCamera(&commonCamera);


    //------------------------------------------------------------------------------
    // ADD OVERLAY MAP for info and cursor
    //-------------------------------------------------------------------------------
    overlayMap = AddMap("overlayMap", MapType::SCREEN);

    // Initialize map and set camera.
    // We do this before adding sprites becouse sprites get their positions normalized at the map initialization.
    overlayMap->InitAsScreenMap(screenSize);
    overlayCamera.Init(screenSize, Vec2f());
    overlayMap->SetCamera(&overlayCamera);


    // add layer for info sprites
    SpriteLayer *sl = overlayMap->AddSpriteLayer("InfoLayer");

    // add text sprites for info
    Vec2f pos(commonWidgetsPos.x, commonWidgetsPos.y - 10);
    if(settings.IsYCoordinateUp()==false){
        pos.y = headerMap->GetScreenMapDesignSize().y + commonWidgetsMap->GetScreenMapDesignSize().y + 10 ;
    }

    TextSprite* ts = sl->AddTextSprite("Interacted widget:  ", fontLibrary.At(2), textColorsLibrary.At(1), pos, TextHandleVariant::LEFT_TOP);
    pos.x += ts->GetWidth();
    interactedWidgetInfo = sl->AddTextSprite("", fontLibrary.At(1), textColorsLibrary.Find("greyLight"), pos, TextHandleVariant::LEFT_TOP);

    // add layer for cursor
    sl = overlayMap->AddSpriteLayer("CursorLayer");

    // add cursor sprite
    SourceSprite *ss = FindSourceSpriteWithName(sourceGraphics,"cursor");
    assert(ss);
    StandardSprite *s = sl->AddStandardSprite(ss, Vec2f());
    mouse.SetCursorSprite(s);







    //-------------------------------------------------------------------------------

    // Load text data.
    textBook = textLibrary.CreateNewBook("testingBook");
    textBook->LoadContent("texts.txt", textColorsLibrary, fontLibrary);

    textSegment = textLibrary.GetTextSegmentFromBook("testingBook", 0);             // store pointer for quick access
    assert(textSegment);

    textField = dynamic_cast<GuiTextField*>(textWidgetsMap->FindWidget("bigTextField"));    // store pointer for quick access
    assert(textField);

    //---
    GuiTextField *tf = dynamic_cast<GuiTextField*>(textWidgetsMap->FindWidget("smallTextField"));   assert(tf);
    tf->Start(textBook->GetTextSegments().at(1));

    GuiTextInput *ti = dynamic_cast<GuiTextInput*>(textWidgetsMap->FindWidget("changeWord"));    assert(ti);
    ti->SetTextMaximumLength(16);


    //---
    inventory.Build(sourceGraphics, composedDesignsMap);
    dialogBox.Build(sourceGraphics, composedDesignsMap);


    //-------------------------------------------------------------------------------

    //--- set keyboard keys accessing gui widgets
    guiKeyboardAndJoystickInput.SetNavigateForwardKeys({KeyCode::RIGHT, KeyCode::TAB});
    guiKeyboardAndJoystickInput.SetNavigateBackwardKeys({KeyCode::LEFT});
    guiKeyboardAndJoystickInput.SetPressKeys({KeyCode::ENTER});
    guiKeyboardAndJoystickInput.SetIncrementValueKeys({KeyCode::UP});
    guiKeyboardAndJoystickInput.SetDecrementValueKeys({KeyCode::DOWN});

    //--- set joystick controlls for accessing gui widgets
    guiKeyboardAndJoystickInput.SetNavigateForwardJoystickPOV_X(Joystick::POV_X::RIGHT);
    guiKeyboardAndJoystickInput.SetNavigateBackwardJoystickPOV_X(Joystick::POV_X::LEFT);
    guiKeyboardAndJoystickInput.SetIncrementValueJoystickPOV_Y(Joystick::POV_Y::UP);
    guiKeyboardAndJoystickInput.SetDecrementValueJoystickPOV_Y(Joystick::POV_Y::DOWN);
    guiKeyboardAndJoystickInput.SetJoystickPressButtons({0,1,2,3});

    //---
    GetEngineScene()->PostInit();

    //---
    SetInitialized(true);

    return true;
}



void DemoScene::Start()
{

    //---
    //headerMap->SetWidgetsToInitialState();
    //commonWidgetsMap->SetWidgetsToInitialState();
    //textWidgetsMap->SetWidgetsToInitialState();
    //composedDesignsMap->SetWidgetsToInitialState();

    //interactedWidgetInfo->SetTextString("");

    GuiButton *bSpriteCursor = dynamic_cast<GuiButton*>(headerMap->FindWidget("spriteCursor"));     assert(bSpriteCursor);
    bSpriteCursor->SetChecked(true);
    application->GetEngineApp()->SetSystemMouseCursorVisible(false);

    textField->Start(textSegment);
    inventory.UpdateInventorySlots();
    dialogBox.StartSequence(textBook->GetTextSequences().at(0));


    //--- show 'commonWidgetsMap' at start, hide other
    shownMap = commonWidgetsMap;
    commonWidgetsMap->SetVisible(true);
    textWidgetsMap->SetVisible(false);
    composedDesignsMap->SetVisible(false);

    GuiWidget *w = headerMap->FindWidget("commonWidgets");  assert(w);
    static_cast<GuiButton*>(w)->SetChecked(true);

    //---
    SetKeyboardHighlightingWidgets();

}



void DemoScene::Update()
{

    //---
    UpdateInteractedWidgetInfo();


    //------------------------------------------------------------------------
    // HEADER
    //------------------------------------------------------------------------
    if(GuiWidget::GetInteracted_Pressed("spriteCursor")){
        if(dynamic_cast<GuiButton*>(GuiWidget::GetInteracted())->IsChecked()){
            mouse.GetCursorSprite()->SetVisible(true);

            application->GetEngineApp()->SetSystemMouseCursorVisible(false);
        }else{
            mouse.GetCursorSprite()->SetVisible(false);
            application->GetEngineApp()->SetSystemMouseCursorVisible(true);
        }

    }else if(GuiWidget::GetInteracted_Pressed("commonWidgets")){

        if(shownMap!=commonWidgetsMap){
            commonWidgetsMap->SetVisible(true);
            textWidgetsMap->SetVisible(false);
            composedDesignsMap->SetVisible(false);
            shownMap = commonWidgetsMap;
            SetKeyboardHighlightingWidgets();
        }

    }else if(GuiWidget::GetInteracted_Pressed("textWidgets")){

        if(shownMap!=textWidgetsMap){
            commonWidgetsMap->SetVisible(false);
            textWidgetsMap->SetVisible(true);
            composedDesignsMap->SetVisible(false);
            shownMap = textWidgetsMap;
            SetKeyboardHighlightingWidgets();
        }

    }else if(GuiWidget::GetInteracted_Pressed("composedDesigns")){

        if(shownMap!=composedDesignsMap){
            commonWidgetsMap->SetVisible(false);
            textWidgetsMap->SetVisible(false);
            composedDesignsMap->SetVisible(true);
            shownMap = composedDesignsMap;
            SetKeyboardHighlightingWidgets();
        }
    }


    //------------------------------------------------------------------------
    // COMMON WIDGETS
    //------------------------------------------------------------------------
    if(shownMap==commonWidgetsMap){

        if(GuiWidget::GetInteracted_ValueChanged("changeBars")){
            float value = dynamic_cast<GuiSlider*>(GuiWidget::GetInteracted())->GetValue();

            for(GuiWidget *w : commonWidgetsMap->GetWidgets()){
                if(w->GetKind()==GuiWidgetKind::BAR){
                    GuiBar *b = static_cast<GuiBar*>(w);

                    // Value of the slider is in range 0.0 - 1.0. The relation between bar value and slider value is linear.
                    float barValue = b->GetValueMin() + value*(b->GetValueMax() - b->GetValueMin());
                    b->SetValue(barValue);
                }
            }
        }


    //------------------------------------------------------------------------
    // TEXT WIDGETS
    //------------------------------------------------------------------------
    }else if(shownMap==textWidgetsMap){


        //bool restartTextField = false;

        if(GuiWidget::GetInteracted_Pressed("justifyRows")){
            bool checked = dynamic_cast<GuiButton*>(GuiWidget::GetInteracted())->IsChecked();

            if(checked){
                if(textField->GetTextAlignment()!=GuiTextField::Alignment::JUSTIFY){
                    textField->SetTextAlignment(GuiTextField::Alignment::JUSTIFY);
                }
            }else{
                if(textField->GetTextAlignment()!=GuiTextField::Alignment::LEFT){
                    textField->SetTextAlignment(GuiTextField::Alignment::LEFT);
                }
            }

        }else if(GuiWidget::GetInteracted_Pressed("noDelay")){

            if(textField->GetDrawingDelay()!=GuiTextField::DrawingDelay::NONE){
                textField->SetDrawingDelay(GuiTextField::DrawingDelay::NONE, 0);
            }

        }else if(GuiWidget::GetInteracted_Pressed("delayByRows")){

            if(textField->GetDrawingDelay()!=GuiTextField::DrawingDelay::BY_ROWS){
                textField->SetDrawingDelay(GuiTextField::DrawingDelay::BY_ROWS, 200);
            }

        }else if(GuiWidget::GetInteracted_Pressed("delayByWords")){

            if(textField->GetDrawingDelay()!=GuiTextField::DrawingDelay::BY_WORDS){
                textField->SetDrawingDelay(GuiTextField::DrawingDelay::BY_WORDS, 100);
            }

        }else if(GuiWidget::GetInteracted_Pressed("delayByCharacters")){

            if(textField->GetDrawingDelay()!=GuiTextField::DrawingDelay::BY_CHARACTERS){
                textField->SetDrawingDelay(GuiTextField::DrawingDelay::BY_CHARACTERS, 25);
            }
        }

        if(textField->GetTextSegment()==nullptr){       // changed property reset textField
            textField->Start(textSegment);
        }


        if(GuiWidget::GetInteracted_Pressed("bigTextField")){

            //--- check if clickable text has been pressed
            std::string clickedTextName = textField->GetClickedButtonName();
            if(clickedTextName != ""){
                DbgOutput("clickedTextName: " +  clickedTextName);

                if(clickedTextName=="setTextInputString"){
                    GuiTextInput * ti = dynamic_cast<GuiTextInput*>(textWidgetsMap->FindWidget("changeWord"));
                    assert(ti);
                    std::string s = "button clicked";
                    ti->SetText(s);

                    GuiTextField *tf = static_cast<GuiTextField*>(textWidgetsMap->FindWidget("smallTextField"));
                    TextSegment *ts = tf->GetTextSegment();
                    if(ts->Replace("word", s)){
                        tf->Start(ts);
                    }
                }

            }else{

                if(textField->IsDelayedDrawingActive()){
                    textField->SkipDelay();
                }else{
                    textField->ScrollTextUp();
                }
            }

        }else if(keyboard.IsKeyPressed(KeyCode::SPACE)){
            if(textField->IsCursorOver()){
                if(textField->IsDelayedDrawingActive()){
                    textField->SkipDelay();
                }else{
                    textField->ScrollTextUp();
                }
            }

        }else if(keyboard.IsKeyPressed(KeyCode::BACKSPACE)){
            if(textField->IsCursorOver()){
                textField->ScrollTextDown();
            }


        }else if(GuiWidget::GetInteracted_ValueChanged("changeWord")){
            std::string text = dynamic_cast<GuiTextInput*>(GuiWidget::GetInteracted())->GetText();
            //DbgOutput("changeWord: " +  text);

            GuiTextField *tf = static_cast<GuiTextField*>(textWidgetsMap->FindWidget("smallTextField"));
            TextSegment *ts = tf->GetTextSegment();
            if(ts->Replace("word", text)){
                tf->Start(ts);
            }
        }


    //------------------------------------------------------------------------
    // COMPOSED DESIGNS
    //------------------------------------------------------------------------
    }else if(shownMap==composedDesignsMap){

        inventory.Update();
        dialogBox.Update();

    }


}


void DemoScene::SetKeyboardHighlightingWidgets()
{

    std::vector<GuiWidget*>widgets;
    GuiWidget *w = nullptr;
    GuiWidget *wHighligtedWidget = nullptr;

    w = headerMap->FindWidget("commonWidgets");             assert(w);  widgets.push_back(w);
    w = headerMap->FindWidget("textWidgets");               assert(w);  widgets.push_back(w);
    w = headerMap->FindWidget("composedDesigns");           assert(w);  widgets.push_back(w);
    w = headerMap->FindWidget("spriteCursor");              assert(w);  widgets.push_back(w);


    if(shownMap == commonWidgetsMap){
        w = commonWidgetsMap->FindWidget("Options");            assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("Play");               assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("Settings");           assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("textButton1");        assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("blueSlider");         assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("greenScrollBar");     assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("greenSpinBox");       assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("verticalStepSlider"); assert(w);  widgets.push_back(w);
        w = commonWidgetsMap->FindWidget("changeBars");         assert(w);  widgets.push_back(w);

        wHighligtedWidget = headerMap->FindWidget("commonWidgets");

    }else if(shownMap == textWidgetsMap){
        w = textWidgetsMap->FindWidget("justifyRows");         assert(w);  widgets.push_back(w);
        w = textWidgetsMap->FindWidget("noDelay");             assert(w);  widgets.push_back(w);
        w = textWidgetsMap->FindWidget("delayByCharacters");   assert(w);  widgets.push_back(w);
        w = textWidgetsMap->FindWidget("delayByWords");        assert(w);  widgets.push_back(w);
        w = textWidgetsMap->FindWidget("delayByRows");         assert(w);  widgets.push_back(w);

        wHighligtedWidget = headerMap->FindWidget("textWidgets");

    }else if(shownMap == composedDesignsMap){

        wHighligtedWidget = headerMap->FindWidget("composedDesigns");

    }

    guiKeyboardAndJoystickInput.SetWidgets(widgets, wHighligtedWidget);

}


void DemoScene::UpdateInteractedWidgetInfo()
{

    std::string text;

    GuiWidget * interactedWidget = GuiWidget::GetInteracted();
    if(interactedWidget){

        text = std::string("  name: ") + interactedWidget->GetName()+
               std::string("  tag: ") + std::to_string(interactedWidget->GetTag());

        //---
        std::string kindInfo;
        if(interactedWidget->GetKind()==GuiWidgetKind::BUTTON){
            kindInfo = "  kind: button";

            if(static_cast<GuiButton*>(interactedWidget)->IsCheckable()){
                kindInfo += std::string("  checkable: true");
            }else{
                kindInfo += std::string("  checkable: false");
            }

            if(static_cast<GuiButton*>(interactedWidget)->IsChecked()){
                kindInfo += std::string("  checked: true");
            }else{
                kindInfo += std::string("  checked: false");
            }

        }else if(interactedWidget->GetKind()==GuiWidgetKind::SLIDER){
            kindInfo = "  kind: slider";

            GuiSlider* s = static_cast<GuiSlider*>(interactedWidget);

            std::ostringstream out;
            out.precision(2);
            out << std::fixed << s->GetValue();
            std::string valueText = out.str();

            kindInfo += std::string("  value: ") + valueText;
        }

        text += kindInfo;

        //---
        if(interactedWidget->GetKind()==GuiWidgetKind::BUTTON){
            std::string cursorText;

            if(interactedWidget->IsCursorOver()){
                if(cursorText != "") cursorText += " | ";
                cursorText += "OVER";
            }
            if(interactedWidget->IsCursorDown()){
                if(cursorText != "") cursorText += " | ";
                cursorText += "DOWN";
            }
            //if(interactedWidget->IsPressed()){
            //    if(cursorText != "") cursorText += " | ";
            //    cursorText += "PRESSED";
            //}

            cursorText = "  cursor " + cursorText;

            //---
            text += cursorText;
        }

        if(interactedWidget->GetCustomObject()){
            text += "    " + interactedWidget->GetCustomObject()->GetInfo();
        }
    }


    if(text != interactedWidgetInfo->GetTextString()){
        interactedWidgetInfo->SetTextString(text);
    }


}


/*
void DemoScene::TestMouse()
{

    if(mouse.ButtonPressed(MouseButton::LEFT)){
        DbgOutput("MouseButton::LEFT pressed");
    }

    if(mouse.ButtonPressed(MouseButton::MIDDLE)){
        DbgOutput("MouseButton::MIDDLE pressed");
    }

    if(mouse.ButtonPressed(MouseButton::RIGHT)){
        DbgOutput("MouseButton::RIGHT pressed");
    }

    if(mouse.ButtonDown(MouseButton::LEFT)){
        DbgOutput("MouseButton::LEFT down");
    }

    if(mouse.ButtonDown(MouseButton::MIDDLE)){
        DbgOutput("MouseButton::MIDDLE down");
    }

    if(mouse.ButtonDown(MouseButton::RIGHT)){
        DbgOutput("MouseButton::RIGHT down");
    }

    if(mouse.ButtonReleased(MouseButton::LEFT)){
        DbgOutput("MouseButton::LEFT released");
    }

    if(mouse.ButtonReleased(MouseButton::MIDDLE)){
        DbgOutput("MouseButton::MIDDLE released");
    }

    if(mouse.ButtonReleased(MouseButton::RIGHT)){
        DbgOutput("MouseButton::RIGHT released");
    }

    if(mouse.IsPositionChanged()){
    //    DbgOutput("Mouse position  x:" + std::to_string(mouse.GetX()) + " y:" + std::to_string(mouse.GetY()));
    }
    if(mouse.IsWheelChanged()){
        DbgOutput("Mouse wheel  x:" + std::to_string(mouse.GetWheelX()) + " y:" + std::to_string(mouse.GetWheelY()));
    }

}


void DemoScene::TestKeyboard()
{

    if(keyboard.KeyPressed(KeyCode::KEY_G)){
        DummyFunction();
    }

    if(keyboard.KeyPressed(KeyCode::KEY_F2)){
        DbgOutput("KEY_F2 pressed");
    }
    if(keyboard.KeyDown(KeyCode::KEY_F2)){
        DbgOutput("KEY_F2 down");
    }
    if(keyboard.KeyReleased(KeyCode::KEY_F2)){
        DbgOutput("KEY_F2 released");
    }

    if(keyboard.KeyPressed(KeyCode::KEY_4)){
        DbgOutput("KEY_4 pressed");
    }
    if(keyboard.KeyDown(KeyCode::KEY_4)){
        DbgOutput("KEY_4 down");
    }
    if(keyboard.KeyReleased(KeyCode::KEY_4)){
        DbgOutput("KEY_4 released");
    }

    //---
    std::string dbg;


    for(int i=0; i<keyboard.GetKeys().size(); i++){
        ButtonState &b = keyboard.GetKeys()[i];
        dbg = "";
        if(b.pressed){
            dbg += " pressed ";
        }
        if(b.down){
            dbg += " down ";
        }
        if(b.released){
            dbg += " released ";
        }

        if(dbg!=""){
             DbgOutput("Keyboard key:" + keyboard.GetKeysNames()[i] + "  " + dbg);
        }
    }

    if(keyboard.GetCharPressed()!=0){
        const char c =  keyboard.GetCharPressed();
        DbgOutput("Printable char pressed: " + std::string(1, c));
    }
}


void DemoScene::TestJostick()
{


    std::string dbg = "";

    Joystick &joystick = joysticks[0];

    for(int i=0; i<joysticks[0].GetButtons().size(); i++){

        ButtonState &b = joysticks[0].GetButtons()[i];
        dbg = "";
        if(b.pressed){
            dbg += " pressed ";
        }
        if(b.down){
            dbg += " down ";
        }
        if(b.released){
            dbg += " released ";
        }

        if(dbg!=""){
             DbgOutput("Joystick button id:" + std::to_string(i) + dbg);
        }
    }


    dbg = "";
    if(joysticks[0].GetPOV_X()!=Joystick::POV::NONE){
        dbg = Joystick::DbgGetPOVstring(joysticks[0].GetPOV_X());
    }
    if(joysticks[0].GetPOV_Y()!=Joystick::POV::NONE){
        if(dbg.length()>0) dbg += "  ";
        dbg += Joystick::DbgGetPOVstring(joysticks[0].GetPOV_Y());
    }
    if(dbg.length()>0){
        DbgOutput("Joystick pov: " + dbg);
    }

    dbg = "";
    if(AreEqual(joysticks[0].GetXaxis(), 0.0, 0.1) == false){
        dbg = " x:" + std::to_string(joysticks[0].GetXaxis());
    }
    if(AreEqual(joysticks[0].GetYaxis(), 0.0, 0.1) == false){
        dbg += " y:" + std::to_string(joysticks[0].GetYaxis());
    }
    if(AreEqual(joysticks[0].GetZaxis(), 0.0, 0.1) == false){
        dbg += " z:" + std::to_string(joysticks[0].GetZaxis());
    }

    if(dbg.length()>0){
        DbgOutput("Joystick axis " + dbg);
    }

}


void DemoScene::TestTouch()
{


}
*/


//==================================================================================================


void Inventory::Build(SourceGraphics *sourceGraphics, Map *map)
{


    // Sprites for items were not added in the editor so we will create them now.
    // Collect source sprites with constant parameter "item" which stores the name of parameter
    std::vector<jugimap::SourceSprite*>sourcesprites;
    CollectSourceSpritesWithConstParameter(sourceGraphics, sourcesprites, "item");

    // we will store created sprites in a layer called "items"
    SpriteLayer* itemsLayer = dynamic_cast<SpriteLayer*>(FindLayerWithName(map, "items", LayerKind::SPRITE));
    assert(itemsLayer);

    // we will also add text sprites for amount depiction to a layer called "items amount"
    SpriteLayer* itemsAmountLayer = dynamic_cast<SpriteLayer*>(FindLayerWithName(map, "items amount", LayerKind::SPRITE));
    assert(itemsAmountLayer);


    for(SourceSprite *ss : sourcesprites){

        std::string name = Parameter::GetValue(ss->GetConstantParameters(), "item");
        int amount = Parameter::GetIntValue(ss->GetConstantParameters(), "amount");

        if(ss->GetKind()==SpriteKind::STANDARD){
            StandardSprite *s =  itemsLayer->AddStandardSprite(ss, Vec2f(0.0,0.0));
            inventoryItems.push_back(InventoryItem(name, s, amount));
        }
    }


    //---- Create inventorySlots vector for quick access to button slots.
    for(GuiWidget* w : map->GetWidgets()){
        if(w->GetKind()==GuiWidgetKind::BUTTON && w->GetDesignKind()=="inventorySlot"){
            int tag = w->GetTag();  assert(tag>=0);

            // Tags are increasing values from 0 ...
            // Store buttons in inventorySlots with index = tag

            if(inventorySlots.size()<=tag){
                inventorySlots.resize(tag+1);
            }
            inventorySlots[tag].slotButton = static_cast<GuiButton*>(w);
            inventorySlots[tag].slotButton->AssignCustomObject(new InventorySlotObjectLink());
            inventorySlots[tag].slotButton->AssignCallback(new InventorySlotCallback());


            // add text sprite for amount and store a link pointer into slot
            inventorySlots[tag].amountSprite = itemsAmountLayer->AddTextSprite("0", fontLibrary.At(3), ColorRGBA(0xFFC2A187), Vec2f(0.0,0.0), TextHandleVariant::CENTER);

        }
    }

    //---- Obtain position for item and amount sprites in slots.
    // This positin is stored as point shape in the first frame of 'inventorySlot-spr1' source sprite !

    SourceSprite *ss = FindSourceSpriteWithName(sourceGraphics, "inventorySlot-spr1");
    assert(ss);

    GraphicItem *i = ss->GetGraphicItems().front();

    VectorShape* sh = FindVectorShapeWithProperties(i->GetExtraShapes(),  1, false, ShapeKind::SINGLE_POINT);
    assert(sh);
    InventorySlot::itemSpritePos = static_cast<SinglePointShape*>(sh->GetGeometricShape())->point;

    sh = FindVectorShapeWithProperties(i->GetExtraShapes(),  2, false, ShapeKind::SINGLE_POINT);
    assert(sh);
    InventorySlot::amountSpritePos = static_cast<SinglePointShape*>(sh->GetGeometricShape())->point;


    //---- colors for amountSprite
    InventorySlot::amountColorRGBA.Set(0xFFC2A187);
    InventorySlot::amountColorRGBA_selectedItem.Set(0xFFFFFFFF);

    //----
    inventorySlider = dynamic_cast<GuiSlider*>(map->FindWidget("inventorySlider"));
    assert(inventorySlider);


}


void Inventory::Update()
{

    if(GuiWidget::GetInteracted_ValueChanged()==inventorySlider){
        inventory.UpdateInventorySlots();
    }

}


void Inventory::UpdateInventorySlots()
{

    // Connect items with inventory slots

    // The number of displayed items is limited by the number of slots. We display items from an 'indexStart' in the
    // 'inventoryItems' and obtain 'indexStart' from a 'inventorySlider'.

     int indexStart = 0;

    if(inventoryItems.size()<= inventorySlots.size()){
        inventorySlider->SetDeltaValue(1.0, false);       // full range

    }else{

        // number of slots in sliding range
        int slotsSlidingRange = inventoryItems.size() - inventorySlots.size();
        slotsSlidingRange += (inventorySlots.size()-2);     // add more slots to sliding range as it looks and feels better when using the slider
        slotsSlidingRange += slotsSlidingRange % 2;     // even number as there are two rows

        indexStart = inventorySlider->GetValue() * slotsSlidingRange;

        // Inventory consists of two rows and it looks better if items remain in the same row during sliding -> indexStart should be 0, 2, 4, 6 ...
        indexStart += indexStart % 2;

        float sliderStep = (inventorySlots.size()-2) / (float)slotsSlidingRange;
        inventorySlider->SetDeltaValue(sliderStep, false);

        //DbgOutput("sliderValue:" + std::to_string(inventorySlider->GetValue()) + " indexStart:" + std::to_string(indexStart));
    }

    int slotIndex = 0;

    for(int i=0; i<inventorySlots.size(); i++){
        inventorySlots[i].item = nullptr;
    }

    // set item sprites
    for(int i=0; i<inventoryItems.size(); i++){

        if(i>=indexStart && slotIndex<inventorySlots.size()){
            inventoryItems[i].slotIndex = slotIndex;
            slotIndex++;

        }else{
            inventoryItems[i].slotIndex = -1;
        }


        if(inventoryItems[i].slotIndex == -1){
            if(inventoryItems[i].sprite->IsVisible()){
                inventoryItems[i].sprite->SetVisible(false);
            }

        }else{
            InventorySlot &isc = inventorySlots[inventoryItems[i].slotIndex];
            Sprite *slotSprite = isc.slotButton->GetSprite();

            Vec2f itemPos = slotSprite->GetPosition() + InventorySlot::itemSpritePos;
            inventoryItems[i].sprite->SetPosition(itemPos);
            if(inventoryItems[i].sprite->IsVisible()==false){
                inventoryItems[i].sprite->SetVisible(true);
            }

            isc.item = &inventoryItems[i];
        }
    }


    // assign items as link object to slotButtons
    for(int i=0; i<inventorySlots.size(); i++){
        InventorySlot &isc = inventorySlots[i];

        if(isc.item){
            //isc.slotButton->SetObjectLink(isc.item);            // assign items as link object to slotButtons
            static_cast<InventorySlotObjectLink*>(isc.slotButton->GetCustomObject())->item = isc.item;


            //---- manage 'amountSprite'
            Sprite *slotSprite = isc.slotButton->GetSprite();
            Vec2f amountPos = slotSprite->GetPosition() + InventorySlot::amountSpritePos;
            if(amountPos.Equals(isc.amountSprite->GetPosition())==false){
                isc.amountSprite->SetPosition(amountPos);
            }

            std::string amount = std::to_string(isc.item->amount);
            if(amount != isc.amountSprite->GetTextString()){
                isc.amountSprite->SetTextString(amount);
            }

            /*
            if(isc.item->selected){
                if(isc.amountSprite->GetColor() != InventorySlotContent::amountColorRGBA_selectedItem){
                    isc.amountSprite->SetColor(InventorySlotContent::amountColorRGBA_selectedItem);
                }
            }else{
                if(isc.amountSprite->GetColor() != InventorySlotContent::amountColorRGBA){
                    isc.amountSprite->SetColor(InventorySlotContent::amountColorRGBA);
                }
            }
            */
            ColorRGBA c = (isc.item->selected)? InventorySlot::amountColorRGBA_selectedItem : InventorySlot::amountColorRGBA;
            isc.amountSprite->SetColor(c);

            //if(isc.amountSprite->IsVisible()==false){
                isc.amountSprite->SetVisible(true);
            //}

            isc.slotButton->SetChecked(isc.item->selected);

            /*
            if(isc.item->selected){
                if(isc.slotButton->IsChecked()==false){
                    isc.slotButton->SetChecked(true);
                }
            }else{
                if(isc.slotButton->IsChecked()){
                    isc.slotButton->SetChecked(false);
                }
            }
            */


        }else{

            //isc.slotButton->SetObjectLink(nullptr);
            static_cast<InventorySlotObjectLink*>(isc.slotButton->GetCustomObject())->item = nullptr;

            //if(isc.slotButton->IsChecked()){
                isc.slotButton->SetChecked(false);
            //}
            //if(isc.amountSprite->IsVisible()){
                isc.amountSprite->SetVisible(false);
            //}

        }
    }


    // displayed item from 'inventoryItems'
    //
    // Get starting map inventorySlider value to strating position range to



}


//==================================================================================================


std::string InventorySlotObjectLink::GetInfo()
{

    if(item){
        return std::string("item: '") + item->name + "'  amount: " + std::to_string(item->amount);
    }
    return std::string("item: - ");

}


void InventorySlotCallback::OnPressed(jugimap::GuiWidget *_widget)
{

    GuiButton *b = static_cast<GuiButton*>(_widget);
    InventorySlotObjectLink *o = static_cast<InventorySlotObjectLink *>(_widget->GetCustomObject());

    if(o->item==nullptr) return;

    o->item->selected = b->IsChecked();

    if(o->item->slotIndex != -1){
        InventorySlot &isc = inventory.GetSlots().at(o->item->slotIndex);

        if(isc.item->selected){
            isc.amountSprite->SetColor(InventorySlot::amountColorRGBA_selectedItem);

        }else{
            isc.amountSprite->SetColor(InventorySlot::amountColorRGBA);
        }
    }

}



Inventory inventory;


//============================================================================================


void JDialogBox::Build(jugimap::SourceGraphics *sourceGraphics, jugimap::Map* map)
{

    //---- store some pointers for quick access to resources
    textField = dynamic_cast<GuiTextField*>(map->FindWidget("dialogTextField"));    assert(textField);
    textField->SetDrawingDelay(GuiTextField::DrawingDelay::BY_WORDS, 20);
    textField->SetLocalLinesSpacingFactor(-0.1);
    textField->SetScrollingMode(GuiTextField::ScrollingMode::FIXED_STEP);
    textField->SetScrollingModeFixedStepProperties(4, fontLibrary.At(1)->GetCommonPixelHeight(), 250);

    buttonUp = dynamic_cast<GuiButton*>(map->FindWidget("dialogBox_buttonUp"));    assert(buttonUp);
    buttonDown = dynamic_cast<GuiButton*>(map->FindWidget("dialogBox_buttonDown"));    assert(buttonDown);

    //----
    std::vector<Sprite*>sprites;
    CollectSpritesWithName(map, sprites, "characterName");
    assert(sprites.size()==1);
    characterNameTextSprite = static_cast<TextSprite*>(sprites.front());
    characterNameTextSprite->SetTextString("");

    //---- create character portrait sprites

    // Please note: We could add character portrait sprites to the map in the editor and we would just collect them here.
    // The following proccess displays how to add sprites manually.

    //----------------------------------------------------------------------------------

    // We will store created sprites in a layer named "various".
    SpriteLayer* layer = dynamic_cast<SpriteLayer*>(FindLayerWithName(map, "various", LayerKind::SPRITE));
    assert(layer);

    // Sprite positions are relative to the dialog text box sprite so lets find it.
    sprites.clear();
    CollectSpritesWithSourceSpriteName(map, sprites, "textBox_background");
    assert(sprites.size()==1);
    StandardSprite * textBox = static_cast<StandardSprite*>(sprites.front());

    // The position is defined with a single point vector (extra) shape on the sprite image.
    VectorShape *vs = FindSpriteExtraShapeWithProperties(textBox->GetSourceSprite()->GetGraphicItems().front(), 1, false, ShapeKind::SINGLE_POINT);
    assert(vs);
    Vec2f pos = static_cast<SinglePointShape*>(vs->GetGeometricShape())->point;

    // 'p' is relative to 'textBox' handle point -> transform it to map coordinates
    pos += textBox->GetPosition() ;


    // Collect source sprites for character portraits.
    std::vector<SourceSprite*>sourceSprites;
    CollectSourceSpritesWithConstParameter(sourceGraphics, sourceSprites, "character");

    for(SourceSprite *ss : sourceSprites){

        std::string characterName = Parameter::GetValue(ss->GetConstantParameters(), "character");

        if(characterName=="Conan"){
            portraitConan = layer->AddStandardSprite(ss, pos);

        }else if(characterName=="Yasmina"){
             portraitYasmina = layer->AddStandardSprite(ss, pos);
        }
    }

    assert(portraitConan);
    assert(portraitYasmina);

    // Hide sprites, they will be shown when required.
    portraitConan->SetVisible(false);
    portraitYasmina->SetVisible(false);


    // Tween for fade IN/OUT portraits. It is not used as it looks better without it.
    //alphaTween.Init(0.0, 1.0, 0.25, Easing::Kind::EASE_START);

}


void JDialogBox::StartSequence(jugimap::TextSequence * _sequence)
{

    if(_sequence->GetTextSegments().empty()){
        return;
    }

    sequence = _sequence;
    currentTextSegmentIndex = 0;
    TextSegment * ts = sequence->GetTextSegments()[currentTextSegmentIndex];
    StartTextSegment(ts);

}


void JDialogBox::Update()
{

    if(buttonUp->IsPressed()){

        if(textField->ScrollTextDown()){
            // scroll text until the end of the displayed segment
            //DbgOutput("Displayed Start:" + std::to_string(textField->IsDisplayedTextSegmentStart()) +" End:" + std::to_string(textField->IsDisplayedTextSegmentEnd()));

        }else{
            // go to the previous segment in sequence if available
            if(currentTextSegmentIndex-1>=0){
                currentTextSegmentIndex--;
                StartTextSegment(sequence->GetTextSegments().at(currentTextSegmentIndex));
            }
        }

    }else if(buttonDown->IsPressed() ||
             (textField->IsCursorOver() && (keyboard.IsKeyPressed(KeyCode::SPACE) || mouse.IsButtonPressed(MouseButton::LEFT))))
    {

        if(textField->ScrollTextUp()){
            // scroll text until the end of the displayed segment
            //DbgOutput("Displayed Start:" + std::to_string(textField->IsDisplayedTextSegmentStart()) +" End:" + std::to_string(textField->IsDisplayedTextSegmentEnd()));

        }else{
            // go to the next segment in sequence if available
            if(currentTextSegmentIndex+1<sequence->GetTextSegments().size()){
                currentTextSegmentIndex++;
                StartTextSegment(sequence->GetTextSegments().at(currentTextSegmentIndex));
            }
        }
    }


    // Manage disabled state for buttons
    if(currentTextSegmentIndex==0 && textField->IsTextSegmentFirstLineDisplayed()){
        if(buttonUp->IsVisible()){
            buttonUp->SetVisible(false);
        }
    }else{
        if(buttonUp->IsVisible()==false){
            buttonUp->SetVisible(true);
        }
    }


    // Manage disabled state for buttons
    if(currentTextSegmentIndex==sequence->GetTextSegments().size()-1 && textField->IsTextSegmentLastLineDisplayed()){
        if(buttonDown->IsVisible()){
            buttonDown->SetVisible(false);
        }
    }else{
        if(buttonDown->IsVisible()==false){
            buttonDown->SetVisible(true);
        }
    }
}


void JDialogBox::StartTextSegment(jugimap::TextSegment* _textSegment)
{

    // --- Manage character portraits
    std::string characterName = Parameter::GetValue(_textSegment->GetParameters(), "character");

    previousPortrait = currentPortrait;

    if(characterName=="Conan"){
        currentPortrait = portraitConan;

    }else if(characterName=="Yasmina"){
        currentPortrait = portraitYasmina;

    }else{
        currentPortrait = nullptr;
    }

    if(currentPortrait && currentPortrait!=previousPortrait){
        currentPortrait->SetVisible(true);
        //currentPortrait->SetAlpha(0.0);
    }

    if(previousPortrait && currentPortrait!=previousPortrait){
        previousPortrait->SetVisible(false);
        //currentPortrait->SetAlpha(0.0);
    }

    //if((currentPortrait || previousPortrait) && currentPortrait!=previousPortrait){
    //    alphaTween.Play();
    //}


    // --- Manage character name
    characterNameTextSprite->SetTextString(characterName);

    //---
    textField->Start(_textSegment);
}


JDialogBox dialogBox;


}
