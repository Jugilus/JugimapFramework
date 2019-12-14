#ifndef JUGIMAP_VECTOR_SHAPES_H
#define JUGIMAP_VECTOR_SHAPES_H

#include <vector>

#include "jmCommonFunctions.h"
#include "jmGlobal.h"



namespace jugimap{


class JugiMapBinaryLoader;



///\ingroup MapElements
///\brief The base class for geometric shapes.
///
/// The GeometricShape provides the shape data for the VectorShape objects. It can be used also
/// for standalone objects used in collision functions.
struct GeometricShape
{

    virtual ~GeometricShape(){}

    ///\brief Create a new geometric object which is a copy of the given *_geomShape*.
    static GeometricShape *Copy(GeometricShape *_geomShape);


    ///\brief Assign the content of the given *sSrc* to the *sDst*.
    ///
    /// Assigning shapes *sSrc* and *sDst* must be of the same kind!
    static void Assign(GeometricShape *sSrc, GeometricShape *sDst);


    ///\brief Returns the kind of this shape.
    ShapeKind GetKind(){ return kind; }


    ///\brief Returns *true* if this shape is a valid geometric object; otherwise returns false.
    ///
    /// This function is usually called from the VectorShape::IsValid.
    virtual bool IsValid()  = 0;


    ///\brief Calculate and returns the length of this shape.
    ///
    /// This function is usually called from the VectorShape::CalculatePathLength.
    virtual float CalculatePathLength() = 0;


    ///\brief Returns a point along the path at the given parametric position *r*.
    ///
    /// This function is usually called from the VectorShape::GetPathPoint.
    /// \param r A parameter in the range of [0.0 - 1.0] where 0.0 gives the starting point and 1.0 gives the end point of the path.
    /// \param pathLength The length of path calculated via CalculatePathLength.
    virtual Vec2f GetPathPoint(float r, float pathLength) = 0;        //parametric function which takes 'r' from 0.0 - 1.0 and return a 2d vector position on path (0.0 = start point, 1.0 = end point)


protected:
    ShapeKind kind = ShapeKind::NOT_DEFINED;
};



///\ingroup MapElements
///\brief The polyline shape. Defines also a rectangle shape.
struct PolyLineShape : public GeometricShape
{

    std::vector<Vec2f>vertices;     // closed curves have added extra point at the end as duplicate of the first point
    bool closed = false;
    bool convex = true;
    bool rectangle = false;
    Vec2f boundingBoxMin;
    Vec2f boundingBoxMax;


    PolyLineShape(){ kind = ShapeKind::POLYLINE; }
    bool IsValid() override { return  !vertices.empty(); }
    float CalculatePathLength() override;
    Vec2f GetPathPoint(float r, float pathLength) override;

    void UpdateBoundingBox();
};


struct BezierVertex
{
    Vec2f P;
    Vec2f CPprevious;       // used for bezier curves
    Vec2f CPnext;

    BezierVertex(Vec2f _P, Vec2f _BPprevious, Vec2f _BPnext) : P(_P),CPprevious(_BPprevious),CPnext(_BPnext){}
};


///\ingroup MapElements
///\brief The bezier shape.
struct BezierShape : public GeometricShape
{
    std::vector<BezierVertex>vertices;
    bool closed = false;
    std::vector<Vec2f>polylineVertices;      // bezier curve as polycurve


    BezierShape(){ kind = ShapeKind::BEZIER_POLYCURVE; }
    bool IsValid() override { return  !vertices.empty(); }
    float CalculatePathLength() override;
    Vec2f GetPathPoint(float r, float pathLength) override;

};


///\ingroup MapElements
///\brief The ellipse shape.
struct EllipseShape : public GeometricShape
{
    Vec2f center;
    float a = 0;
    float b = 0;


    EllipseShape(){ kind = ShapeKind::ELLIPSE; }
    bool IsValid() override {return  a>0 && b>0;}
    float CalculatePathLength() override;
    Vec2f GetPathPoint(float r, float pathLength) override;
};


///\ingroup MapElements
///\brief The single point shape.
struct SinglePointShape : public GeometricShape
{
    Vec2f point;


    SinglePointShape(){ kind = ShapeKind::SINGLE_POINT; }
    bool IsValid() override { return true;}
    float CalculatePathLength() override {return 0;}
    Vec2f GetPathPoint(float r, float pathLength) override {return Vec2f(point.x, point.y);}
};


///\ingroup MapElements
///\brief The vector shape.
///
/// The VectorShape class represents vector shapes from JugiMap Editor.
/// It serves as a wrapper for a more lightweight GeometricShape objects which it also owns.
 class VectorShape
{
public:
    friend class JugiMapBinaryLoader;


    ///\brief Base constructor.
    VectorShape(){}


    ///\brief Create a vector shape with the given data parameters *_probe*, *_dataFlags* and *_parameters*.
    ///
    /// A geometric shape must be set before this vector shape will be ready to use.
    VectorShape(bool _probe, int _dataFlags, const std::vector<jugimap::Parameter> &_parameters) : probe(_probe), dataFlags(_dataFlags), parameters(_parameters){}


    ///\brief Create a vector shape with the given *_geomShape*.
    VectorShape(GeometricShape *_geomShape) : shape(_geomShape){}


    ///\brief Copy constructor.
    VectorShape(const VectorShape &vs);


    ///\brief Destructor.
    ~VectorShape(){if(shape) delete shape;}


    ///\brief Returns the geometric shape of this vector shape.
    GeometricShape * GetGeometricShape() {return shape;}


    ///\brief  Set the geometric shape of this vector  shape to *_geomShape*.
    void SetGeometricShape(GeometricShape *_geomShape){ shape = _geomShape;}


    ///\brief Returns the kind of this shape.
    ShapeKind GetKind() {return shape->GetKind();}


    ///\brief Returns the *probe* flag of this vector shape.
    bool IsProbe() {return probe;}


    ///\brief Returns the *data flags* of this vector shape.
    int GetDataFlags() {return dataFlags;}


    ///\brief Returns a reference to the vector of data parameters in this vector shape.
    std::vector<jugimap::Parameter>& GetParameters() {return parameters;}


    ///\brief Calculate and returns the length of this vector shape.
    float CalculatePathLength(){pathLength = shape->CalculatePathLength(); return pathLength;}


    ///\brief Returns the length of this vector shape.
    float GetPathLength() { return pathLength;}


    ///\brief Returns a point along the path at the given parametric position *r*.
    Vec2f GetPathPoint(float r) {return shape->GetPathPoint(r,pathLength);}


    ///\brief Returns *true* if this vector shape has a geometric object which is valid; otherwise returns false.
    bool IsValid() {return shape->IsValid();}

private:

    GeometricShape * shape = nullptr;                   // OWNED
    bool probe = false;                                 // custom identifier
    int dataFlags = 0;                                  // custom data
    std::vector<jugimap::Parameter> parameters;            // custom parameters
    float pathLength = 0.0;
};





}
#endif // VECTORSHAPE_H
