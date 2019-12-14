#ifndef JUGIMAP_TEXT_H
#define JUGIMAP_TEXT_H


#include "jmLayers.h"


namespace jugimap {



///\ingroup MapElements
///\brief A font elements.
///
/// The Font class defines fonts for the Text objects.
class Font
{
public:

    static std::string pathPrefix;          ///< An optional string attached to stored file paths. It must end with slash '/'!


    ///\brief Constructor
    ///
    /// Creates a new Font object.
    /// \param _relativeFilePath The path to the font file.
    /// \param _size The size of the font.
    /// \param _kind The kind of the font.
    Font(const std::string &_relativeFilePath, int _size, FontKind _kind) : relativeFilePath(_relativeFilePath), size(_size), kind(_kind){}

    Font(const Font &f) = delete;
    Font& operator=(const Font &f) = delete;

    ///\brief Destructor.
    virtual ~Font(){}


    ///\brief Returns the kind of this font.
    FontKind GetKind(){ return kind; }

    ///\brief Returns the file path of this font.
    std::string GetRelativeFilePath(){ return pathPrefix + relativeFilePath; }

    ///\brief Returns the size of this font.
    int GetSize(){ return size; }

private:
    FontKind kind = FontKind::TRUE_TYPE;
    std::string relativeFilePath;
    int size;
};


//==================================================================================================

///class TextLayer;


//==================================================================================================

class Text;


///\ingroup MapElements
///\brief A layer for displaying texts.
///
/// The TextLayer class provides a way of using texts as part of map structure.
/// It stores and owns Text objects. Texts are automatically added to text layers when they are constructed.
///
class TextLayer : public Layer
{
public:

    TextLayer(const std::string &_name);
    virtual ~TextLayer();


    virtual void InitEngineLayer() override;

    virtual void UpdateEngineLayer() override;

    virtual void DrawEngineLayer() override;


    ///\brief Remove and delete the given text *_text* from this text layer.
    ///
    /// If the text belong to other layer it will not be deleted.
    virtual void RemoveAndDeleteText(Text * _text);


    ///\brief Returns a reference to the vector of stored texts in this text layer.
    std::vector<Text*>& GetTexts(){return texts;}


    /// \brief Set a flag which indicates if Text::UpdateEngineText should be called for stored texts.
    ///
    /// Set to *true* if a stored text has been changed and its engine text needs to be updated.
    /// Set to *false* after engine texts has been updated.
    /// This function should usually not be used manually as it is called by functions which change text properties
    /// and by TextLayer::UpdateEngineLayer.
    ///
    /// \see IsEngineLayerUpdateRequired
    void RequireEngineLayerUpdate(bool _engineLayerUpdateRequired){ engineLayerUpdateRequired = _engineLayerUpdateRequired;}


    /// \brief Returns 'true' if this text layer has texts which require their engine texts to be updated; otherwise returns false.
    ///
    /// \see RequireEngineLayerUpdate
    bool IsEngineLayerUpdateRequired(){ return engineLayerUpdateRequired;}


private:
    std::vector<Text*>texts;                  // OWNED

    bool engineLayerUpdateRequired = false;


};


//==================================================================================================


///\ingroup MapElements
///\brief A text element.
///
/// The Text class defines texts which can be used in the TextLayer objects.
class Text
{
public:

    /// The Property enumerator provides flags for sprite properties.
    enum Property
    {
        NONE =      0,
        TEXT_STRING =  1 << 0,
        POSITION =     1 << 1,
        COLOR =        1 << 2,
        ALPHA =        1 << 3,

        ALL = TEXT_STRING | POSITION | COLOR | ALPHA

    };



    ///\brief Constructor
    ///
    /// Creates a new Text object.
    /// \param _textLayer The text layer where the text object is stored.
    /// \param _font The font used for the text. The text does **not** take over the ownership of the font!
    /// \param _textString The text string.
    /// \param _position The position of the text.
    /// \param _color The color of the text.
    Text(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color);
    Text(const Text &t) = delete;
    Text& operator=(const Text &t) = delete;

    ///\brief Destructor.
    ///
    /// **Important:** Do not delete Text objects directly. Use TextLayer::RemoveAndDeleteText instead!
    virtual ~Text();


    virtual void InitEngineText(){}

    virtual void UpdateEngineText(){}

    virtual void DrawEngineText(){}


    ///\brief Returns true if the engine text of this text has been initialized; otherwise returns false.
    ///
    /// \see InitEngineText
    virtual bool IsEngineTextInitialized() {return false;}


    //------


    ///\brief Set the visibility of this text.
    ///
    /// \see IsVisible
    virtual void SetVisible(bool _visible){ visible = _visible; }


    ///\brief Returns *true* if this text is visible; otherwise returns false.
    ///
    /// \see IsVisible
    bool IsVisible(){ return visible; }


    //------

    ///\brief Set a boolean flag which indicates if this text's engine text is used directly.
    ///
    /// By default a text is manipulated via Text interface and its engine text is updated via UpdateEngineText function.
    /// If you wish to manipulate the engine text directly you should set this flag to true in which case the engine text will no longer get updated via UpdateEngineText.
    /// \see IsEngineTextUsedDirectly
    void SetEngineTextUsedDirectly(bool _engineTextUsedDirectly){ engineTextUsedDirectly = _engineTextUsedDirectly; }


