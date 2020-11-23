#ifndef JUGIMAP_COMMON_H
#define JUGIMAP_COMMON_H

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>



namespace jugimap {



/// \addtogroup Common
/// @{



/// \brief 2d vector.
template< typename T>
struct Vec2
{
    T x;                    ///< The x coordinate.
    T y;                    ///< The y coordinate.


    /// Constructs a vector with coordinates (0,0).
    Vec2(): x(0),y(0){}

    /// Constructs a vector with the given coordinates **_x** and **_y**
    Vec2(T _x, T _y) : x(_x),y(_y){}

    /// Sets the coordinates of this vector to the given **_x** and **_y**.
    inline void Set(T _x, T _y){ x = _x;  y = _y;}

    /// Returns *true* if this vector is equal to given vector **v**; otherwise returns false.
    inline bool operator==(const Vec2<T> &v) const { return(x==v.x && y==v.y);}

    /// Returns *true* if this vector is not equal to given vector **v**; otherwise returns false.
    inline bool operator!=(const Vec2<T> &v) const { return !(*this==v);}

    /// \brief Returns true if the given vectors **v** is equal to this vector; otherwise returns false.
    /// For floating-point types: two compared numbers are considered equal if their difference is smaller then the given **epsilon**.
    inline bool Equals(const Vec2<T> &v, float epsilon = 0.00001f) const { return(x==v.x && y==v.y); }

    /// Return a vector object formed by changing the sign of this vector components.
    inline Vec2<T> operator-() const { return Vec2<T>(-x,-y); }

    /// Returns a vector object formed by multiplying this vector with the given vector **v**.
    inline Vec2<T> operator*(const Vec2<T>&v) const { return Vec2<T>(x*v.x,y*v.y);}

    /// Returns a vector object formed by dividing this vector with the given vector **v**.
    inline Vec2<T> operator/(const Vec2<T>&v) const { return Vec2<T>(x/v.x,y/v.y);}

    /// Returns a vector object formed as the sum of the given **s** to this vector.
    inline Vec2<T> operator+(const Vec2<T>&v) const { return Vec2<T>(x+v.x,y+v.y);}

    /// Returns a vector object formed by subtracting the given **v** from this vector.
    inline Vec2<T> operator-(const Vec2<T>&v) const { return Vec2<T>(x-v.x,y-v.y);}

