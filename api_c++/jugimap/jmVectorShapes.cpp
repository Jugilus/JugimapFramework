#include <assert.h>
#include "jmVectorShapes.h"



using namespace jugimap;

namespace jugimap{



Vec2f GetPathPointOnPolyline(std::vector<Vec2f>&Vertices, float pathLength, float r)
{

    // position factor 'r' is for the whole polyline so we must do two things:
    // 1) we find segment in polyline where point V will be found
    // 2) we transform 'r' to a value relative to that segment

    Vec2f V;

    double distanceToV = r * pathLength;
    double lengthCounter = 0.0;
    for(int i=0; i<Vertices.size()-1; i++){
        Vec2f P = Vertices[i];
        Vec2f Pnext = Vertices[i+1];

        double lengthSegment = P.Distance(Pnext);
        if(lengthCounter+lengthSegment >= (distanceToV-0.1)){       // the point V is between p and Pnext
            double rStart = lengthCounter/pathLength;
            double rEnd = (lengthCounter+lengthSegment)/pathLength;
            double rCurrent = (r-rStart)/(rEnd-rStart);
            V = MakeVec2fAtDistanceFactorFromP1(P, Pnext, rCurrent);
            break;
        }
        lengthCounter += lengthSegment;
    }

    return V;
}



GeometricShape *GeometricShape::Copy(GeometricShape * _geomShape)
{

    switch (_geomShape->GetKind()) {
    //case RECTANGLE:
    //    return new JMRectangleShape(*static_cast<JMRectangleShape*>(s));

    case ShapeKind::POLYLINE:
        return new PolyLineShape(*static_cast<PolyLineShape*>(_geomShape));
        break;

    case ShapeKind::BEZIER_POLYCURVE:
        return new BezierShape(*static_cast<BezierShape*>(_geomShape));
        break;

    case ShapeKind::ELLIPSE:
        return new EllipseShape(*static_cast<EllipseShape*>(_geomShape));
        break;

    case ShapeKind::SINGLE_POINT:
        return new SinglePointShape(*static_cast<SinglePointShape*>(_geomShape));
        break;

    default:
        assert(false);
    }

    return nullptr;
}


void GeometricShape::Assign(GeometricShape *sSrc , GeometricShape *sDst)
{

    if(sSrc->GetKind()!=sDst->GetKind()) return;

    switch (sSrc->GetKind()) {
    //case RECTANGLE:
    //    *static_cast<JMRectangleShape*>(sDst) = *static_cast<JMRectangleShape*>(sSrc);

    case ShapeKind::POLYLINE:
        *static_cast<PolyLineShape*>(sDst) = *static_cast<PolyLineShape*>(sSrc);
        break;

    case ShapeKind::BEZIER_POLYCURVE:
        *static_cast<BezierShape*>(sDst) = *static_cast<BezierShape*>(sSrc);
        break;

    case ShapeKind::ELLIPSE:
        *static_cast<EllipseShape*>(sDst) = *static_cast<EllipseShape*>(sSrc);
        break;

    case ShapeKind::SINGLE_POINT:
        *static_cast<SinglePointShape*>(sDst) = *static_cast<SinglePointShape*>(sSrc);
        break;

    default:
        assert(false);
    }
}



//===========================================================================================


float PolyLineShape::CalculatePathLength()
{
    if(vertices.size()<2) return 0;


    float pathLength = 0.0;

    for(int i=0; i<vertices.size()-1; i++){
        Vec2f v = vertices[i];
        Vec2f vNext = vertices[i+1];
        pathLength += v.Distance(vNext);
    }

    if(closed) pathLength += vertices[vertices.size()-1].Distance(vertices[0]);

    return pathLength;
}


Vec2f PolyLineShape::GetPathPoint(float r, float pathLength)
{
    assert(vertices.size()>1);

    Vec2f v;

    if(vertices.size()==2){
        v = MakeVec2fAtDistanceFactorFromP1(vertices[0], vertices[1], r);

    }else{
        v = GetPathPointOnPolyline(vertices, pathLength, r);
    }

    return v;
}


void PolyLineShape::UpdateBoundingBox()
{
    if( vertices.empty()) return;

    boundingBoxMin = Vec2f(vertices[0].x, vertices[0].y);
    boundingBoxMax = Vec2f(vertices[0].x, vertices[0].y);

    for(int i=1; i<vertices.size(); i++){
        if(vertices[i].x < boundingBoxMin.x) boundingBoxMin.x = vertices[i].x;
        if(vertices[i].y < boundingBoxMin.y) boundingBoxMin.y = vertices[i].y;
        if(vertices[i].x > boundingBoxMax.x) boundingBoxMax.x = vertices[i].x;
        if(vertices[i].y > boundingBoxMax.y) boundingBoxMax.y = vertices[i].y;
    }
}


//===========================================================================================


Vec2f GetBezierPoint_AtDistanceFactorFromP1(Vec2f P1, Vec2f P2, Vec2f P3, Vec2f P4, float fDistance)
{
    float t = fDistance;
    float t2 = 1.0-t;

    Vec2f V;
    V.x =  P1.x * t2*t2*t2 + 3*P2.x * t*t2*t2 + 3*P3.x * t2*t*t + P4.x * t*t*t;
    V.y =  P1.y * t2*t2*t2 + 3*P2.y * t*t2*t2 + 3*P3.y * t2*t*t + P4.y * t*t*t;

    return V;
}


float BezierShape::CalculatePathLength()
{
    assert(vertices.size()>1);

    float pathLength = 0.0;


    for(int i=0; i<vertices.size()-1; i++){

        Vec2f BP1 = vertices[i].P;
        Vec2f BP2 = vertices[i].CPnext;
        Vec2f BP3 = vertices[i+1].CPprevious;
        Vec2f BP4 = vertices[i+1].P;

        Vec2f P = BP1;
        Vec2f Pprev = P;

        for(double t=0.0; t<=1.00001; t+=0.01){
            P = GetBezierPoint_AtDistanceFactorFromP1(BP1, BP2, BP3, BP4, t);
            double dist = P.Distance(Pprev);
            pathLength += dist;
            Pprev = P;
        }
    }

    return pathLength;

}


Vec2f BezierShape::GetPathPoint(float r, float pathLength)
{
    assert(vertices.size()>1);

    Vec2f v;

    /*
    This method use standard parametric bezier equations for obtaining points along the path.
    The problem of this method is that obtained points are not evenly spaced which gives variable speed of movement.
    A solution would be to use arc-length parameterization of the bezier path (too complicated for this small demo)
    */

    if(vertices.size()==2){

        Vec2f BP1 = vertices[0].P;
        Vec2f BP2 = vertices[0].CPnext;
        Vec2f BP3 = vertices[1].CPprevious;
        Vec2f BP4 = vertices[1].P;

        v = GetBezierPoint_AtDistanceFactorFromP1(BP1, BP2, BP3, BP4, r);


    }else{

        // another complicated thing left empty...
    }

    return v;
}


//===========================================================================================


float EllipseShape::CalculatePathLength()
{
    return pi * ( 3*(a+b) - std::pow((3*a +b)*(a+3*b), 0.5));   // perimeter aproximation
}


Vec2f EllipseShape::GetPathPoint(float r, float pathLength)
{
    return Vec2f(center.x + a*std::cos(r*2*pi), center.y + b*std::sin(r*2*pi));
}


//===========================================================================================


VectorShape::VectorShape(const VectorShape &vs)
{
    if(vs.shape){
        shape = GeometricShape::Copy(vs.shape);
    }
}


//===========================================================================================



}
