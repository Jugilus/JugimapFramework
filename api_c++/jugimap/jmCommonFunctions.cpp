#include <string>
#include <sstream>
#ifdef _WIN32
    #include "Windows.h"
#endif
#include "jmCommonFunctions.h"



namespace jugimap {




void DbgOutput(const std::string &_text)
{

#ifdef _WIN32

    // this works only for ASCII text !

    std::wstring wsTmp(_text.begin(), _text.end());

    std::wstringstream ss;
    ss << wsTmp << std::endl;
    OutputDebugStringW(ss.str().c_str());

#endif

}


double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


int iRand(int iMin, int iMax)
{
    return rand() % (iMax-iMin+1) + iMin;
}


std::string ExtractNameFromPath(std::string path)
{

    size_t indexLastSlash = path.find_last_of("\\/");
    if (std::string::npos != indexLastSlash) path.erase(0, indexLastSlash+1);
    size_t indexPeriod = path.rfind('.');
    if (std::string::npos != indexPeriod) path.erase(indexPeriod);

    return path;
}


//================================================================================================================================


AffineMat3f MakeTransformationMatrix(Vec2f pos, Vec2f scale, Vec2i flip, float rotation, Vec2f handle)
{
    AffineMat3f m = MakeTransformationMatrix(pos, scale, flip, rotation);
    m = m.Translate(-handle.x, -handle.y);
    return m;
}


AffineMat3f MakeTransformationMatrix(Vec2f pos, Vec2f scale, Vec2i flip, float rotation)
{
    AffineMat3f m = AffineMat3f::Translation(pos.x, pos.y);
    m = m.Rotate(rotation*pi/180.0);
    float scaleX = scale.x;
    if(flip.x) scaleX = -scaleX;
    float scaleY = scale.y;
    if(flip.y) scaleY = -scaleY;
    //m = m.Scale(scaleX, scaleY);
    m = m.Scale((flip.x==1)? -scale.x : scale.x, (flip.y==1)? -scale.y : scale.y);

    return m;
}


Rectf TransformRectf(const Rectf &rect, const AffineMat3f &matrix)
{

    Vec2f p1 = matrix * Vec2f( rect.min.x, rect.min.y );
    Vec2f p2 = matrix * Vec2f( rect.max.x, rect.min.y );
    Vec2f p3 = matrix * Vec2f( rect.max.x, rect.max.y );
    Vec2f p4 = matrix * Vec2f( rect.min.x, rect.max.y );

    Vec2f pMin(std::min<float>({p1.x,p2.x,p3.x,p4.x}),  std::min<float>({p1.y,p2.y,p3.y,p4.y}));
    Vec2f pMax(std::max<float>({p1.x,p2.x,p3.x,p4.x}),  std::max<float>({p1.y,p2.y,p3.y,p4.y}));

    return Rectf( std::round( pMin.x ), std::round( pMin.y ), std::round( pMax.x ), std::round( pMax.y ));
}




}
