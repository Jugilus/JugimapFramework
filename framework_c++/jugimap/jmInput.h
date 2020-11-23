#ifndef JUGIMAP_INPUT_H
#define JUGIMAP_INPUT_H

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <array>
#include <string>

#include "jmCommon.h"


namespace jugimap{
	
class Sprite;


/// \addtogroup Input
/// @{




struct ButtonState
{
    bool pressed = false;
    bool down = false;
    bool released = false;

    void _Set(bool _down);
    void Reset(){ pressed = down = released = false; }
    void ResetPerUpdateFlags(){ pressed = released = false; }

};


///\brief The key codes of keyboard keys
enum class KeyCode
{

    UNKNOWN = 0,                ///< Unknown

    BACKSPACE=8,                ///< Backspace
    TAB=9,                      ///< Tab
    ENTER=13,                   ///< Enter
    PAUSE=19,                   ///< Pause
    ESCAPE=27,                  ///< Escape
    SPACE=32,                   ///< Space

    PAGEUP=33,                  ///< Page Up
    PAGEDOWN=34,                ///< Page Down
    END=35,                     ///< End
    HOME=36,                    ///< Home

    LEFT=37,                    ///< Left
    UP=38,                      ///< Up
    RIGHT=39,                   ///< Right
    DOWN=40,                    ///< Down

    PRINT=42,                   ///< Print
    INSERT=45,                  ///< Insert
    DELETEkey=46,               ///< Delete

    NUM_0=48,                   ///< 0
    NUM_1=49,                   ///< 1
    NUM_2=50,                   ///< 2
    NUM_3=51,                   ///< 3
    NUM_4=52,                   ///< 4
    NUM_5=53,                   ///< 5
    NUM_6=54,                   ///< 6
    NUM_7=55,                   ///< 7
    NUM_8=56,                   ///< 8
    NUM_9=57,                   ///< 9

    A=65,                       ///< A
    B=66,                       ///< B
    C=67,                       ///< C
    D=68,                       ///< D
    E=69,                       ///< E
    F=70,                       ///< F
    G=71,                       ///< G
    H=72,                       ///< G
    I=73,                       ///< I
    J=74,                       ///< J
    K=75,                       ///< K
    L=76,                       ///< L
    M=77,                       ///< M
    N=78,                       ///< N
    O=79,                       ///< O
    P=80,                       ///< P
    Q=81,                       ///< Q
    R=82,                       ///< R
    S=83,                       ///< S
    T=84,                       ///< T
    U=85,                       ///< U
    V=86,                       ///< V
    W=87,                       ///< W
    X=88,                       ///< X
    Y=89,                       ///< Y
    Z=90,                       ///< Z

    /*
    KEY_NUM0=96,
    KEY_NUM1=97,
    KEY_NUM2=98,
    KEY_NUM3=99,
    KEY_NUM4=100,
    KEY_NUM5=101,
    KEY_NUM6=102,
    KEY_NUM7=103,
    KEY_NUM8=104,
    KEY_NUM9=105,

    KEY_NUMMULTIPLY=106,
    KEY_NUMADD=107,
    KEY_NUMSUBTRACT=109,
    KEY_NUMDECIMAL=110,
    KEY_NUMDIVIDE=111,
    */

    F1=112,                     ///< F1
    F2=113,                     ///< F2
    F3=114,                     ///< F3
    F4=115,                     ///< F4
    F5=116,                     ///< F5
    F6=117,                     ///< F6
    F7=118,                     ///< F7
    F8=119,                     ///< F8
    F9=120,                     ///< F9
    F10=121,                    ///< F10
    F11=122,                    ///< F11
    F12=123,                    ///< F12


    // 165 - 190 - reserved for upper case letters using printable characters table
    TILDE=192,                  ///< Tilde
    HYPHEN=193,                 ///< Hyphen
    EQUAL=194,                  ///< Equal
    SEMICOLON=195,              ///< Semicolon
    QUOTE=196,                  ///< Quote
    COMMA=197,                  ///< Comma
    PERIOD=198,                 ///< Period
    SLASH=199,                  ///< Slash
    LEFT_BRACKET=200,           ///< Left Bracket
    RIGHT_BRACKET=201,          ///< Right Bracket
    BACKSLASH=202,              ///< Backslash

    LEFT_SHIFT=210,             ///< Left Shift
    RIGHT_SHIFT=211,            ///< Right Shift
    LEFT_CONTROL=212,           ///< Left Control
    RIGHT_CONTROL=213,          ///< Right Control
    LEFT_ALT=214,               ///< Left Alt
    RIGHT_ALT=215,              ///< Right Alt

};


///\brief The Keyboard class provide information about keyboard input.
class Keyboard
{
public:

    Keyboard();

    /// Returns true if a key with the given *_keyCode* is pressed; otherwise returns false.
    bool IsKeyPressed(KeyCode _keyCode);

