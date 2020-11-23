#include <assert.h>

#include "jmCommonFunctions.h"
#include "jmVectorShapes.h"
#include "jmSprites.h"
#include "jmSourceGraphics.h"
#include "jmCollision.h"




namespace jugimap{



std::vector<Vec2f>Collision::intersections;


bool Collision::ShapesOverlap(GeometricShape *shape1, GeometricShape *shape2)
{

    if(shape1->GetKind()==ShapeKind::POLYLINE){

        PolyLineShape *poly1 = static_cast<PolyLineShape*>(shape1);

        if(poly1->rectangle){

            if(shape2->GetKind()==ShapeKind::POLYLINE){
                PolyLineShape *poly2 = static_cast<PolyLineShape*>(shape2);

                if(poly2->rectangle){
                    return RectToRect(poly1->boundingBoxMin, poly1->boundingBoxMax, poly2->boundingBoxMin, poly2->boundingBoxMax);

                }else{
                    if(RectToRect(poly1->boundingBoxMin, poly1->boundingBoxMax, poly2->boundingBoxMin, poly2->boundingBoxMax)==false){
                        return false;
                    }
                    return PolyToPoly(poly1->vertices, poly2->vertices);
                }

            }else if(shape2->GetKind()==ShapeKind::ELLIPSE){
                EllipseShape *circle2 = static_cast<EllipseShape*>(shape2);
                return RectToCircle(poly1->boundingBoxMin, poly1->boundingBoxMax, circle2->center, circle2->a);
            }


        }else{

            if(shape2->GetKind()==ShapeKind::POLYLINE){
                PolyLineShape *poly2 = static_cast<PolyLineShape*>(shape2);

                if(RectToRect(poly1->boundingBoxMin, poly1->boundingBoxMax, poly2->boundingBoxMin, poly2->boundingBoxMax)==false){
                    return false;
                }
                return PolyToPoly(poly1->vertices, poly2->vertices);

            }else if(shape2->GetKind()==ShapeKind::ELLIPSE){
                EllipseShape *circle2 = static_cast<EllipseShape*>(shape2);
                return CircleToPoly(circle2->center, circle2->a, poly1->vertices);
            }
        }


    }else if(shape1->GetKind()==ShapeKind::ELLIPSE){

        EllipseShape *circle1 = static_cast<EllipseShape*>(shape1);

        if(shape2->GetKind()==ShapeKind::POLYLINE){
            PolyLineShape *poly2 = static_cast<PolyLineShape*>(shape2);

            if(poly2->rectangle){
                return RectToCircle(poly2->boundingBoxMin, poly2->boundingBoxMax, circle1->center, circle1->a);

            }else{
                return CircleToPoly(circle1->center, circle1->a, poly2->vertices);
            }

        }else if(shape2->GetKind()==ShapeKind::ELLIPSE){
            EllipseShape *circle2 = static_cast<EllipseShape*>(shape2);
            return CircleToCircle(circle1->center, circle1->a, circle2->center, circle2->a);
        }
    }

    return false;
}


bool Collision::PointInShape(Vec2f point, GeometricShape *shape)
{

    if(shape->GetKind()==ShapeKind::POLYLINE){
        PolyLineShape *poly = static_cast<PolyLineShape*>(shape);

        if(poly->rectangle){
            return PointInRect(point, poly->boundingBoxMin, poly->boundingBoxMax);

        }else{
            if(PointInRect(point, poly->boundingBoxMin, poly->boundingBoxMax)==false){
                return false;
            }
            return PointInPoly(point, poly->vertices);
        }


    }else if(shape->GetKind()==ShapeKind::ELLIPSE){
        EllipseShape *circle = static_cast<EllipseShape*>(shape);
        return PointInCircle(point, circle->center, circle->a);
    }

    return false;
}


bool Collision::RaycastToShape(Vec2f rayStart, Vec2f rayEnd, GeometricShape *shape, Vec2f &intersection, float rayLength)
{
    intersections.clear();

    if(rayLength>0){
        //qDebug()<<"dist1:"<<lp1.Distance(lp2);
        rayEnd = MakeVec2fAtDistanceFromP1(rayStart, rayEnd, rayLength);
        //qDebug()<<"dist2:"<<lp1.Distance(lp2);
    }

    GetRaycastToShapesIntersections(rayStart, rayEnd, shape, intersections);
    return GetNearestIntersection(rayStart, intersection);
}


bool Collision::RaycastToShapes(Vec2f rayStart, Vec2f rayEnd, std::vector<GeometricShape*>&shapes, Vec2f &intersection, float rayLength)
{
    intersections.clear();

    if(rayLength>0){
        rayEnd = MakeVec2fAtDistanceFromP1(rayStart, rayEnd, rayLength);
    }

    for(GeometricShape* vs : shapes){
        GetRaycastToShapesIntersections(rayStart, rayEnd, vs, intersections);
    }

    return GetNearestIntersection(rayStart, intersection);
}


void Collision::GetRaycastToShapesIntersections(Vec2f rayStart, Vec2f rayEnd, GeometricShape *shape, std::vector<Vec2f>&intersections, float rayLength)
{

    if(rayLength>0){
        rayEnd = MakeVec2fAtDistanceFromP1(rayStart, rayEnd, rayLength);
    }

    if(shape->GetKind()==ShapeKind::POLYLINE){
        PolyLineShape *poly = static_cast<PolyLineShape*>(shape);
        LineToPoly_GetIntersections(rayStart, rayEnd, poly->vertices, intersections);

    }else if(shape->GetKind()==ShapeKind::ELLIPSE){
        EllipseShape *circle = static_cast<EllipseShape*>(shape);
        LineToCircle_GetIntersections(rayStart, rayEnd, circle->center, circle->a, intersections);
    }
}


bool Collision::GetNearestIntersection(Vec2f rayStart, Vec2f &intersection)
{
    if(intersections.empty()) return false;

    float distMin = rayStart.Distance(intersections[0]);
    intersection = intersections[0];
    for(int i=1; i<intersections.size(); i++){
        float dist = rayStart.Distance(intersections[i]);
        if(dist<distMin){
            distMin = dist;
            intersection = intersections[i];
        }
    }
    return true;

}


void Collision::TransformShapeToWorldCoordinates(GeometricShape *srcShape, GeometricShape *dstShape, Vec2f pos, Vec2f scale, Vec2i flip, float rotation, Vec2f handle)
{

    if(srcShape->GetKind() != dstShape->GetKind()) return;

    AffineMat3f m = MakeTransformationMatrix(pos, scale, flip, rotation, handle);
    TransformShapeToWorldCoordinates(srcShape, dstShape, m, scale, flip, rotation);

    /*
    if(srcShape->GetKind()==JMShapeKind::POLYLINE){
        JMPolyLineShape *srcPoly = static_cast<JMPolyLineShape*>(srcShape);
        JMPolyLineShape *dstPoly = static_cast<JMPolyLineShape*>(dstShape);
        if(srcPoly->vertices.size() != dstPoly->vertices.size()) return;

        if(srcPoly->rectangle && AreSame(rotation,0)==false){   // rotirani rectangle nima več rectangle flag!
            dstPoly->rectangle = false;
        }

        AffineMat3f m = GetLinearTransformMatrix(pos, scale, flip, rotation, handle);

        for(int i=0; i<srcPoly->vertices.size(); i++){
            dstPoly->vertices[i] = m.Transform(srcPoly->vertices[i]);
        }
        dstPoly->UpdateBoundingBox();

    }else if(srcShape->GetKind()==JMShapeKind::ELLIPSE){
        JMEllipseShape *srcEllipse = static_cast<JMEllipseShape*>(srcShape);
        JMEllipseShape *dstEllipse = static_cast<JMEllipseShape*>(dstShape);

        AffineMat3f m = GetLinearTransformMatrix(pos, scale, flip, rotation, handle);

        dstEllipse->center = m.Transform(srcEllipse->center);
        dstEllipse->a = srcEllipse->a * scale.x;
        dstEllipse->b = srcEllipse->b * scale.y;
    }
    */
}


void Collision::TransformShapeToWorldCoordinates(GeometricShape *srcShape, GeometricShape *dstShape, const AffineMat3f &m, Vec2f scale, Vec2i flip, float rotation)
{
    if(srcShape->GetKind() != dstShape->GetKind()) return;


    if(srcShape->GetKind()==ShapeKind::POLYLINE){
        PolyLineShape *srcPoly = static_cast<PolyLineShape*>(srcShape);
        PolyLineShape *dstPoly = static_cast<PolyLineShape*>(dstShape);
        if(srcPoly->vertices.size() != dstPoly->vertices.size()) return;

        //if(srcPoly->rectangle && AreSame(rotation,0)==false){   // check and set rectangle flag!
        if(srcPoly->rectangle && AreEqual(rotation, 0.0)==false){   // check and set rectangle flag!
            dstPoly->rectangle = false;
        }

        //AffineMat3f m = GetLinearTransformMatrix(pos, scale, flip, rotation, handle);

        for(int i=0; i<srcPoly->vertices.size(); i++){
            dstPoly->vertices[i] = m.Transform(srcPoly->vertices[i]);
        }
        dstPoly->UpdateBoundingBox();

    }else if(srcShape->GetKind()==ShapeKind::ELLIPSE){
        EllipseShape *srcEllipse = static_cast<EllipseShape*>(srcShape);
        EllipseShape *dstEllipse = static_cast<EllipseShape*>(dstShape);

        //AffineMat3f m = GetLinearTransformMatrix(pos, scale, flip, rotation, handle);

        dstEllipse->center = m.Transform(srcEllipse->center);
        dstEllipse->a = srcEllipse->a * scale.x;
        dstEllipse->b = srcEllipse->b * scale.y;
    }
}

//---------------------------------------------------------------------------


bool Collision::PolyToPoly(const std::vector<Vec2f> &poly1, const std::vector<Vec2f> &poly2)
{

    if(poly1.size()<3 || poly2.size()<3) return false;

    for(const Vec2f &v : poly1){
        if(PointInPoly(v, poly2)) return true;
    }

    for(const Vec2f &v : poly2){
        if(PointInPoly(v, poly1)) return true;
    }

    Vec2f l1_v1 = poly1.back();
    for(int i1=0; i1<poly1.size(); i1++){
        Vec2f l1_v2 = poly1[i1];

        Vec2f l2_v1 = poly2.back();
        for(int i2=0; i2<poly2.size(); i2++){
            Vec2f l2_v2 = poly2[i2];

            if(LinesCross(l1_v1,l1_v2, l2_v1,l2_v2)){
                return true;
            }

            l2_v1=l2_v2;
        }
        l1_v1 = l1_v2;
    }

    return false;
}


bool Collision::CircleToPoly(Vec2f center, float radius, const std::vector<Vec2f> &poly)
{

    if(poly.size()<3) return false;

    if(PointInPoly(center,poly)) return true;


    Vec2f v1 = poly.back();

    for(int i=0; i<poly.size(); i++){
        Vec2f v2 = poly[i];

        if(LineToCircle(v1,v2, center,radius)) return true;

        v1=v2;
    }

    return false;
}


bool Collision::RectToCircle(Vec2f Av1, Vec2f Av2, Vec2f center, float radius){

    float testX = center.x;
    float testY = center.y;
    if(testX < Av1.x) testX = Av1.x;
    if(testX > Av2.x) testX = Av2.x;
    if(testY < Av1.y) testY = Av1.y;
    if(testY > Av2.y) testY = Av2.y;
    return ((center.x-testX)*(center.x-testX)+(center.y-testY)*(center.y-testY)) < radius*radius;
}

//----
/*
bool JMCollision::PointInPoly(Vec2f point, const std::vector<Vec2f> &poly)
{

    if(poly.size()<3) return false;

    Vec2f v1 = poly.back();
    int cur_quad = GetQuad(point, v1);
    int next_quad = 0;
    int total = 0;

    for(int i=0; i<poly.size(); i++){
        Vec2f v2 = poly[i];
        next_quad = GetQuad(point, v2);
        int diff = next_quad-cur_quad;

        if(diff==2 || diff==-2){
            if((v2.x - (((v2.y - point.y) * (v1.x - v2.x))/(v1.y - v2.y))) < point.x){
                diff = -diff;
            }

        }else if(diff==3){
            diff=-1;

        }else if(diff==-3){
            diff=1;
        }

        total += diff;
        cur_quad = next_quad;
        v1=v2;
    }

    if(std::abs(total)==4){
        return true;
    }

    return false;
}
*/

bool Collision::PointInPoly(Vec2f point, const std::vector<Vec2f> &poly)
//bool Point_in_polygon(double tx, double ty, const std::vector<TXYDoublePoint> &Poly)
{
    if(poly.size() < 3) return false;

    unsigned j;
    int yflag0, yflag1, inside_flag;
    double  vtx0, vty0, vtx1, vty1;

    vtx0 = poly[poly.size()-1].x;
    vty0 = poly[poly.size()-1].y;

    // get test bit for above/below X axis
    yflag0 = (vty0 >= point.y);

    vtx1 = poly[0].x;
    vty1 = poly[0].y;

    inside_flag = 0;
    for (j = 1; j <= poly.size(); ++j)
    {
        yflag1 = (vty1 >= point.y);

        if (yflag0 != yflag1)
        {

            if ( ((vty1-point.y) * (vtx0-vtx1) >=
                  (vtx1-point.x) * (vty0-vty1)) == yflag1 )
            {
                inside_flag ^= 1;
            }
        }

        // Move to the next pair of vertices, retaining info as possible.
        yflag0 = yflag1;
        vtx0 = vtx1;
        vty0 = vty1;

        unsigned k = (j >= poly.size()) ? j - poly.size() : j;
        vtx1 = poly[k].x;
        vty1 = poly[k].y;
    }
    return inside_flag != 0;
}



bool Collision::LineToPoly(Vec2f line_v1, Vec2f line_v2, const std::vector<Vec2f> &poly)
{

    if(poly.size()<3) return false;

    if(PointInPoly(line_v1,poly)) return true;

    Vec2f poly_v1 = poly.back();


    for(int i=0; i<poly.size(); i++){
        Vec2f poly_v2 = poly[i];

        if(LinesCross(line_v1,line_v2, poly_v1, poly_v2)) return true;

        poly_v1 = poly_v2;
    }

    return false;

}


bool Collision::LineToCircle(Vec2f v1, Vec2f v2, Vec2f center, float radius)
{

    float sx = v2.x-v1.x;
    float sy = v2.y-v1.y;

    float q = ((center.x-v1.x) * (v2.x-v1.x) + (center.y - v1.y) * (v2.y-v1.y)) / (sx*sx + sy*sy);

    if(q < 0.0) q = 0.0;
    if(q > 1.0) q = 1.0;

    float cx=(1-q)*v1.x + q*v2.x;
    float cy=(1-q)*v1.y + q*v2.y;


    if(PointToPointDist(center, Vec2f(cx,cy)) < radius){
        return true;
    }

    return false;
}


void Collision::LineToPoly_GetIntersections(Vec2f line_v1, Vec2f line_v2, const std::vector<Vec2f> &poly, std::vector<Vec2f> &intersections)
{

    if(poly.size()<3) return;


    Vec2f intersection;

    Vec2f poly_v1 = poly.back();
    for(int i=0; i<poly.size(); i++){
        Vec2f poly_v2 = poly[i];

        if(LinesCross(line_v1,line_v2, poly_v1, poly_v2, intersection)){
            intersections.push_back(intersection);
        }

        poly_v1 = poly_v2;
    }
}


void Collision::LineToCircle_GetIntersections(Vec2f line_v1, Vec2f line_v2, Vec2f center, float radius, std::vector<Vec2f> &intersections)
{

    float dx = line_v2.x - line_v1.x;
    float dy = line_v2.y - line_v1.y;

    float A = dx * dx + dy * dy;
    float B = 2 * (dx * (line_v1.x - center.x) + dy * (line_v1.y - center.y));
    float C = (line_v1.x - center.x) * (line_v1.x - center.x) + (line_v1.y - center.y) * (line_v1.y - center.y) - radius * radius;

    float det = B * B - 4 * A * C;

    if ((A <= 0.0000001) || (det < 0)){
        // No solutions


    }else if (det == 0){
        // One solution

        float t = -B / (2 * A);
        Vec2f intersection = Vec2f(line_v1.x + t * dx, line_v1.y + t * dy);

        Vec2f lp = line_v2 - line_v1;
        Vec2f i = intersection - line_v1;
        if(lp.Dot(i)>0){                                                                  // intersection lies in ray direction
            if(line_v1.Distance2(line_v2) >= line_v1.Distance2(intersection)){            // intersection lies within ray length
                intersections.push_back(intersection);
            }
        }

    }else{
        // Two solutions
        float t = (float)((-B + std::sqrt(det)) / (2 * A));
        Vec2f intersection = Vec2f(line_v1.x + t * dx, line_v1.y + t * dy);
        Vec2f lp = line_v2 - line_v1;
        Vec2f i = intersection - line_v1;

        if(lp.Dot(i)>0){
            if(line_v1.Distance2(line_v2) >= line_v1.Distance2(intersection)){
                intersections.push_back(intersection);
            }
        }

        //---
        t = (float)((-B - std::sqrt(det)) / (2 * A));
        intersection = Vec2f(line_v1.x + t * dx, line_v1.y + t * dy);

        i = intersection - line_v1;

        if(lp.Dot(i)>0){
            if(line_v1.Distance2(line_v2) >= line_v1.Distance2(intersection)){
                intersections.push_back(intersection);
            }
        }
    }
}


bool Collision::LinesCross(Vec2f v0, Vec2f v1, Vec2f v2, Vec2f v3)
{

    float n = (v0.y-v2.y)*(v3.x-v2.x)-(v0.x-v2.x)*(v3.y-v2.y);
    float d = (v1.x-v0.x)*(v3.y-v2.y)-(v1.y-v0.y)*(v3.x-v2.x);

    if(std::abs(d)< 0.0001){
       // Lines are parallel!
        return false;
    }

   // Lines might cross!
    float Sn = (v0.y-v2.y)*(v1.x-v0.x)-(v0.x-v2.x)*(v1.y-v0.y);

    float AB=n/d;
    if(AB>0.0 && AB<1.0){
        float CD=Sn/d;
        if(CD>0.0 && CD<1.0){
           // Intersection Point
            //vIntersection.x = v0.x + AB*(v1.x-v0.x);
            //vIntersection.y = v0.y + AB*(v1.y-v0.y);
            return true;
        }
    }
    // Lines didn't cross, because the intersection was beyond the end points of the lines

   // Lines do Not cross!
    return false;
}


bool Collision::LinesCross(Vec2f v0, Vec2f v1, Vec2f v2, Vec2f v3, Vec2f &vIntersection)
{

    float n = (v0.y-v2.y)*(v3.x-v2.x)-(v0.x-v2.x)*(v3.y-v2.y);
    float d = (v1.x-v0.x)*(v3.y-v2.y)-(v1.y-v0.y)*(v3.x-v2.x);

    if(std::abs(d)< 0.0001){
       // Lines are parallel!
        return false;
    }

   // Lines might cross!
    float Sn = (v0.y-v2.y)*(v1.x-v0.x)-(v0.x-v2.x)*(v1.y-v0.y);

    float AB=n/d;
    if(AB>0.0 && AB<1.0){
        float CD=Sn/d;
        if(CD>0.0 && CD<1.0){
           // Intersection Point
            vIntersection.x = v0.x + AB*(v1.x-v0.x);
            vIntersection.y = v0.y + AB*(v1.y-v0.y);
            return true;
        }
    }
    // Lines didn't cross, because the intersection was beyond the end points of the lines

   // Lines do Not cross!
    return false;
}



/*
int JMCollision::GetQuad(Vec2f axis, Vec2f vert)
{

    if(vert.x<axis.x){
        if(vert.y<axis.y){
            return 1;
        }else{
            return 4;
        }
    }else{
        if(vert.y<axis.y){
            return 2;
        }else{
            return 3;
        }
    }
}
*/


//=====================================================================================================


SingleShapeCollider::~SingleShapeCollider()
{
    ClearShapes();
}


void SingleShapeCollider::ClearShapes()
{
    delete shape;
    shape = nullptr;
}


void SingleShapeCollider::AddShapes()
{
    GeometricShape *gsLocal = GetStandardSprite()->GetActiveImage()->GetSpriteShapes().front()->GetGeometricShape();
    shape = GeometricShape::Copy(gsLocal);
    Collision::TransformShapeToWorldCoordinates(gsLocal, shape, GetStandardSprite()->GetGlobalPosition(), GetStandardSprite()->GetGlobalScale(), GetStandardSprite()->GetGlobalFlip(), GetStandardSprite()->GetGlobalRotation(), Vec2f());
}


void SingleShapeCollider::UpdateShapes(const AffineMat3f &m)
{
    //assert(GetStandardSprite()->GetActiveImage()->GetSpriteShapes().empty()==false);
    if(shape==nullptr) return;
    
    GeometricShape *gsLocal = GetStandardSprite()->GetActiveImage()->GetSpriteShapes().front()->GetGeometricShape();
    Collision::TransformShapeToWorldCoordinates(gsLocal, shape, m, GetStandardSprite()->GetGlobalScale(), GetStandardSprite()->GetGlobalFlip(), GetStandardSprite()->GetGlobalRotation());   // Handle is not used because source collision shapes are already relative to handle point!

}


bool SingleShapeCollider::PointInside(Vec2f _pos)
{
    if(shape==nullptr) return false;




    return Collision::PointInShape(_pos, shape);
}


bool SingleShapeCollider::Overlaps(Collider *_otherCollider)
{
    if(shape==nullptr) return false;

    if(_otherCollider->GetKind()==Kind::SINGLE_SHAPE){
        SingleShapeCollider *collider = static_cast<SingleShapeCollider*>(_otherCollider);

        if(collider->GetShape()==nullptr) return false;

        return Collision::ShapesOverlap(shape, collider->GetShape());


    }else if(_otherCollider->GetKind()==Kind::MULTI_SHAPE){
        MultiShapesCollider *collider = static_cast<MultiShapesCollider*>(_otherCollider);

        if(collider->GetShapes().empty()) return false;

        for(GeometricShape *gs : collider->GetShapes()){
            if(Collision::ShapesOverlap(shape, gs)){
                return true;
            }
        }
    }

    return false;
}


bool SingleShapeCollider::Overlaps(GeometricShape *_geomShape)
{
    if(shape==nullptr) return false;

    return Collision::ShapesOverlap(shape, _geomShape);
}


bool SingleShapeCollider::RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos)
{
    if(shape==nullptr) return false;

    return Collision::RaycastToShape(_rayStart, _rayEnd, shape, _hitPos);
}


