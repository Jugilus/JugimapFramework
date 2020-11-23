#ifndef JUGIMAP_BINARY_LOADER_H
#define JUGIMAP_BINARY_LOADER_H

#include <string>
#include <iostream>
#include <fstream>

#include "jmCommon.h"



namespace jugimap{



class SourceGraphics;
class Map;
class GraphicFile;
class GraphicItem;
class SourceSprite;
class VectorShape;
class FrameAnimation;
class TimelineAnimation;
class AnimationTrack;
class Layer;
class SpriteLayer;
class VectorLayer;
class Sprite;
class ComposedSprite;
class ObjectFactory;
class BinaryStreamReader;



/// \ingroup MapLoaders
/// \brief The JugiMapBinaryLoader class loads data from jugimap binary files (.jmb).
class JugiMapBinaryLoader
{
public:
    friend class ComposedSprite;


    static std::string pathPrefix;                              ///<  An optional string attached to the *filePath* parameter in the loading functions. It must end with slash '/'!
    static std::string error;                                   ///<  Error message if loading fails.
    static std::vector<std::string> messages;                   ///<  Messages about skipped chunks of data during the loading.
    //static bool usePixelCoordinatesForTileLayerSprites; 		///<  Sprites on the editor's tile layers have positions stored in grid coordinates. If this parameter is *true*, the coordinates will be converted to pixel coordinates.

    static int zOrderCounterStart;                              ///<  Starting z-order for loaded layers (for engines which manages drawing order of sprites via z-order parameter).
    static int zOrderCounter;                                   ///<  Z-order counter for loaded layers.
    //static int zOrderCounterStep;                               ///<  Z-order counter step.


    /// Loads source data from a jugimap binary file (.jmb) at the given **_filePath** into an empty **_SourceGraphics**. Returns *true* if loading succeeds, otherwise returns *false*.
    static bool LoadSourceGraphics(std::string _filePath, SourceGraphics *_sourceGraphics);

    /// Loads map data from a jugimap binary file (.jmb) at the given **_filePath** using the associated **_SourceGraphics**. Returns *true* if loading succeeds, otherwise returns *false*.
    static bool LoadMap(std::string _filePath, jugimap::Map *_map, SourceGraphics *_sourceGraphics);


private:
    Map* map = nullptr;
    SourceGraphics* sourceGraphics = nullptr;
    ComposedSprite* loadedComposedSprite = nullptr;
    int indexBeginSourceSprites = 0;
    int indexBeginImageFiles = 0;


    static void Reset();

    bool _LoadSourceGraphics(BinaryStreamReader &stream, int size);
    jugimap::GraphicFile *LoadGraphicFile(BinaryStreamReader &stream, int size);
    jugimap::GraphicItem *LoadGraphicItem(BinaryStreamReader &stream, int size);
    jugimap::SourceSprite *LoadSourceSprite(BinaryStreamReader &stream, int size);
    ComposedSprite *LoadComposedSpriteData(BinaryStreamReader &stream, int size);
    bool _LoadMap(BinaryStreamReader &stream, int size);
    jugimap::SpriteLayer *LoadLayer(BinaryStreamReader &stream, int size);
    bool LoadSpriteData(BinaryStreamReader &stream, Sprite *s);
    jugimap::VectorLayer *LoadVectorLayer(BinaryStreamReader &stream, int size);
    jugimap::VectorShape *LoadVectorShape(BinaryStreamReader &stream, int size, bool vectorLayerShape=true);
    jugimap::FrameAnimation *LoadFrameAnimation(BinaryStreamReader &stream, int size, jugimap::SourceSprite *ss);
    jugimap::TimelineAnimation *LoadTimelineAnimation(BinaryStreamReader &stream, int size, jugimap::SourceSprite *ss);
    AnimationTrack *LoadAnimationTrack(BinaryStreamReader &stream, int size);
    bool LoadParameters(BinaryStreamReader &stream, std::vector<Parameter> &parameters);
    void LoadParallaxAndScreenLayerParameters(BinaryStreamReader &stream, Layer *layer);
    void InitCollisionShapes(GraphicItem *gi);


    struct SaveSignature
    {
        static const int FORMAT = 88880;
        static const int FORMAT_VERSION = 1;

        static const int EXPORTED_MAP_FORMAT = 99990;
        static const int EXPORTED_MAP_FORMAT_VERSION = 2;

        static const int EXPORTED_SOURCE_GRAPHICS_FORMAT = 100000;
        static const int EXPORTED_SOURCE_GRAPHICS_FORMAT_VERSION = 2;

        static const int HEADER = 100;
        static const int MISC_DATA = 110;
        static const int THUMBNAIL = 120;

        static const int VARIABLES = 200;
        static const int CHILD_ITEMS = 210;
        static const int PARAMETERS = 220;
        static const int CONSTANT_PARAMETERS = 230;
        static const int PARALLAX_LAYER_PARAMETERS = 240;
        static const int SCREEN_LAYER_PARAMETERS = 250;

        static const int SOURCE_SETS = 300;
        static const int SOURCE_SET = 310;
        static const int SOURCE_OBJECTS = 330;
        static const int SOURCE_OBJECT = 340;

        static const int SOURCE_IMAGE_FILES = 400;
        static const int SOURCE_IMAGE_FILE = 410;
        static const int SOURCE_IMAGES = 420;
        static const int SOURCE_IMAGES_V2 = 421;
        static const int SOURCE_IMAGE = 430;