    /// Returns true if a key with the given *_keyCode* is hold down; otherwise returns false.
    bool IsKeyDown(KeyCode _keyCode);

    /// Returns true if a key with the given *_keyCode* is released; otherwise returns false.
    bool IsKeyReleased(KeyCode _keyCode);

    /// Returns the ascii code of a printable character if pressed; otherwise returns 0;
    char GetCharPressed(){ return charPressed; }

    /// Reset all key states.
    void Reset();

    //-------------------------------------------------------

    void ResetPerUpdateFlags();
    void _SetKeyState(KeyCode _keyCode, bool _keyDown);
    std::array<ButtonState,256> & GetKeys(){ return keys; }
    std::vector<std::string> & GetKeysNames(){ return keyNames; }

private:
    std::array<ButtonState,256>keys;
    std::vector<char>printableASCIIcodes;
    std::vector<std::string>keyNames;
    char charPressed = 0;

};


/// Global Keyboard object used for querying keyboard input.
extern Keyboard keyboard;



//-------------------------------------------------------------------------------------


///\brief The mouse buttons.
enum class MouseButton
{
    NONE = 0,           ///< None
    LEFT = 1,           ///< Left button.
    MIDDLE = 2,         ///< Middle button.
    RIGHT = 3,          ///< Right button.
};


///\brief The Mouse class provide information about mouse input.
class Mouse
{
public:

    /// Returns the x coordinate of the mouse cursor on screen.
    int GetX(){ return pos.x; }

    /// Returns the y coordinate of the mouse cursor on screen.
    int GetY(){ return pos.y; }

    /// Returns the position of the mouse cursor on screen.
    Vec2i GetPos(){ return pos; }

    /// Returns true if the position of the cursor changed; otherwise returns false.
    bool IsPositionChanged(){ return posChanged; }


    /// Returns the value of horizontal wheel.
    int GetWheelX(){ return wheel.x; }

    /// Returns the value of vertical wheel.
    int GetWheelY(){ return wheel.y; }

    /// Returns the values of vertical and horizonatal wheel.
    Vec2i GetWheel(){ return wheel; }

    /// Returns true if the value of horizontal wheel changed; otherwise returns false.
    bool IsWheelXChanged(){ return wheelXChanged; }

    /// Returns true if the value of horizontal wheel changed; otherwise returns false.
    bool IsWheelYChanged(){ return wheelYChanged; }


    /// Returns true if the given *_mouse button* is pressed; otherwise returns false.
    bool IsButtonPressed(MouseButton _mouseButton){ return buttons[(int)_mouseButton].pressed; }

    /// Returns true if the given *_mouse button* is hold down; otherwise returns false.
    bool IsButtonDown(MouseButton _mouseButton){ return buttons[(int)_mouseButton].down; }

    /// Returns true if the given *_mouse button* is released; otherwise returns false.
    bool IsButtonReleased(MouseButton _mouseButton){ return buttons[(int)_mouseButton].released; }

    /// \brief Set the cursor sprite to the given *_cursorSprite*.
    ///
    /// This function does not hide system cursor. That can be done with EngineApp::SetSystemMouseCursorVisible.
    void SetCursorSprite(jugimap::Sprite *_cursorSprite);

    /// \brief Clear the cursor sprite.
    ///
    /// This function will hide the cursor sprite and set its link pointer to nullptr.
    void ClearCursorSprite();

    /// Returns the cursor sprite if exists; otherwise returns nullptr.
    Sprite* GetCursorSprite(){ return cursorSprite; }

    /// Reset all buttons states.
    void Reset();

    //----

    void ResetPerUpdateFlags();
    void _SetPosition(Vec2i _pos) { posChanged = ! _pos.Equals(pos);   pos = _pos; }
    void _SetWheel(Vec2i _wheel);
    void _SetButtonState(MouseButton _mouseButton, bool _down);


private:

    Vec2i pos;
    Vec2i wheel;
    std::array<ButtonState,4> buttons;

    bool wheelYChanged = false;
    bool wheelXChanged = false;
    bool posChanged =false;

    //----
    Sprite *cursorSprite = nullptr;             // LINK

};


/// Global Mouse object used for querying mouse input.
extern Mouse mouse;



//-------------------------------------------------------------------------------------



struct Finger : public ButtonState
{
    int id = 0;
    Vec2i position;
};


///\brief The Touch class provide information about touch input.
class Touch
{
public:


    /// Returns true if the given *_finger* is pressed; otherwise returns false.
    bool IsFingerPressed(int _finger){ return fingers[_finger].pressed; }

    /// Returns true if the given *_finger* is hold down; otherwise returns false.
    bool IsFingerDown(int _finger){ return fingers[_finger].down; }

