#ifndef JUGIMAP_GUI_WIDGETS_A_H
#define JUGIMAP_GUI_WIDGETS_A_H


#include <memory>
#include "jmSprites.h"
#include "jmGuiCommon.h"


namespace jugimap {


/// \addtogroup Gui
/// @{


enum class GuiButtonVisualState
{
    NORMAL = 0,
    NORMAL_CURSOR_OVER = 1,
    NORMAL_CURSOR_DOWN = 2,
    CHECKED = 3,
    CHECKED_CURSOR_OVER = 4,
    CHECKED_CURSOR_DOWN = 5,
    NORMAL_HIGHLIGHTED = 6,
    CHECKED_HIGHLIGHTED = 7,
    DISABLED = 8,
};


enum class GuiButtonSpritePropertyKind
{

    NOT_DEFINED,
    SCALE,
    ROTATION,
    ALPHA,
    LABEL_COLOR,
    COLOR_OVERLAY,
    VISIBILITY,
    FRAME

};


class GuiButtonSpritePropertyController
{
public:

    GuiButtonSpritePropertyController(Sprite *_sprite, GuiButtonSpritePropertyKind _controlledProperty) : sprite(_sprite), controlledProperty(_controlledProperty){}
    virtual ~GuiButtonSpritePropertyController(){}
    virtual void SetStartingState(GuiButtonVisualState _buttonVisualState) = 0;
    virtual void Update(GuiButtonVisualState _buttonVisualState) = 0;


protected:
    Sprite *sprite = nullptr;
    GuiButtonSpritePropertyKind controlledProperty = GuiButtonSpritePropertyKind::NOT_DEFINED;
    GuiButtonVisualState oldButtonVisualState = GuiButtonVisualState::NORMAL;

};


struct GuiButtonSpriteFloatPropertyData
{

    float value = 0.0;

    // values if looping is used
    float valueMin = 0.0;
    float valueMax = 0.0;
    int loopDurationMS = 0;

    enum class LoopType
    {
        REVERT_DIRECTION,
        RESTART
    };
    LoopType loopType = LoopType::REVERT_DIRECTION;


    //int reverseSign = 1;

    static GuiButtonSpriteFloatPropertyData ParseFromString(const std::string &text);

};


class GuiButtonSpriteFloatPropertyController : public GuiButtonSpritePropertyController
{
public:


    GuiButtonSpriteFloatPropertyController(Sprite *_sprite, GuiButtonSpritePropertyKind _controlledProperty) : GuiButtonSpritePropertyController(_sprite, _controlledProperty){}
    void ParseMouseOverTransitionData(const std::string &text);

    void SetStartingState(GuiButtonVisualState _buttonVisualState) override;
    void Update(GuiButtonVisualState _buttonVisualState) override;

    float GetSpritePropertyValue();
    void ApplyPropertyValueToSprite(float _value);


    std::vector<GuiButtonSpriteFloatPropertyData>propertyDataForStates = std::vector<GuiButtonSpriteFloatPropertyData>(9);      // for every button visual state
    //std::vector<std::shared_ptr<GuiButtonSpriteFloatPropertyData>>propertyDataForStates = std::vector<std::shared_ptr<GuiButtonSpriteFloatPropertyData>>(7);      // for every button visual state


private:
    float currentValue = 0.0;
    float newValue = 0.0;
    //float oldValue = 0.0;
    float targetValue = 0.0;

    int reverseSign = 1;

    bool mouseOverTransition = false;
    float mouseOverTransitionSpeed = 0.0;

    int timeCurrentMS = 0;
    int timeStartMS = 0;
    int timeEndMS = 0;

};


class GuiButtonSpriteBoolPropertyController : public GuiButtonSpritePropertyController
{
public:

    GuiButtonSpriteBoolPropertyController(Sprite *_sprite, GuiButtonSpritePropertyKind _controlledProperty) : GuiButtonSpritePropertyController(_sprite, _controlledProperty){}

    void SetStartingState(GuiButtonVisualState _buttonVisualState) override;
    void Update(GuiButtonVisualState _buttonVisualState) override;

    bool GetSpritePropertyValue();
    void ApplyPropertyValueToSprite(bool _value);


    std::vector<bool>propertyDataForStates = std::vector<bool>(9);      // for every button visual state

private:
    bool currentValue = false;
    bool newValue = false;
    bool oldValue =false;

};


class GuiButtonSpriteIntPropertyController : public GuiButtonSpritePropertyController
{
public:

