#ifndef JUGIMAP_GUI_COMMON_H
#define JUGIMAP_GUI_COMMON_H

#include "jmGlobal.h"
#include "jmInput.h"
#include "jmCommonFunctions.h"


namespace jugimap {


class Sprite;
class Map;

class GuiWidget;



/// \addtogroup Gui
/// @{



///\brief The GuiWidgetCallback class is the base callback class for widgets.
struct GuiWidgetCallback
{

    virtual ~GuiWidgetCallback(){}


    ///\brief A function call when the cursor is pressed.
    /// ///
    ///  This callback is available for GuiButton.
    virtual void OnPressed(GuiWidget*){}


    ///\brief A function call when the cursor is over the widget.
    /// ///
    ///  This callback is available for GuiButton.
    virtual void OnCursorOver(GuiWidget*){}


    ///\brief A function call when the cursor is hold down.
    ///
    ///  This callback is available for GuiButton.
    virtual void OnCursorDown(GuiWidget*){}


    ///\brief A function call when the value of a widget is changed.
    ///
    /// This callback is available for GuiSlider and GuiTextInput.
    virtual void OnValueChanged(GuiWidget*){}


    ///\brief A function call when a slider's value is changed via increment or decrement button.
    ///
    /// This callback is available for GuiSlider.
    virtual void OnValueChangedViaButton(GuiWidget*){}

};



///\brief The GuiWidget class is the base widget class.
class GuiWidget
{

public:

    GuiWidget(){}
    virtual ~GuiWidget();
    GuiWidget(const GuiWidget &src) = delete;
    GuiWidget(const GuiWidget &&src) = delete;
    GuiWidget& operator=(const GuiWidget &src) = delete;
    GuiWidget& operator=(const GuiWidget &&src) = delete;


    //-----------------------------------------------------------------

    ///\brief Returns true if the cursor is hold down over this widget; otherwise returns false.
    ///
    /// This function is applicable to GuiButton widgets.
    bool IsCursorDown(){ return cursorDown; }


    ///\brief Returns true if the cursor is over this widget; otherwise returns false.
    ///
    /// This function is applicable to GuiButton widgets.
    bool IsCursorOver(){ return cursorOver; }


    ///\brief Returns true if the cursor is pressed while over this widget; otherwise returns false.
    ///
    /// This function is applicable to GuiButton widgets.
    bool IsPressed(){ return pressed; }


    ///\brief Returns true if this widget's value is changed; otherwise returns false.
    ///
    /// This function is applicable to GuiSlider and GuiTextInput widgets.
    bool IsValueChanged(){ return valueChanged; }


    ///\brief Set the disabled state of this widget.
    ///
    /// \see IsDisabled
    virtual void SetDisabled(bool _disabled);


    ///\brief Returns true if this widget is disabled; otherwise returns false.
    ///
    ///\see SetDisabled
    bool IsDisabled(){ return disabled; }


    /// \brief Set the visibility of this widget.
    ///
    /// This function is not available for GuiTextField.
    /// \see IsVisible
    virtual void SetVisible(bool _visible);


    /// \brief Returns true if this widget is visible; otherwise returns false.
    ///
    /// \see SetVisible
    bool IsVisible(){ return visible; }


    ///\brief Set the name of this widget.
    ///
    /// \see GetName
    void SetName(const std::string &_name){ name = _name;}


    ///\brief Returns the name of this widget.
    ///
    /// \see SetName
    std::string GetName(){ return name; }


    ///\brief Set the tag of this widget.
    ///
    /// \see GetTag
    void SetTag(int _tag){ tag = _tag;}


    ///\brief Returns the tag of this widget.
    ///
    /// \see SetTag
    int GetTag(){ return tag; }


    ///\brief Returns the kind of this widget.
    GuiWidgetKind GetKind(){ return kind; }


    ///\brief Returns the design kind of this widget.
    std::string GetDesignKind(){ return designKind; }


    ///\brief Assign the given *_customObject* to this widget.
    ///
    /// **Important!** The widget take ownership of the custom object!
    /// \see GetCustomObject
    void AssignCustomObject(CustomObject* _customObject);


    /// \brief Returns the custom object of this widget or nullptr if none.
    ///
    /// \see AssignCustomObject
    CustomObject* GetCustomObject(){ return obj; }


