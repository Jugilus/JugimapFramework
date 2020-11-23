#ifndef JUGIMAP_PLATFORMER_DEMO_TEST__DEMO_SCENE_H
#define JUGIMAP_PLATFORMER_DEMO_TEST__DEMO_SCENE_H


#include "../jugimap/jugimap.h"

#include "entities.h"
#include "tileGrid.h"
#include "various.h"
#include "shapesDrawingLayer.h"


namespace jugiApp{



class PlatformerScene : public jugimap::Scene
{
public:

    virtual bool Init() override;
    virtual void Start() override;
    virtual void Update() override;

    void UpdateInMapCursorPositions();


protected:

    jugimap::SourceGraphics *worldSourceGraphics = nullptr;             // LINK
    jugimap::Map *worldMap = nullptr;                                   // LINK
    jugimap::SourceGraphics *backgroundSourceGraphics = nullptr;        // LINK
    jugimap::Map *backgroundMap = nullptr;                              // LINK
    jugimap::SourceGraphics *guiSourceGraphics = nullptr;               // LINK
    jugimap::Map *guiMap = nullptr;                                     // LINK
    jugimap::Map *infoMap = nullptr;                                    // LINK
    ShapesDrawingLayer *worldMapDrawingLayer = nullptr;                 // LINK
    ShapesDrawingLayer *parallaxMapDrawingLayer = nullptr;              // LINK
    ShapesDrawingLayer *guiMapDrawingLayer = nullptr;                   // LINK
    jugimap::SpriteLayer *worldMapTextLayer = nullptr;                  // LINK
    std::vector<jugimap::TextSprite*>labels;                            // LINK

    jugimap::WorldMapCamera worldCamera;
    jugimap::WorldMapCamera worldCameraA;
    jugimap::WorldMapCamera worldCameraB;
    jugimap::ScreenMapCamera guiCamera;
    jugimap::ScreenMapCamera infoCamera;
    CameraController cameraController;
    CameraController cameraControllerA;
    CameraController cameraControllerB;

    //--- state
    bool doubleCamera = false;
    bool dynamicCrystals = false;
    bool showInfoBox = false;

    //---
    jugimap::Vec2f cursorInWorldMapPosition;
    jugimap::Vec2f cursorInGuiMapPosition;


    //--- for managing InfoBox
    jugimap::Tween infoBoxAniTween;
    int infoBoxState = infoHIDDEN;
    static const int infoHIDDEN = 1;
    static const int infoSHOW_ANI = 2;
    static const int infoSHOWN = 3;
    static const int infoHIDE_ANI = 4;

    virtual void SetPhysicsSimulationDisabled(bool _disabled){}
    virtual void SetDynamicCrystalsPhysics(){}
    virtual void UpdateTexts();
    virtual void AssignCamerasToMaps(){}

    void ManageDeactivateWalkers();
    void TestSpawnWalkers();
    void ManageInfoBox();
    void AddButtonsAndLabelsToGuiMap();
    //void MakeButtonsAndTextsOLD();


    enum ButtonTag
    {
        NONE = 0,
        SHOW_SPRITE_SHAPES = 1,
        SHOW_MAP_SHAPES = 2,
        PAUSE_CAMERA = 3,
        ZANY_CAMERA = 4,
        DOUBLE_CAMERA = 5,
        DYNAMIC_CRYSTALS = 6,
        PAUSE_SPRITES = 7,
        SPAWN_WALKERS = 8,
        SHOW_INFO = 9
    };

};


}






#endif