    GuiButtonSpriteIntPropertyController(Sprite *_sprite, GuiButtonSpritePropertyKind _controlledProperty) : GuiButtonSpritePropertyController(_sprite, _controlledProperty){}

    void SetStartingState(GuiButtonVisualState _buttonVisualState) override;
    void Update(GuiButtonVisualState _buttonVisualState) override;

    int GetSpritePropertyValue();
    void ApplyPropertyValueToSprite(int _value);


    std::vector<int>propertyDataForStates = std::vector<int>(9);      // for every button visual state

private:
    int currentValue = false;
    int newValue = false;
    int oldValue =false;

};


struct GuiButtonSpriteColorPropertyData
{

    ColorRGBAf value;

    // values if looping is used
    ColorRGBAf valueMin;
    ColorRGBAf valueMax;
    int loopDurationMS = 0;

    enum class LoopType
    {
        REVERT_DIRECTION,
        RESTART
    };
    LoopType loopType = LoopType::REVERT_DIRECTION;


    //int reverseSign = 1;

    static GuiButtonSpriteColorPropertyData ParseFromString(const std::string &text);

};


class GuiButtonSpriteColorPropertyController : public GuiButtonSpritePropertyController
{
public:


    GuiButtonSpriteColorPropertyController(Sprite *_sprite, GuiButtonSpritePropertyKind _controlledProperty) : GuiButtonSpritePropertyController(_sprite, _controlledProperty){}
    void ParseMouseOverTransitionTime(const std::string &text);
    void ParseColorOverlayBlend(std::string text);

    void SetStartingState(GuiButtonVisualState _buttonVisualState) override;
    void Update(GuiButtonVisualState _buttonVisualState) override;

    ColorRGBAf GetSpriteOverlayColor();
    void ApplyOverlayColorToSprite(ColorRGBAf _value);


    std::vector<GuiButtonSpriteColorPropertyData>propertyDataForStates = std::vector<GuiButtonSpriteColorPropertyData>(9);      // for every button visual state


private:
    ColorRGBAf currentValue;
    ColorRGBAf newValue;
    ColorRGBAf targetValue;

    int reverseSign = 1;

    bool mouseOverTransition = false;
    float mouseOverTransitionSpeed = 0.0;

    ColorOverlayBlend blend;


    bool reachedR = false;
    bool reachedG = false;
    bool reachedB = false;
    bool reachedA = false;


    void CheckColorReach(float &color, float targetColor, int sign, bool &reached);
    void CheckColorReach(float &color, float targetColor1, float targetColor2, int sign, bool &reached);
};


class GuiButtonExclusiveGroup;


///\brief The GuiButton class provides a widget for buttons.
class GuiButton : public GuiWidget
{
public:

    ///\brief Construct a new gui button from the given *_sprite*.
    ///
    /// The button properties are obtained from the sprite custom parameters.
    GuiButton(Sprite *_sprite);

    ~GuiButton() override;

    void SetToInitialState() override;
    void Update() override;
    void SetDisabled(bool _disabled) override;
    void SetVisible(bool _visible) override;



    ///\brief Returns the sprite of this button.
    Sprite* GetSprite(){ return rootSprite; }


    ///\brief Set the text label of this button if applicable.
    ///
    /// Text label can be applied if the button sprite is a TextSprite or a ComposedSprite which contains
    /// a child TextSprite marked as the label.
    void SetTextLabel(const std::string &_label);


    ///\brief Returns true if this button is checkable; otherwise returns false;
    ///
    /// A checkable button provides toggle behavior.
    /// \see SetCheckable, IsChecked, SetChecked
    bool IsCheckable(){ return checkable; }


    ///\brief Set checkable status of this button.
    ///
    /// A checkable button provides toggle behavior.
    /// \see IsCheckable, IsChecked, SetChecked
    void SetCheckable(bool _checkable){ checkable = _checkable; }


    ///\brief Returns true if this button is checked; otherwise returns false.
    ///
    /// The checked state is applicable if the button is checkable.
    /// \see SetChecked, SetCheckable, IsCheckable
    bool IsChecked(){ return checked;}


