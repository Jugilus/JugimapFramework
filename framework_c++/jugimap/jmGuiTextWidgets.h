#ifndef JUGIMAP_GUI_TEXT_WIDGETS_H
#define JUGIMAP_GUI_TEXT_WIDGETS_H

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>
#include <array>
#include <string>
#include "../utf8cpp/utf8.h"


#include "jmGuiCommon.h"
#include "jmGuiText.h"
#include "jmFont.h"
#include "jmCommon.h"


namespace jugimap{
	


class TextLibrary;
class TextSprite;
class ComposedSprite;
class StandardSprite;
class TextSegment;
class TextSegmentWord;
class GuiSlider;
class TextFieldLineSubtext;
class TextFieldLine;



/// \addtogroup Gui
/// @{


///\brief The GuiTextField class provides a widget for displaying text.
class GuiTextField : public GuiWidget
{
public:

    ///\brief Global factor for vertical lines spacing.
    ///
    /// This factor provides additional spacing between lines. The spacing is obtained by multiplying this factor with the text line height.
    /// A negative value will cause negative (reduced) spacing.
    static float globalLinesSpacingFactor;

    ///\brief The text alignment.
    enum class Alignment
    {

        ///\brief Text lines are aligned to the left edge.
        LEFT,

        /// Extend text lines from the left to the right edge by increasing the gap between words.
        /// The gap has a maximum value which means that lines may not reach the right edge if the gap would become too big!
        JUSTIFY
    };

    ///\brief The drawing delay of displayed text.
    enum class DrawingDelay
    {
        NONE,               ///< Text is displayed instantly.
        BY_ROWS,            ///< Text is displayed gradually by rows.
        BY_WORDS,           ///< Text is displayed gradually by words.
        BY_CHARACTERS       ///< Text is displayed gradually by characters.
    };


    /// The scrolling mode of displayed text.
    /// This parameter allow different ways to display text segments which does not fit vertically into the text field.
    enum class ScrollingMode
    {
        /// Scrolling range is determined from the text height and the height of the text field.
        /// This is a standard mode used with the scrolling slider and is suitable for general texts.
        NORMAL,

        /// Scrolling range is a fixed number of lines.
        /// This ensure that on every scroll command unique lines get displayed. In practice this mode works better with texts like
        /// dialogs where repeated lines would be distracting.
        /// This mode does not work with the scrolling slider but require external calls to GuiTextField::ScrollTextUp or GuiTextField::ScrollTextDown. It is also
        /// not suitable for texts where different fonts with too much difference in size are used.
        FIXED_STEP
    };



    ///\brief Construct a new GuiTextField from the given *_vectorShape* in the *_map*.
    ///
    /// The widget properties are obtained from the vector shape custom parameters.
    GuiTextField(VectorShape * _vectorShape, Map* _map);

    ~GuiTextField();


    void Update() override;
    void SetDisabled(bool _disabled) override;
    void SetVisible(bool _visible) override;



    /// \brief Returns the text segment if exists; otherwise returns nullptr.
    TextSegment* GetTextSegment(){ return textSegment;}


    /// \brief Returns the height of the given *_textSegment* in this text field.
    ///
    /// This function can be used if you want to manually set the height of the text field to fit a particular text segment.
    /// Calling this function will reset the text field so use it accordingly.
    int GetTextSegmentHeight(TextSegment *_textSegment);


    /// \brief Returns the name of the clicked button if such event happens; otherwise returns an empty string.
    ///
    /// This a querry function for text segments with defined buttons.
    std::string GetClickedButtonName(){ return clickedTextName; }



    ///\brief Set the scrolling mode.
    void SetScrollingMode(ScrollingMode _scrollingMode);


    ///\brief Returns the scrolling mode of this text field.
    ScrollingMode GetScrollingMode(){ return scrollingMode; }


    ///\brief Set the scrolling properties if scrolling mode is ScrollingMode::FIXED_STEP.
    ///
    /// \param _numberOfLines Number of scrolled lines.
    /// \param _lineHeight The height of the lines.
    /// \param _scrollDurationMS Duration of scrolling in milliseconds.
    void SetScrollingModeFixedStepProperties(int _numberOfLines, int _lineHeight, float _scrollDurationMS);


    ///\brief Set the drawing delay to the given *_drawingDelay* and duration to *_drawingdelayMS* (in milliseconds).
    void SetDrawingDelay(DrawingDelay _drawingDelay, int _drawingdelayMS);


    ///\brief Returns the drawing delay of this text field.
    DrawingDelay GetDrawingDelay(){ return drawingDelay; }


    ///\brief Set the text alignment to the given *_textAlignment*.
    void SetTextAlignment(Alignment _textAlignment);


