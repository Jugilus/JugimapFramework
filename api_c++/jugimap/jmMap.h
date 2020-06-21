#ifndef JUGIMAP_MAP_H
#define JUGIMAP_MAP_H


#include "jmCommon.h"
#include "jmGlobal.h"


namespace jugimap {

class Layer;
class SpriteLayer;
class VectorLayer;
class Sprite;
class VectorShape;
class JugiMapBinaryLoader;
class Camera;



class Map
{
public:
    friend class JugiMapBinaryLoader;



    /// Constructor.
    Map(){}


    /// Destructor.
    virtual ~Map();

    Map(const Map &l) = delete;
    Map& operator=(const Map &l) = delete;

    ///\brief Delete the map content.
    ///
    /// This function deletes layers and their content (sprites, vector shapes, ...).
    /// SourceGraphics objects (source sprites, textures, ...) assigned to map objects are **not** deleted!
    virtual void DeleteContent();


    ///\brief Initialize this map as a word map.
    virtual void InitWorldMap();


    ///\brief Initialize this map as a parallax map linked to a world map with the given size *_worldMapSize*.
    virtual void InitParallaxMap(Vec2i _worldMapSize);


    ///\brief Initialize this map as a screen map with the given design size *_screenMapDesignSize*.
    virtual void InitScreenMap(Vec2i _screenMapDesignSize);


    ///\brief Initialize the engine map objects.
    ///
    /// This function initialize the engine objects of all map elements. It should be called after a map was initialized by type.
    virtual void InitEngineMap();


    ///\brief Update the engine map objects.
    ///
    /// This function must be called before the engine begin with drawing (rendering).
    virtual void UpdateEngineMap();


    ///\brief Draw the engine map objects.
    ///
    /// This function is needed only for the engines which do not handle drawing (rendering) internally.
    virtual void DrawEngineMap();


    ///\brief Assign a camera to this map.
    ///
    /// A camera must be assigned before calling the UpdateEngineMap() function.
    /// The type of an assigned camera depends of the map type:
    /// - For a world map use a WorldMapCamera camera.
    /// - For a parallax map use the same camera as is used for the world map.
    /// - For a screen map use a ScreenMapCamera.
    void SetCamera(Camera *_camera) { camera = _camera; }


    ///\brief Returns the type of this map.
    MapType GetType(){ return type; }


    ///\brief Returns the tile size of this map.
    Vec2i GetTileSize() {return tileSize; }


    ///\brief Returns the number of tile rows and columns.
    Vec2i GetTilesCount() {return nTiles; }


    ///\brief Returns the camera assigned to this map.
    Camera* GetCamera(){ return camera; }


    ///\brief Returns a reference to the layers of this map.
    std::vector<Layer*>& GetLayers() {return layers; }


    ///\brief Returns a reference to the parameters of this map.
    std::vector<jugimap::Parameter>& GetParameters() {return parameters; }


    ///\brief Returns the world map size.
    ///
    /// The returned value depends of the map type:
    /// - For a world map it is the size of the map.
    /// - For a parallax map it is the size of the linked world map.
    /// - For a screen map it is not defined (zero vector).
    /// This function should not be called before the map initalization (calling InitWorldMap, InitParallaxMap or InitScreenMap)!
    Vec2i GetWorldMapSize() { return worldMapSize; }


    ///\brief Returns the screen map design size.
    ///
    /// The returned value depends of the map type:
    /// - For world maps and parallax maps it is not defined.
    /// - For a screen map it is the design size set at initialization.
    /// This function should not be called before the map initalization (calling InitWorldMap, InitParallaxMap or InitScreenMap)!
    Vec2i GetScreenMapDesignSize() { return screenMapDesignSize; }


    ///\brief Returns the size of this map.
    ///
    /// This function should not be called before the map initalization (calling InitWorldMap, InitParallaxMap or InitScreenMap)!
    //Vec2i GetSize() { return Vec2i(boundingBox.Width(), boundingBox.Height());}


    void ModifyXParallaxFactorsForFixedMapWidth(Vec2i _designViewport);

    void ModifyYParallaxFactorsForFixedMapHeight(Vec2i _designViewport);


    ///\brief Capture the sprite properties, usually the position only,  required for the *lerp drawing*.
    ///
    /// \see Globals::SetUseLerpDrawing
    void CaptureForLerpDrawing();


    ///\brief Adds the given *_layer* to this map.
    ///
    /// If the *_index* is specified the layer will be inserted at that position in the layers vector; otherwise it will be added at the end.
    /// This function is available for the cases where we manually create maps or layers.
    void _AddLayer(jugimap::Layer *_layer, int _index=-1);


    ///\brief Sets the name of this map to the given *_name*.
    ///
    /// This function is available for the cases where we manually create maps.
    void _SetName(const std::string &_name){ name = _name; }


    ///\brief Sets the name of this map to the given *_name*.
    ///
    /// This function is available for the cases where we manually create maps.
    void _SetType(MapType _type){ type = _type; }


    ///\brief Sets the starting z-order.
    ///
    /// This function should be used before loading the map to set starting z-order value for layers.
    /// \see GetZOrderStart
    void _SetZOrderStart(int _zOrderStart){ zOrderStart = _zOrderStart; }