    /// Adds the given vector **v** to this vector and return a reference to this vector.
    inline Vec2<T>& operator+=(const Vec2<T>&v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    /// Subtracts the given vector **v** from this vector and return a reference to this vector.
    inline Vec2<T>& operator-=(const Vec2<T>&v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    /// Multiplyes this vector with the given vector **v** and return a reference to this vector.
    inline Vec2<T>& operator*=(const Vec2<T>&v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    /// Divides this vector with the given vector **v** and return a reference to this vector.
    inline Vec2<T>& operator/=(const Vec2<T>&v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    /// Adds the given **s** to this vector and return a reference to this vector.
    inline Vec2<T>& operator+=(T s)
    {
        x += s;
        y += s;
        return *this;
    }

    /// Subtracts the given **s** from this vector and return a reference to this vector.
    inline Vec2<T>& operator-=(T s)
    {
        x -= s;
        y -= s;
        return *this;
    }

    /// Multiplies this vector with the given **s** and return a reference to this vector.
    inline Vec2<T>& operator*=(T s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    /// Divides this vector with the given **s** and return a reference to this vector.
    inline Vec2<T>& operator/=(T s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    /// Returns a vector object formed by multiplying this vector with the given **s**.
    inline Vec2<T> operator*(double s) const { return Vec2<T>(x*s,y*s);}

    /// Returns a vector object formed by dividing this vector with the given **s**.
    inline Vec2<T> operator/(double s) const { return Vec2<T>(x/s,y/s);}

    /// Returns a vector object formed by adding the given **s** to this vector.
    inline Vec2<T> operator+(T s) const { return Vec2<T>(x+s,y+s);}

    /// Returns a vector object formed by subtracting the given **s** from this vector.
    inline Vec2<T> operator-(T s) const { return Vec2<T>(x-s,y-s);}

    /// Returns the length of this vector.
    inline double GetLength() const { return std::sqrt(x*x+y*y);}

    /// Returns the square length of this vector.
    inline double GetLength2() const { return x*x+y*y;}

    /// Return the distance from the given vector **v**.
    inline double Distance(const Vec2<T>&v) const { return (v-*this).GetLength();}

    /// Returns the square distance from the given vector **v**.
    inline double Distance2(const Vec2<T>&v) const { return (v-*this).GetLength2();}

    /// Returns a vector object formed by normalizing this vector.
    inline Vec2<T> Normalize() const { return *this/GetLength();}

    /// Returns the dot product of this vector and the given vector **v**.
    inline T Dot(const Vec2<T> &v) const { return x*v.x+y*v.y;}

};


template<>
inline bool Vec2<float>::Equals(const Vec2<float>&v, float epsilon) const
{
    return  fabs(x - v.x)<epsilon && fabs(y - v.y)<epsilon ;
}

typedef Vec2<int> Vec2i;            ///< Vec2 struct in integer precision.
typedef Vec2<float> Vec2f;          ///< Vec2 struct in float precision.




/// \brief Rectangle.
template<typename T>
struct Rect
{

    Vec2<T> min;            ///< The minimum coordinate (the left-top corner when y coordinate points downward).
    Vec2<T> max;            ///< The maximum coordinate (the right-bottom corner when y coordinate points downward).


    /// Constructs an empty 'null' rectangle.
    Rect(){}

    /// Constructs a rectangle from the given minimum and maximum coordinates.
    Rect( T xMin, T yMin, T xMax, T yMax) { min = Vec2<T>( xMin,yMin ); max = Vec2<T>( xMax,yMax ); }

    /// Constructs a rectangle from the given minimum and maximum vectors .
    Rect(const Vec2<T> &_min, const Vec2<T> &_max){ min = _min; max = _max; }

    /// Returns true if this rectangle is equal to the given rectangle **r**; otherwise returns false.
    bool operator==(const Rect<T> &r) const { return(min==r.min && max==r.max); }

    /// Returns true if this rectangle is not equal to the given rectangle **r**; otherwise returns false.
    bool operator!=(const Rect<T> &r) const { return !(*this==r); }

    /// Returns true if this rectangle is equal to the given rectangle **r**; otherwise returns false.
    /// For floating-point types: two compared numbers are considered equal if their difference is smaller then the given **epsilon**.
    inline bool Equals(const Rect<T> &r, float epsilon = 0.00001f) const { return(min==r.min && max==r.max); }

    /// Returns the width of this rectangle.
    inline T GetWidth() const { return max.x-min.x; }

    /// Returns the height of this rectangle.
    inline T GetHeight() const { return max.y-min.y; }

    /// Returns a rectangle formed by uniting this rectangle with the given rectangle **r**.
    Rect<T> Unite(const Rect<T> &r) const
    {
        T x0 = std::min<T>( min.x, r.min.x );
        T y0 = std::min<T>( min.y, r.min.y );
        T x1 = std::max<T>( max.x, r.max.x );
        T y1 = std::max<T>( max.y, r.max.y );
        return Rect<T>( x0,y0,x1,y1 );
    }

     /// Returns a rectangle formed by expanding this rectangle by the given vector **v**.
    Rect<T> Expand(const Vec2<T> &v) const
    {
        T x0 = std::min<T>( min.x, v.x );
        T y0 = std::min<T>( min.y, v.y );
        T x1 = std::max<T>( max.x, v.x );
        T y1 = std::max<T>( max.y, v.y );
        return Rect<T>( x0,y0,x1,y1 );
    }

    /// Returns a vector storing the size of this rectangle; the *width* is stored in *x* and the height is stored in *y*
    inline Vec2<T> GetSize() const { return max-min;}

    /// Returns a vector storing the center point of this rectangle.
    Vec2<T> GetCenter() const {return (min+max)/2;}

    /// Returns true if the minimum and maximum coordinates are equal; otherwise returns false.
    inline bool IsEmpty(){return (max.x==min.x || max.y==min.y);}

    /// Returns true if the maximum coordinate is bigger or equal to the minimum coordinate; otherwise returns false.
    inline bool IsValid(){return (max.x>=min.x || max.y>=min.y);}

    /// Returns true if the given vector point **v** is inside this rectangle; otherwise returns false.
    inline bool Contains(const Vec2<T> &v) const { return v.x>=min.x && v.x<max.x && v.y>=min.y && v.y<max.y; }

    /// Returns true if the given rectangle **r** is inside this rectangle; otherwise returns false.
    inline bool Contains(const Rect<T> &r) const { return min.x<=r.min.x && max.x>=r.max.x && min.y<=r.min.y && max.y>=r.max.y;}

    /// Returns true if the given rectangle **r** is intersects this rectangle; otherwise returns false.
    inline bool Intersects(const Rect<T> &r) const {return r.max.x>min.x && r.min.x<max.x && r.max.y>min.y && r.min.y<max.y;}

};


template<>
inline bool Rect<float>::Equals(const Rect<float>&r, float epsilon) const
{
    return min.Equals(r.min, epsilon) && max.Equals(r.max, epsilon);
}

typedef Rect<int> Recti;                ///< Rect struct in integer precision.
typedef Rect<float> Rectf;              ///< Rect struct in float precision.



/// \brief Matrix representation of the affine transformation.
template<typename T>
struct AffineMat3
{

    Vec2<T> i;              ///< The first row of the matrix.
    Vec2<T> j;              ///< The second row of the matrix.
    Vec2<T> t;              ///< The third row of the matrix.

    /// Constructs an identity matrix.
    AffineMat3(){i.x=1; j.y=1;}

    /// Constructs a matrix from the given row vectors **_i**, **_j** and **_t**.
    AffineMat3(const Vec2<T> &_i, const Vec2<T> &_j, const Vec2<T> &_t): i(_i),j(_j),t(_t){}

    /// Constructs a matrix from the given row vector components **ix**, **iy**, **jx**, **jy**, **tx** and **ty**.
    AffineMat3(T ix, T iy, T jx, T jy, T tx, T ty){ i.x=ix; i.y=iy; j.x=jx; j.y=jy; t.x=tx; t.y=ty; }


    /// Returns a matrix formed by inverting this matrix.
    inline AffineMat3<T> Invert() const
    {
        float idet = 1.0/(i.x*j.y-i.y*j.x);
        return AffineMat3<T>(
            j.y*idet , -i.y*idet,
            -j.x*idet , i.x*idet,
            (j.x*t.y-j.y*t.x)*idet , (i.y*t.x-i.x*t.y)*idet );
    }

    /// Returns a matrix formed by multiplying this matrix with the given vector **v**.
    Vec2<T> operator*(const Vec2<T> &v) const
    {
        return Vec2<T>( i.x*v.x + j.x*v.y + t.x , i.y*v.x + j.y*v.y + t.y );
    }

    /// Returns a matrix formed by multiplying this matrix with the given matrix **m**.
    inline AffineMat3<T> operator*(const AffineMat3<T> &m ) const
    {
        return AffineMat3<T>(
            i.x*m.i.x + j.x*m.i.y       , i.y*m.i.x + j.y*m.i.y ,
            i.x*m.j.x + j.x*m.j.y       , i.y*m.j.x + j.y*m.j.y ,
            i.x*m.t.x + j.x*m.t.y + t.x , i.y*m.t.x + j.y*m.t.y + t.y );
    }

    /// Returns a vector formed by multiplying the given vector **v** with this matrix.
    inline Vec2<T> Transform(const Vec2<T> &v) const
    {
        return Vec2<T>( i.x*v.x + j.x*v.y + t.x , i.y*v.x + j.y*v.y + t.y );
    }


    /// Returns a vector formed by multiplying the given coordinate **x**,**y** with this matrix.
    inline Vec2<T> Transform(T x, T y)  const
    {
        return Vec2<T>( i.x*x + j.x*y + t.x , i.y*x + j.y*y + t.y );
    }


    /// Returns a matrix formed by multiplying this matrix with the given matrix components **ix**, **iy**, **jx**, **jy**, **tx** and **ty**.
    inline AffineMat3<T> Transform(float ix, float iy, float jx, float jy, float tx, float ty)  const
    {
        return AffineMat3<T>(
            i.x*ix + j.x*iy       , i.y*ix + j.y*iy ,
            i.x*jx + j.x*jy       , i.y*jx + j.y*jy ,
            i.x*tx + j.x*ty + t.x , i.y*tx + j.y*ty + t.y );
    }


    /// Returns a matrix formed by translating this matrix by the given vector coordinates **tx** and **ty**.
    inline AffineMat3<T> Translate(T tx, T ty) const
    {
        return Transform( 1,0,0,1,tx,ty );
    }


    /// Returns a matrix formed by translating this matrix by the given vector **tv**.
    inline AffineMat3<T> Translate(const Vec2<T> &tv) const { return Transform( 1,0,0,1,tv.x,tv.y );}

    /// Returns a matrix formed by rotating this matrix by the given angle **rz**; the angle is in radians.
    inline AffineMat3<T> Rotate(double rz) const { return Transform( std::cos( rz ),-std::sin( rz ), std::sin( rz ), std::cos( rz ),0,0 );}

    /// Returns a matrix formed by scaling this matrix by the given vector components **sx** and **sy**.
    inline AffineMat3<T> Scale(T sx, T sy) const { return Transform( sx,0,0,sy,0,0 );}

    /// Returns a matrix formed by scaling this matrix by the given vector **sv**.
    inline AffineMat3<T> Scale(const Vec2<T> &sv) const { return Transform( sv.x,0,0,sv.y,0,0 );}

    /// Creates a matrix representing a translation by the given vector coordinates **tx** and **ty**.
    AffineMat3<T> static Translation(T tx, T ty){ return AffineMat3<T>( 1,0,0,1,tx,ty ); }

    /// Creates a matrix representing a translation by the given vector **tv**.
    AffineMat3<T> static Translation(const Vec2<T> &tv){ return AffineMat3<T>( 1,0,0,1,tv.x,tv.y );}

    /// Creates a matrix representing a rotation by the given angle **rz**; the angle is in radians.
    AffineMat3<T> static Rotation(double rz){ return AffineMat3<T>( std::cos( rz ),-std::sin( rz ), std::sin( rz ), std::cos( rz ),0,0 ); }

    /// Creates a matrix representing a scaling by the given vector components **sx** and **sy**.
    AffineMat3<T> static Scaling(T sx, T sy){ return AffineMat3<T>( sx,0,0,sy,0,0 ); }

    /// Creates a matrix representing a scaling by the given vector **sv**.
    AffineMat3<T> static Scaling(const Vec2<T> &sv){ return AffineMat3<T>( sv.x,0,0,sv.y,0,0 );}


    /// Creates a matrix representing an orthographic projection.
    AffineMat3<T> static Ortho(T left, T right, T bottom, T top)
    {

        T w = right-left;
        T h = top-bottom;

        AffineMat3<T> r;
        r.i.x=2/w;
        r.j.y=2/h;
        r.t.x=-(right+left)/w;
        r.t.y=-(top+bottom)/h;
        return r;
    }

};

typedef AffineMat3<float> AffineMat3f;      ///< AffineMat3 struct in float precision.




//================================================================================================================================


/// \brief The Parameter struct defines a pair *name : value*.
///
/// The Parameter struct represents a paramater from JugiMap Editor.
struct Parameter
{
    std::string name;               ///< The name of this parameter.
    std::string value;              ///< The value of this parameter.
    int kind = -1;                  ///< \brief The paramters kind as defined in JugiMap editor. This value is used at the map loading and has no real purpose.


    /// \brief Searches for a parameter with the given **name** and **value** in the **parameters**. If the **value** is not specified it searches only for the **name**.
    /// If a parameter is found returns true; otherwise returns false.
    static bool Exists(const std::vector<Parameter> &parameters, const std::string &name, const std::string &value="");

    /// Searches for a parameter with the given **name** in the **parameters**. If found returns its value, otherwise returns the given **defaultValue**.
    static std::string GetValue(const std::vector<Parameter> &parameters, const std::string &name, const std::string &defaultValue="");

    /// Searches for a parameter with the given **name** in the **parameters**. If found returns its value converted to integer, otherwise returns the given **defaultValue**.
    static int GetIntValue(const std::vector<Parameter> &parameters, const std::string &name, int defaultValue=0);

    /// Searches for a parameter with the given **name** in the **parameters**. If found returns its value converted to float, otherwise returns the given **defaultValue**.
    static float GetFloatValue(const std::vector<Parameter> &parameters, const std::string &name, float defaultValue=0.0);


    static Parameter Parse(const std::string &s);

};



//================================================================================================================================


/// \brief The Easing struct defines interpolation methods used for obtaining values between a start value and an end value. It is used in
/// timeline animations and tweens.
class Easing
{
public:

    /// \brief The kind of Easing.
    enum class Kind : int
    {
        LINEAR = 0,             ///< Linear transition from the start value to the end value.
        EASE_START = 1,         ///< Ease-out from the start value (quadratic interpolation).
        EASE_END = 2,           ///< Ease-in into the end value.
        EASE_START_END = 3,     ///< Ease-out from the start value and ease-in into the end value.
        CONSTANT = 4            ///< Constant value; the same as the start value.
    };


    /// Returns an ouput value according to the Easing::Kind. The given parameter **p** must be in range from 0.0 to 1.0.
    float GetValue(float p);

    Kind kind = Kind::LINEAR;

};


//================================================================================================================================


/// \brief The Tween class defines transition between two values over time.
class Tween
{
public:
    friend class Tweens;

    /// The tween mode.
    enum class Mode
    {
        NORMAL,             ///< The tween runs from the starting value to the end value and then stops.
        REVERSE,            ///< The tween runs from the end value to the start value and then stops.
        LOOP,               ///< The tween runs in loops from the start value to the end value.
        LOOP_REVERSE        ///< The tween runs in loops from the start value to the end value and reverse direction on each loop.
    };

    //~Tween();


    /// \brief Initialize the parameters of this tween.
    ///
    /// \param _valueStart The starting value.
    /// \param _valueEnd The end value.
    /// \param _durationS Duration of one loop in seconds.
    /// \param _easingKind The easing kind.
    void Init(float _valueStart, float _valueEnd, float _durationS, Easing::Kind _easingKind);


    /// \brief Start running this tween.
    void Play();


    /// \brief Updates the tween value and returns it.
    ///
    /// This function must be called inside the application update loop.
    float Update();


    /// \brief Returns true if this tween is not running; otherwise return false.
    bool IsIdle() { return state==stateIDLE || state == stateNEVER_INITIALIZED; }


    /// \brief Stop running the tween.
    void Stop();


    /// \brief Pause running the tween.
    ///
    /// \see Resume
    void Pause();


    /// \brief Resume running the tween.
    ///
    /// \see Pause
    void Resume();


    /// \brief Set the easing kind of this tween.
    void SetEasingKind(Easing::Kind _kind) { easing.kind = _kind; }


    /// \brief Set the mode of this tween.
    void SetMode(Mode _mode) { mode = _mode;}


    /// \brief Returns the starting value of this tween;
    float GetStartValue(){ return valueStart;}


    /// \brief Returns the end value of this tween;
    float GetEndValue(){ return valueEnd;}




private:
    float value = 0;
    float valueStart = 0.0;
    float valueEnd = 0.0;
    int timeStartMS = 0;
    int timeEndMS = 0;
    int durationMS = 0;
    Easing easing;

    int state = stateNEVER_INITIALIZED;
    static const int stateNEVER_INITIALIZED = -1;
    static const int stateIDLE = 0;
    static const int statePLAYING = 1;
    static const int statePAUSED = 3;


    Mode mode = Mode::NORMAL;
    bool reverse = false;

    int stateStored = 0;
    int msTimeStored = 0;


};


//================================================================================================================================


/// \brief The CustomObject class is the base class for custom objects.
struct CustomObject
{
public:

    virtual ~CustomObject(){}

    virtual std::string GetInfo(){ return ""; }


    /// Set the tag of this custom object.
    void SetTag(int _tag){ tag = _tag; }

    /// Returns the tag of this custom object.
    int GetTag(){ return tag; }


private:
    int tag = 0;

};



/// @}      //end addtogroup Common

}




#endif


