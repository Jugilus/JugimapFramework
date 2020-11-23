#ifndef JUGIMAP_SCENE_H
#define JUGIMAP_SCENE_H

#include <chrono>
#include "jmCommon.h"
#include "jmGlobal.h"



namespace jugimap {


class Map;
class SourceGraphics;
class EngineScene;
class App;




///\ingroup MapElements
///\brief The Scene class is the base class for the application scenes.
class Scene
{
public:
    friend class App;


    ///\brief Constructor
    Scene();


    virtual bool Init() = 0;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void PreUpdate();
    virtual void PostUpdate();
    virtual void Draw();


    ///\brief Set a flag that indicated if this scene has been initialized.
    void SetInitialized(bool _initialized){ initialized = _initialized;}


    ///\brief Returns true if this scene has been initialized; otherwise returns false.
    bool IsInitialized(){ return initialized;}


    ///\brief Returns the engine scene object of this scene.
    EngineScene* GetEngineScene(){ return engineScene; }


    ///\brief Returns a reference to the vector of stored maps.
    std::vector<Map*>& GetMaps(){ return maps; }


    ///\brief Load a new SourceGraphics object from the given file *_filePath* and store it in this scene.
    ///
    /// \param _filePath The path of the Jugimap source graphic file exported from the editor.
    /// The function returns a link pointer to the added source graphics object.
    SourceGraphics* LoadAndAddSourceGraphics(const std::string &_filePath);


    ///\brief Load a new Map object from the given file *_filePath* and store it in this scene.
    ///
    /// \param _filePath The path of the Jugimap map file exported from the editor.
    /// \param _sourceGraphics The source graphics of this map.
    /// \param _mapType The map type.
    /// \param _mapZOrder The starting zOrder value of the map. The default value -1 means that the zOrder will be obtained automaticaly.
    /// Do not manually specify the zOrder unless you have a lot of maps in the scene.
    /// The function returns a link pointer to the added map object.
    Map* LoadAndAddMap(const std::string &_filePath, SourceGraphics* _sourceGraphics, MapType _mapType, int _mapZOrder = -1);


    ///\brief Add a new empty Map object to this scene.
    ///
    /// \param _name The map name.
    /// \param _mapType The map type.
    /// \param _mapZOrder The starting zOrder value of the map. The default value -1 means that the zOrder will be obtained automaticaly.
    /// Do not manually specify the zOrder unless you have a lot of maps in the scene.
    /// The function returns a link pointer to the added map object.
    Map* AddMap(const std::string &_name, MapType _mapType, int _mapZOrder = -1);


    ///\brief Returns a reference to the vector of stored source graphics.
    std::vector<SourceGraphics*>& GetSourceGraphics(){ return sourceGraphics; }


protected:

    ///\brief Destructor.
    virtual ~Scene();


private:
    bool initialized = false;
    int tag = 0;

    EngineScene* engineScene = nullptr;                 // owned
    std::vector<Map*>maps;                              // owned
    std::vector<SourceGraphics*>sourceGraphics;         // owned

};



///\ingroup MapElements
///\brief The EngineScene class defines engine specific scene properties.
class EngineScene
{
public:

    EngineScene(Scene *_scene) : scene(_scene){}
    virtual ~EngineScene(){}


    ///\brief Interface function for running engine specific code before the scene initialization.
    virtual void PreInit(){}


    ///\brief Interface function for running engine specific code after the scene initialization.
    virtual void PostInit(){}


    ///\brief Interface function for running engine specific code before the scene update.
    virtual void PreUpdate(){}


    ///\brief Interface function for running engine specific code after the scene update.
    virtual void PostUpdate(){}


    ///\brief Returns the scene object.
    Scene* GetScene(){ return scene; }


protected:
    Scene *scene = nullptr;             // LINK

};



}

#endif


