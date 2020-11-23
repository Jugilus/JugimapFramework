#include "../utf8cpp/utf8.h"
#include "jmSprites.h"
#include "jmGuiWidgetsA.h"
#include "jmGuiCommon.h"




namespace jugimap {





GuiCursorDeviceInput guiCursorDeviceInput;

//InteractedGUIWidget interactedGuiWidget;


GuiKeyboardAndJoystickInput guiKeyboardAndJoystickInput;

//================================================================================


GuiWidget::~GuiWidget()
{

    if(obj) delete obj;
    if(callbackObj) delete callbackObj;

}


void GuiWidget::SetDisabled(bool _disabled)
{
    disabled = _disabled;
    if(disabled){
        pressed = false;
        cursorOver = false;
        cursorDown = false;
        valueChanged = false;
    }

}


void GuiWidget::SetVisible(bool _visible)
{

    visible = _visible;
    if(visible==false){
        pressed = false;
        cursorOver = false;
        cursorDown = false;
        valueChanged = false;
    }
}


void GuiWidget::AssignCustomObject(CustomObject* _customObject)
{
    if(obj && obj!=_customObject){
        delete obj;
    }

    obj = _customObject;
}


void GuiWidget::AssignCallback(GuiWidgetCallback *_callback)
{
    if(callbackObj && callbackObj!=_callback){
        delete callbackObj;
    }

    callbackObj = _callback;
}


void GuiWidget::_SetHighlighted(bool _highlighted)
{

    //highlighted = _highlighted;

    if(highlightMarkerSprite && highlightMarkerSprite->IsVisible()!=_highlighted){
        highlightMarkerSprite->SetVisible(_highlighted);
    }

}


void GuiWidget::_SetInteractedWidget(GuiWidget *_widget)
{

    interactedWidget = _widget;


    if(guiKeyboardAndJoystickInput.GetHighlightedWidget() && guiKeyboardAndJoystickInput.GetHighlightedWidget()!=interactedWidget){
        guiKeyboardAndJoystickInput._ClearHighlightedWidget();
    }
    if(guiKeyboardAndJoystickInput.GetHighlightedWidget()==nullptr){
        guiKeyboardAndJoystickInput._ProposeWidgetIndex(interactedWidget);
    }

}


//--------------------------------------------------------------------------------

GuiWidget *GuiWidget::interactedWidget = nullptr;



//================================================================================



void GuiKeyboardAndJoystickInput::Update()
{


    highlightedPressed = false;
    highlightedDown = false;
    highlightedChangeValue = 0;


    if(disabled==false && widgets.empty()==false){

        for(KeyCode k : pressKeys){
            if(keyboard.IsKeyPressed(k)){
                highlightedPressed = true;
            }
            if(keyboard.IsKeyDown(k)){
                highlightedDown = true;
            }
        }

        for(int index : joyPressButtons){
            if(joysticks[0].IsButtonPressed(index)){
                highlightedPressed = true;
            }
            if(joysticks[0].IsButtonDown(index)){
                highlightedDown = true;
            }
        }


        //----
        int navigate = 0 ;

        for(KeyCode k : navigateForwardKeys){
            if(keyboard.IsKeyPressed(k)){
                navigate = 1;
            }
        }
        for(KeyCode k : navigateBackwardKeys){
            if(keyboard.IsKeyPressed(k)){
                navigate = -1;
            }
        }
        if(joyNavigateForwardPOV_X!=Joystick::POV_X::NONE && joysticks[0].IsPOV_XPressed(joyNavigateForwardPOV_X)){
            navigate = 1;
        }else if(joyNavigateForwardPOV_Y!=Joystick::POV_Y::NONE && joysticks[0].IsPOV_YPressed(joyNavigateForwardPOV_Y)){
            navigate = 1;
        }
        if(joyNavigateBackwardPOV_X!=Joystick::POV_X::NONE && joysticks[0].IsPOV_XPressed(joyNavigateBackwardPOV_X)){
            navigate = -1;
        }else if(joyNavigateBackwardPOV_Y!=Joystick::POV_Y::NONE && joysticks[0].IsPOV_YPressed(joyNavigateBackwardPOV_Y)){
            navigate = -1;
        }

        if(navigate!=0){
            _SetHighlightedNavigate(navigate);
        }

        //----
        for(KeyCode k : incrementValueKeys){
            if(keyboard.IsKeyPressed(k)){
                highlightedChangeValue = 1;
            }
        }
        for(KeyCode k : decrementValueKeys){
            if(keyboard.IsKeyPressed(k)){
                highlightedChangeValue = -1;
            }
        }
        if(joyIncrementValue_POV_X!=Joystick::POV_X::NONE && joysticks[0].IsPOV_XPressed(joyIncrementValue_POV_X)){
            highlightedChangeValue = 1;
        }else if(joyIncrementValue_POV_Y!=Joystick::POV_Y::NONE && joysticks[0].IsPOV_YPressed(joyIncrementValue_POV_Y)){
            highlightedChangeValue = 1;
        }
        if(joyDecrementValue_POV_X!=Joystick::POV_X::NONE && joysticks[0].IsPOV_XPressed(joyDecrementValue_POV_X)){
            highlightedChangeValue = -1;
        }else if(joyDecrementValue_POV_Y!=Joystick::POV_Y::NONE && joysticks[0].IsPOV_YPressed(joyDecrementValue_POV_Y)){
            highlightedChangeValue = -1;
        }

        if(highlightedWidget){
            //interactedGuiWidget._SetWidget(highlightedWidget);
            GuiWidget::_SetInteractedWidget(highlightedWidget);
        }
    }

}


void GuiKeyboardAndJoystickInput::SetWidgets(std::vector<GuiWidget*>_widgets, GuiWidget *_highlightedWidget)
{
    widgets = _widgets;
    indexHighlighted = -1;
    highlightedWidget = nullptr;

    if(_highlightedWidget){
        _ProposeWidgetIndex(_highlightedWidget);
    }

}


void GuiKeyboardAndJoystickInput::_ClearHighlightedWidget()
{

    if(highlightedWidget){
        highlightedWidget->_SetHighlighted(false);
        highlightedWidget = nullptr;
        indexHighlighted = -1;
    }
}


void GuiKeyboardAndJoystickInput::_ProposeWidgetIndex(GuiWidget *_widget)
{

    for(int i=0; i<(int)widgets.size(); i++){
        if(widgets[i]==_widget){
            indexHighlighted = i;
        }
    }

}


void GuiKeyboardAndJoystickInput::_SetHighlightedPressed(bool _pressed)
{
    if(highlightedWidget==nullptr) return;

    highlightedPressed = _pressed;
}


void GuiKeyboardAndJoystickInput::_SetHighlightedDown(bool _down)
{
    if(highlightedWidget==nullptr) return;

    highlightedDown = _down;
}


void GuiKeyboardAndJoystickInput::_SetHighlightedChangeValue(int _highlightedChangeValue)
{
    if(highlightedWidget==nullptr) return;

    highlightedChangeValue = _highlightedChangeValue;
}


void GuiKeyboardAndJoystickInput::_SetHighlightedNavigate(int _highlightedNavigate)
{

    if(widgets.empty()) return;

    if(_highlightedNavigate==-1){
        indexHighlighted --;
        if(indexHighlighted<0 || indexHighlighted>= int(widgets.size())){
            indexHighlighted = widgets.size()-1;
        }

    }else if(_highlightedNavigate==1){
        indexHighlighted ++;
        if(indexHighlighted<0 || indexHighlighted>= int(widgets.size())){
            indexHighlighted = 0;
        }
    }

    if(indexHighlighted==-1){
        if(highlightedWidget){
            highlightedWidget->_SetHighlighted(false);
            highlightedWidget = nullptr;
        }
    }else{

        GuiWidget * w = widgets.at(indexHighlighted);
        if(w != highlightedWidget){
            if(highlightedWidget){
                highlightedWidget->_SetHighlighted(false);
            }
            highlightedWidget = w;
            highlightedWidget->_SetHighlighted(true);
        }
        GuiWidget::_SetInteractedWidget(highlightedWidget);
    }

}


}



