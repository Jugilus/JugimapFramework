#include <assert.h>
#include "jmDrawing.h"
#include "jmVectorShapesUtilities.h"



namespace jugimap{


bool IsPolygonConvex(std::vector<Vec2f> &vertices, bool &cw)
{

    bool negative = false;
    bool positive = false;
    int nVertices = vertices.size();

    for (int i = 0; i < nVertices; i++){

        int iNext = (i + 1) % nVertices;
        int iNextNext = (iNext + 1) % nVertices;

        Vec2f v1 = vertices[iNext] - vertices[i];
        Vec2f v2 = vertices[iNextNext] - vertices[iNext];

        float cross_product = v1.x*v2.y - v1.y*v2.x;

        if(cross_product < 0){
            negative = true;
            cw = true;

        }else if (cross_product > 0){
            positive = true;
            cw = false;
        }
        if (negative && positive) return false;
    }

    return true;
}



bool SameGeometricShapes(std::vector<VectorShape *> &shapes1, std::vector<VectorShape *> &shapes2)
{

    if(shapes1.size()!=shapes2.size()){
        return false;

    }else if(shapes1.size()==1 && shapes2.size()==1){
        return GeometricShapesEqual(shapes1[0]->GetGeometricShape(), shapes2[0]->GetGeometricShape());

    }else{

        for(VectorShape * vs1 : shapes1){
            bool foundSameShape = false;
            for(VectorShape * vs2 : shapes2){
                if(GeometricShapesEqual(vs1->GetGeometricShape(), vs2->GetGeometricShape())){
                    foundSameShape = true;
                    break;
                };
            }
            if(foundSameShape==false){
                return false;
            }
        }
    }

    return true;
}



bool GeometricShapesEqual(GeometricShape * gs1, GeometricShape *gs2)
{

    if(gs1->GetKind() != gs2->GetKind()) return false;


    if(gs1->GetKind()==ShapeKind::POLYLINE){
        PolyLineShape *poly1 = static_cast<PolyLineShape *>(gs1);
        PolyLineShape *poly2 = static_cast<PolyLineShape *>(gs2);

        if(poly1->vertices.size() != poly2->vertices.size()){
            return false;
        }
        if(poly1->IsClosed() != poly2->IsClosed()){
            return false;
        }

        for(int i=0; i<poly1->vertices.size(); i++){
            if(poly1->vertices[i].Equals(poly2->vertices[i])==false){
                return false;
            }
        }


    }else if(gs1->GetKind()==ShapeKind::BEZIER_POLYCURVE){
        BezierShape *bezpoly1 = static_cast<BezierShape *>(gs1);
        BezierShape *bezpoly2 = static_cast<BezierShape *>(gs2);

        if(bezpoly1->vertices.size() != bezpoly2->vertices.size()){
            return false;
        }
        if(bezpoly1->IsClosed() != bezpoly2->IsClosed()){
            return false;
        }

        for(int i=0; i<bezpoly1->vertices.size(); i++){
            if(bezpoly1->vertices[i].P.Equals(bezpoly2->vertices[i].P)==false){
                return false;
            }
            if(bezpoly1->vertices[i].CPprevious.Equals(bezpoly2->vertices[i].CPprevious)==false){
                return false;
            }
            if(bezpoly1->vertices[i].CPnext.Equals(bezpoly2->vertices[i].CPnext)==false){
                return false;
            }
        }

    }else if(gs1->GetKind()==ShapeKind::ELLIPSE){
        EllipseShape *ellipse1 = static_cast<EllipseShape *>(gs1);
        EllipseShape *ellipse2 = static_cast<EllipseShape *>(gs2);

        if(ellipse1->center.Equals(ellipse2->center)==false){
            return false;
        }

        if(AreEqual(ellipse1->a, ellipse2->a)==false || AreEqual(ellipse1->b, ellipse2->b)==false){
            return false;
        }


    }else if(gs1->GetKind()==ShapeKind::SINGLE_POINT){
        SinglePointShape *sp1 = static_cast<SinglePointShape *>(gs1);
        SinglePointShape *sp2 = static_cast<SinglePointShape *>(gs2);

        if(sp1->point.Equals(sp2->point)==false){
            return false;
        }
    }

    return true;
}

/*

void _AddVertexToPolyline(std::vector<Vec2f>&Vertices, Vec2f v,  float pointsDistanceMin)
{
    if(Vertices.empty()){
        Vertices.push_back(v);
    }else{
        float dist = v.Distance(Vertices.back());
        if(dist>=pointsDistanceMin){
            Vertices.push_back(v);
        }
    }
}


void BezierPolycurveToPolyline(std::vector<BezierVertex>&bezierVertices,  std::vector<Vec2f>&vertices)
{

    float pointsDistanceMin = 2.0;
    vertices.clear();
    if(bezierVertices.empty()) return;


    for(int i=0; i<bezierVertices.size()-1; i++){

        Vec2f BP1 = bezierVertices[i].P;
        Vec2f BP2 = bezierVertices[i].CPnext;
        Vec2f BP3 = bezierVertices[i+1].CPprevious;
        Vec2f BP4 = bezierVertices[i+1].P;

        float length14 = BP1.Distance(BP4);
        if(length14 <= 2){
            _AddVertexToPolyline(vertices, BP1, pointsDistanceMin);
            continue;
        }

        // straight line between BP1 and BP4
        //if( AreSameVec2f(BP1,BP2) && AreSameVec2f(BP3,BP4)){
        if( BP1.Equals(BP2) && BP3.Equals(BP4)){
            _AddVertexToPolyline(vertices, BP1, pointsDistanceMin);
            _AddVertexToPolyline(vertices, BP4, pointsDistanceMin);
            continue;
        }

        float t = 0.0;
        float length = BP1.Distance(BP2) + BP2.Distance(BP3) + BP3.Distance(BP4);
        float f = length14/length;
        float dStep = 0.05 * f;            // this looks ok

        _AddVertexToPolyline(vertices, BP1, pointsDistanceMin);

        while(t <= 1.00-dStep){
            float t2 = 1.0-t;
            float xB =  BP1.x * t2*t2*t2 + 3*BP2.x * t*t2*t2 + 3*BP3.x * t2*t*t + BP4.x * t*t*t;
            float yB =  BP1.y * t2*t2*t2 + 3*BP2.y * t*t2*t2 + 3*BP3.y * t2*t*t + BP4.y * t*t*t;

            _AddVertexToPolyline(vertices, Vec2f(xB,yB), pointsDistanceMin);
            t += dStep;
        }

        //last point
        _AddVertexToPolyline(vertices, BP4, pointsDistanceMin);
    }

}

*/


void MoveGeometricShape(GeometricShape * gs, Vec2f dPos)
{

    if(gs->GetKind()==ShapeKind::POLYLINE){

        PolyLineShape *poly = static_cast<PolyLineShape *>(gs);

        for(int i=0; i<poly->vertices.size(); i++){
            poly->vertices[i] = poly->vertices[i] + dPos;
        }

    }else if(gs->GetKind()==ShapeKind::BEZIER_POLYCURVE){

        BezierShape *bezpoly = static_cast<BezierShape *>(gs);

        for(int i=0; i<bezpoly->vertices.size(); i++){
            bezpoly->vertices[i].P = bezpoly->vertices[i].P + dPos;
            bezpoly->vertices[i].CPprevious = bezpoly->vertices[i].CPprevious + dPos;
            bezpoly->vertices[i].CPnext = bezpoly->vertices[i].CPnext + dPos;
        }
        //for(int i=0; i<bezpoly->polylineVertices.size(); i++){
        //    bezpoly->polylineVertices[i] = bezpoly->polylineVertices[i] + dPos;
        //}


    }else if(gs->GetKind()==ShapeKind::ELLIPSE){

        EllipseShape *ellipse = static_cast<EllipseShape *>(gs);
        ellipse->center = ellipse->center + dPos;

    }else if(gs->GetKind()==ShapeKind::SINGLE_POINT){

        SinglePointShape *sp = static_cast<SinglePointShape *>(gs);
        sp->point = sp->point + dPos;
    }

    //---- path
    for(int i=0; i<gs->GetPath().size(); i++){
        //gs->GetPathPoints()[i] = gs->GetPathPoints()[i] + dPos;           // NO becouse it clear distance and smooth corner parameters!
        gs->GetPath()[i].x += dPos.x;
        gs->GetPath()[i].y += dPos.y;
    }

}


void DrawTransformedGeometricShape(Drawer *drawer, GeometricShape *vs,  AffineMat3f m, float scale)
{

    Vec2f v, v1, v2;

    if(vs->GetKind()==ShapeKind::POLYLINE){

        PolyLineShape *poly = static_cast<PolyLineShape*>(vs);
        for(int i=0; i<int(poly->vertices.size())-1; i++){
            v1 = poly->vertices[i];
            v2 = poly->vertices[i+1];

            v1 = m.Transform(v1);
            v2 = m.Transform(v2);

            drawer->Line(v1,v2);

        }
        if(poly->IsClosed()){
            v1 = poly->vertices[poly->vertices.size()-1];
            v2 = poly->vertices[0];

            v1 = m.Transform(v1);
            v2 = m.Transform(v2);

            drawer->Line(v1,v2);
        }

    }else if(vs->GetKind()==ShapeKind::BEZIER_POLYCURVE){

        /*
        BezierShape *bezpoly = static_cast<BezierShape*>(vs);
        for(int i=0; i<bezpoly->polylineVertices.size()-1; i++){
            v1 = bezpoly->polylineVertices[i];
            v2 = bezpoly->polylineVertices[i+1];

            v1 = m.Transform(v1);
            v2 = m.Transform(v2);

            drawer->Line(v1,v2);
        }
        if(bezpoly->IsClosed()){
            v1 = bezpoly->polylineVertices[bezpoly->polylineVertices.size()-1];
            v2 = bezpoly->polylineVertices[0];

            v1 = m.Transform(v1);
            v2 = m.Transform(v2);

            drawer->Line(v1,v2);
        }
        */

        //BezierShape *bezpoly = static_cast<BezierShape*>(vs);
        for(int i=0; i<int(vs->GetPath().size())-1; i++){
            v1 = vs->GetPath()[i];
            v2 = vs->GetPath()[i+1];

            v1 = m.Transform(v1);
            v2 = m.Transform(v2);

            drawer->Line(v1,v2);
        }

        /*
        if(bezpoly->IsClosed()){
            v1 = bezpoly->polylineVertices[bezpoly->polylineVertices.size()-1];
            v2 = bezpoly->polylineVertices[0];

            v1 = m.Transform(v1);
            v2 = m.Transform(v2);

            drawer->Line(v1,v2);
        }
        */


    }else if(vs->GetKind()==ShapeKind::ELLIPSE){

        EllipseShape *e = static_cast<EllipseShape*>(vs);
        v = e->center;

        v = m.Transform(v);

        drawer->EllipseOutline(v, Vec2f(scale*e->a, scale*e->b));


    }else if(vs->GetKind()==ShapeKind::SINGLE_POINT){

        SinglePointShape *p = static_cast<SinglePointShape*>(vs);
        v = p->point;
        v = m.Transform(v);
        drawer->Dot(v);

    }

}


void DrawGeometricShape(Drawer* drawer, GeometricShape *vs, jugimap::Vec2f offset)
{


    Vec2f v, v1, v2;

    if(vs->GetKind()==ShapeKind::POLYLINE){

        PolyLineShape *poly = static_cast<PolyLineShape*>(vs);

        for(int i=0; i<int(poly->vertices.size())-1; i++){
            v1 = poly->vertices[i] + offset;
            v2 = poly->vertices[i+1] + offset;

            drawer->Line(v1,v2);
        }
        if(poly->IsClosed()){
            v1 = poly->vertices[poly->vertices.size()-1] + offset;
            v2 = poly->vertices[0] + offset;

            drawer->Line(v1,v2);
        }

    }else if(vs->GetKind()==ShapeKind::BEZIER_POLYCURVE){


        for(int i=0; i<int(vs->GetPath().size())-1; i++){
            v1 = vs->GetPath()[i] + offset;
            v2 = vs->GetPath()[i+1] + offset;

            drawer->Line(v1,v2);
        }

        /*
        BezierShape *bezpoly = static_cast<BezierShape*>(vs);

        for(int i=0; i<bezpoly->polylineVertices.size()-1; i++){
            v1 = bezpoly->polylineVertices[i] + offset;;
            v2 = bezpoly->polylineVertices[i+1] + offset;;

            drawer->Line(v1,v2);
        }
        if(bezpoly->IsClosed()){
            v1 = bezpoly->polylineVertices[bezpoly->polylineVertices.size()-1] + offset;;
            v2 = bezpoly->polylineVertices[0] + offset;;

            drawer->Line(v1,v2);
        }
        */


    }else if(vs->GetKind()==ShapeKind::ELLIPSE){

        EllipseShape *e = static_cast<EllipseShape*>(vs);
        v = e->center + offset;;
        drawer->EllipseOutline(v, Vec2f(e->a,e->b));


    }else if(vs->GetKind()==ShapeKind::SINGLE_POINT){

        SinglePointShape *p = static_cast<SinglePointShape*>(vs);
        v = p->point + offset;
        drawer->Dot(v);
    }

}






}
