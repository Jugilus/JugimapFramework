#ifndef JUGIMAP_COMMON_FUNCTIONS_H
#define JUGIMAP_COMMON_FUNCTIONS_H

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include "jmCommon.h"





namespace jugimap {



/// \addtogroup CommonFunctions
/// @{


//const double pi = 3.14159265358979323846;


const double mathPI = 3.14159265;




/// \brief Print ASCII text in IDE output window - MS Windows only.
void DbgOutput(const std::string &_text);


/// \brief Returns true if the given **a** and **b** are equal; otherwise returns false.
///
/// The compared floating-point numbers are considered equal if their difference is smaller then the given **epsilon**.
inline bool AreEqual(double a, double b, double epsilon=0.0001)
{
    return fabs(a - b) < epsilon;
}


/// \brief Returns 1 if the sign of the given **value** is positive number; returns -1 if it's negative; returns 0 if it's 0.
template <typename T>
inline int Sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


/// \brief Returns a random float number between (and including) the given **fMin** and **fMax**.
double fRand(double fMin, double fMax);


/// \brief Returns a random integer number between (and including) the given **iMin** and **iMax**.
int iRand(int iMin, int iMax);


/// \brief Returns a **value** clamped to the **valueMin** and **valueMax**.
template<typename T>
inline T ClampValue(T value, T valueMin, T valueMax)
{
    if(value<=valueMin)
        return valueMin;
    else if(value>=valueMax)
        return valueMax;
    else
        return value;

}


std::string ExtractNameFromPath(std::string path);


std::vector<std::string>SplitString(std::string text, const std::string &delimiter);


void RemoveStringWhiteSpacesOnStartAndEnd(std::string &s);


inline bool IsWhiteSpaceCharacter(char c) { return (c==' ' || c== '\n' || c=='\r' || c=='\t' || c=='\v' || c== '\f'); }


inline void DummyFunction()
{
}



/// Returns a Vec2f object formed by converting Vec2i to Vec2f.
inline Vec2f Vec2iToVec2f(const Vec2i &v)
{
    return Vec2f(v.x, v.y);
}


/// Returns a Vec2i object formed by converting Vec2f to Vec2i.
inline Vec2i Vec2fToVec2i(const Vec2f &v)
{
    return Vec2i(std::round(v.x), std::round(v.y));
}


/// Returns a Vec2f at the given **distance** from **P1** to **P2** using linear interpolation.
template<typename T>
Vec2f MakeVec2fAtDistanceFromP1(const Vec2<T>& p1, const Vec2<T>& p2, double distance)
{
    double fDistance = distance/p1.Distance(p2);
    return Vec2f(p1.x + fDistance* (p2.x-p1.x), p1.y + fDistance* (p2.y-p1.y));
}


///\brief Returns a Vec2f at the given **fDistance** from **P1** to **P2** using linear interpolation.
/// **fDistance** is defined as 'distanceP1toP/distanceP1toP2'.
template<typename T>
Vec2f MakeVec2fAtDistanceFactorFromP1(const Vec2<T>& p1, const Vec2<T>& p2, double fDistance)
{
    return Vec2f(p1.x + fDistance* (p2.x-p1.x), p1.y + fDistance* (p2.y-p1.y));
}



/// Returns a Rectf object formed by converting the given Recti object **r** to Rectf.
inline Rectf RectiToRectf(const Recti &r)
{
    return Rectf(Vec2iToVec2f(r.min), Vec2iToVec2f(r.max));
}

/// Returns a Recti object formed by converting the given Rectf object **r** to Recti.
inline Recti RectfToRecti(const Rectf &r)
{
    return Recti(Vec2fToVec2i(r.min), Vec2fToVec2i(r.max));
}


/// Returns an AffineMat3f matrix for the given transformations **pos**, **scale**, **flip**, **rotation** and **handle**.
AffineMat3f MakeTransformationMatrix(Vec2f pos, Vec2f scale, Vec2i flip, float rotation, Vec2f handle);

/// Returns an AffineMat3f matrix for the given transformations **pos**, **scale**, **flip** and **rotation**.
AffineMat3f MakeTransformationMatrix(Vec2f pos, Vec2f scale, Vec2i flip, float rotation);


/// Returns a Rectf bounding of the given rectangle **rect** transformed by **matrix**.
Rectf TransformRectf(const Rectf &rect, const AffineMat3f &matrix);



//-----


template<typename T>
inline double DistanceTwoPoints(const T& P1, const T& P2)
{
    return std::sqrt((P2.x-P1.x)*(P2.x-P1.x) + (P2.y-P1.y)*(P2.y-P1.y));
}


inline double DistanceTwoPoints(double x1, double y1, double x2, double y2)
{
    return std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}


template<typename T>
inline double SquareDistanceTwoPoints(const T& P1, const T& P2)
{
    return (P1.x - P2.x)*(P1.x - P2.x) + (P1.y - P2.y)*(P1.y - P2.y);
}


template<typename T>
inline double distToSegment(const T& P, const T& P1, const T& P2, T& ProjectedPoint)
{
  double l2 = SquareDistanceTwoPoints(P1, P2);
  if (l2 == 0){
      ProjectedPoint.Set(P1.x, P1.y);
      return std::sqrt(SquareDistanceTwoPoints(P, P1));
  }
  double t = ((P.x - P1.x) * (P2.x - P1.x) + (P.y - P1.y) * (P2.y - P1.y)) / l2;
  if (t < 0){
      ProjectedPoint.Set(P1.x, P1.y);
      return std::sqrt(SquareDistanceTwoPoints(P, P1));
  }
  if (t > 1){
      ProjectedPoint.Set(P2.x, P2.y);
      return std::sqrt(SquareDistanceTwoPoints(P, P2));
  }
  ProjectedPoint.Set(P1.x +  t* ( P2.x - P1.x ), P1.y +  t* ( P2.y - P1.y ) );
  return std::sqrt(SquareDistanceTwoPoints(P, ProjectedPoint));
}


template<typename T>
inline double distToSegment(const T& P, const T& P1, const T& P2)
{

    T projectedPoint;
    return distToSegment(P, P1, P2 , projectedPoint);

    /*
    double l2 = SquareDistanceTwoPoints(P1, P2);
    if (l2 == 0) return std::sqrt(SquareDistanceTwoPoints(P, P1));
    double t = ((P.x - P1.x) * (P2.x - P1.x) + (P.y - P1.y) * (P2.y - P1.y)) / l2;
    if (t < 0){
        return std::sqrt(SquareDistanceTwoPoints(P, P1));
    }
    if (t > 1){
        return std::sqrt(SquareDistanceTwoPoints(P, P2));
    }
    T ProjectedPoint(P1.x +  t* ( P2.x - P1.x ), P1.y +  t* ( P2.y - P1.y ) );
    return std::sqrt(SquareDistanceTwoPoints(P, ProjectedPoint));
    */
}


template<typename T>
inline double distToLine(const T& P, const T& P1, const T& P2, T& ProjectedPoint)
{
  double l2 = SquareDistanceTwoPoints(P1, P2);
  if (l2 == 0) return std::sqrt(SquareDistanceTwoPoints(P, P1));
  double t = ((P.x - P1.x) * (P2.x - P1.x) + (P.y - P1.y) * (P2.y - P1.y)) / l2;

  ProjectedPoint.Set(P1.x +  t* ( P2.x - P1.x ), P1.y +  t* ( P2.y - P1.y ) );
  return std::sqrt(SquareDistanceTwoPoints(P, ProjectedPoint));
}


template<typename T>
inline double distToLine(const T& P, const T& P1, const T& P2)
{
    T projectedPoint;
    return distToLine(P, P1, P2 , projectedPoint);
}


template<typename T>
inline double AngleBetweenTwoPoints(const T& P1, const T& P2)
{
    return std::atan2(P2.y-P1.y, P2.x-P1.x) * 180.0/mathPI;
}


//-----


template<typename T>
int GetIndexOfVectorElement(std::vector<T*>&v, T* element)
{
    int index = -1;

    if(v.empty()==false){
        for(int i=0; i<v.size(); i++){
            if(v[i]==element){
                index = i;
            }
        }
    }

    return index;
}


template<typename T>
bool RemoveElementFromVector(std::vector<T*>&v, T* element)
{

    for(int i = int(v.size())-1; i>=0; i-- ){
        if( v[i]==element ){
            v.erase(v.begin()+i);
            return true;
        }
    }

    return false;

}


template<typename T>
bool AddElementToVectorIfNotIn(std::vector<T*>&v, T* element)
{

    if(std::find(v.begin(), v.end(), element) == v.end()) {
        v.push_back(element);
        return true;
    }

    return false;
}



/// @}      //end addtogroup CommonFunctions



}




#endif