//=====================================================================================================


MultiShapesCollider::~MultiShapesCollider()
{
    ClearShapes();
}


void MultiShapesCollider::ClearShapes()
{

    for(GeometricShape* gs : shapes){
        delete gs;
    }
    shapes.clear();
}


void MultiShapesCollider::AddShapes()
{

    for(VectorShape *vs : GetStandardSprite()->GetActiveImage()->GetSpriteShapes()){
        GeometricShape *gsLocal = vs->GetGeometricShape();
        GeometricShape *gsWorld = GeometricShape::Copy(gsLocal);
        Collision::TransformShapeToWorldCoordinates(gsLocal, gsWorld, GetStandardSprite()->GetGlobalPosition(), GetStandardSprite()->GetGlobalScale(), GetStandardSprite()->GetGlobalFlip(), GetStandardSprite()->GetGlobalRotation(), Vec2f());
        shapes.push_back(gsWorld);
    }
}


void MultiShapesCollider::UpdateShapes(const AffineMat3f &m)
{

    if(shapes.size() != GetStandardSprite()->GetActiveImage()->GetSpriteShapes().size()) return;

    std::vector<VectorShape*>&sprShapes = GetStandardSprite()->GetActiveImage()->GetSpriteShapes();

    for(int i=0; i<shapes.size(); i++){
        Collision::TransformShapeToWorldCoordinates(sprShapes[i]->GetGeometricShape(), shapes[i], m, GetStandardSprite()->GetGlobalScale(), GetStandardSprite()->GetGlobalFlip(), GetStandardSprite()->GetGlobalRotation());   // Handle is not used because source collision shapes are already relative to handle point!
    }

}


