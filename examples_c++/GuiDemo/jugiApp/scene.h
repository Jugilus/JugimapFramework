#ifndef API_DEMO_TEST__SCENE_H
#define API_DEMO_TEST__SCENE_H


#include "../jugimap/jugimap.h"


namespace jugiApp{

class DemoScene : public jugimap::Scene
{
public:

    bool Init() override;
    void Start() override;
    void Update() override;
    //void Draw() override;


protected:

    jugimap::SourceGraphics *sourceGraphics = nullptr;              // LINK
    jugimap::Map* headerMap = nullptr;                              // LINK
    jugimap::Map* commonWidgetsMap = nullptr;                       // LINK
    jugimap::Map* textWidgetsMap = nullptr;                         // LINK
    jugimap::Map* composedDesignsMap = nullptr;                     // LINK
    jugimap::Map* overlayMap = nullptr;                             // LINK
    jugimap::Map* shownMap = nullptr;                               // LINK

    // cameras
    jugimap::ScreenMapCamera headerCamera;
    jugimap::ScreenMapCamera commonCamera;
    jugimap::ScreenMapCamera overlayCamera;

    // stored object pointers for quick access
    jugimap::TextSprite *interactedWidgetInfo = nullptr;        // LINK
    jugimap::GuiTextField *textField = nullptr;                 // LINK
    jugimap::TextSegment *textSegment = nullptr;                // LINK
    jugimap::TextBook *textBook = nullptr;                      // LINK



    void UpdateInteractedWidgetInfo();
    void SetKeyboardHighlightingWidgets();

    //void TestMouse();
    //void TestKeyboard();
    //void TestJostick();
    //void TestTouch();

};


// ===============================================================================
//      INVENTORY
// ===============================================================================


struct InventoryItem
{
    std::string name;
    jugimap::Sprite *sprite = nullptr;                      // LINK
    int amount = 0;
    bool selected = false;
    int slotIndex = -1;

    InventoryItem(const std::string &_name, jugimap::Sprite *_sprite, int _amount) : name(_name), sprite(_sprite), amount(_amount){}
};


struct InventorySlot
{
    jugimap::GuiButton* slotButton = nullptr;               // LINK
    InventoryItem* item = nullptr;                          // LINK
    jugimap::TextSprite *amountSprite = nullptr;            // LINK

    static jugimap::Vec2f itemSpritePos;
    static jugimap::Vec2f amountSpritePos;
    static jugimap::ColorRGBA amountColorRGBA;
    static jugimap::ColorRGBA amountColorRGBA_selectedItem;
};


struct InventorySlotObjectLink : public jugimap::CustomObject
{

    InventoryItem * item = nullptr;                         // LINK
    std::string GetInfo() override;

};


struct InventorySlotCallback : public jugimap::GuiWidgetCallback
{

    void OnPressed(jugimap::GuiWidget *_widget) override;

};


class Inventory
{
public:

    void Build(jugimap::SourceGraphics *sourceGraphics, jugimap::Map* map);

    void Update();
    void UpdateInventorySlots();

    std::vector<InventorySlot>& GetSlots(){ return inventorySlots; }

private:
    std::vector<InventoryItem>inventoryItems;
    std::vector<InventorySlot>inventorySlots;
    jugimap::GuiSlider *inventorySlider = nullptr;           // LINK

};


extern Inventory inventory;



// ===============================================================================
//      DIALOG BOX
// ===============================================================================


class JDialogBox
{
public:

    void Build(jugimap::SourceGraphics *sourceGraphics, jugimap::Map* map);

    void StartSequence(jugimap::TextSequence * _sequence);
    void Update();

private:

    jugimap::GuiTextField *textField = nullptr;             // LINK
    jugimap::GuiButton *buttonUp = nullptr;                 // LINK
    jugimap::GuiButton *buttonDown = nullptr;               // LINK
    jugimap::TextSequence * sequence = nullptr;             // LINK
    int currentTextSegmentIndex = -1;

    jugimap::StandardSprite *portraitConan = nullptr;       // LINK
    jugimap::StandardSprite *portraitYasmina = nullptr;     // LINK
    jugimap::StandardSprite *currentPortrait = nullptr;     // LINK
    jugimap::StandardSprite *previousPortrait = nullptr;     // LINK

    jugimap::TextSprite *characterNameTextSprite;
    //jugimap::Tween alphaTween;


    void StartTextSegment(jugimap::TextSegment* _textSegment);


};

extern JDialogBox dialogBox;


}


#endif
