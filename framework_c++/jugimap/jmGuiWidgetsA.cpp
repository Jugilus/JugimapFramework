#include <sstream>
#include "../utf8cpp/utf8.h"
#include "jmSourceGraphics.h"
#include "jmVectorShapes.h"
#include "jmSprites.h"
#include "jmInput.h"
#include "jmLayers.h"
#include "jmUtilities.h"
#include "jmGuiCommon.h"
#include "jmGuiWidgetsA.h"




namespace jugimap {






//====================================================================================================



bool IsGuiButtonStateMouseOver(GuiButtonVisualState _buttonVisualState)
{
    if(_buttonVisualState==GuiButtonVisualState::NORMAL || _buttonVisualState==GuiButtonVisualState::CHECKED || _buttonVisualState==GuiButtonVisualState::DISABLED){
        return false;
    }
    return true;
}


GuiButtonSpriteFloatPropertyData GuiButtonSpriteFloatPropertyData::ParseFromString(const std::string &text)
{

    GuiButtonSpriteFloatPropertyData fv;

    std::vector<std::string>subparameters = SplitString(text, ",");

    if(subparameters.size()==1){
        fv.value = std::stof(subparameters.front());

    }else if(subparameters.size()>=4){
        fv.value = std::stof(subparameters[0]);
        fv.valueMin = std::stof(subparameters[1]);
        fv.valueMax = std::stof(subparameters[2]);
        fv.loopDurationMS = 1000 * std::stof(subparameters[3]);
        fv.loopType = LoopType::REVERT_DIRECTION;
        if(subparameters.size()==5){
            std::string loopType = subparameters[4];
            RemoveStringWhiteSpacesOnStartAndEnd(loopType);
            if(loopType=="restart"){
                fv.loopType = LoopType::RESTART;
            }
        }

    }
    return fv;
}


void GuiButtonSpriteFloatPropertyController::ParseMouseOverTransitionData(const std::string &text)
{

    std::vector<std::string>subparameters = SplitString(text, ",");

    if(subparameters.size()==2){
        float delta = std::stof(subparameters[0]);
        int durationMS = 1000 * std::stof(subparameters[1]);
        mouseOverTransitionSpeed = delta / durationMS;
    }


}


void GuiButtonSpriteFloatPropertyController::SetStartingState(GuiButtonVisualState _buttonVisualState)
{

    currentValue = GetSpritePropertyValue();

    newValue = propertyDataForStates[(int)_buttonVisualState].value;
    //newValue = propertyDataForStates[(int)_buttonVisualState]->value;
    targetValue = newValue;


    //if(controlledProperty == GuiButtonSpritePropertyKind::POSITION_X || controlledProperty == GuiButtonSpritePropertyKind::POSITION_Y){
        // Position 'newValue' is delta position while 'currentValue' is absolute sprite position !
    //    if(AreEqual(currentValue+newValue, currentValue)==false){
    //        ApplyPropertyValueToSprite(currentValue+newValue);
    //    }

    //}else{
        if(AreEqual(newValue, currentValue)==false){
            ApplyPropertyValueToSprite(newValue);
        }
    //}




}


void GuiButtonSpriteFloatPropertyController::Update(GuiButtonVisualState _buttonVisualState)
{

    float value = propertyDataForStates[(int)_buttonVisualState].value;
    float valueMin = propertyDataForStates[(int)_buttonVisualState].valueMin;
    float valueMax = propertyDataForStates[(int)_buttonVisualState].valueMax;
    int loopDurationMS = propertyDataForStates[(int)_buttonVisualState].loopDurationMS;
    GuiButtonSpriteFloatPropertyData::LoopType loopType = propertyDataForStates[(int)_buttonVisualState].loopType;

    //currentValue = GetSpritePropertyValue();


    if(_buttonVisualState != oldButtonVisualState){

        targetValue = value;

        if(IsGuiButtonStateMouseOver(_buttonVisualState)!=IsGuiButtonStateMouseOver(oldButtonVisualState)){
            if(mouseOverTransitionSpeed>0.0001){
                mouseOverTransition = true;
            }
        }
    }

    float deltaValue = 0.0;

    //----
    if(mouseOverTransition){

        deltaValue = time.GetLogicTimeMS() * mouseOverTransitionSpeed;

        if(currentValue < targetValue){
            newValue = currentValue + deltaValue;
            if(newValue >= targetValue){
                newValue = targetValue;
                mouseOverTransition = false;
            }

        }else{
            newValue = currentValue - deltaValue;
            if(newValue <= targetValue){
                newValue = targetValue;
                mouseOverTransition = false;
            }
        }

    }else{

        if(loopDurationMS>0 && std::abs(valueMax - valueMin) > 0.0001){

            deltaValue = time.GetLogicTimeMS() * (valueMax - valueMin) / loopDurationMS;

            if(loopType == GuiButtonSpriteFloatPropertyData::LoopType::REVERT_DIRECTION){

                newValue = currentValue + deltaValue*reverseSign;

                if(newValue <= valueMin){
                    newValue = valueMin;
                    reverseSign = -reverseSign;

                }else if(newValue >= valueMax){
                    newValue = valueMax;
                    reverseSign = -reverseSign;
                }


            }else if(loopType == GuiButtonSpriteFloatPropertyData::LoopType::RESTART){

                newValue = currentValue + deltaValue;

                if(newValue>valueMax){
                    newValue = valueMin;
                }
            }

        }else{

            newValue = targetValue;

        }

    }

    if(AreEqual(newValue, currentValue)==false){
        ApplyPropertyValueToSprite(newValue);
        currentValue = newValue;
    }


    oldButtonVisualState = _buttonVisualState;

}


float GuiButtonSpriteFloatPropertyController::GetSpritePropertyValue()
{

    if(controlledProperty == GuiButtonSpritePropertyKind::SCALE){
        return sprite->GetScale().x;

    }else if(controlledProperty == GuiButtonSpritePropertyKind::ROTATION){
        return sprite->GetRotation();

    }else if(controlledProperty == GuiButtonSpritePropertyKind::ALPHA){
        return sprite->GetAlpha();

    }

    return 0.0;
}


void GuiButtonSpriteFloatPropertyController::ApplyPropertyValueToSprite(float _value)
{

    if(controlledProperty == GuiButtonSpritePropertyKind::SCALE){
        sprite->SetScale(Vec2f(_value, _value));

    }else if(controlledProperty == GuiButtonSpritePropertyKind::ROTATION){
        if(settings.IsYCoordinateUp()==false){
            _value = -_value;
        }
        sprite->SetRotation(_value);


    }else if(controlledProperty == GuiButtonSpritePropertyKind::ALPHA){
        sprite->SetAlpha(_value);
    }

}


//====================================================================================================


void GuiButtonSpriteBoolPropertyController::SetStartingState(GuiButtonVisualState _buttonVisualState)
{

    currentValue = GetSpritePropertyValue();
    newValue = propertyDataForStates[(int)_buttonVisualState];

    if(newValue != currentValue){
        ApplyPropertyValueToSprite(newValue);
    }

}


void GuiButtonSpriteBoolPropertyController::Update(GuiButtonVisualState _buttonVisualState)
{

    oldValue = currentValue;
    currentValue = GetSpritePropertyValue();

    newValue = propertyDataForStates[(int)_buttonVisualState];

    if(newValue != currentValue){
        ApplyPropertyValueToSprite(newValue);
    }

    oldButtonVisualState = _buttonVisualState;

}


bool GuiButtonSpriteBoolPropertyController::GetSpritePropertyValue()
{

    if(controlledProperty == GuiButtonSpritePropertyKind::VISIBILITY){
        return sprite->IsVisible();
    }

    return false;
}


void GuiButtonSpriteBoolPropertyController::ApplyPropertyValueToSprite(bool _value)
{

    if(controlledProperty == GuiButtonSpritePropertyKind::VISIBILITY){
        sprite->SetVisible(_value);
    }

}


//====================================================================================================


void GuiButtonSpriteIntPropertyController::SetStartingState(GuiButtonVisualState _buttonVisualState)
{

    currentValue = GetSpritePropertyValue();
    newValue = propertyDataForStates[(int)_buttonVisualState];

    if(newValue != currentValue){
        ApplyPropertyValueToSprite(newValue);
    }
}


void GuiButtonSpriteIntPropertyController::Update(GuiButtonVisualState _buttonVisualState)
{

    oldValue = currentValue;
    currentValue = GetSpritePropertyValue();

    newValue = propertyDataForStates[(int)_buttonVisualState];

    if(newValue != currentValue){
        ApplyPropertyValueToSprite(newValue);
    }

    oldButtonVisualState = _buttonVisualState;

}


int GuiButtonSpriteIntPropertyController::GetSpritePropertyValue()
{

    if(controlledProperty == GuiButtonSpritePropertyKind::FRAME){

        return static_cast<StandardSprite*>(sprite)->GetActiveImageFrameIndex();
    }

    return 0;
}


void GuiButtonSpriteIntPropertyController::ApplyPropertyValueToSprite(int _value)
{

    if(controlledProperty == GuiButtonSpritePropertyKind::FRAME){
        static_cast<StandardSprite*>(sprite)->SetActiveImageByFrameIndex(_value);
    }

}


//====================================================================================================



GuiButtonSpriteColorPropertyData GuiButtonSpriteColorPropertyData::ParseFromString(const std::string &text)
{

    GuiButtonSpriteColorPropertyData fv;

    std::vector<std::string>subparameters = SplitString(text, ",");

    if(subparameters.size()==1){
        fv.value = ColorRGBA::ParseFromHexString(subparameters.front());

    }else if(subparameters.size()>=4){
        fv.value = ColorRGBA::ParseFromHexString(subparameters[0]);
        fv.valueMin = ColorRGBA::ParseFromHexString(subparameters[1]);
        fv.valueMax = ColorRGBA::ParseFromHexString(subparameters[2]);
        fv.loopDurationMS = 1000 * std::stof(subparameters[3]);

        fv.loopType = LoopType::REVERT_DIRECTION;
        if(subparameters.size()==5){
            std::string loopType = subparameters[4];
            RemoveStringWhiteSpacesOnStartAndEnd(loopType);
            if(loopType=="restart"){
                fv.loopType = LoopType::RESTART;
            }
        }
    }

    return fv;
}


void GuiButtonSpriteColorPropertyController::ParseMouseOverTransitionTime(const std::string &text)
{

    std::vector<std::string>subparameters = SplitString(text, ",");

    if(subparameters.size()==2){
        float delta = std::stof(subparameters[0]);
        delta = ClampValue(delta, 0.0f, 1.0f);
        int durationMS = 1000 * std::stof(subparameters[1]);
        mouseOverTransitionSpeed = delta / durationMS;
    }


}


void GuiButtonSpriteColorPropertyController::ParseColorOverlayBlend(std::string text)
{

    RemoveStringWhiteSpacesOnStartAndEnd(text);

    if(text=="normal"){
        blend = ColorOverlayBlend::NORMAL;

    }else if(text=="multiply"){
        blend = ColorOverlayBlend::MULTIPLY;

    }else if(text=="linDodge"){
        blend = ColorOverlayBlend::LINEAR_DODGE;

    }else if(text=="color"){
        blend = ColorOverlayBlend::COLOR;

    }

    sprite->SetColorOverlayBlend(blend);

}


void GuiButtonSpriteColorPropertyController::SetStartingState(GuiButtonVisualState _buttonVisualState)
{

    currentValue = GetSpriteOverlayColor();
    newValue = propertyDataForStates[(int)_buttonVisualState].value;
    //newValue = propertyDataForStates[(int)_buttonVisualState]->value;
    targetValue = newValue;

    if(newValue != currentValue){
        ApplyOverlayColorToSprite(newValue);
    }

}


void GuiButtonSpriteColorPropertyController::Update(GuiButtonVisualState _buttonVisualState)
{


    ColorRGBAf value = propertyDataForStates[(int)_buttonVisualState].value;
    ColorRGBAf valueMin = propertyDataForStates[(int)_buttonVisualState].valueMin;
    ColorRGBAf valueMax = propertyDataForStates[(int)_buttonVisualState].valueMax;
    int loopDurationMS = propertyDataForStates[(int)_buttonVisualState].loopDurationMS;
    GuiButtonSpriteColorPropertyData::LoopType loopType = propertyDataForStates[(int)_buttonVisualState].loopType;
    //int reverseSign = propertyDataForStates[(int)_buttonVisualState]->reverseSign;

    //oldValue = currentValue;
    //currentValue = GetSpriteOverlayColor();

    if(_buttonVisualState != oldButtonVisualState){

        if(_buttonVisualState==GuiButtonVisualState::CHECKED || _buttonVisualState==GuiButtonVisualState::CHECKED_CURSOR_DOWN){
            DummyFunction();
        }

        targetValue = value;

        if(IsGuiButtonStateMouseOver(_buttonVisualState)!=IsGuiButtonStateMouseOver(oldButtonVisualState)){
            if(mouseOverTransitionSpeed>0.0001){
                mouseOverTransition = true;
            }
        }

        reachedR = reachedG = reachedB = reachedA = false;
    }


    //----
    if(mouseOverTransition){

        if(reachedR==false){
            int sign = (currentValue.r <= targetValue.r)? 1 : -1;
            float deltaValueR = time.GetLogicTimeMS() * mouseOverTransitionSpeed * sign;
            newValue.r = currentValue.r + deltaValueR;

            CheckColorReach(newValue.r, targetValue.r, sign, reachedR);
        }


        if(reachedG==false){
            int sign = (currentValue.g <= targetValue.g)? 1 : -1;
            float deltaValueG = time.GetLogicTimeMS() * mouseOverTransitionSpeed * sign;
            newValue.g = currentValue.g + deltaValueG;

            CheckColorReach(newValue.g, targetValue.g, sign, reachedG);
        }


        if(reachedB==false){
            int sign = (currentValue.b <= targetValue.b)? 1 : -1;
            float deltaValueB = time.GetLogicTimeMS() * mouseOverTransitionSpeed * sign;
            newValue.b = currentValue.b + deltaValueB;

            CheckColorReach(newValue.b, targetValue.b, sign, reachedB);
        }


        if(reachedA==false){
            int sign = (currentValue.a <= targetValue.a)? 1 : -1;
            float deltaValueA =  time.GetLogicTimeMS() * mouseOverTransitionSpeed * sign;
            newValue.a = currentValue.a + deltaValueA;

            CheckColorReach(newValue.a, targetValue.a, sign, reachedA);
        }


        if(reachedR && reachedG && reachedB && reachedA){
            newValue = targetValue;
            reachedR = reachedG = reachedB = reachedA = false;
            mouseOverTransition = false;
        }


    }else{

        if(loopDurationMS>0 && valueMax!=valueMin){


            if(keyboard.IsKeyPressed(KeyCode::W)){
                DummyFunction();
            }

            if(loopType == GuiButtonSpriteColorPropertyData::LoopType::REVERT_DIRECTION){
            //---------------------------------------------------------------------------------------

                if(reachedR==false){

                    float deltaValueR = reverseSign * time.GetLogicTimeMS() * (valueMax.r - valueMin.r) / loopDurationMS;
                    newValue.r = currentValue.r + deltaValueR;
                    int sign = (deltaValueR>=0)? 1 : -1;

                    CheckColorReach(newValue.r, valueMin.r, valueMax.r, sign, reachedR);
                }


                if(reachedG==false){

                    float deltaValueG = reverseSign *  time.GetLogicTimeMS() * (valueMax.g - valueMin.g) / loopDurationMS;
                    newValue.g = currentValue.g + deltaValueG;
                    int sign = (deltaValueG>=0)? 1 : -1;

                    CheckColorReach(newValue.g, valueMin.g, valueMax.g, sign, reachedG);
                }


                if(reachedB==false){

                    float deltaValueB = reverseSign * time.GetLogicTimeMS() * (valueMax.b - valueMin.b) / loopDurationMS;
                    newValue.b = currentValue.b + deltaValueB;
                    int sign = (deltaValueB>=0)? 1 : -1;

                    CheckColorReach(newValue.b, valueMin.b, valueMax.b, sign, reachedB);
                }

                if(reachedA==false){
                    float deltaValueA =  reverseSign * time.GetLogicTimeMS() * (valueMax.a - valueMin.a) / loopDurationMS;
                    newValue.a = currentValue.a + deltaValueA;
                    int sign = (deltaValueA>=0)? 1 : -1;

                    CheckColorReach(newValue.a, valueMin.a, valueMax.a, sign, reachedA);
                }

                if(reachedR && reachedG && reachedB && reachedA){
                    reverseSign = -reverseSign;
                    reachedR = reachedG = reachedB = reachedA = false;

                    DbgOutput(" reached all!");
                }


            }else if(loopType == GuiButtonSpriteColorPropertyData::LoopType::RESTART){
            //---------------------------------------------------------------------------------------


                if(reachedR==false){

                    float deltaValueR = time.GetLogicTimeMS() * (valueMax.r - valueMin.r) / loopDurationMS;
                    newValue.r = currentValue.r + deltaValueR;
                    int sign = (deltaValueR>=0)? 1 : -1;

                    /*
                    if(newValue.r <= valueMin.r){
                        newValue.r = valueMin.r;
                        reachedR = true;

                    }else if(newValue.r >= valueMax.r){
                        newValue.r = valueMax.r;
                        reachedR = true;
                    }
                    */

                    CheckColorReach(newValue.r, valueMin.r, valueMax.r, sign, reachedR);
                }


                if(reachedG==false){

                    float deltaValueG = time.GetLogicTimeMS() * (valueMax.g - valueMin.g) / loopDurationMS;
                    newValue.g = currentValue.g + deltaValueG;
                    int sign = (deltaValueG>=0)? 1 : -1;

                    /*
                    if(newValue.g <= valueMin.g){
                        newValue.g = valueMin.g;
                        reachedG = true;

                    }else if(newValue.g >= valueMax.g){
                        newValue.g = valueMax.g;
                        reachedG = true;
                    }
                    */

                    CheckColorReach(newValue.g, valueMin.g, valueMax.g, sign, reachedG);

                }


                if(reachedB==false){

                    float deltaValueB = time.GetLogicTimeMS() * (valueMax.b - valueMin.b) / loopDurationMS;
                    newValue.b = currentValue.b + deltaValueB;
                    int sign = (deltaValueB>=0)? 1 : -1;

                    /*
                    if(newValue.b <= valueMin.b){
                        newValue.b = valueMin.b;
                        reachedB = true;

                    }else if(newValue.b >= valueMax.b){
                        newValue.b = valueMax.b;
                        reachedB = true;
                    }
                    */

                    CheckColorReach(newValue.b, valueMin.b, valueMax.b, sign, reachedB);

                }

                if(reachedA==false){
                    float deltaValueA =  time.GetLogicTimeMS() * (valueMax.a - valueMin.a) / loopDurationMS;
                    newValue.a = currentValue.a + deltaValueA;
                    int sign = (deltaValueA>=0)? 1 : -1;

                    /*
                    if(newValue.a <= valueMin.a){
                        newValue.a = valueMin.a;
                        reachedA = true;

                    }else if(newValue.a >= valueMax.a){
                        newValue.a = valueMax.a;
                        reachedA = true;

                    }
                    */

                    CheckColorReach(newValue.a, valueMin.a, valueMax.a, sign, reachedA);

                }

                if(reachedR && reachedG && reachedB && reachedA){
                    newValue = valueMin;
                    reachedR = reachedG = reachedB = reachedA = false;
                }
            }

        }else{

            newValue = targetValue;
        }
    }


    if(newValue != currentValue){
        ApplyOverlayColorToSprite(newValue);
        currentValue = newValue;
    }

    oldButtonVisualState = _buttonVisualState;

}


void GuiButtonSpriteColorPropertyController::CheckColorReach(float &color, float targetColor, int sign, bool &reached)
{

    if(sign==1){
        if(color >= targetColor){
            color = targetColor;
            reached = true;
        }

    }else{
        if(color <= targetColor){
            color = targetColor;
            reached = true;
        }
    }
}


void GuiButtonSpriteColorPropertyController::CheckColorReach(float &color, float targetColor1, float targetColor2, int sign, bool &reached)
{

    if(sign==1){
        if(targetColor1>=targetColor2){

            if(color >= targetColor1){
                color = targetColor1;
                reached = true;
            }

        }else{

            if(color >= targetColor2){
                color = targetColor2;
                reached = true;
            }

        }

    }else{

        if(targetColor1<=targetColor2){

            if(color <= targetColor1){
                color = targetColor1;
                reached = true;
            }

        }else{

            if(color <= targetColor2){
                color = targetColor2;
                reached = true;
            }
        }
    }
}


ColorRGBAf GuiButtonSpriteColorPropertyController::GetSpriteOverlayColor()
{

    if(controlledProperty == GuiButtonSpritePropertyKind::COLOR_OVERLAY){
        return ColorRGBAf(sprite->GetOverlayColorRGBA());

    }else if(controlledProperty == GuiButtonSpritePropertyKind::LABEL_COLOR){
        if(sprite->GetKind()==SpriteKind::TEXT){
            TextSprite *s = static_cast<TextSprite*>(sprite);
            return ColorRGBAf(s->GetColor());
        }
    }

    return ColorRGBAf();
}


void GuiButtonSpriteColorPropertyController::ApplyOverlayColorToSprite(ColorRGBAf _value)
{

    if(controlledProperty == GuiButtonSpritePropertyKind::COLOR_OVERLAY){
        sprite->SetColorOverlayRGBA(ColorRGBA(255*_value.r, 255*_value.g, 255*_value.b, 255*_value.a));

    }else if(controlledProperty == GuiButtonSpritePropertyKind::LABEL_COLOR){
        if(sprite->GetKind()==SpriteKind::TEXT){
            TextSprite *s = static_cast<TextSprite*>(sprite);
            s->SetColor(ColorRGBA(255*_value.r, 255*_value.g, 255*_value.b, 255*_value.a));
        }
    }


}



//====================================================================================================



GuiButton::GuiButton(Sprite *_sprite)
{

    kind = GuiWidgetKind::BUTTON;
    rootSprite = _sprite;

    name = Parameter::GetValue(rootSprite->GetParameters(), "wName");
    tag = Parameter::GetIntValue(rootSprite->GetParameters(), "wTag");
    designKind = Parameter::GetValue(rootSprite->GetSourceSprite()->GetConstantParameters(), "wDesign");
    checkable = Parameter::Exists(rootSprite->GetSourceSprite()->GetConstantParameters(), "wCheckable");


    if(Parameter::Exists(rootSprite->GetParameters(), "wInitialState")){
        std::string initialStateS = Parameter::GetValue(rootSprite->GetParameters(), "wInitialState");
        RemoveStringWhiteSpacesOnStartAndEnd(initialStateS);
        if(initialStateS=="normal"){
            initialState = GuiButtonVisualState::NORMAL;

        }else if(initialStateS=="checked"){
            initialState = GuiButtonVisualState::CHECKED;

        }else if(initialStateS=="disabled"){
            initialState = GuiButtonVisualState::DISABLED;
        }
    }

    if(name=="Play"){
        DummyFunction();
    }

    if(Parameter::Exists(rootSprite->GetParameters(), "wExclusiveGroup")){
        std::string exclusiveGroupName = Parameter::GetValue(rootSprite->GetParameters(), "wExclusiveGroup");
        exclusiveGroup = guiButtonExclusiveGroups.FindGroup(exclusiveGroupName);
        if(exclusiveGroup==nullptr){
            exclusiveGroup = new GuiButtonExclusiveGroup(exclusiveGroupName);
            guiButtonExclusiveGroups.AddGroup(exclusiveGroup);
        }
        exclusiveGroup->AddButton(this);
    }


    //if(_sprite->GetKind()==SpriteKind::TEXT){
    //    static_cast<TextSprite*>(_sprite)->SetClickable(true);
       // DummyFunction();
    //}
    BuildControllersForSprite(_sprite);


    std::vector<Sprite*>sprites;

    // controllers for text
    if(rootSprite->GetKind()==SpriteKind::COMPOSED){

        //--- highlight sprite
        sprites.clear();
        CollectSpritesWithConstParameter(static_cast<ComposedSprite*>(rootSprite), sprites, "wHighlightMarker");
        if(sprites.empty()==false){
            highlightMarkerSprite = sprites.front();
            highlightMarkerSprite->SetVisible(false);
        }

        sprites.clear();
        CollectSpritesWithOrigConstParameter(static_cast<ComposedSprite*>(rootSprite), sprites, "wLabel");
        if(sprites.empty()==false && sprites.front()->GetKind()==SpriteKind::TEXT){
            labelSprite = static_cast<TextSprite*>(sprites.front());
        }

    }else if(rootSprite->GetKind()==SpriteKind::TEXT){
        labelSprite = static_cast<TextSprite*>(rootSprite);
    }

}


GuiButton::~GuiButton()
{

    for(GuiButtonSpritePropertyController * c : spriteControllers){
        delete c;
    }
}


void GuiButton::BuildControllersForSprite(Sprite * _sprite)
{


    //if(_sprite->GetKind()==SpriteKind::STANDARD){

        _AddControllersForSprite(_sprite);


    //}else
    if(_sprite->GetKind()==SpriteKind::COMPOSED){

        ComposedSprite *cs = static_cast<ComposedSprite*>(_sprite);

        for(Layer *l : cs->GetLayers()){
            if(l->GetKind()==LayerKind::SPRITE){
                SpriteLayer *sl = static_cast<SpriteLayer*>(l);
                for(Sprite *s : sl->GetSprites()){

                    _AddControllersForSprite(s);

                    if(s->GetKind()==SpriteKind::COMPOSED){
                        BuildControllersForSprite(s);
                    }
                }
            }
        }
    }
}


void GuiButton::_AddControllersForSprite(Sprite * _sprite)
{

    //std::vector<Parameter>& cp = _sprite->GetSourceSprite()->GetConstantParameters();
    if(_sprite->GetSourceSpriteReplacements()){
        DummyFunction();
    }
    std::vector<Parameter>& cp = _sprite->GetOriginalSourceSprite()->GetConstantParameters();

    if(cp.empty()) return;


    std::vector<std::string>propertyNames;
    std::vector<GuiButtonSpritePropertyKind>propertyKinds;


    //---- float properties
    propertyNames = std::vector<std::string>{ "wAlpha", "wScale", "wRotation"};
    propertyKinds = std::vector<GuiButtonSpritePropertyKind>{ GuiButtonSpritePropertyKind::ALPHA, GuiButtonSpritePropertyKind::SCALE, GuiButtonSpritePropertyKind::ROTATION};

    for(int i=0; i<propertyNames.size(); i++){

        const std::string &propertyName = propertyNames[i];
        GuiButtonSpritePropertyKind propertyKind = propertyKinds[i];

        _AddFloatPropertyControllerIfNeeded(_sprite, cp, propertyName, propertyKind);
    }


    //---- color properties
    propertyNames = std::vector<std::string>{ "wVisible"};
    propertyKinds = std::vector<GuiButtonSpritePropertyKind>{ GuiButtonSpritePropertyKind::VISIBILITY};

    for(int i=0; i<propertyNames.size(); i++){

        const std::string &propertyName = propertyNames[i];
        GuiButtonSpritePropertyKind propertyKind = propertyKinds[i];

        _AddBooleanPropertyControllerIfNeeded(_sprite, cp, propertyName, propertyKind);
    }


    //---- int properties
    propertyNames = std::vector<std::string>{ "wFrame"};
    propertyKinds = std::vector<GuiButtonSpritePropertyKind>{ GuiButtonSpritePropertyKind::FRAME};


    for(int i=0; i<propertyNames.size(); i++){

        const std::string &propertyName = propertyNames[i];
        GuiButtonSpritePropertyKind propertyKind = propertyKinds[i];

        if(propertyKind==GuiButtonSpritePropertyKind::FRAME && _sprite->GetKind()!=SpriteKind::STANDARD) continue;

        _AddIntPropertyControllerIfNeeded(_sprite, cp, propertyName, propertyKind);

    }


    //---- color properties
    propertyNames = std::vector<std::string>{ "wLabelColor" , "wColorOverlay"};
    propertyKinds = std::vector<GuiButtonSpritePropertyKind>{ GuiButtonSpritePropertyKind::LABEL_COLOR, GuiButtonSpritePropertyKind::COLOR_OVERLAY};


    for(int i=0; i<propertyNames.size(); i++){

        const std::string &propertyName = propertyNames[i];
        GuiButtonSpritePropertyKind propertyKind = propertyKinds[i];

        if(propertyKind==GuiButtonSpritePropertyKind::LABEL_COLOR && _sprite->GetKind()!=SpriteKind::TEXT) continue;
        if(propertyKind==GuiButtonSpritePropertyKind::COLOR_OVERLAY && _sprite->GetKind()!=SpriteKind::STANDARD) continue;

        _AddColorPropertyControllerIfNeeded(_sprite, cp, propertyName, propertyKind);

    }

}


void GuiButton::_AddFloatPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind)
{


    std::string parameterName = propertyName;

    if(Parameter::Exists(cp, parameterName)){

        GuiButtonSpriteFloatPropertyController * c = new GuiButtonSpriteFloatPropertyController(_sprite, propertyKind);
        c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        //c->propertyDataForStates[(int)GuiButtonState::NORMAL] = std::make_shared<GuiButtonSpriteFloatPropertyData>(GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::Value(cp, parameterName)));

        parameterName = propertyName + "_CO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
            //c->propertyDataForStates[(int)GuiButtonState::NORMAL_MOUSE_OVER] = std::make_shared<GuiButtonSpriteFloatPropertyData>(GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::Value(cp, parameterName)));

        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_CD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
            //c->propertyDataForStates[(int)GuiButtonState::NORMAL_MOUSE_DOWN] = std::make_shared<GuiButtonSpriteFloatPropertyData>(GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::Value(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checked";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
            //c->propertyDataForStates[(int)GuiButtonState::CHECKED] = std::make_shared<GuiButtonSpriteFloatPropertyData>(GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::Value(cp, parameterName)));

        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_checkedCO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
            //c->propertyDataForStates[(int)GuiButtonState::CHECKED_MOUSE_OVER] = std::make_shared<GuiButtonSpriteFloatPropertyData>(GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::Value(cp, parameterName)));

        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED];
        }

        parameterName = propertyName + "_checkedCD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
            //c->propertyDataForStates[(int)GuiButtonState::CHECKED_MOUSE_DOWN] = std::make_shared<GuiButtonSpriteFloatPropertyData>(GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::Value(cp, parameterName)));

        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER];
        }

        parameterName = propertyName + "_HL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checkedHL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED];
        }


        parameterName = propertyName + "_disabled";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
            //c->propertyDataForStates[(int)GuiButtonState::DISABLED] = std::make_shared<GuiButtonSpriteFloatPropertyData>(GuiButtonSpriteFloatPropertyData::ParseFromString(Parameter::Value(cp, parameterName)));

        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_transitionDur";
        if(Parameter::Exists(cp, parameterName)){
            c->ParseMouseOverTransitionData(Parameter::GetValue(cp, parameterName));
        }

        spriteControllers.push_back(c);
    }

}


