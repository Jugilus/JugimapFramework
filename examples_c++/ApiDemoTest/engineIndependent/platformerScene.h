#ifndef API_DEMO_TEST__SCENE_H
#define API_DEMO_TEST__SCENE_H


#include "../jugimap/jugimap.h"
#include "entities.h"
#include "tileGrid.h"
#include "various.h"
#include "shapesDrawingLayer.h"



namespace apiTestDemo{


class ShapesDrawingLayer;



class PlatformerScene : public jugimap::Scene
{
public:


    virtual ~PlatformerScene() override;
    virtual bool Init() override;
    virtual void Start() override;
    virtual void Update() override;

    void UpdateProjectedMousePositions();


protected:



    jugimap::SourceGraphics *worldSourceGraphics = nullptr;             // OWNED
    jugimap::Map *worldMap = nullptr;                                   // OWNED
    jugimap::SourceGraphics *backgroundSourceGraphics = nullptr;        // OWNED
    jugimap::Map *backgroundMap = nullptr;                              // OWNED
    jugimap::SourceGraphics *guiSourceGraphics = nullptr;               // OWNED
    jugimap::Map *guiMap = nullptr;                                     // OWNED
    jugimap::Map *infoMap = nullptr;                                    // OWNED
    ShapesDrawingLayer *worldMapDrawingLayer = nullptr;                 // LINK
    ShapesDrawingLayer *testParallaxMapDrawingLayer = nullptr;          // LINK
    ShapesDrawingLayer *guiMapDrawingLayer = nullptr;                   // LINK
    jugimap::TextLayer *worldMapTextLayer = nullptr;                    // LINK
    jugimap::TextLayer *guiMapTextLayer = nullptr;                      // LINK
    jugimap::Font *font = nullptr;                                      //OWNED
    std::vector<jugimap::Text*>labels;                                  // LINKs

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

    //--- for managing InfoBox
    jugimap::Tween infoBoxAniTween;
    int infoBoxState = infoHIDDEN;
    static const int infoHIDDEN = 1;
    static const int infoSHOW_ANI = 2;
    static const int infoSHOWN = 3;
    static const int infoHIDE_ANI = 4;

    //--- starting zOrder value for map layering (for engines which handle sprites drawing order by the 'z-order' parameter)
    int zOrderBackgroundMap = 1000;
    int zOrderWorldMap = 1000;
    int zOrderTestParallaxMap = 1000;
    int zOrderGuiMap = 1000;
    int zOrderInfoMap = 1000;

    //--- parameters for precise text placements (so we can use one engine independent function for creating texts)
    jugimap::Vec2f buttonLabelOffset;
    int extraLabelsCount = 0;
    int extraLabelsYOffset = 30;
    float extraLabelsYSpacingFactor = 1.0;

    std::vector<int>skippedButtons;


    virtual void UpdateEngineObjects();
    virtual void SetPhysicsSimulationDisabled(bool _disabled){}
    virtual void SetDynamicCrystalsPhysics(){}
    virtual void UpdateTexts();
    virtual void AssignCamerasToMaps(){}

    void ManageDeactivateWalkers();
    void TestSpawnWalkers();
    void ManageInfoBox();
    void MakeButtonsAndTexts();



    enum ButtonTag
    {
        NONE = 0,
        SHOW_COLLISION_SHAPES = 1,
        SHOW_VECTOR_SHAPES = 2,
        PAUSE_CAMERA = 3,
        ZANY_CAMERA = 4,
        DOUBLE_CAMERA = 5,
        DYNAMIC_CRYSTALS = 6,
        PAUSE_SPRITES = 7,
        SPAWN_WALKERS = 8,
        SHOW_INFO = 9
    };

    std::vector<Button*>buttons;


};



}



#endif