    ///\brief Returns the starting z-order value.
    ///
    /// \see _SetZOrderStart
    int GetZOrderStart(){ return zOrderStart; }


    ///\brief Returns the next z-order to be used with a next (manually) added layer to the map.
    int GetNextZOrder();



protected:

    virtual void InitMapParameters();
    void SetLayersPlanes();
    //void TileLayers();
    void UpdateBoundingBox();
    //void SetAlignOffsetsFromMap();
    void Normalize();



private:
    std::string name;
    Vec2i tileSize;
    Vec2i nTiles;
    std::vector<jugimap::Parameter> parameters;
    std::vector<Layer*>layers;                         // OWNED

    //---
    MapType type = MapType::WORLD;
    Vec2i worldMapSize;                                 // used for WORLD and PARALLAX maps
    Vec2i screenMapDesignSize;                          // used for SCREEN maps
    Rectf boundingBox;

    //---
    Camera* camera = nullptr;                           // LINK

    int zOrderStart = 1000;

};



}


/// \ingroup MapElements
/// \class jugimap::Map
/// \brief The root map element.
///
/// The Map class represents a map created in JugiMap Editor.
/// It stores and owns Layer objects.
///
/// There are three types of maps:
/// 1. World map
/// 2. Parallax map
/// 3. Screen map
///
/// The type defines how a map will be displayed in game. A map loaded from a file has no defined type - we define its type by initializing it
/// according to its purpose. Of course, a map need to be designed according to its purpose as well.
///
/// An object which controls how a map is displayed is the Camera. Every map must have assigned a camera according to its type: world and parallax maps
/// must have assigned the same WorldMapCamera object, a screen map must have assigned a ScreenMapCamera object.
///
/// Another required step of the map setup is initialization of engine objects. Jugimap extended classes wrap
/// engine (game development framework) objects and these objects need to be created and initialized before they can be used.
///
/// The code bellow shows how to load and initialize the maps:
///
/// \code
///
///    // Load source graphics used by the maps
///
///    SourceGraphics* sourceGraphics = new SourceGraphics();
///    if(JugiMapBinaryLoader::LoadSourceGraphics(sourceGraphicsFilePath, sourceGraphics)==false){
///        // load error !
///    }
///
///    SourceGraphics* guiSourceGraphics = new SourceGraphics();
///    if(JugiMapBinaryLoader::LoadSourceGraphics(guiSourceGraphicsFilePath, guiSourceGraphics)==false){
///        // load error !
///    }
///
///
///    // Load maps
///
///    Map* levelMap = glob::objectFactory->NewMap();
///    if(JugiMapBinaryLoader::LoadMap(levelMapFilePath, levelMap, sourceGraphics)==false){
///        // load error !
///    }
///
///    Map* backgroundMap = glob::objectFactory->NewMap();
///    if(JugiMapBinaryLoader::LoadMap(backgroundMapFilePath, backgroundMap, sourceGraphics)==false){
///        // load error !
///    }
///
///    Map* guiMap = glob::objectFactory->NewMap();
///    if(JugiMapBinaryLoader::LoadMap(guiMapFilePath, guiMap, guiSourceGraphics)==false){
///        // load error !
///    }
///
///
///    // Initilaize maps by type.
///
///    // levelMap is a world map and we initialize it as such.
///    // We must initialize world maps before parallax maps as these require world map size as a parameter.
///    levelMap->InitWorldMap();
///
///    // backgroundMap is a parallax map and we initialize it as such.
///    backgroundMap->InitParallaxMap(levelMap->GetWorldMapSize());
///
///    // guiMap is a screen map and we initialize it as such.
///    // We need to specify a box "guiMapDesignSize" inside which the gui map layers will be aligned.
///    Vec2i guiMapDesignSize(270, glob::screenSize.y);
///    guiMap->InitScreenMap(guiMapDesignSize);
///
///
///    // Initilaize engine map objects.
///
///    levelMap->InitEngineMap();
///    backgroundMap->InitEngineMap();
///    guiMap->InitEngineMap();
///
///
///     // Set cameras
///
///    // levelMap requires a WorldMapCamera which will transform the world map into screen coordinates relative to specified viewport.
///    WorldMapCamera* levelCamera = new WorldMapCamera();
///    levelCamera->Init(Rectf(270,0, glob::screenSize.x, glob::screenSize.y), levelMap->GetWorldMapSize());
///    levelMap->SetCamera(levelCamera);            // A world map must have assigned a WorldMapCamera object !
///
///    // backgroundMap requires the same WorldMapCamera object.
///    backgroundMap->SetCamera(levelCamera);
///
///    // guiMap requires ScreenMapCamera which will transform the screen map guiMapDesignSize into screen coordinates.
///    ScreenMapCamera* guiCamera = new ScreenMapCamera();
///    Vec2f guiMapPosition = Vec2f(0,0);
///    guiCamera->Init(guiMapDesignSize, guiMapPosition);
///    guiMap->SetCamera(guiCamera);
///
/// \endcode
///
/// The maps were created via objectFactory->NewMap(). We must **always use object factory** to create objects of the engine extended classes!
///
/// See \ref Camera for more information about cameras.
///





#endif