void GuiButton::_AddIntPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind)
{


    std::string parameterName = propertyName;

    if(Parameter::Exists(cp, parameterName)){

        GuiButtonSpriteIntPropertyController * c = new GuiButtonSpriteIntPropertyController(_sprite, propertyKind);
        c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);

        parameterName = propertyName + "_CO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_CD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checked";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_checkedCO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED];
        }

        parameterName = propertyName + "_checkedCD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER];
        }

        parameterName = propertyName + "_HL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checkedHL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED];
        }

        parameterName = propertyName + "_disabled";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = _ParseIntPropertyFromParameter(cp, parameterName, propertyKind);
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        spriteControllers.push_back(c);
    }

}


void GuiButton::_AddBooleanPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind)
{


    std::string parameterName = propertyName;

    if(Parameter::Exists(cp, parameterName)){

        GuiButtonSpriteBoolPropertyController * c = new GuiButtonSpriteBoolPropertyController(_sprite, propertyKind);
        c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));

        parameterName = propertyName + "_CO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_CD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checked";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_checkedCO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED];
        }

        parameterName = propertyName + "_checkedCD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER];
        }


        parameterName = propertyName + "_HL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checkedHL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED];
        }


        parameterName = propertyName + "_disabled";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = bool(std::stoi(Parameter::GetValue(cp, parameterName)));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        spriteControllers.push_back(c);
    }

}


