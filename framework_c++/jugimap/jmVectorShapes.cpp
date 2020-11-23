#include <assert.h>
#include "jmVectorShapes.h"



using namespace jugimap;

namespace jugimap{


/*
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
*/


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


Vec2f GeometricShape::GetPathPoint(float p)
{

    if(path.empty()) return Vec2f();


    float distance = p * pathLength;
    int indexP1 = 0;
    int indexP2 = path.size()-1;

    while(indexP2 - indexP1 > 1){

        int indexCheck = (indexP1+indexP2)/2;

        if(path[indexCheck].distance > distance){
            indexP2 = indexCheck;

        }else if(path[indexCheck].distance < distance){
            indexP1 = indexCheck;

        }else{
            return Vec2f(path[indexCheck].x, path[indexCheck].y);

        }
    }

    double distP1P2 = DistanceTwoPoints(path[indexP1], path[indexP2]);
    double fDist = 0;
    if(AreEqual(distP1P2, 0.0)==false){
        fDist = (distance-path[indexP1].distance)/distP1P2;
    }
    Vec2f pos = MakeVec2fAtDistanceFactorFromP1(path[indexP1], path[indexP2], fDist);


    return pos;

}


Vec2f GeometricShape::GetPathPoint(float p, float &directionAngle)
{

    float distance = p * pathLength;
    int indexP1 = 0;
    int indexP2 = path.size()-1;

    while(indexP2 - indexP1 > 1){

        int indexCheck = (indexP1+indexP2)/2;

        if(path[indexCheck].distance > distance){
            indexP2 = indexCheck;

        }else if(path[indexCheck].distance < distance){
            indexP1 = indexCheck;

        }else{

            if(indexCheck+1<path.size()){
                directionAngle = AngleBetweenTwoPoints(path[indexCheck], path[indexCheck+1]);
            }else if(indexCheck-1>=0){
                directionAngle = AngleBetweenTwoPoints(path[indexCheck-1], path[indexCheck]);
            }else{
                directionAngle = 0.0;
            }
            if(directionAngle<0) directionAngle += 360;
            return Vec2f(path[indexCheck].x, path[indexCheck].y);

        }
    }

    //Vec2f pos = GetPointAtDistanceFromP1( Points[indexP1],  Points[indexP2], distance-Points[indexP1].distance);

    double distP1P2 = DistanceTwoPoints(path[indexP1], path[indexP2]);
    double fDist = 0;
    if(AreEqual(distP1P2, 0.0)==false){
        fDist = (distance-path[indexP1].distance)/distP1P2;
    }
    Vec2f pos = MakeVec2fAtDistanceFactorFromP1(path[indexP1], path[indexP2], fDist);


    //---:
    //directionAngle = Points[indexP1].angle;
    directionAngle = AngleBetweenTwoPoints(path[indexP1], path[indexP2]);
    if(directionAngle<0) directionAngle += 360;


    //DbgOutput("");
    //DbgOutput("indexP1:" + std::to_string(indexP1) + " indexP2:" + std::to_string(indexP2));
    //DbgOutput("A directionAngle:" + std::to_string(directionAngle));


    int indexPrevious = indexP1-1;
    if(indexPrevious<0 && closed){
        indexPrevious = path.size()-1;
        if(DistanceTwoPoints(path[0],path[indexPrevious])<0.5){
            indexPrevious--;
        }
    }

    if(indexPrevious>=0){
        if(path[indexP1].smoothCorner){

            //float prevDirectionAngle = Points[indexPrevious].angle;
            float prevDirectionAngle = AngleBetweenTwoPoints(path[indexPrevious], path[indexP1]);
            if(prevDirectionAngle<0) prevDirectionAngle += 360;

            if(std::abs(prevDirectionAngle-directionAngle)>180){

                if(prevDirectionAngle<directionAngle){
                    prevDirectionAngle += 360;
                    //DbgOutput("   modify: prevDirectionAngle + 360");
                }else{
                    directionAngle += 360;
                    //DbgOutput("   modify: directionAngle + 360");
                }
            }

            //DbgOutput("B1 prevDirectionAngle:" + std::to_string(prevDirectionAngle));


            //if(sgn(prevDirectionAngle)==sgn(directionAngle)){
                directionAngle = prevDirectionAngle + fDist*(directionAngle - prevDirectionAngle);
            //    qDebug()<<"B2 directionAngle:"<< directionAngle;
            //}else{
            //    qDebug()<<"B sgn changed!";
            //}



        //}else{
        //    qDebug()<<"B smoothCorner = false";

        }
    }

    return pos;

}



//===========================================================================================


/*
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
*/


void PolyLineShape::RebuildPath()
{

    path.clear();
    float distance = 0.0;

    for(int i=0; i<vertices.size(); i++){
        if(i>0){
            distance += DistanceTwoPoints(vertices[i-1],vertices[i]);  //  DistanceTwoPoints(vertices[i-1],vertices[i]);
        }
        path.push_back(TShapePoint(Vec2f(vertices[i].x, vertices[i].y), distance));
    }
    if(path.empty()==false){
        if(closed){
            distance +=  DistanceTwoPoints(vertices.back(), vertices.front());     //DistanceTwoPoints(CPoints.back(), CPoints.front());
            path.push_back(TShapePoint(Vec2f(vertices.front().x, vertices.front().y), distance));
        }
        pathLength = path.back().distance;
    }

    DbgOutput("path length:" + std::to_string(pathLength));

    //SetPathPointsAngles();
    UpdateBoundingBox();
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


/*
 *
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


    //This method use standard parametric bezier equations for obtaining points along the path.
    //The problem of this method is that obtained points are not evenly spaced which gives variable speed of movement.
    //A solution would be to use arc-length parameterization of the bezier path (too complicated for this small demo)


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

*/



void BezierShape::RebuildPath()
{

    path.clear();


    if(vertices.empty()==false){

        for(int i=0; i<vertices.size()-1; i++){

            int nPointsStart = path.size();

            Vec2f P(vertices[i].P.x, vertices[i].P.y);
            Vec2f Pnext(vertices[i+1].P.x, vertices[i+1].P.y);

            Vec2f BP1 = P;
            Vec2f BP2 = vertices[i].CPnext;
            Vec2f BP3 = vertices[i+1].CPprevious;
            Vec2f BP4 = Pnext;

            _GetShapePointsFromBezierSegment(path, BP1, BP2, BP3, BP4, 1);

            DbgOutput("i:"+std::to_string(i) + "  Points.size:" + std::to_string(path.size()));
        }

        if(closed){
            Vec2f P(vertices.back().P.x, vertices.back().P.y);
            Vec2f Pnext(vertices.front().P.x, vertices.front().P.y);

            Vec2f BP1 = P;
            Vec2f BP2 = vertices.back().CPnext;
            Vec2f BP3 = vertices.front().CPprevious;
            Vec2f BP4 = Pnext;

            _GetShapePointsFromBezierSegment(path, BP1, BP2, BP3, BP4, 1);
        }

        if(path.empty()==false){
            pathLength = path.back().distance;
        }

        DbgOutput("path length:" + std::to_string(pathLength));
    }

    //SetPathPointsAngles();
    //UpdateBoundingBox();

}


void BezierShape::_GetShapePointsFromBezierSegment(std::vector<TShapePoint> &Points, Vec2f BP1, Vec2f BP2, Vec2f BP3, Vec2f BP4, int i)
{


    double distBP2 = distToSegment(BP2, BP1, BP4);
    double distBP3 = distToSegment(BP3, BP1, BP4);


    if(distBP2 < toPolyThreshold && distBP3 < toPolyThreshold){     // odsek BP1-BP4 je priblizno straight line


        if(Points.empty()){
            Points.push_back(TShapePoint(BP1, 0.0));
            if(i>1){
                Points.back().smoothCorner = true;
            }
        }else{
            double dist = DistanceTwoPoints(Points.back().x, Points.back().y, BP1.x, BP1.y);
            if(dist>=1.0){
                Points.push_back(TShapePoint(BP1, Points.back().distance+dist));
                if(i>1){
                    Points.back().smoothCorner = true;
                }
            }
        }

        double dist = DistanceTwoPoints(Points.back().x, Points.back().y, BP4.x, BP4.y);
        if(dist>=1.0){
            Points.push_back(TShapePoint(BP4, Points.back().distance+dist));
            if(i>1){
                Points.back().smoothCorner = true;
            }
        }

    }else{  // subdivide bezier odsek

        Vec2f L1 = BP1;
        Vec2f L2 = (BP1+BP2)*0.5;
        Vec2f H = (BP2+BP3)*0.5;
        Vec2f L3 = (L2+H)*0.5;

        Vec2f R4 = BP4;
        Vec2f R3 = (BP3+BP4)*0.5;
        Vec2f R2 = (H+R3)*0.5;

        Vec2f L4 = (L3+R2)*0.5;
        Vec2f R1 = L4;

        _GetShapePointsFromBezierSegment(Points, L1, L2, L3, L4, i+1);
        _GetShapePointsFromBezierSegment(Points, R1, R2, R3, R4, i+1);
    }

}



//===========================================================================================

/*
float EllipseShape::CalculatePathLength()
{
    return pi * ( 3*(a+b) - std::pow((3*a +b)*(a+3*b), 0.5));   // perimeter aproximation
}


Vec2f EllipseShape::GetPathPoint(float r, float pathLength)
{
    return Vec2f(center.x + a*std::cos(r*2*pi), center.y + b*std::sin(r*2*pi));
}

*/

void EllipseShape::RebuildPath()
{

    path.clear();


    //static const double PI = 3.14159265;

    //int a = std::abs(CPoints[RIGHT].x - CPoints[CENTER].x);
    //int b = std::abs(CPoints[TOP].y - CPoints[CENTER].y);

    //int n = std::max(a+b, 12 ) & (~3) ;

    float ra = (a+b) / 2.0;
    float da = std::acos(ra / (ra + 0.125 /1.0)) * 2;
    int n = std::round(2 * mathPI / da + 0.5);

    //n *= 4;
   // n = std::max(n,12 );
    //n = std::min(n,1000 );

    float distance = 0.0;
    Vec2f pPrev;

    for(int i=0; i<n; i++){
        float th = (i + 0.5) * 2*mathPI/n;
        Vec2f p(center.x + a * std::cos( th ), center.y + b * std::sin( th ));

        if(i>0){
            distance += DistanceTwoPoints(pPrev,p);
        }
        path.push_back(TShapePoint(p,distance, true));

        pPrev = p;
    }

    //close ellipse
    distance +=  DistanceTwoPoints(path.back(), path.front());
    path.push_back(TShapePoint(Vec2f(path.front().x, path.front().y), distance, true));   // close ellipse

    if(path.empty()==false){
        pathLength = path.back().distance;
    }

    DbgOutput("path length:" + std::to_string(pathLength));

    //SetPathPointsAngles();
    //UpdateBoundingBox();


}


//===========================================================================================


void SinglePointShape::RebuildPath()
{

    path.clear();
    path.push_back(TShapePoint(point));

    pathLength = 0;

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