    /// Returns true if the given *_finger* is released; otherwise returns false.
    bool IsFingerReleased(int _finger){ return fingers[_finger].released; }

    /// Returns the x coordinate of the given *_finger* on screen.
    int GetFingerX(int _finger){ return fingers[_finger].position.x; }

    /// Returns the y coordinate of the given *_finger* on screen.
    int GetFingerY(int _finger){ return fingers[_finger].position.y; }

    /// Returns position of the given *_finger* on screen.
    Vec2i GetFingerPos(int _finger){ return fingers[_finger].position; }

    /// Returns a reference to the vector of finger states.
    std::array<Finger, 10> & GetFingers(){ return fingers;}

    /// Reset all finger states.
    void Reset();

    //----
    void ResetPerUpdateFlags();

    //----
    void _SetFingerState(int _finger, bool _down, Vec2i _position);

private:

    std::array<Finger, 10>fingers;

};

/// Global Touch object used for querying touch input.
extern Touch touch;


//-------------------------------------------------------------------------------------



/// The Joystick class provide information about joystick input.
class Joystick
{
public:

    /// POV X values.
    enum class POV_X
    {
        NONE,
        RIGHT,
        LEFT
    };

    /// POV Y values.
    enum class POV_Y
    {
        NONE,
        UP,
        DOWN,
    };


    static std::string DbgGetPOV_Xstring(POV_X _pov);
    static std::string DbgGetPOV_Ystring(POV_Y _pov);


    /// Returns true if a button with the given *_buttonIndex* is pressed; otherwise returns false.
    bool IsButtonPressed(int _buttonIndex){ return buttons[_buttonIndex].pressed; }

    /// Returns true if a button with the given *_buttonIndex* is hold down; otherwise returns false.
    bool IsButtonDown(int _buttonIndex){ return buttons[_buttonIndex].down; }

    /// Returns true if a button with the given *_buttonIndex* is released; otherwise returns false.
    bool IsButtonReleased(int _buttonIndex){ return buttons[_buttonIndex].released; }

    /// Returns true if the given *_povX* button is pressed; otherwise returns false.
    bool IsPOV_XPressed(POV_X _povX){ return povXButtons[(int)_povX].pressed; }

    /// Returns true if the given *_povX* button is hold down; otherwise returns false.
    bool IsPOV_XDown(POV_X _povX){ return povXButtons[(int)_povX].down; }

    /// Returns true if the given *_povX* button is released; otherwise returns false.
    bool IsPOV_XReleased(POV_X _povX){ return povXButtons[(int)_povX].released; }

    /// Returns true if the given *_povY* button is pressed; otherwise returns false.
    bool IsPOV_YPressed(POV_Y _povY){ return povYButtons[(int)_povY].pressed; }

    /// Returns true if the given *_povY* button is hold down; otherwise returns false.
    bool IsPOV_YDown(POV_Y _povY){ return povYButtons[(int)_povY].down; }

    /// Returns true if the given *_povY* button is released; otherwise returns false.
    bool IsPOV_YReleased(POV_Y _povY){ return povYButtons[(int)_povY].released; }

    /// Returns the value of x axis.
    float GetXaxis(){ return xAxis; }

    /// Returns the value of y axis.
    float GetYaxis(){ return yAxis; }

    /// Returns the value of z axis.
    float GetZaxis(){ return zAxis; }

    /// Returns true if this joystick is connected; otherwise returns false.
    bool IsConnected(){ return connected; }

    /// Reset all button states.
    void Reset();

    /// Returns a reference to the array of button states.
    std::array<ButtonState,16>& GetButtons(){ return buttons;}

    /// \brief Return the joystick name.
    ///
    /// This function does not provide the real joystick name on all engines.
    std::string GetName(){ return name; }


    void ResetPerUpdateFlags();

    //----
    void _SetPOV_X(POV_X _povX);
    void _SetPOV_Y(POV_Y _povY);
    void _SetXaxis(float _xAxis){ xAxis = _xAxis;}
    void _SetYaxis(float _yAxis){ yAxis = _yAxis;}
    void _SetZaxis(float _zAxis){ zAxis = _zAxis;}
    void _SetButtonState(int _buttonIndex, bool _down);
    void _SetConnected(bool _connected){ connected = _connected; }
    void _SetName(const std::string &_name){ name = _name; }


private:
    std::array<ButtonState,16>buttons;
    std::array<ButtonState,3>povXButtons;
    std::array<ButtonState,3>povYButtons;


    float xAxis = 0.0;
    float yAxis = 0.0;
    float zAxis = 0.0;
    bool connected = false;
    std::string name = "Generic joystick";
};


/// Global Joystick objects used for querying joysticks input.
extern std::array<Joystick, 4>joysticks;


/// @}      //end addtogroup Input
///
	

}



#endif // JUGIMAP_INPUT_H