void GuiButton::_AddColorPropertyControllerIfNeeded(Sprite *_sprite, std::vector<Parameter>& cp, const std::string &propertyName, GuiButtonSpritePropertyKind propertyKind)
{

    std::string parameterName = propertyName;

    if(Parameter::Exists(cp, parameterName)){

        GuiButtonSpriteColorPropertyController * c = new GuiButtonSpriteColorPropertyController(_sprite, propertyKind);
        c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));

        parameterName = propertyName + "_CO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_CD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checked";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_checkedCO";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED];
        }

        parameterName = propertyName + "_checkedCD";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_DOWN] = c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_CURSOR_OVER];
        }

        parameterName = propertyName + "_HL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_CURSOR_OVER];
        }

        parameterName = propertyName + "_checkedHL";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::CHECKED_HIGHLIGHTED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL_HIGHLIGHTED];
        }

        parameterName = propertyName + "_disabled";
        if(Parameter::Exists(cp, parameterName)){
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = GuiButtonSpriteColorPropertyData::ParseFromString(Parameter::GetValue(cp, parameterName));
        }else{
            c->propertyDataForStates[(int)GuiButtonVisualState::DISABLED] = c->propertyDataForStates[(int)GuiButtonVisualState::NORMAL];
        }

        parameterName = propertyName + "_transitionDur";
        if(Parameter::Exists(cp, parameterName)){
            c->ParseMouseOverTransitionTime(Parameter::GetValue(cp, parameterName));
        }

        parameterName = propertyName + "_blend";
        if(Parameter::Exists(cp, parameterName)){
            c->ParseColorOverlayBlend(Parameter::GetValue(cp, parameterName));
        }


        if(propertyKind==GuiButtonSpritePropertyKind::COLOR_OVERLAY){
            rootSprite->SetColorOverlayActive(true);
        }

        spriteControllers.push_back(c);
    }

}