    ///\brief Assign the given *_callback* to this widget.
    ///
    /// **Important!** The widget take ownership of the callback!
    /// \see GetCallback
    void AssignCallback(GuiWidgetCallback *_callback);


    /// \brief Returns the callback of this widget or nullptr if none.
    ///
    /// \see AssignCallback
    GuiWidgetCallback * GetCallback(){ return callbackObj; }


    virtual void _SetHighlighted(bool _highlighted);



    // QUERY INTERACTED WIDGET
    //-----------------------------------------------------------------


    /// \brief Returns the interacted widget.
    ///
    /// If there is no widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted(){ return interactedWidget; }


    /// \brief Returns the interacted widget with the given *_name*.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted(const std::string &_name){ return (interactedWidget && interactedWidget->GetName()==_name)? interactedWidget : nullptr;}


    /// \brief Returns the interacted widget with the given *_tag*.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted(int _tag){ return (interactedWidget && interactedWidget->GetTag()==_tag)? interactedWidget : nullptr;}


    /// \brief Returns the interacted widget which has assigned the given *_customObject*.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted(CustomObject* _customObject){ return (interactedWidget && interactedWidget->GetCustomObject()==_customObject)? interactedWidget : nullptr;}


    //-----------------------------------------------------------------


    /// \brief Returns the interacted widget which is pressed.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_Pressed(){ return (interactedWidget && interactedWidget->IsPressed())? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_name* which is pressed.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_Pressed(const std::string &_name){ return (interactedWidget && interactedWidget->IsPressed() && interactedWidget->GetName()==_name)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_tag* which is pressed.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_Pressed(int _tag){ return (interactedWidget && interactedWidget->IsPressed() && interactedWidget->GetTag()==_tag)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with assigned *_customObject* which is pressed.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_Pressed(CustomObject* _customObject){ return (interactedWidget && interactedWidget->IsPressed() && interactedWidget->GetCustomObject()==_customObject)? interactedWidget : nullptr; }


    //-----------------------------------------------------------------


    /// \brief Returns the interacted widget which has changed value.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_ValueChanged(){ return (interactedWidget && interactedWidget->IsValueChanged())? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_name* which has changed value.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_ValueChanged(const std::string &_name){ return (interactedWidget && interactedWidget->IsValueChanged() && interactedWidget->GetName()==_name)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_tag* which has changed value.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_ValueChanged(int _tag){ return (interactedWidget && interactedWidget->IsValueChanged() && interactedWidget->GetTag()==_tag)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with assigned *_customObject* which has changed value.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_ValueChanged(CustomObject* _customObject){ return (interactedWidget && interactedWidget->IsValueChanged() && interactedWidget->GetCustomObject()==_customObject)? interactedWidget : nullptr; }


    //-----------------------------------------------------------------


    /// \brief Returns the interacted widget which has the cursor over.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_CursorOver(){ return (interactedWidget && interactedWidget->IsCursorOver())? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_name* which has the cursor over.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_CursorOver(const std::string &_name){ return (interactedWidget && interactedWidget->IsCursorOver() && interactedWidget->GetName()==_name)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_tag* which has the cursor over.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_CursorOver(int _tag){ return (interactedWidget && interactedWidget->IsCursorOver() && interactedWidget->GetTag()==_tag)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with assigned *_customObject* which has the cursor over.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_CursorOver(CustomObject* _customObject){ return (interactedWidget && interactedWidget->IsCursorOver() && interactedWidget->GetCustomObject()==_customObject)? interactedWidget : nullptr; }


    //-----------------------------------------------------------------


    /// \brief Returns the interacted widget which has the cursor hold down.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_cursorDown(){ return (interactedWidget && interactedWidget->IsCursorDown())? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_name* which has the cursor hold down.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_CursorDown(const std::string &_name){ return (interactedWidget && interactedWidget->IsCursorDown() && interactedWidget->GetName()==_name)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with the given *_tag* which has the cursor hold down.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_CursorDown(int _tag){ return (interactedWidget && interactedWidget->IsCursorDown() && interactedWidget->GetTag()==_tag)? interactedWidget : nullptr; }


    /// \brief Returns the interacted widget with assigned *_customObject* which has the cursor hold down.
    ///
    /// If there is no such widget interacting the function returns nullptr.
    static GuiWidget* GetInteracted_CursorDown(CustomObject* _customObject){ return (interactedWidget && interactedWidget->IsCursorDown() && interactedWidget->GetCustomObject()==_customObject)? interactedWidget : nullptr; }



