#include <string>
#include <sstream>
#include "jmObjectFactory.h"
#include "jmMapBinaryLoader.h"
#include "jmSourceGraphics.h"
#include "jmMap.h"
#include "jmSprites.h"
#include "jmFont.h"
#include "jmInput.h"
#include "jmCamera.h"
#include "jmGuiCommon.h"
#include "jmScene.h"



namespace jugimap {




Scene::Scene()
{
    engineScene = objectFactory->NewEngineScene(this);
}


Scene::~Scene()
{

    //delete engineScene;

    //for(Map* m : maps){
    //    delete m;
    //}

    //for(SourceGraphics* sg : sourceGraphics){
    //    delete sg;
    //}

}


void Scene::PreUpdate()
{

    engineScene->PreUpdate();

    //----
    if(mouse.GetCursorSprite()){
        mouse.GetCursorSprite()->SetPosition(Vec2f(mouse.GetX(), mouse.GetY()));
    }


    guiKeyboardAndJoystickInput.Update();

    guiCursorDeviceInput._SetCursorScreenPosition(Vec2f(mouse.GetX(), mouse.GetY()));
    guiCursorDeviceInput._SetCursorPressed(mouse.IsButtonPressed(MouseButton::LEFT));
    guiCursorDeviceInput._SetCursorDown(mouse.IsButtonDown(MouseButton::LEFT));

    for(Map *m : maps){
        if(m->IsVisible()==false) continue;

        Vec2f cursorInMapPosition = m->GetCamera()->MapPointFromScreenPoint(guiCursorDeviceInput.GetCursorScreenPosition());
        //m->_SetCursorInMapPosition(cursorInMapPosition);
        guiCursorDeviceInput._SetCursorInMapPosition(cursorInMapPosition);

        //bool cursorInsideMapViewport =

        //if(m->GetName()=="commonWidgets"){
        //    DbgOutput("Cursor screen pos x:" + std::to_string(guiCursorDeviceInput.GetCursorScreenPosition().x) + " y:" + std::to_string(guiCursorDeviceInput.GetCursorScreenPosition().y)
        //               + " map pos x:" + std::to_string(guiCursorDeviceInput.GetCursorInMapPosition().x) + " y:" + std::to_string(guiCursorDeviceInput.GetCursorInMapPosition().y));
        //}

        m->UpdateWidgets();
    }

    //----
    time.UpdatePassedTimeMS();

}


void Scene::PostUpdate()
{

    for(Map *m : maps){
        if(m->IsVisible()==false) continue;
        m->UpdateEngineObjects();
    }

    engineScene->PostUpdate();

    //----
    for(Map *m : maps){
        m->GetCamera()->ClearChangeFlags();
    }

    mouse.ResetPerUpdateFlags();
    keyboard.ResetPerUpdateFlags();
    touch.ResetPerUpdateFlags();
    for(Joystick &gc: joysticks) gc.ResetPerUpdateFlags();
    GuiWidget::ResetInteractedPerUpdate();

}


void Scene::Draw()
{

    for(Map *m : maps){
        if(m->IsVisible()==false) continue;
        m->DrawEngineObjects();
    }

}


SourceGraphics* Scene::LoadAndAddSourceGraphics(const std::string &_filePath)
{

    SourceGraphics* sg = new SourceGraphics();

    if(JugiMapBinaryLoader::LoadSourceGraphics(_filePath, sg)){
        sourceGraphics.push_back(sg);

    }else{
        delete sg;
        sg = nullptr;

        settings.SetErrorMessage(JugiMapBinaryLoader::error);
        if(settings.GetErrorMessage()==""){
            settings.SetErrorMessage("Error loading map: " + _filePath);
        }
    }

    return sg;
}


Map* Scene::LoadAndAddMap(const std::string &_filePath, SourceGraphics* _sourceGraphics, MapType _mapType, int _mapZOrder)
{

    if(_mapZOrder==-1){
        _mapZOrder = settings.GetMapZOrderStart() + maps.size() * settings.GetMapZOrderStep();
    }

    Map *map = objectFactory->NewMap(_mapZOrder);
    if(JugiMapBinaryLoader::LoadMap(_filePath, map, _sourceGraphics)){
        maps.push_back(map);
        map->InitEngineObjects(_mapType);

    }else{
        delete map;
        map = nullptr;

        settings.SetErrorMessage(JugiMapBinaryLoader::error);
        if(settings.GetErrorMessage()==""){
            settings.SetErrorMessage("Error loading map: " + _filePath);
        }
    }

    return map;
}


Map* Scene::AddMap(const std::string &_name, MapType _mapType, int _mapZOrder)
{

    if(_mapZOrder==-1){
        _mapZOrder = settings.GetMapZOrderStart() + maps.size() * settings.GetMapZOrderStep();
    }
    Map *map = objectFactory->NewMap(_mapZOrder);
    maps.push_back(map);
    map->InitEngineObjects(_mapType);

    return map;
}









}