int GuiButton::_ParseIntPropertyFromParameter(std::vector<Parameter>& cp, const std::string &parameterName, GuiButtonSpritePropertyKind propertyKind)
{

    if(propertyKind==GuiButtonSpritePropertyKind::LABEL_COLOR){

        std::string hexColorS = Parameter::GetValue(cp, parameterName);
        RemoveStringWhiteSpacesOnStartAndEnd(hexColorS);

        if(hexColorS.length()!=6){
            return 0;

        }else{
            std::string hexR = hexColorS.substr(0,2);
            std::string hexG = hexColorS.substr(2,2);
            std::string hexB = hexColorS.substr(4,2);

            int r = std::stoi(hexR, nullptr, 16);
            int g = std::stoi(hexG, nullptr, 16);
            int b = std::stoi(hexB, nullptr, 16);

            int rgba = r | (g << 8) | (b << 16) | (255 << 24);

            return rgba;
        }

    }else{

        return std::stoi(Parameter::GetValue(cp, parameterName));
    }

}


void GuiButton::SetToInitialState()
{

    //SetState(initialState);

    state = initialState;

    if(state==GuiButtonVisualState::CHECKED && exclusiveGroup){
        checked = true;

        for(GuiButton * b : exclusiveGroup->GetButtons()){
            if(b==this) continue;

            b->state = GuiButtonVisualState::NORMAL;
            for(GuiButtonSpritePropertyController* c : b->spriteControllers){
                c->SetStartingState(b->state);
            }
            b->checked = false;
        }
    }


    for(GuiButtonSpritePropertyController* c : spriteControllers){
        c->SetStartingState(state);
    }

}