    ///\brief Returns *true* if this text's engine text is used directly.
    ///
    /// \see SetEngineTextUsedDirectly
    bool IsEngineTextUsedDirectly(){ return engineTextUsedDirectly; }


    //-----


    /// Returns the text layer where this text is stored.
    TextLayer* GetLayer(){ return textLayer;}


    /// Returns the font of this text.
    Font* GetFont(){ return font; }


    ///\brief Set the tag of this text to the given _tag.
    ///
    /// A tag is an arbitrary integer number which can be assigned to a Text object.
    /// \see GetTag
    void SetTag(int _tag){ tag = _tag; }


    ///\brief Returns the tag of this text.
    ///
    /// \see SetTag
    int GetTag(){ return tag;}



    ///\brief Set the link object of this text to the given _linkObject.
    ///
    /// A link object is an arbitrary object which can be assigned to a text.
    /// A text does **not** take ownership over the link object.
    /// \see GetLinkObject
    void SetLinkObject(void* _linkObject){ linkObject = _linkObject; }


    ///\brief Returns the link object of this text.
    ///
    /// \see SetLinkObject
    void* GetLinkObject(){ return linkObject; }


    //-----


    ///\brief Set the string of this text to the given _textString.
    ///
    /// \see GetTextString
    void SetTextString(const std::string &_textString)
    {
        textString = _textString;
        changeFlags |= Property::TEXT_STRING;
        textLayer->RequireEngineLayerUpdate(true);
    }


    ///\brief Returns the string of this text.
    ///
    /// \see SetTextString
    std::string GetTextString(){ return textString; }



    ///\brief Set the position of this text to the given _position.
    ///
    /// \see GetPosition
    void SetPosition(Vec2f _position)
    {
        position = _position;
        changeFlags |= Property::POSITION;
        textLayer->RequireEngineLayerUpdate(true);
    }

    ///\brief Returns the position of this text.
    ///
    /// For texts in map layers this position matches the global position.
    /// For texts in layers of a composed sprite this position is relative to the composed sprite coordinate system.
    ///
    /// \see SetPosition
    Vec2f GetPosition(){ return position; }



    ///\brief Returns the global position of this text.
    ///
    /// For texts in map layers this position matches the position.
    /// For texts in composed sprite layers this is the position converted to global map coordinates.
    ///
    /// \see GetPosition, SetPosition
    Vec2f GetGlobalPosition();



    ///\brief Set the color of this text to the given _color.
    ///
    /// Only red, green and blue componenet will be used for the text color. Alpha component is set with *SetAlpha*.
    /// \see GetColor, SetAlpha
    void SetColor(ColorRGBA _color)
    {
        color = _color;
        changeFlags |= Property::COLOR;
        textLayer->RequireEngineLayerUpdate(true);
    }


    ///\brief Returns the color of this text.
    ///
    /// \see SetColor
    ColorRGBA GetColor(){ return color; }



    ///\brief Set the alpha of this text to the given _alpha.
    ///
    /// The alpha factor is opacity in the range of [0.0 - 1.0].
    /// \see GetAlpha
    void SetAlpha(float _alpha)
    {
        alpha = _alpha;
        changeFlags |= Property::ALPHA;
        textLayer->RequireEngineLayerUpdate(true);
    }


    ///\brief Returns the alpha factor of this text.
    ///
    /// \see SetAlpha
    float GetAlpha(){ return alpha; }



    ///\brief Returns the height of this text.
    ///
    /// This function require initialized engine text.
    /// \see GetWidth
    float GetHeight()
    {
        if(IsEngineTextInitialized()==false) return 0.0;

        if(changeFlags & Property::TEXT_STRING){
            UpdateEngineText();
        }
        return size.y;
    }


    ///\brief Returns the width of this text.
    ///
    /// This function require initialized engine text.
    /// \see GetHeight
    float GetWidth()
    {
        if(IsEngineTextInitialized()==false) return 0.0;

        if(changeFlags & Property::TEXT_STRING){
            UpdateEngineText();
        }
        return size.x;
    }



    //-------- CHANGE FLAGS


    ///\brief Set the change flags of this text to the given *_changeFlags*.
    ///
    /// The *_changeFlags* must be a value or a bitwise combination of values defined in the Property enumerator.
    /// \see AppendToChangeFlags, GetChangeFlags
    void SetChangeFlags(int _changeFlags);


    ///\brief Append the given *_changeFlags* to the change flags of this text with a biwise operation.
    /// ///
    /// The *_changeFlags* must be a value or a bitwise combination of values defined in the Property enumerator.
    /// \see SetChangeFlags, GetChangeFlags
    void AppendToChangeFlags(int _changeFlags);


    ///\brief Returns the *change flags* of this text.
    ///
    /// \see SetChangeFlags, AppendToChangeFlags
    int GetChangeFlags() {return changeFlags;}


    void _SetSize(Vec2f _size){ size = _size; }


private:
    TextLayer* textLayer = nullptr;             // LINK
    Font* font = nullptr;                       // LINK
    int tag = 0;
    void* linkObject = nullptr;                 // LINK

    bool visible = true;
    bool engineTextUsedDirectly = false;

    std::string textString = "textString";
    Vec2f position;

    float alpha = 1.0;
    ColorRGBA color;
    Vec2f size;

    int changeFlags = 0;


};




}


#endif