    ///\brief Returns the text alignment of this text field.
    Alignment GetTextAlignment(){ return alignment;}


    ///\brief Set to true if alpha fade at margin between box and visible box is used.
    void SetAlphaFadeAtMargin(bool _alphaFadeAtMargin){ alphaFadeAtMargin = _alphaFadeAtMargin;}


    ///\brief Set the text area rectangle.
    ///
    /// **Important!** The border Y-coordinates for text positioning are the minimum and maximum Y-coordinate of the text box.
    /// The used text sprites have their handle Y-coordinate in the middle. This means that half of their height reach out of the
    /// text area. Take this into account when designing the text field rectangle size.
    ///
    /// A call to this function resets the text field.
    void SetTextRect(Recti _box);


    ///\brief Returns the text area rectangle.
    Recti GetTextRect(){ return box; }


    ///\brief Set the visible area rectangle.
    ///
    /// The visible area can be bigger (only height matters) then the text area in which case the text
    /// can alpha fade in and out at margins. Use function GuiTextField::SetAlphaFadeAtMargin to enable or disable alpha fade.
    /// By default the visible area has the same size as the text area.
    ///
    /// A call to this function resets the text field.
    void SetVisibleRect(Recti _visibleBox);


    ///\brief Returns the visible area rectangle.
    Recti GetVisibleRect(){ return visibleBox; }


    ///\brief Set the clickable area rectangle.
    ///
    /// This is the area where the cursor interaction get registered to this text field. By default the clickable area has the same size as the text area.
    void SetClickableRect(Recti _clickableBox);


    ///\brief Returns the clickable area rectangle.
    Recti GetClickableRect(){ return clickableBox; }


    ///\brief Set the local factor for vertical lines spacing.
    ///
    /// This factor is added to the global factor GuiTextField::globalLinesSpacingFactor. It provides additional spacing between lines.
    /// The spacing is obtained by multiplying this factor with the text line height.
    /// A negative value will cause negative (reduced) spacing.
    void SetLocalLinesSpacingFactor(float _spacingFactor){ localLinesSpacingFactor = _spacingFactor; }


    ///\brief Start displaying the given *_textSegment*.
    ///
    /// A call to this function resets the text field.
    void Start(TextSegment *_textSegment);


    ///\brief Reset the text field.
    ///
    /// Calling this function is usually not needed as other function call it if required.
    void Reset();


    ///\brief Returns true if a text segment is currently being drawn by the drawing delay; otherwise returns false.
    bool IsDelayedDrawingActive(){ return !delayedDrawingFinished; }


    ///\brief Skip drawing delay of a text segment and show entire text immediately.
    void SkipDelay();


    ///\brief Scroll the segment text up and returns true. If scrolling is not possible returns false.
    ///
    /// This function scroll the text segement that does not fit into the text area.
    /// The scrolling up is possible until the last line of text is displayed.
    bool ScrollTextUp();


    ///\brief Scroll the segment text down and return true. If scrolling is not possible returns false.
    ///
    /// This function scroll the text segement that does not fit into the text area.
    /// The scrolling down is possible until the first line of text is displayed.
    bool ScrollTextDown();


    float GetRelativeScrollPosition(){ return relScrollPosition; }


    ///\brief Returns true if the first line of the text segement is displayed; otherwise returns false.
    bool IsTextSegmentFirstLineDisplayed();


    ///\brief Returns true if the last line of the text segement is displayed; otherwise returns false.
    bool IsTextSegmentLastLineDisplayed();




private:
    // LINK POINTERS
    SpriteLayer *layer = nullptr;                           // LINK to textLayer where text object are placed
    TextSegment * textSegment = nullptr;                    // LINK to displayed text segment

    // OWNED DATA POINTERS
    GuiSlider * slider = nullptr;                           // OWNED
    std::vector<TextFieldLine*>Lines;                       // OWNED

    //---status flags
    std::string clickedTextName;

    //---
    std::vector<float>gapWords;                         // extra gap between words when align is set to JUSTIFY
    float gapWordsMax = 5.0;                            // max extra gap
    float localLinesSpacingFactor = 0.0;

    ScrollingMode scrollingMode = ScrollingMode::NORMAL;
    Alignment alignment = Alignment::LEFT;
    DrawingDelay drawingDelay = DrawingDelay::NONE;

    int drawingDelayMS = 0;
    int drawingDelayStartMS = 0;

    Recti box;
    Recti visibleBox;
    Recti clickableBox;
    int segmentHeight = 0;
    int scrollingRange = 0;
    float relScrollPosition = 0.0;
    float relScrollStep = 0.25;

    bool delayedDrawingFinished = false;
    bool alphaFadeAtMargin = true;
    Tween scrollTween;