void GuiButton::SetDisabled(bool _disabled)
{

    if(disabled==_disabled) return;

    GuiWidget::SetDisabled(_disabled);

    if(disabled){
        if(state != GuiButtonVisualState::DISABLED){
            state = GuiButtonVisualState::DISABLED;
            for(GuiButtonSpritePropertyController* c : spriteControllers){
                c->SetStartingState(state);
            }
        }
    }else{

        if(state == GuiButtonVisualState::DISABLED){
            if(checked){
                state = GuiButtonVisualState::CHECKED;
            }else{
                state = GuiButtonVisualState::NORMAL;
            }
            for(GuiButtonSpritePropertyController* c : spriteControllers){
                c->SetStartingState(state);
            }
        }
    }

}


void GuiButton::SetVisible(bool _visible)
{

    if(visible==_visible) return;

    GuiWidget::SetVisible(_visible);

    if(visible){
        if(state==GuiButtonVisualState::NORMAL_CURSOR_OVER || state==GuiButtonVisualState::NORMAL_CURSOR_DOWN){
            state = GuiButtonVisualState::NORMAL;
        }else if(state==GuiButtonVisualState::CHECKED_CURSOR_OVER || state==GuiButtonVisualState::CHECKED_CURSOR_DOWN){
            state = GuiButtonVisualState::CHECKED;
        }
        for(GuiButtonSpritePropertyController* c : spriteControllers){
            c->SetStartingState(state);
        }
    }

    rootSprite->SetVisible(visible);

}


void GuiButton::SetChecked(bool _checked)
{

    if(checkable==false) return;

    checked = _checked;

    //set visual state
    if(disabled) return;

    if(checked){
        if(state != GuiButtonVisualState::CHECKED){
            state = GuiButtonVisualState::CHECKED;
            for(GuiButtonSpritePropertyController* c : spriteControllers){
                c->SetStartingState(state);
            }
        }

        if(exclusiveGroup){
            for(GuiButton * b : exclusiveGroup->GetButtons()){
                if(b==this) continue;

                b->state = GuiButtonVisualState::NORMAL;
                for(GuiButtonSpritePropertyController* c : b->spriteControllers){
                    c->SetStartingState(b->state);
                }
                b->checked = false;
            }
        }

    }else{

        if(exclusiveGroup==nullptr){        // It is not possible to set 'checked' to false if button is in a exclusive group !

            if(state != GuiButtonVisualState::NORMAL){
                state = GuiButtonVisualState::NORMAL;
                for(GuiButtonSpritePropertyController* c : spriteControllers){
                    c->SetStartingState(state);
                }
            }
        }
    }


}


void GuiButton::Update()
{

    if(disabled) return;
    if(visible==false) return;


    pressed = false;


    GuiCursorDeviceInput &mouseInput = guiCursorDeviceInput;


    cursorOver = rootSprite->PointInside(guiCursorDeviceInput.GetCursorInMapPosition());
    if(cursorOver== false && guiKeyboardAndJoystickInput.GetHighlightedWidget()==this){
        cursorOver = true;
    }

    cursorDown = guiCursorDeviceInput.IsCursorDown();
    if(cursorDown==false && guiKeyboardAndJoystickInput.GetHighlightedWidget()==this){
        if(guiKeyboardAndJoystickInput.GetHighlighedDown()){
            cursorDown = true;
        }
    }

    //if(cursorOver){
    //    DbgOutput("button: " + GetName()+ "  state:" + std::to_string(int(state)));
    //}


    if(state==GuiButtonVisualState::NORMAL){

        if(cursorOver){
            state = GuiButtonVisualState::NORMAL_CURSOR_OVER;
        }


    }else if(state==GuiButtonVisualState::NORMAL_CURSOR_OVER){

        if(cursorOver==false){
            state = GuiButtonVisualState::NORMAL;

        }else if(cursorDown){
            state = GuiButtonVisualState::NORMAL_CURSOR_DOWN;

            if(checkable){
                state = GuiButtonVisualState::CHECKED_CURSOR_DOWN;

                if(exclusiveGroup){
                    for(GuiButton * b : exclusiveGroup->GetButtons()){
                        if(b==this) continue;

                        b->state = GuiButtonVisualState::NORMAL;
                        for(GuiButtonSpritePropertyController* c : b->spriteControllers){
                            c->SetStartingState(b->state);
                        }
                        b->checked = false;
                    }
                }

            }
            pressed = true;
        }


    }else if(state==GuiButtonVisualState::NORMAL_CURSOR_DOWN){

        //if(sprite->PointInside(mousePos)==false){
        //    state = GuiButtonState::NORMAL;
        //    mouse.Reset();

        //}else
        //if(mouse.ButtonDown(MouseButton::LEFT)==false){
        if(cursorDown==false){
            state = GuiButtonVisualState::NORMAL_CURSOR_OVER;
        }


    }else if(state==GuiButtonVisualState::CHECKED){

        if(cursorOver){
            state = GuiButtonVisualState::CHECKED_CURSOR_OVER;
        }


    }else if(state==GuiButtonVisualState::CHECKED_CURSOR_OVER){

        if(cursorOver==false){
            state = GuiButtonVisualState::CHECKED;

        }else if(cursorDown){
            state = GuiButtonVisualState::CHECKED_CURSOR_DOWN;
            if(checkable && exclusiveGroup==nullptr){
                state = GuiButtonVisualState::NORMAL_CURSOR_DOWN;
            }
            pressed = true;
        }

    }else if(state==GuiButtonVisualState::CHECKED_CURSOR_DOWN){

        //if(sprite->PointInside(mousePos)==false){
        //    state = GuiButtonState::CHECKED;
        //    mouse.Reset();

        //}else
        //if(mouse.ButtonDown(MouseButton::LEFT)==false){
        if(cursorDown==false){
            state = GuiButtonVisualState::CHECKED_CURSOR_OVER;

        }
    }

    //---
    cursorDown = (state==GuiButtonVisualState::NORMAL_CURSOR_DOWN || state==GuiButtonVisualState::CHECKED_CURSOR_DOWN);
    checked = (state==GuiButtonVisualState::CHECKED || state==GuiButtonVisualState::CHECKED_CURSOR_OVER || state==GuiButtonVisualState::CHECKED_CURSOR_DOWN);


    //---
    for(GuiButtonSpritePropertyController* c : spriteControllers){
        c->Update(state);
    }


    //---
    if(cursorOver || cursorDown || pressed){
        GuiWidget::_SetInteractedWidget(this);

        if(callbackObj){
            if(pressed){
                callbackObj->OnPressed(this);
            }
            if(cursorOver){
                callbackObj->OnCursorOver(this);
            }
            if(cursorDown){
                callbackObj->OnCursorDown(this);
            }
        }
    }

}