    //-----------------------------------------------------------------

    virtual void Update() = 0;
    virtual void SetToInitialState(){}

    static void _SetInteractedWidget(GuiWidget *_widget);
    static void ResetInteractedPerUpdate(){ interactedWidget = nullptr; }


protected:

    GuiWidgetKind kind = GuiWidgetKind::NOT_DEFINED;
    std::string name;
    int tag = 0;
    std::string designKind;
    CustomObject* obj = nullptr;                            // owned
    GuiWidgetCallback *callbackObj = nullptr;               // owned

    // state flags
    bool disabled = false;
    bool visible = true;

    // interaction flags
    bool pressed = false;
    bool cursorOver = false;
    bool cursorDown = false;
    bool valueChanged = false;

    Sprite *highlightMarkerSprite = nullptr;                // LINK


private:
    static GuiWidget *interactedWidget;                     // LINK

};


//================================================================================================


///\brief The GuiCursorDeviceInput class provides the mouse and touch input information for widget interactions.
///
/// This object is used internally by widgets.
class GuiCursorDeviceInput
{

public:

    Vec2f GetCursorScreenPosition(){ return cursorScreenPosition; }
    Vec2f GetCursorInMapPosition(){ return cursorPosition; }
    bool IsPressed(){ return cursorPressed; }
    bool IsCursorDown(){ return cursorDown; }


    void _SetCursorScreenPosition(Vec2f _pos){ cursorScreenPosition = _pos; }
    void _SetCursorInMapPosition(Vec2f _pos){ cursorPosition = _pos; }
    void _SetCursorPressed(bool _pressed){ cursorPressed = _pressed; }
    void _SetCursorDown(bool _down){ cursorDown = _down; }


private:
    Vec2f cursorScreenPosition;
    Vec2f cursorPosition;
    bool cursorPressed = false;
    bool cursorDown = false;

};


///\brief Global GuiCursorDeviceInput object used by widgets.
extern GuiCursorDeviceInput guiCursorDeviceInput;



///\brief The GuiKeyboardAndJoystickInput class provides the keyboard and joystick input information for widgets.
///
/// This object is used internally by widgets.
class GuiKeyboardAndJoystickInput
{

public:


    void Update();

    GuiWidget* GetHighlightedWidget(){ return highlightedWidget; }
    bool GetHighlighedPressed(){ return highlightedPressed; }
    bool GetHighlighedDown(){ return highlightedDown; }
    int GetHighlightedChangeValue(){ return highlightedChangeValue; }


    ///\brief Set the disabled state.
    ///
    /// \see IsDisabled
    void SetDisabled(bool _disabled){ disabled = _disabled; }


    ///\brief Returns true if keyboard and joystick input is disabled; otherwise return false.
    ///
    /// \see SetDisabled
    bool IsDisabled(){ return disabled; }


    ///\brief Set widgets which can be interacted with keyboard or joystick.
    ///
    /// The order of widgets is also the order of navigaton between them.
    void SetWidgets(std::vector<GuiWidget*>_widgets, GuiWidget* _highlightedWidget=nullptr);


    ///\brief Returns a reference to the vector of widgets.
    std::vector<GuiWidget*>& GetWidgets(){ return widgets; }


    ///\brief Set the keyborad keys used for forward navigation between widgets.
    void SetNavigateForwardKeys(const std::vector<KeyCode> &_keys){ navigateForwardKeys = _keys; }


    ///\brief Set the keyborad keys used for backward navigation between widgets.
    void SetNavigateBackwardKeys(const std::vector<KeyCode> &_keys){ navigateBackwardKeys = _keys; }


    ///\brief Set the keyborad keys used for pressing the highlighted widget.
    void SetPressKeys(const std::vector<KeyCode> &_keys){ pressKeys = _keys; }


    ///\brief Set the keyborad keys used for incrementing the value of the highlighted widget.
    ///
    /// This function is applicable to GuiSlider widgets.
    void SetIncrementValueKeys(const std::vector<KeyCode> &_keys){ incrementValueKeys = _keys; }


    ///\brief Set the keyborad keys used for incrementing the value of the highlighted widget.
    ///
    /// This function is applicable to GuiSlider widgets.
    void SetDecrementValueKeys(const std::vector<KeyCode> &_keys){ decrementValueKeys = _keys; }


