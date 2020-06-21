#ifndef JUGIMAP_VECTOR_SHAPES_UTILITIES_H
#define JUGIMAP_VECTOR_SHAPES_UTILITIES_H


#include "jmVectorShapes.h"



namespace jugimap{

class Drawer;


bool IsPolygonConvex(std::vector<Vec2f> &vertices, bool &cw);
bool SameGeometricShapes(std::vector<VectorShape *>&shapes1, std::vector<VectorShape *>&shapes2);
bool GeometricShapesEqual(GeometricShape * gs1, GeometricShape *gs2);
//void BezierPolycurveToPolyline(std::vector<BezierVertex>&bezierVertices,  std::vector<Vec2f>&vertices);

void MoveGeometricShape(GeometricShape * gs, Vec2f dPos);


void DrawTransformedGeometricShape(Drawer* drawer, GeometricShape *vs, AffineMat3f m, float scale);
void DrawGeometricShape(Drawer* drawer, GeometricShape *vs, Vec2f offset=Vec2f());


}




#endif