void GuiButton::SetTextLabel(const std::string &_label)
{
    if(labelSprite){
        labelSprite->SetTextString(_label);
    }
}


//===================================================================================================


void GuiButtonExclusiveGroup::AddButton(GuiButton *_button)
{

    if(std::find(buttons.begin(), buttons.end(), _button) != buttons.end()) return;
    buttons.push_back(_button);
}



GuiButtonExclusiveGroups::~GuiButtonExclusiveGroups()
{
    for(GuiButtonExclusiveGroup *g : groups){
        delete g;
    }
}


GuiButtonExclusiveGroup* GuiButtonExclusiveGroups::FindGroup(const std::string &_name)
{

    for(GuiButtonExclusiveGroup *g : groups){
        if(g->GetName()==_name){
            return g;
        }
    }

    return nullptr;
}


GuiButtonExclusiveGroups guiButtonExclusiveGroups;



//===================================================================================================



GuiSlider::GuiSlider(ComposedSprite * _sprite)
{


    kind = GuiWidgetKind::SLIDER;
    rootSprite = _sprite;

    name = Parameter::GetValue(rootSprite->GetParameters(), "wName");
    tag = Parameter::GetIntValue(rootSprite->GetParameters(), "wTag");
    designKind = Parameter::GetValue(rootSprite->GetSourceSprite()->GetConstantParameters(), "wDesign");


    //--- value range
    std::string valueRange =  Parameter::GetValue(rootSprite->GetSourceSprite()->GetConstantParameters(), "wValueRange");
    std::vector<std::string>values = SplitString(valueRange, ",");

    if(values.size()>=2){
        valueMin = std::stof(values[0]);
        valueMax = std::stof(values[1]);
        if(valueMin>valueMax){
            std::swap(valueMin, valueMax);
        }
        value = (valueMin+valueMax)/2.0;
    }
    if(values.size()==3){
        nSnapDivision = std::stoi(values[2]);
    }


    //--- value
    if(Parameter::Exists(rootSprite->GetParameters(), "wValue")){
        value = std::stof(Parameter::GetValue(rootSprite->GetParameters(), "wValue"));
    }
    value = ClampValue(value, valueMin, valueMax);


    std::vector<Sprite*>sprites;

    //--- sliding button
    CollectSpritesWithConstParameter(rootSprite, sprites, "wSlidingButton");
    if(sprites.empty()==false){
        slidingButton = new GuiButton(sprites.front());
        slidingButton->SetName("slidingButton");
    }

    //--- sliding scale sprite
    sprites.clear();
    CollectSpritesWithConstParameter(rootSprite, sprites, "wSlidingScale");
    if(sprites.empty()==false && sprites.front()->GetKind()==SpriteKind::STANDARD){
        slidingScaleSprite = static_cast<StandardSprite*>(sprites.front());

        //--- slider button positions for min and max value
        VectorShape* sh = FindSpriteExtraShapeWithProperties(slidingScaleSprite->GetActiveImage(), 1, false, ShapeKind::SINGLE_POINT);
        assert(sh);
        slidingPosMin = static_cast<SinglePointShape*>(sh->GetGeometricShape())->point;
        slidingPosMin += slidingScaleSprite->GetPosition();       // point is 'relative' to sliderScaleSprite, we need position relative to 'sprite'

        sh = FindSpriteExtraShapeWithProperties(slidingScaleSprite->GetActiveImage(), 2, false, ShapeKind::SINGLE_POINT);
        assert(sh);
        slidingPosMax = static_cast<SinglePointShape*>(sh->GetGeometricShape())->point;
        slidingPosMax += slidingScaleSprite->GetPosition();       // point is 'relative' to sliderScaleSprite, we need position relative to 'sprite'
    }


    //--- increment button
    sprites.clear();
    CollectSpritesWithConstParameter(rootSprite, sprites, "wIncrementButton");
    if(sprites.empty()==false){
        incrementButton = new GuiButton(sprites.front());
        incrementButton->SetName("incrementButton");
    }


    //--- decrement button
    sprites.clear();
    CollectSpritesWithConstParameter(rootSprite, sprites, "wDecrementButton");
    if(sprites.empty()==false){
        decrementButton = new GuiButton(sprites.front());
        decrementButton->SetName("decrementButton");
    }


    //--- valueLabel
    sprites.clear();
    CollectSpritesWithOrigConstParameter(rootSprite, sprites, "wLabel");
    if(sprites.empty()==false && sprites.front()->GetKind()==SpriteKind::TEXT){

        valueLabel = static_cast<TextSprite*>(sprites.front());
        valueLabelPrecision = 0;
        valueLabelSuffix = "";

        std::string sliderValueLabel =  Parameter::GetValue(valueLabel->GetSourceSprite()->GetConstantParameters(), "wLabel");
        if(sliderValueLabel!=""){

            values = SplitString(sliderValueLabel, ",");
            std::string labelSpriteName;

            if(values.size()==1){
                valueLabelPrecision = (int)std::stof(values[0]);

            }else if(values.size()==2){
                valueLabelPrecision = (int)std::stof(values[0]);
                valueLabelSuffix = values[1];
                RemoveStringWhiteSpacesOnStartAndEnd(valueLabelSuffix);
            }
        }
    }


    //--- delta value step
    std::string sDeltaValue =  Parameter::GetValue(rootSprite->GetSourceSprite()->GetConstantParameters(), "wDeltaValue");
    if(sDeltaValue!=""){
        RemoveStringWhiteSpacesOnStartAndEnd(sDeltaValue);
        if(sDeltaValue.back()=='%'){
            deltaValueInPercents = true;
            sDeltaValue = sDeltaValue.substr(0, sDeltaValue.length()-1);

        }else{
            deltaValueInPercents = false;
        }


        deltaValue = std::stof(sDeltaValue);
        if(deltaValueInPercents){
            deltaValue /= 100.0;
        }
    }


    //--- highlight sprite
    sprites.clear();
    CollectSpritesWithConstParameter(rootSprite, sprites, "wHighlightMarker");
    if(sprites.empty()==false){
        highlightMarkerSprite = sprites.front();
        highlightMarkerSprite->SetVisible(false);
    }



}


GuiSlider::~GuiSlider()
{

    if(slidingButton) delete slidingButton;
    if(incrementButton) delete incrementButton;
    if(decrementButton) delete decrementButton;

}


void GuiSlider::SetToInitialState()
{

    if(slidingButton) slidingButton->SetToInitialState();
    if(incrementButton) incrementButton->SetToInitialState();
    if(decrementButton) decrementButton->SetToInitialState();

    SetSnapDivision(nSnapDivision);

    value = ClampValue(value, valueMin, valueMax);

    SetSlidingButtonPositionFromValues();
    SetValueLabelText();

}


void GuiSlider::SetDisabled(bool _disabled)
{

    if(disabled==_disabled) return;

    GuiWidget::SetDisabled(_disabled);

    //--- reset interaction flags
    if(_disabled){
        valueChangedViaStepButtons = false;
    }
}


void GuiSlider::SetVisible(bool _visible)
{

    if(visible==_visible) return;

    GuiWidget::SetVisible(_visible);

    //--- reset interaction flags
    if(_visible==false){
        valueChangedViaStepButtons = false;
    }

    rootSprite->SetVisible(visible);
}


void GuiSlider::SetValue(float _value)
{

    value = ClampValue(_value, valueMin, valueMax);

    //----
    SetSlidingButtonPositionFromValues();
    SetValueLabelText();
}


void GuiSlider::SetValueMin(float _valueMin)
{

    valueMin = _valueMin;
    value = ClampValue(value, valueMin, valueMax);

    SetSlidingButtonPositionFromValues();
}


void GuiSlider::SetValueMax(float _valueMax)
{

    valueMax = _valueMax;
    value = ClampValue(value, valueMin, valueMax);

    SetSlidingButtonPositionFromValues();
}