    ///\brief Set the checked state of this button.
    ///
    /// The checked state is applicable if the button is checkable.
    /// \see IsChecked, SetCheckable, IsCheckable
    void SetChecked(bool _checked);


private:
    Sprite *rootSprite = nullptr;                                       // LINK
    TextSprite *labelSprite = nullptr;                                  // LINK
    GuiButtonVisualState state = GuiButtonVisualState::NORMAL;
    GuiButtonVisualState initialState = GuiButtonVisualState::NORMAL;
    bool checkable = false;
    bool checked = false;

    std::vector<GuiButtonSpritePropertyController*>spriteControllers;     // OWNED

    GuiButtonExclusiveGroup* exclusiveGroup = nullptr;                       // LINK


    void BuildControllersForSprite(Sprite * _sprite);
    void _AddControllersForSprite(Sprite * _sprite);
    void _AddFloatPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind);
    void _AddIntPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind);
    void _AddBooleanPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind);
    void _AddColorPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind);

    int _ParseIntPropertyFromParameter(std::vector<Parameter>& cp, const std::string &parameterName, GuiButtonSpritePropertyKind propertyKind);

};


class GuiButtonExclusiveGroup
{
public:

    GuiButtonExclusiveGroup(const std::string &_name) : name(_name){}

    void AddButton(GuiButton* _button);
    std::string GetName(){ return name; }
    std::vector<GuiButton*>& GetButtons(){ return buttons; }


private:
    std::string name;
    std::vector<GuiButton*> buttons;        // LINKS

};



class GuiButtonExclusiveGroups
{
public:

    ~GuiButtonExclusiveGroups();
    GuiButtonExclusiveGroup* FindGroup(const std::string &_name);
    void AddGroup(GuiButtonExclusiveGroup *_g){ groups.push_back(_g); }


private:
    std::vector<GuiButtonExclusiveGroup*> groups;        // OWNED
};


extern GuiButtonExclusiveGroups guiButtonExclusiveGroups;


//================================================================================================


enum class GuiSliderState
{
    IDLE = 0,
    MOVING = 1,
    //HIGHLIGHTED = 2,
    DISABLED = 3
};



///\brief The GuiSlider class provides a widget for sliders.
class GuiSlider : public GuiWidget
{
public:

    ///\brief Construct a new gui slider from the given *_sprite*.
    ///
    /// The slider properties are obtained from the sprite custom parameters.
    GuiSlider(ComposedSprite * _sprite);

    ~GuiSlider() override;

    void SetToInitialState() override;
    void Update() override;
    void SetDisabled(bool _disabled) override;
    void SetVisible(bool _visible) override;


    ///\brief Returns true is the value has been changed with increment or decrement button.
    bool IsValueChangedViaStepButtons(){ return valueChangedViaStepButtons;}


    ///\brief Set the value of this slider.
    void SetValue(float _value);


    ///\brief Returns the value of this slider.
    float GetValue(){ return value; }


    ///\brief Returns the value of this slider rounded to integer.
    int GetIntValue(){ return (int) std::roundf(value); }


    ///\brief Set the minimum value of this slider.
    void SetValueMin(float _valueMin);


    ///\brief Returns the minimum value of this slider.
    float GetValueMin(){ return valueMin; }


    ///\brief Set the maximum value of this slider.
    void SetValueMax(float _valueMax);


    ///\brief Returns the maximum value of this slider.
    float GetValueMax(){ return valueMax; }


    ///\brief Set the displayed label precision (number of decimal digits) of this slider.
    void SetValueLabelPrecision(int _precision){ valueLabelPrecision = _precision; }


    ///\brief Set the label suffix of this slider.
    void SetValueLabelSuffix(const std::string &_suffix){ valueLabelSuffix = _suffix; }


    ///\brief Set the delta value step fot the incrementing and decrementing buttons.
    ///
    /// If the *_inPercents* is set to true, the *_spinStep* must be a percentage number from 0 to 100
    /// in which case the actual step will be obtained from the slider range.
    void SetDeltaValue(float _deltaValue, bool _inPercents){ deltaValue = _deltaValue/100.0; deltaValueInPercents = _inPercents; }


    ///\brief Set the snap division.
    ///
    /// A slider with snap division will be constrained to snapping point along the scale.
    /// The snap division must be larger then 1. To disable snapping set the snap division to 0.
    void SetSnapDivision(int _nSnapDivision);



private:

