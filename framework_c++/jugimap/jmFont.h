#ifndef JUGIMAP_FONT_H
#define JUGIMAP_FONT_H


#include "jmLayers.h"


namespace jugimap {





///\ingroup MapElements
///\brief The Font class defines fonts used in TextSprite objects.
class Font
{
public:

    static std::string textForHeight;
    static std::string pathPrefix;          ///< An optional string attached to stored file paths. It must end with slash '/'!


    ///\brief Constructor
    ///
    /// Creates a new Font object.
    /// \param _relativeFilePath The path to the font file.
    /// \param _size The size of the font.
    /// \param _kind The kind of the font.
    Font(const std::string &_relativeFilePath, int _loadFontSize, FontKind _kind) : relativeFilePath(_relativeFilePath), loadFontSize(_loadFontSize), kind(_kind){}

    Font(const Font &f) = delete;
    Font& operator=(const Font &f) = delete;

    ///\brief Destructor.
    virtual ~Font(){}


    ///\brief Returns the kind of this font.
    FontKind GetKind(){ return kind; }

    ///\brief Returns the file path of this font.
    std::string GetRelativeFilePath(){ return pathPrefix + relativeFilePath; }

    ///\brief Returns the size of this font.
    int GetLoadFontSize(){ return loadFontSize; }

    int GetCommonPixelHeight(){ return pixelHeightCommon; }

    virtual void InitEngineObjects(){}

    virtual int GetPixelHeight(const std::string &s) { return 0; }
    //virtual int GetPixelWidth(int unicode) { return 0; }
    virtual int GetPixelWidth(const std::string &s) { return 0; }
    virtual Vec2f GetPixelSize(const std::string &s) { return Vec2f(); }
    //void SetText(Text *_text){ text = _text;}




protected:
    FontKind kind = FontKind::TRUE_TYPE;
    std::string relativeFilePath;
    int loadFontSize = -1;

    int pixelHeightCommon = 0;


    struct CharWidth
    {
        unsigned short width = 0;
        bool set = false;

    };

    std::vector<CharWidth>charPixelWidth;          // table for unicode characters
};



///\ingroup MapElements
/// \brief The FontsLibrary class defines storage for Font objects.
class FontsLibrary
{
public:

    ~FontsLibrary();
    void DeleteData();


    /// \brief Add the given font *_font* with identification name *_name* to this library. The function returns the index of the added font in the library.
    int Add(Font* _font, const std::string &_name){ fonts.push_back({_name, _font}); return fonts.size()-1; }


    /// \brief Find and returns a font with the given *_name*.
    ///
    ///  If the font is not found the function returns the first font in the library, or nullptr if library is empty.
    Font* Find(const std::string &_name);


    /// \brief Returns a font at the given *_index*.
    ///
    ///   If the index is out of storage bounds the function returns the first font in the library, or nullptr if library is empty.
    Font* At(int _index);


    /// \brief Returns the number of stored fonts.
    int GetSize(){ return fonts.size(); }

private:

    std::vector<std::pair<std::string, Font*>> fonts;

};


/// \brief A global library for fonts.
extern FontsLibrary fontLibrary;


}


#endif