void GuiSlider::Update()
{

    if(disabled) return;
    if(visible==false) return;

    float valueStored = value;
    valueChanged = false;
    valueChangedViaStepButtons = false;


    GuiCursorDeviceInput &id = guiCursorDeviceInput;


    if(slidingButton){

        slidingButton->Update();

        if(state==GuiSliderState::IDLE){

            if(slidingButton->IsCursorDown()){
                Vec2f buttonPos = slidingButton->GetSprite()->GetPosition();
                Vec2f localCursorPos = rootSprite->GetLocalPositionFromInMapPosition(guiCursorDeviceInput.GetCursorInMapPosition());

                Vec2f projMousePos;
                distToLine(localCursorPos, slidingPosMin, slidingPosMax, projMousePos);
                startMovingOffset = slidingButton->GetSprite()->GetPosition() - projMousePos;
                state = GuiSliderState::MOVING;

                //DbgOutput("GuiSlider START MOVING startMovingOffset x:" + std::to_string(startMovingOffset.x) +"  y:" + std::to_string(startMovingOffset.y));

            }

        }else if(state==GuiSliderState::MOVING){

            if(slidingButton->IsCursorDown()){

                //Vec2f localMousePos = sprite->ConvertFromWorldPos(mousePos);
                Vec2f localCursorPos = rootSprite->GetLocalPositionFromInMapPosition(guiCursorDeviceInput.GetCursorInMapPosition());

                Vec2f projMousePos;
                distToLine(localCursorPos, slidingPosMin, slidingPosMax, projMousePos);

                virtualSlidingPos = projMousePos + startMovingOffset;

                virtualSlidingPos = GetSnappedPosition(virtualSlidingPos);

                virtualSlidingPos.x = ClampValue(virtualSlidingPos.x, std::min(slidingPosMin.x, slidingPosMax.x), std::max(slidingPosMin.x, slidingPosMax.x));
                virtualSlidingPos.y = ClampValue(virtualSlidingPos.y, std::min(slidingPosMin.y, slidingPosMax.y), std::max(slidingPosMin.y, slidingPosMax.y));


                Vec2f buttonPos = slidingButton->GetSprite()->GetPosition();

                if(virtualSlidingPos.Equals(buttonPos)==false){
                    slidingButton->GetSprite()->SetPosition(virtualSlidingPos);
                    SetValueFromSlidingButtonPosition();

                    //DbgOutput("GuiSlider MOVING newButtonPos x:" + std::to_string(virtualSlidingPos.x) +"  y:" + std::to_string(virtualSlidingPos.y));
                }

            }else{
                state = GuiSliderState::IDLE;
            }
        }

        //---
        if(GuiWidget::GetInteracted() == slidingButton){
            GuiWidget::_SetInteractedWidget(this);
        }

    }


    float deltaValueActual = deltaValue;
    if(deltaValueInPercents){
        deltaValueActual = (deltaValue)*(valueMax-valueMin);
    }
    if(nSnapDivision>1){
        deltaValueActual = (valueMax-valueMin)/nSnapDivision;
    }

    //----
    bool increment = false;
    if(incrementButton){
        incrementButton->Update();
        if(incrementButton->IsPressed()){
            increment = true;
        }

        //---
        if(GuiWidget::GetInteracted() == incrementButton){
            GuiWidget::_SetInteractedWidget(this);
        }
    }

    if(increment==false){
        if(guiKeyboardAndJoystickInput.GetHighlightedWidget()==this){
            if(guiKeyboardAndJoystickInput.GetHighlightedChangeValue()==1){
                increment = true;
            }
        }
    }

    if(increment){
        value += deltaValueActual;
        value = ClampValue(value, valueMin, valueMax);

        //----
        SetSlidingButtonPositionFromValues();
        SetValueLabelText();

        //---
        //if(interactedGuiWidget.GetWidget() == incrementButton){
            GuiWidget::_SetInteractedWidget(this);
        //}


    }

    //----
    bool decrement = false;
    if(decrementButton){
        decrementButton->Update();
        if(decrementButton->IsPressed()){
            decrement = true;
        }

        //---
        if(GuiWidget::GetInteracted() == decrementButton){
            GuiWidget::_SetInteractedWidget(this);
        }
    }

    if(decrement==false){
        if(guiKeyboardAndJoystickInput.GetHighlightedWidget()==this){
            if(guiKeyboardAndJoystickInput.GetHighlightedChangeValue()==-1){
                decrement = true;
            }
        }
    }

    if(decrement){
        value -= deltaValueActual;
        value = ClampValue(value, valueMin, valueMax);

        //----
        SetSlidingButtonPositionFromValues();
        SetValueLabelText();

        //---
        //if(interactedGuiWidget.GetWidget() == decrementButton){
            GuiWidget::_SetInteractedWidget(this);
        //}
    }


    //---
    if(incrementButton){
        incrementButton->SetDisabled(AreEqual(value, valueMax));
    }

    if(decrementButton){
        decrementButton->SetDisabled(AreEqual(value, valueMin));
    }


    //----
    if(AreEqual(value, valueStored)==false){
        valueChanged = true;

        if(increment || decrement){
            valueChangedViaStepButtons = true ;
        }

        if(callbackObj){
            if(valueChanged){
                callbackObj->OnValueChanged(this);
            }
            if(valueChangedViaStepButtons){
                callbackObj->OnValueChangedViaButton(this);
            }
        }
    }

}


void GuiSlider::SetSlidingButtonPositionFromValues()
{
    if(slidingButton==nullptr) return;

    value = ClampValue(value, valueMin, valueMax);
    float f = (value-valueMin)/(valueMax-valueMin);

    Vec2f buttonPos = slidingPosMin + (slidingPosMax - slidingPosMin) * f;
    Vec2f snappedPos = GetSnappedPosition(buttonPos);

    slidingButton->GetSprite()->SetPosition(snappedPos);

    if(snappedPos.Equals(buttonPos)==false){
        SetValueFromSlidingButtonPosition();
    }
}


void GuiSlider::SetValueFromSlidingButtonPosition()
{

    if(slidingButton==nullptr) return;


    Vec2f buttonPos = slidingButton->GetSprite()->GetPosition();

    float f = DistanceTwoPoints(buttonPos,slidingPosMin)/DistanceTwoPoints(slidingPosMax,slidingPosMin);
    f = ClampValue(f, 0.0f, 1.0f);

    value = valueMin + (valueMax - valueMin) * f;

    //----
    SetValueLabelText();
}


void GuiSlider::SetValueLabelText()
{
    if(valueLabel==nullptr) return;

    //std::string valueText = std::to_string(value);

    std::ostringstream out;
    out.precision(valueLabelPrecision);
    out << std::fixed << value;
    std::string valueText = out.str();
    //DbgOutput("SetValueText value: " + std::to_string(value));
    DbgOutput("SetValueText value: " + std::to_string(GetIntValue()));


    if(valueLabelSuffix != ""){
        valueText += " " + valueLabelSuffix;
    }

    if(valueText != valueLabel->GetTextString()){
        valueLabel->SetTextString(valueText);
    }

}


void GuiSlider::SetSnapDivision(int _nSnapDivision)
{

    snapPositions.clear();

    nSnapDivision = _nSnapDivision;

    if(nSnapDivision>1){
        float length = DistanceTwoPoints(slidingPosMin, slidingPosMax);
        float step = length/nSnapDivision;

        float dist = 0;
        while(dist<=length+step/2.0){
            Vec2f p = MakeVec2fAtDistanceFromP1(slidingPosMin, slidingPosMax, dist);
            snapPositions.push_back(p);
            dist += step;
        }
    }

}



Vec2f GuiSlider::GetSnappedPosition(Vec2f _pos)
{

    if(snapPositions.empty()) return _pos;


    Vec2f snappedPos = _pos;

    float distMin = 100000.0;

    for(Vec2f &p : snapPositions){
        float dist = DistanceTwoPoints(_pos, p);
        if(dist<distMin){
            snappedPos = p;
            distMin = dist;
        }
    }

    return snappedPos;
}



//=====================================================================================================




