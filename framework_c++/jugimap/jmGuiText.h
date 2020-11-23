#ifndef JUGIMAP_GUI_TEXT_H
#define JUGIMAP_GUI_TEXT_H

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>
#include <array>
#include <string>
#include "../utf8cpp/utf8.h"


#include "jmGuiCommon.h"
#include "jmFont.h"
#include "jmCommon.h"


namespace jugimap{
	


class TextLibrary;
class TextBook;
class TextSequence;
class TextSprite;
class ComposedSprite;
class StandardSprite;



class UnicodeString
{
public:

    //std::string s;

    UnicodeString(){}
    UnicodeString(const std::string &_s);

    void Set(const std::string &_s);
    void Append(const std::string &_s);
    void Append(const UnicodeString &_us);
    void Append(int _code);
    void Clear();
    bool IsEmpty(){ return codes.empty(); }
    int GetLength(){ return codes.size(); }

    std::string ToStdString();
    std::string ToStdString(int _startIndex, int _endIndex=-1);


    std::vector<int> & GetCodes(){ return codes; }

private:
    std::vector<int>codes;

};


void VerifyAndFixUtf8string(std::string& str);


std::string GetSubstring(const std::string &text, int indexStart, int indexEnd, bool &endReached);


void GetStringUnicodes(const std::string &text, std::vector<int> &unicodes);


int GetStringNumberOfCharacters(const std::string &text);



class TextSegmentWord
{
public:
    std::string word;
    int nCharacters = 0;

    float width = -1;                       // width in pixels
    float height = -1;

    //std::string replacementKeyword;
    std::string originalWord;
    ColorRGBA color{255,255,255};
    Font * font = nullptr;                  // LINK
    float fFontHeight = 1.0;

    std::string button;
    //ColorRGBA buttonColor{255,255,255};
    ColorRGBA buttonColor_CO{200,0,0};
    //int offset = 0;


    //int horSpacing = 0;
    //int verSpacing = 0;




    enum Content{
        TEXT,
        //HOR_OFFSET,
        OFFSET,
        VER_SPACING,
        INDENT,
        TAB_SPACING
    };

    Content content = TEXT;


    void UpdateSize(bool _forceUpdate = false);

};



///\ingroup Gui
///\brief The TextSegment class defines a segment of text which can be accessed and displayed separatelly from other texts.
///
/// Text segments are usually created by parsing texts from text files via special markup codes.
/// In addition to text data, a text segment also stores styling information and custom parameters.
/// Text segments are displayed on screen with GuiTextField widgets.
class TextSegment
{
public:
    friend class TextLibrary;
    friend class TextBook;
    friend class TextSequence;


    TextSegment(int _id) : id(_id)
    {
        currentColor = defaultColor;                    // index 0
        currentFont = defaultFont;                          // index 0
        currentIndent = defaultIndent;
        currentTabSpacing = defaultTabSpacing;

    }

    ~TextSegment();


    /// \brief Returns the identification number of this text segment.
    int GetId(){ return id; }


    ///\brief Returns a reference to the vector of stored custom parameter.
    std::vector<Parameter>& GetParameters() { return parameters; }


    ///\brief Replace the given word *_searchedWord* with the *newWord*.
    ///
    /// **Important!** The *_searchedWord* is allways the original word stored in the text segment at creation.
    /// When you call replace more times for the same original word, use the original word as *_searchedWord* every time!
    ///
    /// If the searched word was found and replaced the function returns true, otherwise it returns false.
    bool Replace(const std::string &_searchedWord, const std::string &newWord);


    ///\brief Returns true if this text segment contain the given *_word*; otherwise it returns false.
    ///
    /// The flag *compareWithOriginalWord* decide if comparing is done with original words or current words. A current word can be
    /// different then original if TextSegment::Replace has been used.
    bool Contains(const std::string &_word, bool compareWithOriginalWord = false);


    void AddParagraph(const std::string &_text, ColorsLibrary &_colors, FontsLibrary &_fonts);


    std::vector<TextSegmentWord*> &GetWords(){ return words; }
    void SetWordSizesIfNeeded();


private:
    int id = 0;
    std::vector<TextSegmentWord*> words;
    std::vector<Parameter> parameters;



static ColorRGBA defaultColor;
static Font *defaultFont;
static ColorRGBA currentColor;
static Font *currentFont;
static std::string currentButton;
static ColorRGBA currentButtonColor;
static ColorRGBA currentButtonColor_CO;
//static int currentOffset;
static int defaultIndent;
static int defaultTabSpacing;
static int currentIndent;
static int currentTabSpacing;