bool MultiShapesCollider::PointInside(Vec2f _pos)
{
    if(shapes.empty()) return false;

    for(GeometricShape *gs : shapes){
        if(Collision::PointInShape(_pos, gs)){
            return true;
        }
    }

    return false;
}


bool MultiShapesCollider::Overlaps(Collider *_otherCollider)
{
    if(shapes.empty()) return false;


    if(_otherCollider->GetKind()==Kind::SINGLE_SHAPE){
        SingleShapeCollider *collider = static_cast<SingleShapeCollider*>(_otherCollider);

        if(collider->GetShape()==nullptr) return false;

        for(GeometricShape *gs : shapes){
            if(Collision::ShapesOverlap(gs, collider->GetShape())){
                return true;
            }
        }

    }else if(_otherCollider->GetKind()==Kind::MULTI_SHAPE){
        MultiShapesCollider *collider = static_cast<MultiShapesCollider*>(_otherCollider);
        if(collider->GetShapes().empty()) return false;

        for(GeometricShape *gs1 : shapes){
            for(GeometricShape *gs2 : collider->GetShapes()){
                if(Collision::ShapesOverlap(gs1, gs2)){
                    return true;
                }
            }
        }
    }

    return false;
}


bool MultiShapesCollider::Overlaps(GeometricShape *_geomShape)
{

    if(shapes.empty()) return false;

    for(GeometricShape *gs1 : shapes){
        if(Collision::ShapesOverlap(gs1, _geomShape)){
            return true;
        }
    }

    return false;
}


bool MultiShapesCollider::RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos)
{
    if(shapes.empty()) return false;

    for(GeometricShape *gs : shapes){
        if(Collision::RaycastToShape(_rayStart, _rayEnd, gs, _hitPos)){
            return true;
        }
    }

    return false;

}



}
