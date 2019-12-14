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


const double pi = 3.14159265358979323846;


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




/// @}      //end addtogroup CommonFunctions



}




#endif


