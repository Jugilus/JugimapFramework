#ifndef JUGIMAP_CAMERA_H
#define JUGIMAP_CAMERA_H

#include <cmath>
#include <vector>
#include <string>
#include "jmCommon.h"




namespace jugimap {



class ScreenMapCamera;
class WorldMapCamera;


/// \ingroup Cameras
/// \brief The base camera class.
class Camera
{
public:

    /// The camera kind.
    enum class Kind{SCREEN, WORLD, NOT_DEFINED};

    /// The camera change flags.
    enum Change{
        NONE = 0,
        POSITION = 1,
        SCALE = 2,
        ROTATION = 4,
        HANDLE = 8,
        ALL = POSITION | SCALE | ROTATION | HANDLE
    };



    /// Returns the kind of camera.
    Kind GetKind(){ return kind; }

    /// Returns the viewport - a screen rectangle where the map is projected.
    Rectf GetViewport(){ return viewport; }

    /// Returns the position of the projected map's origin point.
    Vec2f GetProjectedMapPosition() { return projectedMapPosition;}

    /// Returns the scale of projection.
    float GetScale(){ return scale; }

    /// Returns the rotation of projection.
    float GetRotation(){ return rotation; }

    /// Set the scale of projection.
    void SetScale(float _scale);

    /// Set the rotation of projection.
    void SetRotation(float _rotation);

    /// Returns the map point from the given **_screenPoint**.
    Vec2f MapPointFromScreenPoint(Vec2f _screenPoint){ return invMatrix.Transform(_screenPoint);}

    /// Returns the screen point from the given **_mapPoint**.
    Vec2f ScreenPointFromMapPoint(Vec2f _mapPoint){ return matrix.Transform(_mapPoint);}

    /// Returns the visible area of the map screen.
    Rectf GetMapVisibleRect(){return mapVisibleRect;}

    /// Returns the change value.
    int GetChangeFlags(){ return changeFlags; }

    /// Clears the change value.
    void ClearChangeFlags(){ changeFlags = 0; }



protected:
    Kind kind = Kind::NOT_DEFINED;
    float scale = 1.0;
    float rotation = 0.0;
    Vec2f projectedMapPosition;        // bottom-left corner
    Rectf viewport;
    AffineMat3f matrix;
    jugimap::AffineMat3f invMatrix;
    Rectf mapVisibleRect;
    int changeFlags = 0;


    virtual void UpdateTransformation() = 0;
};



/// \ingroup Cameras
/// \brief  A camera for screen maps.
///
/// The ScreenMapCamera object defines transformation from the map coordinates to the screen coordinates.
class ScreenMapCamera : public Camera
{
public:
    static bool allowMapScaling;
    static bool allowMapRotation;


    ScreenMapCamera(){ kind = Camera::Kind::SCREEN; }

    /// Initialize a screen map camera from the given parameters.
    void Init(Vec2i _screenMapDesignSize, Vec2f _mapPosition, float _mapScale=1.0, float _mapRotation=0.0, Vec2f _handle=Vec2f());

    /// Set the position of the map on the screen.
    void SetMapPosition(Vec2f _mapPosition);

    /// Set the handle point of the map.
    void SetMapHandle(Vec2f _mapHandle);

    /// Returns the size of the map;
    Vec2i GetScreenMapDesignSize(){ return screenMapDesignSize;}

    /// Returns the position of the map on the screen.
    Vec2f GetMapPosition(){ return mapPosition; }

    /// Returns the handle point of the map.
    Vec2f GetMapHandle(){ return mapHandle; }


protected:
    Vec2i screenMapDesignSize;
    Vec2f mapPosition;
    Vec2f mapHandle;

    virtual void UpdateTransformation() override;

};


/// \ingroup Cameras
/// \brief A camera for world maps and parallax maps.
/// The WorldMapCamera object defines transformation from the map coordinates to the screen viewport coordinates.
class WorldMapCamera : public Camera
{

public:
    static bool allowScaling;
    static bool allowRotation;


    WorldMapCamera(){ kind = Kind::WORLD; }

    /// Initialize a world camera from the given **_viewport** and **_worldMapSize**.
    void Init(Rectf _viewport, Vec2i _worldmapSize);

    /// Returns the pointed map point. That is the point which is shown in the center of the viewport.
    Vec2f GetPointedPosition();

    /// Set the pointed map point. That is the point which is shown in the center of the viewport.
    void SetPointedPosition(Vec2f _pointedPosition, bool _captureForLerpDrawing=false);

    ///\brief Set the *constrainedInScrollableRect* flag. This constrain the pointed map position to the scrollable area of the map in order
    /// to prevent outsides of the map coming into viewport. If camera scales or rotates the outside of the map may still come into viewport.
    void SetConstrainedInScrollableRect(bool _constrainedInScrollableRect);

    /// Returns true if *constrainedInScrollableRect* is set to true; otherwise returns false.
    bool IsConstrainedInScrollableRect(){ return constrainedInScrollableRect;}

    /// Returns the map size.
    jugimap::Vec2i GetWorldSize(){ return worldMapSize;}

    /// Returns the scrollable rectangle area of the map.
    Rectf GetMapScrollableRect(){ return mapScrollableRect; }


protected:
    Vec2i worldMapSize;
    Rectf mapScrollableRect;
    //---
    Vec2f pointedPosition;
    Vec2f previousPointedPosition;
    //---
    bool constrainedInScrollableRect = true;

    virtual void UpdateTransformation() override;

};



}



#endif


