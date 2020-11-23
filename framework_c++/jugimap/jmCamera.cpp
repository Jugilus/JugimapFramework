#include <algorithm>
#include "jmCommonFunctions.h"
#include "jmGlobal.h"
#include "jmMap.h"
#include "jmCamera.h"



namespace jugimap {




void Camera::SetScale(float _scale)
{
    scale = _scale;
    changeFlags |= Change::SCALE;
    UpdateTransformation();
}


void Camera::SetRotation(float _rotation)
{
    rotation = _rotation;
    changeFlags |= Change::ROTATION;
    UpdateTransformation();
}




//====================================================================


bool ScreenMapCamera::allowMapScaling = true;
bool ScreenMapCamera::allowMapRotation = true;


void ScreenMapCamera::Init(Vec2i _screenMapDesignSize, Vec2f _mapPosition, float _mapScale, float _mapRotation, Vec2f _handle)
{

    screenMapDesignSize = _screenMapDesignSize;
    mapPosition = _mapPosition;
    scale = _mapScale;
    rotation = _mapRotation;
    mapHandle = _handle;
    changeFlags = Change::ALL;
    UpdateTransformation();
}


void ScreenMapCamera::SetMapHandle(Vec2f _mapHandle)
{
    mapHandle = _mapHandle;
    changeFlags |= Change::HANDLE;
    UpdateTransformation();
}


void ScreenMapCamera::SetMapPosition(Vec2f _mapPosition)
{
    mapPosition = _mapPosition;
    changeFlags |= Change::POSITION;
    UpdateTransformation();
}


void ScreenMapCamera::UpdateTransformation()
{

    if(allowMapScaling==false){
        scale = 1.0;
        changeFlags &= ~Change::SCALE;   //clear 'changedSCALE' bit
    }
    if(allowMapRotation==false){
        rotation = 0.0;
        changeFlags &= ~Change::ROTATION;
    }

    //---
    matrix = MakeTransformationMatrix(mapPosition, Vec2f(scale,scale), Vec2i(), rotation, mapHandle);
    invMatrix = matrix.Invert();

    projectedMapPosition = matrix.Transform(Vec2f(0.0, 0.0));
    viewport = TransformRectf(Rectf(0,0,screenMapDesignSize.x,screenMapDesignSize.y), matrix);
    //mapVisibleRect = Rectf(0,0, glob::screenSize.x, glob::screenSize.y);
    mapVisibleRect = Rectf(0,0, settings.GetScreenSize().x, settings.GetScreenSize().y);
}


//====================================================================


bool WorldMapCamera::allowScaling = true;
bool WorldMapCamera::allowRotation = true;


void WorldMapCamera::Init(jugimap::Rectf _viewport, Vec2i _worldmapSize)
{
    viewport = _viewport;
    worldMapSize = _worldmapSize;
    changeFlags = Change::ALL;
    UpdateTransformation();
}


/*
void WorldMapCamera::InitPointedPosition(Vec2f _pointedPosition)
{
    pointedPosition = previousPointedPosition = _pointedPosition;
    changeFlags |= Change::POSITION;
    UpdateTransformation();
}
*/


void WorldMapCamera::SetPointedPosition(Vec2f _pointedPosition, bool _captureForLerpDrawing)
{
    if(_captureForLerpDrawing){
        pointedPosition = previousPointedPosition = _pointedPosition;
    }
    previousPointedPosition = pointedPosition;
    pointedPosition = _pointedPosition;
    changeFlags |= Change::POSITION;
    UpdateTransformation();
}


void WorldMapCamera::SetConstrainedInScrollableRect(bool _constrainedInScrollableRect)
{
    constrainedInScrollableRect = _constrainedInScrollableRect;
    UpdateTransformation();
}


Vec2f WorldMapCamera::GetPointedPosition()
{

    //if(settings.LerpDrawingEnabled()){
    //    return previousPointedPosition + (pointedPosition - previousPointedPosition) * settings.GetLerpDrawingFactor();
    //}
    return pointedPosition;
}



void WorldMapCamera::UpdateTransformation()
{

    if(allowScaling==false){
        scale = 1.0;
        changeFlags &= ~Change::SCALE;
    }
    if(allowRotation==false){
        rotation = 0.0;
        changeFlags &= ~Change::ROTATION;
    }

    if(constrainedInScrollableRect){
        Vec2f mapNoScrollingBorder((viewport.GetWidth()/2.0)/scale, (viewport.GetHeight()/2.0)/scale);
        if(mapNoScrollingBorder.x>worldMapSize.x/2.0) mapNoScrollingBorder.x = worldMapSize.x/2.0;
        if(mapNoScrollingBorder.y>worldMapSize.y/2.0) mapNoScrollingBorder.y = worldMapSize.y/2.0;

        mapScrollableRect.min = Vec2f(mapNoScrollingBorder.x,mapNoScrollingBorder.y);
        mapScrollableRect.max = Vec2f(worldMapSize.x - mapNoScrollingBorder.x, worldMapSize.y - mapNoScrollingBorder.y);

        pointedPosition.x = ClampValue(pointedPosition.x, mapScrollableRect.min.x, mapScrollableRect.max.x);
        pointedPosition.y = ClampValue(pointedPosition.y, mapScrollableRect.min.y, mapScrollableRect.max.y);

    }else{
        mapScrollableRect = Rectf(0.0,0.0, worldMapSize.x, worldMapSize.y);
    }


    AffineMat3f m = MakeTransformationMatrix(-viewport.GetCenter(), Vec2f(scale,scale), Vec2i(), rotation);
    Vec2f p = m.Transform(pointedPosition.x, pointedPosition.y);
    projectedMapPosition = -p;
    matrix = MakeTransformationMatrix(projectedMapPosition, Vec2f(scale,scale), Vec2i(), rotation);
    invMatrix = matrix.Invert();


    //----
    Vec2f p1 = MapPointFromScreenPoint(Vec2f(viewport.min.x, viewport.min.y));
    Vec2f p2 = MapPointFromScreenPoint(Vec2f(viewport.max.x, viewport.min.y));
    Vec2f p3 = MapPointFromScreenPoint(Vec2f(viewport.max.x, viewport.max.y));
    Vec2f p4 = MapPointFromScreenPoint(Vec2f(viewport.min.x, viewport.max.y));
    mapVisibleRect.min.x = ClampValue<float>(std::min({p1.x, p2.x, p3.x, p4.x}), 0, worldMapSize.x);
    mapVisibleRect.min.y = ClampValue<float>(std::min({p1.y, p2.y, p3.y, p4.y}), 0, worldMapSize.y);
    mapVisibleRect.max.x = ClampValue<float>(std::max({p1.x, p2.x, p3.x, p4.x}), 0, worldMapSize.x);
    mapVisibleRect.max.y = ClampValue<float>(std::max({p1.y, p2.y, p3.y, p4.y}), 0, worldMapSize.y);

}




}
