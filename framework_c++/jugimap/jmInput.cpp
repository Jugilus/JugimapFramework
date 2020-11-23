#include "jmCommon.h"
#include "jmCommonFunctions.h"
#include "jmSprites.h"
#include "jmSourceGraphics.h"
#include "jmGlobal.h"
#include "jmInput.h"



namespace jugimap{
	
	


void ButtonState::_Set(bool _down)
{

    if(_down==true){
        if(down==false){
            down = true;
            pressed = true;
            released = false;
        }
    }else{
        if(down==true){
            down = false;
            pressed = false;
            released = true;
        }
    }

}


//======================================================================================

Keyboard::Keyboard()
{

    printableASCIIcodes.resize(256, 0);

    printableASCIIcodes[(int)KeyCode::SPACE] = 32;
    printableASCIIcodes[(int)KeyCode::QUOTE] = 39;
    printableASCIIcodes[(int)KeyCode::COMMA] = 44;
    printableASCIIcodes[(int)KeyCode::HYPHEN] = 45;
    printableASCIIcodes[(int)KeyCode::PERIOD] = 46;
    printableASCIIcodes[(int)KeyCode::SLASH] = 47;

    printableASCIIcodes[(int)KeyCode::NUM_0] = 48;
    printableASCIIcodes[(int)KeyCode::NUM_1] = 49;
    printableASCIIcodes[(int)KeyCode::NUM_2] = 50;
    printableASCIIcodes[(int)KeyCode::NUM_3] = 51;
    printableASCIIcodes[(int)KeyCode::NUM_4] = 52;
    printableASCIIcodes[(int)KeyCode::NUM_5] = 53;
    printableASCIIcodes[(int)KeyCode::NUM_6] = 54;
    printableASCIIcodes[(int)KeyCode::NUM_7] = 55;
    printableASCIIcodes[(int)KeyCode::NUM_8] = 56;
    printableASCIIcodes[(int)KeyCode::NUM_9] = 57;

    printableASCIIcodes[(int)KeyCode::SEMICOLON] = 59;
    printableASCIIcodes[(int)KeyCode::EQUAL] = 61;

    printableASCIIcodes[(int)KeyCode::A] = 65;
    printableASCIIcodes[(int)KeyCode::B] = 66;
    printableASCIIcodes[(int)KeyCode::C] = 67;
    printableASCIIcodes[(int)KeyCode::D] = 68;
    printableASCIIcodes[(int)KeyCode::E] = 69;
    printableASCIIcodes[(int)KeyCode::F] = 70;
    printableASCIIcodes[(int)KeyCode::G] = 71;
    printableASCIIcodes[(int)KeyCode::H] = 72;
    printableASCIIcodes[(int)KeyCode::I] = 73;
    printableASCIIcodes[(int)KeyCode::J] = 74;
    printableASCIIcodes[(int)KeyCode::K] = 75;
    printableASCIIcodes[(int)KeyCode::L] = 76;
    printableASCIIcodes[(int)KeyCode::M] = 77;
    printableASCIIcodes[(int)KeyCode::N] = 78;
    printableASCIIcodes[(int)KeyCode::O] = 79;
    printableASCIIcodes[(int)KeyCode::P] = 80;
    printableASCIIcodes[(int)KeyCode::Q] = 81;
    printableASCIIcodes[(int)KeyCode::R] = 82;
    printableASCIIcodes[(int)KeyCode::S] = 83;
    printableASCIIcodes[(int)KeyCode::T] = 84;
    printableASCIIcodes[(int)KeyCode::U] = 85;
    printableASCIIcodes[(int)KeyCode::V] = 86;
    printableASCIIcodes[(int)KeyCode::W] = 87;
    printableASCIIcodes[(int)KeyCode::X] = 88;
    printableASCIIcodes[(int)KeyCode::Y] = 89;
    printableASCIIcodes[(int)KeyCode::Z] = 90;

    printableASCIIcodes[(int)KeyCode::LEFT_BRACKET] = 91;
    printableASCIIcodes[(int)KeyCode::BACKSLASH] = 92;
    printableASCIIcodes[(int)KeyCode::RIGHT_BRACKET] = 93;


    // add lower case letters shifted 100 indexes relative to upper case
    int i = 65;     // ascii code for 'A'
    int j = 97;     // ascii code for 'a'
    while(i<=90){
        printableASCIIcodes[i+100] = j;
        i++;
        j++;
    }


    //-------

    keyNames.resize(keys.size(), "");

    keyNames[(int)KeyCode::BACKSPACE] = "KEY_BACKSPACE";
    keyNames[(int)KeyCode::TAB] = "KEY_TAB";
    keyNames[(int)KeyCode::ENTER] = "KEY_ENTER";
    keyNames[(int)KeyCode::PAUSE] = "KEY_PAUSE";
    keyNames[(int)KeyCode::ESCAPE] = "KEY_ESCAPE";
    keyNames[(int)KeyCode::SPACE] = "KEY_SPACE";

    keyNames[(int)KeyCode::PAGEUP] = "KEY_PAGEUP";
    keyNames[(int)KeyCode::PAGEDOWN] = "KEY_PAGEDOWN";
    keyNames[(int)KeyCode::END] = "KEY_END";
    keyNames[(int)KeyCode::HOME] = "KEY_HOME";
    keyNames[(int)KeyCode::LEFT] = "KEY_LEFT";
    keyNames[(int)KeyCode::UP] = "KEY_UP";
    keyNames[(int)KeyCode::RIGHT] = "KEY_RIGHT";
    keyNames[(int)KeyCode::DOWN] = "KEY_DOWN";
    keyNames[(int)KeyCode::PRINT] = "KEY_PRINT";
    keyNames[(int)KeyCode::INSERT] = "KEY_INSERT";
    keyNames[(int)KeyCode::DELETEkey] = "KEY_DELETE";

    keyNames[(int)KeyCode::NUM_0] = "KEY_0";
    keyNames[(int)KeyCode::NUM_1] = "KEY_1";
    keyNames[(int)KeyCode::NUM_2] = "KEY_2";
    keyNames[(int)KeyCode::NUM_3] = "KEY_3";
    keyNames[(int)KeyCode::NUM_4] = "KEY_4";
    keyNames[(int)KeyCode::NUM_5] = "KEY_5";
    keyNames[(int)KeyCode::NUM_6] = "KEY_6";
    keyNames[(int)KeyCode::NUM_7] = "KEY_7";
    keyNames[(int)KeyCode::NUM_8] = "KEY_8";
    keyNames[(int)KeyCode::NUM_9] = "KEY_9";

    keyNames[(int)KeyCode::A] = "KEY_A";
    keyNames[(int)KeyCode::B] = "KEY_B";
    keyNames[(int)KeyCode::C] = "KEY_C";
    keyNames[(int)KeyCode::D] = "KEY_D";
    keyNames[(int)KeyCode::E] = "KEY_E";
    keyNames[(int)KeyCode::F] = "KEY_F";
    keyNames[(int)KeyCode::G] = "KEY_G";
    keyNames[(int)KeyCode::H] = "KEY_H";
    keyNames[(int)KeyCode::I] = "KEY_I";
    keyNames[(int)KeyCode::J] = "KEY_J";
    keyNames[(int)KeyCode::K] = "KEY_K";
    keyNames[(int)KeyCode::L] = "KEY_L";
    keyNames[(int)KeyCode::M] = "KEY_M";
    keyNames[(int)KeyCode::N] = "KEY_N";
    keyNames[(int)KeyCode::O] = "KEY_O";
    keyNames[(int)KeyCode::P] = "KEY_P";
    keyNames[(int)KeyCode::Q] = "KEY_Q";
    keyNames[(int)KeyCode::R] = "KEY_R";
    keyNames[(int)KeyCode::S] = "KEY_S";
    keyNames[(int)KeyCode::T] = "KEY_T";
    keyNames[(int)KeyCode::U] = "KEY_U";
    keyNames[(int)KeyCode::V] = "KEY_V";
    keyNames[(int)KeyCode::W] = "KEY_W";
    keyNames[(int)KeyCode::X] = "KEY_X";
    keyNames[(int)KeyCode::Y] = "KEY_Y";
    keyNames[(int)KeyCode::Z] = "KEY_Z";

    keyNames[(int)KeyCode::F1] = "KEY_F1";
    keyNames[(int)KeyCode::F2] = "KEY_F2";
    keyNames[(int)KeyCode::F3] = "KEY_F3";
    keyNames[(int)KeyCode::F4] = "KEY_F4";
    keyNames[(int)KeyCode::F5] = "KEY_F5";
    keyNames[(int)KeyCode::F6] = "KEY_F6";
    keyNames[(int)KeyCode::F7] = "KEY_F7";
    keyNames[(int)KeyCode::F8] = "KEY_F8";
    keyNames[(int)KeyCode::F9] = "KEY_F9";
    keyNames[(int)KeyCode::F10] = "KEY_F10";
    keyNames[(int)KeyCode::F11] = "KEY_F11";
    keyNames[(int)KeyCode::F12] = "KEY_F12";

    keyNames[(int)KeyCode::TILDE] = "KEY_TILDE";             // ~
    keyNames[(int)KeyCode::HYPHEN] = "KEY_HYPHEN";
    keyNames[(int)KeyCode::EQUAL] = "KEY_EQUAL";
    keyNames[(int)KeyCode::LEFT_BRACKET] = "KEY_LEFT_BRACKET";
    keyNames[(int)KeyCode::RIGHT_BRACKET] = "KEY_RIGHT_BRACKET";
    keyNames[(int)KeyCode::BACKSLASH] = "KEY_BACKSLASH";
    keyNames[(int)KeyCode::SEMICOLON] = "KEY_SEMICOLON";
    keyNames[(int)KeyCode::QUOTE] = "KEY_QUOTE";
    keyNames[(int)KeyCode::COMMA] = "KEY_COMMA";
    keyNames[(int)KeyCode::PERIOD] = "KEY_PERIOD";
    keyNames[(int)KeyCode::SLASH] = "KEY_SLASH";

    keyNames[(int)KeyCode::LEFT_SHIFT] = "KEY_LEFT_SHIFT";
    keyNames[(int)KeyCode::RIGHT_SHIFT] = "KEY_RIGHT_SHIFT";
    keyNames[(int)KeyCode::LEFT_CONTROL] = "KEY_LEFT_CONTROL";
    keyNames[(int)KeyCode::RIGHT_CONTROL] = "KEY_RIGHT_CONTROL";
    keyNames[(int)KeyCode::LEFT_ALT] = "KEY_LEFT_ALT";
    keyNames[(int)KeyCode::RIGHT_ALT] = "KEY_RIGHT_ALT";
    //keyNames[(int)KeyCode::KEY_LEFT_SYS] = "KEY_LEFT_SYS";
    //keyNames[(int)KeyCode::KEY_RIGHT_SYS] = "KEY_RIGHT_SYS";

}


void Keyboard::_SetKeyState(KeyCode _keyCode, bool _keyDown)
{

    assert((int)_keyCode>=0 && (int)_keyCode<keys.size());

    ButtonState &k = keys[(int)_keyCode];
    k._Set(_keyDown);

    if(k.pressed==true){

        int code = (int)_keyCode;

        if(code>=int(KeyCode::A) && code <= int(KeyCode::Z)){
            if(IsKeyDown(KeyCode::LEFT_SHIFT) || IsKeyDown(KeyCode::RIGHT_SHIFT)){
                //code += 100;    // lowere letters are shifted +100 in 'printableASCIIcodes' table
            }else{
                code += 100;
            }
        }
        if(printableASCIIcodes[code] != 0){
            charPressed = printableASCIIcodes[code];
        }
    }
}


bool Keyboard::IsKeyDown(KeyCode _keyCode)
{
    return keys[(int)_keyCode].down;
}


bool Keyboard::IsKeyPressed(KeyCode _keyCode)
{
    return keys[(int)_keyCode].pressed;
}


bool Keyboard::IsKeyReleased(KeyCode _keyCode)
{
    return keys[(int)_keyCode].released;
}


void Keyboard::Reset()
{
    for(ButtonState &k : keys){
        k.Reset();
    }
    charPressed = 0;
}


void Keyboard::ResetPerUpdateFlags()
{

    for(ButtonState &k : keys){
        k.ResetPerUpdateFlags();
    }
    charPressed = 0;
}


	
Keyboard keyboard;


//======================================================================================


void Mouse::Reset()
{

    for(ButtonState &b : buttons){
        b.Reset();
    }

    posChanged = false;
    wheelXChanged = false;
    wheelYChanged = false;
}


void Mouse::ResetPerUpdateFlags()
{

    //--- clear 'per update' parameters
    for(ButtonState &b : buttons){
        b.ResetPerUpdateFlags();
    }

    posChanged = false;
    wheelXChanged = false;
    wheelYChanged = false;

}


void Mouse::_SetButtonState(MouseButton _mouseButton, bool _down)
{

    assert((int)_mouseButton>=0 && (int)_mouseButton<buttons.size());

    ButtonState &b = buttons[(int)_mouseButton];

    b._Set(_down);

}

void Mouse::_SetWheel(Vec2i _wheel)
{

    wheelXChanged = ! (_wheel.x != wheel.x);
    wheelYChanged = ! (_wheel.y != wheel.y);
    wheel = _wheel;
}


void Mouse::SetCursorSprite(Sprite *_cursorSprite)
{
    cursorSprite = _cursorSprite;
    cursorSprite->SetPosition(Vec2f(pos.x, pos.y));
    cursorSprite->SetVisible(true);
}


void Mouse::ClearCursorSprite()
{
    if(cursorSprite==nullptr) return;
    cursorSprite->SetVisible(false);
    cursorSprite = nullptr;
}


//void Mouse::SetSystemMouseCursorVisibility(bool _visible)
//{
//    commands->SetSystemMouseCursorVisible(_visible);
//}


Mouse mouse;


//======================================================================================


void Touch::Reset()
{
    for(Finger &f : fingers){
        f.Reset();
    }
}


void Touch::ResetPerUpdateFlags()
{
    for(Finger &f : fingers){
        f.ResetPerUpdateFlags();
    }

}


void Touch::_SetFingerState(int _finger, bool _down, Vec2i _position)
{

    if(_finger<0 || _finger>=fingers.size()) return;

    Finger &f = fingers[_finger];
    f._Set(_down);
    f.position = _position;

}


Touch touch;


//======================================================================================


std::string Joystick::DbgGetPOV_Xstring(POV_X _pov)
{

    if(_pov==POV_X::RIGHT){
        return "right";

    }else if(_pov==POV_X::LEFT){
        return "left";

    }

    return "none";
}


std::string Joystick::DbgGetPOV_Ystring(POV_Y _pov)
{

    if(_pov==POV_Y::UP){
        return "up";

    }else if(_pov==POV_Y::DOWN){
        return "down";

    }

    return "none";

}



void Joystick::Reset()
{
    for(ButtonState &b : buttons){
        b.Reset();
    }
    for(ButtonState &b : povXButtons){
        b.ResetPerUpdateFlags();
    }
    for(ButtonState &b : povYButtons){
        b.ResetPerUpdateFlags();
    }
}



void Joystick::ResetPerUpdateFlags()
{
    for(ButtonState &b : buttons){
        b.ResetPerUpdateFlags();
    }
    for(ButtonState &b : povXButtons){
        b.ResetPerUpdateFlags();
    }
    for(ButtonState &b : povYButtons){
        b.ResetPerUpdateFlags();
    }
}



void Joystick::_SetPOV_X(POV_X _povX)
{

    ButtonState &b = povXButtons[(int)_povX];

    b._Set(true);

    for(int i=0; i< povXButtons.size(); i++){
        if(i != (int)_povX){
           povXButtons[i]._Set(false);
        }
    }

}


void Joystick::_SetPOV_Y(POV_Y _povY)
{

    ButtonState &b = povYButtons[(int)_povY];

    b._Set(true);

    for(int i=0; i< povYButtons.size(); i++){
        if(i != (int)_povY){
            povYButtons[i]._Set(false);
        }
    }

}



void Joystick::_SetButtonState(int _buttonIndex, bool _down)
{

    //assert(_buttonIndex>=0 && _buttonIndex<buttons.size());
    if(_buttonIndex<0 || _buttonIndex>=buttons.size()) return;


    ButtonState &b = buttons[_buttonIndex];
    b._Set(_down);

}


std::array<Joystick, 4>joysticks;




}