    std::vector<TextFieldLineSubtext*>clickableSubtexts;        // LINKS

    // properties for FIXED STEP scrolling mode
    int fsNumberOfLines = 5;
    int fsLineHeight = 30;
    int fsScrollDurationMS = 250;
    int indexFirstLineInBox = -1;


    void RebuildLines(bool _rebuildEngineTexts=true);
    void RebuildGaps();

    void Update_DelayByRow();
    void Update_DelayByWord();
    void Update_DelayByCharacters();

    void UpdateVisibleRange(float offset);

    std::string GetClickedText();


};

//-----------------------------------------------------------------------------------------------------------------------

class TextFieldLineSubtext
{
private:
    friend class GuiTextField;
    friend class TextFieldLine;

    std::string text;
    float xPos = 0;
    ColorRGBA color;
    Font* font = nullptr;                       // LINK
    TextSprite * textSprite = nullptr;          // LINK
    int charCount = -1;                         // Used for drawing delay BY_LETTERS !
    bool visibleViaDrawingDelay = true;
    //bool inVisibleRange = true;

    ColorRGBA color_CO;                         // used for clickable texts
    std::string button;
};

//-----------------------------------------------------------------------------------------------------------------------

class TextFieldLine
{
private:
    friend class GuiTextField;

    std::vector<TextFieldLineSubtext>subtexts;
    float height = 0;                           // pixel height
    int yPos = 0;
    bool inVisibleBox = false;
    bool inBox = false;


    ~TextFieldLine();

    void Add(const TextSegmentWord &_word, float textPos);
    void AddAsSeparateWords(const TextSegmentWord &_word, float textPos);
    void UpdateEngineText(int _subtextIndex, SpriteLayer *_layer, Font *_font, int _xBox, int _yPos);

};


//========================================================================================================================


///\brief The GuiTextField class provides a widget for text input.
///
/// The text editing is started with GuiTextInput::Start and completed by pressing the Enter key.
/// This widget currently works only with the ASCII character set.
class GuiTextInput : public GuiWidget
{
public:


    ///\brief Construct a new gui text input from the given *_rootSprite*.
    GuiTextInput(ComposedSprite* _rootSprite);

    //~GuiTextInput();

    void SetDisabled(bool _disabled) override;
    void SetVisible(bool _visible) override;
    void Update() override;



    ///\brief Start the text editing.
    void Start();

    ///\brief Set the text and start editing.
    void Start(const std::string &_text);


    ///\brief Set the text.
    void SetText(const std::string &_text);

    ///\brief Returns the text.
    std::string GetText(){ return text; }


    ///\brief Set the maximum number of characters of the text.
    void SetTextMaximumLength(int _maxLength){ maxLength = _maxLength; }


    ///\brief Set if the escape key aborts editing.
    void SetAllowEscToAbort(bool _allowEscToAbort){ allowEscToAbort = _allowEscToAbort; }


    ///\brief Set if empty text is allowed.
    void SetAllowEmptyText(bool _allowEmptyText){ allowEmptyText = _allowEmptyText; }


    ///\brief Set a string of characters which can not appear in the text.
    void SetBannedChars(const std::string &_bannedChars){ bannedChars = _bannedChars; }


    ///\brief Set a string of banned characters in file name.
    void MakeCharsFileNameSafe();


    ///\brief Set the flashing interval of the text cursor in milliseconds.
    void SetCursorFlashingSpeed(int _cursorFlashingMS);


    void Finish();


private:
    ComposedSprite* rootSprite = nullptr;           // LINK
    StandardSprite* inputBoxSprite = nullptr;      // LINK
    TextSprite *textSprite = nullptr;           // LINK
    TextSprite *cursorSprite = nullptr;         // LINK


    std::string text;
    std::string defaultText;

    int maxLength = 12;
    char currentChar = 0;
    bool active = false;
    ColorRGBA cursorColor{0,255,255};

    int cursorFlashingMS = 250;
    int cursorFlashingStartMS = 0;

    bool allowEscToAbort = false;
    bool allowEmptyText = true;
    std::string bannedChars;

    std::vector<char>allowedAsciCodes;


    // visual appearance for widget states
    int indexInputBoxSprite_idle = 0;
    int indexInputBoxSprite_CO = 0;
    int indexInputBoxSprite_active = 0;
    int indexInputBoxSprite_disabled = 0;

    ColorRGBA textColor_idle{255,255,255};
    //ColorRGBA textColor_CO{255,255,255};
    ColorRGBA textColor_active{255,255,255};
    //ColorRGBA textColor_disabled{255,255,255};


    void UpdateTextOnChange();


};


/// @}


	
}




#endif
