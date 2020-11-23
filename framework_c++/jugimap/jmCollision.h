#ifndef JUGIMAP_COLLISION_H
#define JUGIMAP_COLLISION_H

#include <vector>
#include "jmCommon.h"



namespace jugimap{


struct GeometricShape;


struct Collision
{

    // Returns true if the given **shape1** and **shape2** overlaps; otherwise return false.
    static bool ShapesOverlap(GeometricShape *shape1, GeometricShape *shape2);

    // Returns true if the given **point** is inside **shape**; otherwise return false.
    static bool PointInShape(Vec2f point, GeometricShape *shape);

    // Returns true if the line from the given **rayStart** to **rayEnd** intersects **shape**; otherwise returns false.
    // The nearest intersection point is stored in the **intersection**. If **rayLength** is specified (bigger then zero)
    // then **rayEnd** is modified to a point at that length.
    static bool RaycastToShape(Vec2f rayStart, Vec2f rayEnd, GeometricShape *shape, Vec2f &intersection, float rayLength=-1);

    static bool RaycastToShapes(Vec2f rayStart, Vec2f rayEnd, std::vector<GeometricShape*>&shapes, Vec2f &intersection, float rayLength=-1);
    static void GetRaycastToShapesIntersections(Vec2f rayStart, Vec2f rayEnd, GeometricShape *shape, std::vector<Vec2f>&intersections, float rayLength=-1);

    // Transform the given **srcShape* and store results in **dstShape** using **pos**, **scale**, **flip**, **rotation** and **handle**
    // **srcShape** and **dstShape** must be of the same kind and have the same number of vertices otherwise the transformation is not performed.
    static void TransformShapeToWorldCoordinates(GeometricShape *srcShape, GeometricShape *dstShape, Vec2f pos, Vec2f scale, Vec2i flip, float rotation, Vec2f handle);
    
    // Transform the given **srcShape* and store results in **dstShape** using **pos**, **scale**, **flip**, **rotation** and **handle**
    // **srcShape** and **dstShape** must be of the same kind and have the same number of vertices otherwise the transformation is not performed.
    static void TransformShapeToWorldCoordinates(GeometricShape *srcShape, GeometricShape *dstShape, const AffineMat3f &m, Vec2f scale, Vec2i flip, float rotation);

private:
    static std::vector<Vec2f>intersections;
    static bool GetNearestIntersection(Vec2f rayStart, Vec2f &intersection);



    //---- SHAPE to SHAPE

    static inline bool RectToRect(Vec2f Av1, Vec2f Av2,  Vec2f Bv1, Vec2f Bv2)
    {
        return Av1.x < Bv2.x  &&  Av2.x > Bv1.x  &&  Av1.y < Bv2.y  &&  Av2.y > Bv1.y;
    }

    static bool RectToCircle(Vec2f Av1, Vec2f Av2, Vec2f center, float radius);


    static inline bool CircleToCircle(Vec2f center1, float radius1, Vec2f center2, float radius2)
    {
        return (center1.x-center2.x)*(center1.x-center2.x) + (center1.y-center2.y)*(center1.y-center2.y) < (radius1+radius2)*(radius1+radius2);
    }


    static bool PolyToPoly(const std::vector<Vec2f> &poly1, const std::vector<Vec2f> &poly2);


    static bool CircleToPoly(Vec2f center, float radius, const std::vector<Vec2f> &poly);


    //---- POINT in SHAPE

    static inline bool PointInRect(Vec2f v,  Vec2f Av1, Vec2f Av2)
    {
        return v.x>=Av1.x && v.x<=Av2.x && v.y>=Av1.y && v.y<=Av2.y;
    }


    static inline bool PointInCircle(Vec2f v, Vec2f cv, float cr)
    {
        return (cv.x-v.x)*(cv.x-v.x) + (cv.y-v.y)*(cv.y-v.y) < cr*cr;
    }


    static bool PointInPoly(Vec2f point, const std::vector<Vec2f> &poly);


    //----- LINE to SHAPE

    static bool LineToPoly(Vec2f line_v1, Vec2f line_v2, const std::vector<Vec2f> &poly);


    static bool LineToCircle(Vec2f v1, Vec2f v2, Vec2f center, float radius);


    static void LineToPoly_GetIntersections(Vec2f line_v1, Vec2f line_v2, const std::vector<Vec2f> &poly, std::vector<Vec2f> &intersections);


    static void LineToCircle_GetIntersections(Vec2f line_v1, Vec2f line_v2, Vec2f center, float radius, std::vector<Vec2f> &intersections);


    //-----

    static bool LinesCross(Vec2f v0, Vec2f v1, Vec2f v2, Vec2f v3);


    static bool LinesCross(Vec2f v0, Vec2f v1, Vec2f v2, Vec2f v3, Vec2f &vIntersection);


    static inline float PointToPointDist(Vec2f v1, Vec2f v2)
    {
        return std::sqrt((v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y));
    }



};




class StandardSprite;



class Collider
{
public:

    enum class Kind{SINGLE_SHAPE, MULTI_SHAPE, NOT_DEFINED};

    Collider(StandardSprite* _standardSprite) : standardSprite(_standardSprite){}
    virtual ~Collider(){}

    StandardSprite* GetStandardSprite(){ return standardSprite; }

    Kind GetKind(){ return kind; }
    virtual void AddShapes() = 0;
    virtual void UpdateShapes(const AffineMat3f &m) = 0;
    virtual void ClearShapes() = 0;
    virtual int GetShapesCount() = 0;
    virtual bool PointInside(Vec2f _pos)= 0;
    virtual bool Overlaps(Collider *_otherCollider)= 0;
    virtual bool Overlaps(GeometricShape *_geomShape){ return false; }
    virtual bool RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos)= 0;

    void _SetKind(Kind _kind){ kind = _kind; }

private:
    StandardSprite* standardSprite = nullptr;           //LINK
    Kind kind = Kind::NOT_DEFINED;

};


class SingleShapeCollider : public Collider
{
public:

    SingleShapeCollider(StandardSprite* _standardSprite) : Collider(_standardSprite){ _SetKind(Kind::SINGLE_SHAPE); }
    virtual ~SingleShapeCollider();

    GeometricShape* GetShape(){ return shape; }

    virtual void AddShapes() override;
    virtual void UpdateShapes(const AffineMat3f &m) override;
    virtual void ClearShapes() override;
    virtual int GetShapesCount() override { return (shape!=nullptr)? 1 : 0; }
    virtual bool PointInside(Vec2f _pos) override;
    virtual bool Overlaps(Collider *_otherCollider) override;
    virtual bool Overlaps(GeometricShape *_geomShape) override;
    virtual bool RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos) override;

private:
    GeometricShape* shape = nullptr;

};


class MultiShapesCollider : public Collider
{
public:

    MultiShapesCollider(StandardSprite* _standardSprite) : Collider(_standardSprite){ _SetKind(Kind::MULTI_SHAPE); }
    virtual ~MultiShapesCollider();

    std::vector<GeometricShape*>& GetShapes(){ return shapes; }

    virtual void AddShapes() override;
    virtual void UpdateShapes(const AffineMat3f &m) override;
    virtual void ClearShapes() override;
    virtual int GetShapesCount() override { return shapes.size(); }

    virtual bool PointInside(Vec2f _pos) override;
    virtual bool Overlaps(Collider *_otherCollider) override;
    virtual bool Overlaps(GeometricShape *_geomShape) override;
    virtual bool RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos) override;

private:
    std::vector<GeometricShape*>shapes;

};



}

#endif