    // link pointers
    ComposedSprite *rootSprite = nullptr;                           // LINK
    StandardSprite *slidingScaleSprite = nullptr;               // LINK
    TextSprite *valueLabel = nullptr;                           // LINK

    // owned data
    GuiButton *slidingButton = nullptr;                         // OWNED
    GuiButton *incrementButton = nullptr;                       // OWNED
    GuiButton *decrementButton = nullptr;                       // OWNED


    GuiSliderState state = GuiSliderState::IDLE;

    // status flags
    bool valueChangedViaStepButtons = false;

    // data properties
    float value = 5.0;
    float valueMin = 0.0;
    float valueMax = 10.0;

    float deltaValue = 0.05;
    bool deltaValueInPercents = true;

    int nSnapDivision = -1;
    std::vector<Vec2f>snapPositions;

    Vec2f slidingPosMin;
    Vec2f slidingPosMax;
    Vec2f virtualSlidingPos;

    Vec2f startMovingOffset;

    int valueLabelPrecision = 0;
    std::string valueLabelSuffix;


    void SetSlidingButtonPositionFromValues();
    void SetValueFromSlidingButtonPosition();
    void SetValueLabelText();
    Vec2f GetSnappedPosition(Vec2f _pos);


};



//================================================================================================



class GuiBarSpriteController
{
public:

    GuiBarSpriteController(Sprite *_sprite) : sprite(_sprite){}
    virtual ~GuiBarSpriteController(){}


    virtual void Set(float value, float valueMin, float valueMax) = 0;
    //virtual float GetValue(float valueMin, float valueMax) = 0;

protected:
    Sprite *sprite = nullptr;       // LINK

};


class GuiBarSpriteScaleController : public GuiBarSpriteController
{
public:

    struct PositionColor
    {
        float pos = 0.0;        // relative position in bar 0.0 - 1.0
        ColorRGBA color;
    };


    GuiBarSpriteScaleController(Sprite * _sprite) : GuiBarSpriteController(_sprite){}
    void ParseFromString(const std::string &_text);

    void Set(float value, float valueMin, float valueMax) override;
    //float GetValue(float valueMin, float valueMax) override;


private:
    bool xScale = false;
    bool yScale = false;
    std::vector<PositionColor>colors;
};


class GuiBarSpriteFrameController : public GuiBarSpriteController
{
public:

    struct PositionColor
    {
        float pos = 0.0;        // relative position in bar 0.0 - 1.0
        ColorRGBA color;
    };


    GuiBarSpriteFrameController(Sprite * _sprite) : GuiBarSpriteController(_sprite){}
    void ParseFromString(const std::string &_text);

    void Set(float value, float valueMin, float valueMax) override;


private:
    int frameIndexMin = 0;
    int frameIndexMax = 0;
};



///\brief The GuiBar class provides a widget for bars.
///
/// A bar is a visual representation of values in range.
class GuiBar : public GuiWidget
{

public:

    ///\brief Construct a new gui bar from the given *_sprite*.
    ///
    /// The bar properties are obtained from the sprite custom parameters.
    GuiBar(Sprite* _sprite);

    ~GuiBar();


    void SetToInitialState() override;
    void Update() override;


    ///\brief Set the value of this bar.
    void SetValue(float _value);


    ///\brief Returns the value of this bar.
    float GetValue(){ return value; }


    ///\brief Returns the value of this bar rounded to integer.
    int GetIntValue(){ return (int) std::roundf(value); }


    ///\brief Set the minimum value of this bar.
    void SetValueMin(float _valueMin);


    ///\brief Returns the minimum value of this bar.
    float GetValueMin(){ return valueMin; }


    ///\brief Set the maximum value of this bar.
    void SetValueMax(float _valueMax);


    ///\brief Returns the maximum value of this bar.
    float GetValueMax(){ return valueMax; }


private:
    Sprite *sprite = nullptr;                   // LINK
    TextSprite *valueLabel = nullptr;                           // LINK

    float value = 5.0;
    float valueMin = 0.0;
    float valueMax = 10.0;

    GuiBarSpriteController* barSpriteController = nullptr;      //OWNED

    int valueLabelPrecision = 0;
    std::string valueLabelSuffix;


    void BuildControllersForSprite(Sprite * _sprite);
    void _AddControllersForSprite(Sprite * _sprite);

    void SetValueLabelText();

};


/// @}

}



#endif