void GuiBarSpriteScaleController::ParseFromString(const std::string &_text)
{

    std::vector<std::string>values = SplitString(_text, ",");

    if(values.size()>=1){

        std::string sValue = values[0];
        RemoveStringWhiteSpacesOnStartAndEnd(sValue);

        if(sValue=="x"){
            xScale = true;

        }else if(sValue=="y"){
            yScale = true;

        }else if(sValue=="xy"){
            xScale = true;
            yScale = true;
        }
    }

    for(int i=1; i<int(values.size())-1; i+=2){

        std::string sHexColor  =   values[i];
        RemoveStringWhiteSpacesOnStartAndEnd(sHexColor);
        std::string sColorPos = values[i+1];

        PositionColor pc;
        pc.color = ColorRGBA::ParseFromHexString(sHexColor);
        pc.pos = std::stof(sColorPos);

        colors.push_back(pc);
    }

    if(colors.empty()==false){
        sprite->SetColorOverlayBlend(ColorOverlayBlend::SIMPLE_MULTIPLY);
        sprite->SetColorOverlayActive(true);
    }

}


void GuiBarSpriteScaleController::Set(float value, float valueMin, float valueMax)
{

    value = ClampValue(value, valueMin, valueMax);

    float scale = (value - valueMin) / (valueMax-valueMin);

    if(xScale){
        if(AreEqual(scale, sprite->GetScale().x)==false){
            sprite->SetScale(Vec2f(scale, sprite->GetScale().y));
        }
    }

    if(yScale){
        if(AreEqual(scale, sprite->GetScale().y)==false){
            sprite->SetScale(Vec2f(sprite->GetScale().x, scale));
        }
    }

    //----
    if(colors.empty()==false){

        ColorRGBA color;

        for(int i=0; i<int(colors.size()); i++){

            if(i+1 < int(colors.size())){

                if(scale>=colors[i].pos && scale<=colors[i+1].pos){
                    ColorRGBA c1 = colors[i].color;
                    ColorRGBA c2 = colors[i+1].color;
                    float f = (scale-colors[i].pos) / (colors[i+1].pos - colors[i].pos);

                    color.r = c1.r * (1.0f - f) + c2.r * f;
                    color.g = c1.g * (1.0f - f) + c2.g * f;
                    color.b = c1.b * (1.0f - f) + c2.b * f;
                    color.a = c1.a * (1.0f - f) + c2.a * f;

                    break;
                }


            }else{
                color = colors[i].color;

                break;
            }
        }


        if(color != sprite->GetOverlayColorRGBA()){
            sprite->SetColorOverlayRGBA(color);
        }
    }

}


//-------------------------------------------------------------------------------------------------------


void GuiBarSpriteFrameController::ParseFromString(const std::string &_text)
{

    std::vector<std::string>values = SplitString(_text, ",");

    if(values.size()>=1){

        std::string sValue = values[0];

    }

}


void GuiBarSpriteFrameController::Set(float value, float valueMin, float valueMax)
{

    value = ClampValue(value, valueMin, valueMax);

    float ratio = (value - valueMin) / (valueMax-valueMin);

    int nFrames = sprite->GetSourceSprite()->GetGraphicItems().size();
    float frameF = ratio * nFrames;
    int frameIndex = (int)frameF;

    frameIndex = ClampValue(frameIndex, 0, nFrames-1);

    if(frameIndex != static_cast<StandardSprite*>(sprite)->GetActiveImageFrameIndex()){
        static_cast<StandardSprite*>(sprite)->SetActiveImageByFrameIndex(frameIndex);
    }

}



//-------------------------------------------------------------------------------------------------------



GuiBar::GuiBar(Sprite *_sprite)
{

    kind = GuiWidgetKind::BAR;
    sprite = _sprite;

    name = Parameter::GetValue(sprite->GetParameters(), "wName");
    tag = Parameter::GetIntValue(sprite->GetParameters(), "wTag");
    designKind = Parameter::GetValue(sprite->GetSourceSprite()->GetConstantParameters(), "wDesign");


    //--- value range
    std::string valueRange =  Parameter::GetValue(sprite->GetSourceSprite()->GetConstantParameters(), "wValueRange");
    std::vector<std::string>values = SplitString(valueRange, ",");

    if(values.size()>=2){
        valueMin = std::stof(values[0]);
        valueMax = std::stof(values[1]);
        if(valueMin>valueMax){
            std::swap(valueMin, valueMax);
        }
        value = (valueMin+valueMax)/2.0;
    }

    //--- value
    if(Parameter::Exists(sprite->GetParameters(), "wValue")){
        value = std::stof(Parameter::GetValue(sprite->GetParameters(), "wValue"));
    }
    value = ClampValue(value, valueMin, valueMax);


    //--- value label
    if(sprite->GetKind()==SpriteKind::COMPOSED){

        std::vector<Sprite*>sprites;
        CollectSpritesWithOrigConstParameter(static_cast<ComposedSprite*>(sprite), sprites, "wLabel");
        if(sprites.empty()==false && sprites.front()->GetKind()==SpriteKind::TEXT){

            valueLabel = static_cast<TextSprite*>(sprites.front());
            valueLabelPrecision = 0;
            valueLabelSuffix = "";

            std::string sliderValueLabel =  Parameter::GetValue(valueLabel->GetSourceSprite()->GetConstantParameters(), "wLabel");
            if(sliderValueLabel!=""){
                values = SplitString(sliderValueLabel, ",");

                if(values.size()==1){
                    valueLabelPrecision = (int)std::stof(values[0]);

                }else if(values.size()==2){
                    valueLabelPrecision = (int)std::stof(values[0]);
                    valueLabelSuffix = values[1];
                    RemoveStringWhiteSpacesOnStartAndEnd(valueLabelSuffix);
                }
            }
        }
    }


    BuildControllersForSprite(sprite);

}


GuiBar::~GuiBar()
{

    if(barSpriteController) delete barSpriteController;

}


void GuiBar::BuildControllersForSprite(Sprite * _sprite)
{

    if(barSpriteController) return;

    //if(_sprite->GetKind()==SpriteKind::STANDARD){

        _AddControllersForSprite(_sprite);


    //}else
    if(_sprite->GetKind()==SpriteKind::COMPOSED){

        ComposedSprite *cs = static_cast<ComposedSprite*>(_sprite);

        for(Layer *l : cs->GetLayers()){
            if(l->GetKind()==LayerKind::SPRITE){
                SpriteLayer *sl = static_cast<SpriteLayer*>(l);
                for(Sprite *s : sl->GetSprites()){

                    _AddControllersForSprite(s);

                    if(s->GetKind()==SpriteKind::COMPOSED){
                        BuildControllersForSprite(s);
                    }
                }
            }
        }
    }
}


void GuiBar::_AddControllersForSprite(Sprite * _sprite)
{

    if(barSpriteController) return;

    std::vector<Parameter>& cp = _sprite->GetSourceSprite()->GetConstantParameters();


    if(Parameter::Exists(cp, "wScale")){

        barSpriteController = new GuiBarSpriteScaleController(_sprite);
        static_cast<GuiBarSpriteScaleController*>(barSpriteController)->ParseFromString(Parameter::GetValue(cp, "wScale"));

    }else if(Parameter::Exists(cp, "wFrame") && _sprite->GetKind()==SpriteKind::STANDARD){

        barSpriteController = new GuiBarSpriteFrameController(_sprite);
        //static_cast<GuiBarSpriteScaleController*>(barSpriteController)->ParseFromString(Parameter::Value(cp, "scale"));

    }
}


void GuiBar::SetToInitialState()
{
    SetValue(value);
}


void GuiBar::SetValue(float _value)
{

    value = _value;

    if(barSpriteController){
        barSpriteController->Set(value, valueMin, valueMax);
    }
    SetValueLabelText();

}


void GuiBar::SetValueMin(float _valueMin)
{
    valueMin = _valueMin;
    if(barSpriteController){
        barSpriteController->Set(value, valueMin, valueMax);
    }
}


void GuiBar::SetValueMax(float _valueMax)
{
    valueMax = _valueMax;
    if(barSpriteController){
        barSpriteController->Set(value, valueMin, valueMax);
    }
}


void GuiBar::Update()
{
    if(barSpriteController==nullptr) return;

}


/*
void GuiBar::SetValueFromBarSpriteController()
{
    if(barSpriteController){
        value = barSpriteController->GetValue(valueMin, valueMax);
    }
}
*/


void GuiBar::SetValueLabelText()
{

    if(valueLabel==nullptr) return;

    //std::string valueText = std::to_string(value);

    std::ostringstream out;
    out.precision(valueLabelPrecision);
    out << std::fixed << value;
    std::string valueText = out.str();
    //DbgOutput("SetValueText value: " + std::to_string(value));
    DbgOutput("SetValueText value: " + std::to_string(GetIntValue()));


    if(valueLabelSuffix != ""){
        valueText += " " + valueLabelSuffix;
    }

    if(valueText != valueLabel->GetTextString()){
        valueLabel->SetTextString(valueText);
    }


}


}