        static const int MAP = 500;
        static const int LAYERS = 510;
        static const int LAYERS_GROUP = 520;
        static const int LAYER = 530;
        static const int VECTOR_LAYERS_GROUP = 540;
        static const int VECTOR_LAYER = 550;
        static const int STATIC_LAYER = 560;

        static const int OBJECTS = 700;
        static const int OBJECTS_V2 = 701;
        static const int OBJECT = 710;
        static const int VECTOR_SHAPES = 750;
        static const int VECTOR_SHAPES_V2 = 751;
        static const int VECTOR_SHAPE = 760;
        static const int VECTOR_SHAPE_CONTROL_POINTS = 770;

        static const int COMPOSED_SPRITE = 800;

        static const int ANIMATIONS = 900;
        static const int FRAME_ANIMATIONS = 950;
        static const int FRAME_ANIMATION = 960;
        static const int ANIMATION_FRAMES = 970;


        static const int TIMELINE_ANIMATIONS = 1000;
        static const int TIMELINE_ANIMATION = 1010;
        static const int ANIMATION_MEMBERS = 1020;
        static const int ANIMATION_TRACK = 1030;
        static const int ANIMATION_KEYS = 1040;
        static const int AK_TRANSLATION = 1100;
        static const int AK_ROTATION = 1110;
        static const int AK_SCALING = 1120;
        static const int AK_FLIP_HIDE = 1130;
        static const int AK_ALPHA_CHANGE = 1140;
        static const int AK_OVERLAY_COLOR_CHANGE = 1150;
        static const int AK_PATH_MOVEMENT = 1160;
        static const int AK_FRAME_CHANGE = 1170;
        static const int AK_FRAME_ANIMATION = 1180;
        static const int AK_TIMELINE_ANIMATION = 1190;
        static const int AK_META_TRACK = 1250;

    };

    const int saveID_DUMMY_SIF = -10;
    //const int saveID_DUMMY_SIF_TEXT_SPRITE = -15;
    const int saveID_DUMMY_COMPOSED_SPRITE = -20;
    //const int saveID_DUMMY_EMPTY_COMPOSED_SPRITE = -30;
    static const int saveID_DUMMY_EMPTY_FRAME = -40;


    // JMParameter kind
    static const int jmINTEGER = 0;
    static const int jmFLOAT = 1;
    static const int jmBOOLEAN = 2;
    static const int jmSTRING = 3;
    static const int jmVALUES_SET = 4;

    // layer kind
    static const int jmTILE_LAYER = 0;
    static const int jmSPRITE_LAYER = 1;
    static const int jmVECTOR_LAYER = 2;
    static const int jmSTATIC_SINGLE_SPRITE_LAYER = 3;

    // file kind
    static const int jmSINGLE_IMAGE = 0;
    static const int jmTILE_SHEET_IMAGE = 1;
    static const int jmSPRITE_SHEET_IMAGE = 2;
    static const int jmSPINE_FILE = 3;
    static const int jmSPRITER_FILE = 4;
    static const int jmTEXT_OBJECT = 5;

    // shape kind
    static const int jmRECTANGLE = 1;
    static const int jmPOLYLINE = 2;
    static const int jmBEZIER_POLYCURVE = 3;
    static const int jmELLIPSE = 4;
    static const int jmSINGLE_POINT = 5;

    // parallax layer modes
    //static const int typeSTANDARD_PARALLAX_LAYER = 0;
    //static const int typeSTRETCHING_SINGLE_SPRITE_LAYER = 1;

    static const int typePARALLAX_LAYER = 0;
    static const int typePARALLAX_STRETCHING_SINGLE_SPRITE = 1;
    static const int typeSCREEN_LAYER = 2;
    static const int typeSCREEN_STRETCHING_SINGLE_SPRITE = 3;


    static const int stretchXY_WORLD_SIZE = 0;
    static const int stretchXY_VIEWPORT_SIZE = 1;

    //static const int plNO_CHANGE = 0;
    //static const int plTILING_X = 1;
    //static const int plTILING_Y = 2;
    //static const int plTILING_XY = 3;
    //static const int plSINGLE_SPRITE_STRETCH_X = 4;
    //static const int plSINGLE_SPRITE_STRETCH_Y = 5;
    //static const int plSINGLE_SPRITE_STRETCH_XY = 6;

    //--- align
    //static const int jmALIGN_X_LEFT = 0;
    //static const int jmALIGN_X_MIDDLE = 1;
    //static const int jmALIGN_X_RIGHT = 2;

    //---
    //static const int jmALIGN_Y_TOP = 0;
    //static const int jmALIGN_Y_MIDDLE = 1;
    //static const int jmALIGN_Y_BOTTOM = 2;



    //-------------------------------

    struct FVectorVertex
    {
        int x=-1, y=-1;
        float xBPprevious;       // used for bezier curves
        float yBPprevious;
        float xBPnext;
        float yBPnext;
    };

    struct FVectorShape
    {
        int kind = -1;                                  // JMVectorShape::RECTANGLE, JMVectorShape::POLYLINE, JMVectorShape::BEZIER_POLYCURVE or JMVectorShape::ELLIPSE
        bool closed = false;                            // closed polyline or bezier curve
        std::string name;                             // custom string
        int id = 0;                                     // custom integer
        int dataFlags = 0;                              // custom integer
        bool probe = false;                             // custom identifier
        std::vector<FVectorVertex>vertices;
        std::vector<Parameter> parameters;
    };

};




}





#endif