    ///\brief Set the joystick POV X button used for forward navigation between widgets.
    void SetNavigateForwardJoystickPOV_X(Joystick::POV_X _pov){ joyNavigateForwardPOV_X = _pov; }


    ///\brief Set the joystick POV Y button used for forward navigation between widgets.
    void SetNavigateForwardJoystickPOV_Y(Joystick::POV_Y _pov){ joyNavigateForwardPOV_Y = _pov; }


    ///\brief Set the joystick POV X button used for backward navigation between widgets.
    void SetNavigateBackwardJoystickPOV_X(Joystick::POV_X _pov){ joyNavigateBackwardPOV_X = _pov; }


    ///\brief Set the joystick POV Y button used for backward navigation between widgets.
    void SetNavigateBackwardJoystickPOV_Y(Joystick::POV_Y _pov){ joyNavigateBackwardPOV_Y = _pov; }


    ///\brief Set the joystick buttons used for pressing the highlighted widget.
     void SetJoystickPressButtons(const std::vector<int> &_joyPressButtons){ joyPressButtons = _joyPressButtons; }


    ///\brief Set the joystick POV X button for incrementing the value of the highlighted widget.
    ///
    /// This function is applicable to GuiSlider widgets.
    void SetIncrementValueJoystickPOV_X(Joystick::POV_X _pov){ joyIncrementValue_POV_X = _pov; }


    ///\brief Set the joystick POV Y button for incrementing the value of the highlighted widget.
    ///
    /// This function is applicable to GuiSlider widgets.
    void SetIncrementValueJoystickPOV_Y(Joystick::POV_Y _pov){ joyIncrementValue_POV_Y = _pov; }


    ///\brief Set the joystick POV X button for decrementing the value of the highlighted widget.
    ///
    /// This function is applicable to GuiSlider widgets.
    void SetDecrementValueJoystickPOV_X(Joystick::POV_X _pov){ joyDecrementValue_POV_X = _pov; }


    ///\brief Set the joystick POV Y button for decrementing the value of the highlighted widget.
    ///
    /// This function is applicable to GuiSlider widgets.
    void SetDecrementValueJoystickPOV_Y(Joystick::POV_Y _pov){ joyDecrementValue_POV_Y = _pov; }



    void _ClearHighlightedWidget();
    void _ProposeWidgetIndex(GuiWidget * _widget);


private:
    bool disabled = false;
    GuiWidget *highlightedWidget = nullptr;             // LINK
    bool highlightedPressed = false;
    bool highlightedDown = false;
    int highlightedChangeValue = 0;

    std::vector<GuiWidget*>widgets;                     // LINKs
    int indexHighlighted = -1;

    // keyboard navigation keys
    std::vector<KeyCode>navigateForwardKeys;
    std::vector<KeyCode>navigateBackwardKeys;
    std::vector<KeyCode>incrementValueKeys;
    std::vector<KeyCode>decrementValueKeys;
    std::vector<KeyCode>pressKeys;

    // joystick navigation keys
    Joystick::POV_X joyNavigateForwardPOV_X = Joystick::POV_X::NONE;
    Joystick::POV_X joyNavigateBackwardPOV_X = Joystick::POV_X::NONE;
    Joystick::POV_Y joyNavigateForwardPOV_Y = Joystick::POV_Y::NONE;
    Joystick::POV_Y joyNavigateBackwardPOV_Y = Joystick::POV_Y::NONE;
    Joystick::POV_X joyIncrementValue_POV_X = Joystick::POV_X::NONE;
    Joystick::POV_X joyDecrementValue_POV_X = Joystick::POV_X::NONE;
    Joystick::POV_Y joyIncrementValue_POV_Y = Joystick::POV_Y::NONE;
    Joystick::POV_Y joyDecrementValue_POV_Y = Joystick::POV_Y::NONE;

    std::vector<int>joyPressButtons;



    void _SetHighlightedPressed(bool _pressed);
    void _SetHighlightedDown(bool _down);
    void _SetHighlightedNavigate(int _highlightedNavigate);
    void _SetHighlightedChangeValue(int _highlightedChangeValue);

};


///\brief Global GuiKeyboardAndJoystickInput object used by widgets.
extern GuiKeyboardAndJoystickInput guiKeyboardAndJoystickInput;


/// @}      //end addtogroup Gui

}



#endif