    void ProcessAddingWord(TextSegmentWord *word, ColorsLibrary &colors, FontsLibrary &fonts);

};


///\ingroup Gui
///\brief The TextSequence class defines a storage for text segments.
///
/// Text sequences are used for grouping related text segments. Text sequence are usually defined in text files
/// and created together with text segments. A text sequence can also have stored custom parameters.
///
/// A TextSequence object takes ownership of TextSequence objects added to it.
class TextSequence
{
public:

    TextSequence(){}
    TextSequence(int _id) : id(_id){}
    ~TextSequence();


    /// \brief Returns the identification number of this text segment.
    int GetId(){ return id; }


    ///\brief Returns a reference to the vector of stored text segments.
    std::vector<TextSegment*>& GetTextSegments(){ return TextSegments; }


    ///\brief Returns a reference to the vector of stored custom parameters.
    std::vector<Parameter>& GetParameters() { return parameters; }


private:
    int id = 0;
    std::vector<TextSegment*>TextSegments;          // OWNED pointers
    std::vector<Parameter> parameters;

};


//=======================================================================================


///\ingroup Gui
///\brief The TextBook class defines a storage for text segments and text sequences.
///
/// A TextBook object takes ownership of TextSegment and TextSequence objects added to it.
class TextBook
{
public:

    /// \brief Constructor.
    ///
    /// Create a text book with the given *_name*.
    TextBook(const std::string &_name) : name(_name){}

    TextBook(const TextBook &_src) = delete;
    TextBook(const TextBook &&_src) = delete;
    TextBook& operator=(const TextBook &_src) = delete;


    /// \brief Destructor.
    ~TextBook();


    /// \brief Returns the name of this text book.
    std::string GetName(){ return name; }


    /// \brief Load text segments and text sequences from a text file.
    ///
    /// The text file must be written using *jugimap text markup codes* which allow parsing of the text.
    /// \param _filename The file path of the text file.
    /// \param _colors A reference to a *colors library* which is used during text parsing for obtaining colors.
    /// \param _fonts A reference to a *fonts library* which is used during text parsing for obtaining fonts.
    void LoadContent(const std::string &_filePath, ColorsLibrary &_colors, FontsLibrary &_fonts);


    /// \brief Add the given *_textSegment* to this text book.
    ///
    /// This function is called during the loading and should usually not be called manually.
    bool AddTextSegment(TextSegment *_textSegment);


    /// \brief Add the given *_textSequence* to this text book.
    ///
    /// This function is called during the loading and should usually not be called manually.
    bool AddTextSequence(TextSequence *_textSequence);


    ///\brief Returns a reference to the vector of stored text segments.
    std::vector<TextSegment*>& GetTextSegments(){ return TextSegments; }


    ///\brief Returns a reference to the vector of stored text sequences.
    std::vector<TextSequence*>& GetTextSequences(){ return TextSequences; }



private:
    std::string name;
    std::vector<TextSegment*>TextSegments;              // OWNED pointers
    std::vector<TextSequence*>TextSequences;            // OWNED pointers


    void _ParseParametersIfDefined(std::string &text, std::vector<Parameter>&_parameters);

};



//========================================================================================================================


///\ingroup Gui
///\brief The TextLibrary class defines a storage for text books.
///
/// A TextLibrary object takes ownership of TextBook objects added to it.
class TextLibrary
{
public:
    static std::string path;
    static std::string pathPrefix;


    ~TextLibrary();


    /// \brief Create a new text book with the given *_bookName* and returns a pointer link.
    ///
    /// If a text book with the given name allready exists in this library no book will be created
    /// and the function will return a pointer to the existing text book.
    TextBook* CreateNewBook(const std::string  &_bookName);


    /// \brief Returns a pointer link to the text book with the given *_bookName*.
    ///
    /// If the book is not found the function returns nullptr.
    TextBook *FindBook(const std::string &_bookName);


    /// \brief Returns a pointer link to the text segment within a book.
    ///
    /// \param _bookName The name of the text book.
    /// \param _segmentID The id number of the text segment.
    ///
    /// If the text book is not found or the text segment id is outside the storage vector bounds, the function returns nullptr.
    TextSegment *GetTextSegmentFromBook(const std::string &_bookName, int _segmentID);


private:

    std::vector<TextBook*>books;

};


/// \brief The global text library for storing text books.
extern TextLibrary textLibrary;



	
}




#endif
