#include <assert.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include "jmCommonFunctions.h"
#include "jmInput.h"
#include "jmStreams.h"
#include "jmObjectFactory.h"
#include "jmFont.h"
#include "jmUtilities.h"
#include "jmVectorShapes.h"
#include "jmSprites.h"
#include "jmMap.h"
#include "jmGuiWidgetsA.h"
#include "jmGuiText.h"


namespace jugimap{





UnicodeString::UnicodeString(const std::string &_s)
{
    Set(_s);
}


void UnicodeString::Set(const std::string &_s)
{

    codes.clear();
    Append(_s);

}


void UnicodeString::Append(const std::string &_s)
{

    int count = 0;
    char *i = (char*)_s.data();
    char *iEnd = (char*)_s.data() + _s.size();
    while(i<iEnd){
        int unicode = utf8::next(i, iEnd);
        codes.push_back(unicode);
        //qDebug()<<hex<<count<<" : "<< unicode;
        count++;
    }
}


void UnicodeString::Append(const UnicodeString &_us)
{

    for(int c : _us.codes){
        codes.push_back(c);
    }
}


void UnicodeString::Append(int _code)
{
    codes.push_back(_code);
}


void UnicodeString::Clear()
{
    codes.clear();
}


std::string UnicodeString::ToStdString()
{

    std::string s;

    for(int c : codes){
        utf8::append(c, s);
    }

    return s;
}


std::string UnicodeString::ToStdString(int _startIndex, int _endIndex)
{

    std::string s;

    if(codes.empty()) return s;
    if(_startIndex>=codes.size()) return s;

    if(_startIndex < 0) _startIndex = 0;

    if(_endIndex==-1 || _endIndex>=codes.size()){
        _endIndex = codes.size()-1;
    }
    if(_endIndex<_startIndex) _endIndex = _startIndex;


    for(int i=_startIndex; i<=_endIndex; i++){
        utf8::append(codes[i], s);
    }

    return s;
}




void VerifyAndFixUtf8string(std::string& str)
{
    std::string temp;
    utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
    str = temp;
}


std::string GetSubstring(const std::string &text, int indexStart, int indexEnd, bool &endReached)
{

    std::string s;
    endReached = true;

    int count = 0;
    char *i = (char*)text.data();
    char *iEnd = (char*)text.data() + text.size();


    while(i<iEnd){
        int unicode = utf8::next(i, iEnd);

        if(count<indexStart){
            // empty;

        }else if(count>indexEnd){
            endReached = false;
            break;

        }else{
            utf8::append(unicode, s);
        }

        //qDebug()<<hex<<count<<" : "<< unicode;
        count++;
    }

    return s;
}


void GetStringUnicodes(const std::string &text, std::vector<int>&unicodes)
{

    int count = 0;
    char *i = (char*)text.data();
    char *iEnd = (char*)text.data() + text.size();
    while(i<iEnd){
        int unicode = utf8::next(i, iEnd);
        unicodes.push_back(unicode);
        //qDebug()<<hex<<count<<" : "<< unicode;
        count++;
    }

}


int GetStringNumberOfCharacters(const std::string &text)
{

    int count = 0;
    char *i = (char*)text.data();
    char *iEnd = (char*)text.data() + text.size();
    while(i<iEnd){
        int unicode = utf8::next(i, iEnd);
        //qDebug()<<hex<<count<<" : "<< unicode;
        count++;
    }

    return count;

}


//========================================================================================


TextBook::~TextBook()
{
    for(TextSegment *ts : TextSegments){
        if(ts){
            delete ts;
        }
    }
}


bool TextBook::AddTextSegment(TextSegment *_textSegment)
{

    if(_textSegment->GetId() >= TextSegments.size()){
        TextSegments.resize(_textSegment->GetId()+1, nullptr);
    }

    if(TextSegments[_textSegment->GetId()]==nullptr){
        TextSegments[_textSegment->GetId()] = _textSegment;
        return true;
    }

    return false;
}


bool TextBook::AddTextSequence(TextSequence *_textSequence)
{

    if(_textSequence->GetId() >= TextSequences.size()){
        TextSequences.resize(_textSequence->GetId()+1, nullptr);
    }

    if(TextSequences[_textSequence->GetId()]==nullptr){
        TextSequences[_textSequence->GetId()] = _textSequence;
        return true;
    }

    return false;
}


void TextBook::LoadContent(const std::string &_filePath, ColorsLibrary &_colors, FontsLibrary &_fonts)
{


    std::string filePath = TextLibrary::pathPrefix + TextLibrary::path + _filePath;

    //assert(_colors.empty()==false);
    assert(_fonts.GetSize()>0);


    TextSegment::defaultColor = _colors.At(0);
    TextSegment::defaultFont = _fonts.At(0);


    TextStreamReader *file = objectFactory->NewTextFileStreamReader(filePath);

    //TextFileStreamReader *file = new StdTextFileStreamReader(filePath);
    TextStreamReader &File = *file;

    if(File.IsOpen()==false){
        return;
    }


    //----
    std::string text;
    TextSegment *segment = nullptr;
    TextSequence *sequence = nullptr;
    std::vector<Parameter> parameters;


    while(File.Eof()==false){

        File.ReadLine(text);

        // remove REM parts
        std::size_t pos = text.find("//");
        if(pos!=std::string::npos){
            text = text.substr(0, pos);
        }

        if(text==""){
            continue;                                                                   // skip empty row
        }


        if(text.substr(0,14)==">buttonColors:"){
            std::string value = text.substr(text.find(":")+1);
            RemoveStringWhiteSpacesOnStartAndEnd(value);
            std::vector<std::string>vValues = SplitString(value, ",");
            if(vValues.size()==2){

                if(vValues[0].empty()==false){
                    if(vValues[0].front()=='#'){
                        TextSegment::currentButtonColor = ColorRGBA::ParseFromHexString(vValues[0].substr(1));
                    }else{
                        int index = std::stof(vValues[0]);
                        TextSegment::currentButtonColor = _colors.At(index);
                    }
                }

                if(vValues[1].empty()==false){
                    if(vValues[1].front()=='#'){
                        TextSegment::currentButtonColor_CO = ColorRGBA::ParseFromHexString(vValues[1].substr(1));
                    }else{
                        int index = std::stof(vValues[1]);
                        TextSegment::currentButtonColor_CO = _colors.At(index);
                    }
                }
            }


        //------
        }else if(text.substr(0,10)==">defColor:"){
            std::string value = text.substr(text.find(":")+1);
            RemoveStringWhiteSpacesOnStartAndEnd(value);

            if(value.empty()==false){
                if(value.front()=='#'){
                    value = value.substr(1);
                    TextSegment::defaultColor = ColorRGBA::ParseFromHexString(value);

                }else{
                    int index = std::stof(value);
                    TextSegment::defaultColor = _colors.At(index);
                }
            }


        //------
        }else if(text.substr(0,9)==">defFont:"){
            std::string value = text.substr(text.find(":")+1);
            RemoveStringWhiteSpacesOnStartAndEnd(value);
            int index = std::stof(value);
            if(index<0 || index>=_fonts.GetSize()){
                index = 0;
            }
            TextSegment::defaultFont = _fonts.At(index);


        //------
        }else if(text.substr(0,11)==">defIndent:"){
            std::string value = text.substr(text.find(":")+1);
            RemoveStringWhiteSpacesOnStartAndEnd(value);
            TextSegment::defaultIndent = std::stof(value);


        //------
        }else if(text.substr(0,15)==">defTabSpacing:"){
            std::string value = text.substr(text.find(":")+1);
            RemoveStringWhiteSpacesOnStartAndEnd(value);
            TextSegment::defaultTabSpacing = std::stof(value);


        //------
        }else if(text.substr(0,4)==">id:"){                                     // new segment

            //assert(segment==nullptr);

            //---- store possible current segment
            if(segment){
                if(segment->GetWords().empty()){
                    delete segment;

                }else{

                    if(sequence){
                        sequence->GetTextSegments().push_back(segment);

                    }else if(AddTextSegment(segment)==false){                   // Store text segment in book (which also take over ownership)!
                        delete segment;
                    }
                }
                segment = nullptr;
            }

            //---- store current sequence if active
            if(sequence){
                if(AddTextSequence(sequence)==false){                           // Store text segment in book (which also take over ownership)!
                    delete sequence;
                }
                sequence = nullptr;
            }

            //--- parameters
            parameters.clear();
            _ParseParametersIfDefined(text, parameters);

            RemoveStringWhiteSpacesOnStartAndEnd(text);

            //---
            int id = std::stoi(text.substr(text.find(":")+1));
            segment = new TextSegment(id);

            //---
            continue;       // goto next line


        //------
        }else if(text.substr(0,7)==">seqId:"){

            //---- store possible current segment
            if(segment){
                if(segment->GetWords().empty()){
                    delete segment;

                }else{

                    if(sequence){
                        sequence->GetTextSegments().push_back(segment);

                    }else if(AddTextSegment(segment)==false){           // Store text segment in book (which also take over ownership)!
                        delete segment;
                    }
                }
                segment = nullptr;
            }

            //---- store current sequence if exists
            if(sequence){
                if(AddTextSequence(sequence)==false){                   // Store text segment in book (which also take over ownership)!
                    delete sequence;
                }
                sequence = nullptr;
            }


            //--- parameters
            parameters.clear();
            _ParseParametersIfDefined(text, parameters);

            RemoveStringWhiteSpacesOnStartAndEnd(text);

            int id = std::stoi(text.substr(text.find(":")+1));
            sequence = new TextSequence(id);
            sequence->GetParameters() = parameters;

            //---
            continue;       // goto next line


        //------
        }else if(text.substr(0,4)==">add"){

            //assert(segment==nullptr);       // this should never happen as segment get stored at ">seqId:" line
            assert(sequence);               // this should never happen as sequence get created at ">seqId:" line

            //---- store possible current segment
            if(segment){
                sequence->GetTextSegments().push_back(segment);
                segment = nullptr;
            }


            // adding segments to sequence
            int id = sequence->GetTextSegments().size();        // next index
            segment = new TextSegment(id);

            //--- parameters
            _ParseParametersIfDefined(text, segment->GetParameters());

            //---
            continue;       // goto next line


        }else{

            if(segment){
                segment->AddParagraph(text, _colors, _fonts);
            }
        }

    }



    //---- store last segment
    if(segment){
        if(segment->GetWords().empty()){
            delete segment;

        }else{

            if(sequence){
                sequence->GetTextSegments().push_back(segment);

            }else if(AddTextSegment(segment)==false){                           // Store text segment in book (which also take over ownership)!
                delete segment;
            }
        }
        segment = nullptr;
    }

    // store last sequence
    if(sequence){
       if(AddTextSequence(sequence)==false){
           delete sequence;
       }
       sequence = nullptr;
    }


    File.Close();
    delete file;

}


void TextBook::_ParseParametersIfDefined(std::string &text, std::vector<Parameter>&_parameters)
{


    std::size_t pos1 = text.find("[");
    std::size_t pos2 = text.find("]");

    if(pos1 != std::string::npos && pos2 != std::string::npos ){
        std::string parameters = text.substr(pos1+1, pos2-pos1-1);
        std::vector<std::string>vParameters = SplitString(parameters, ",");
        for(std::string &s : vParameters){
            _parameters.push_back(Parameter::Parse(s));
        }
    }

    //---- remove parameters part from text til the end!
    if(pos1 != std::string::npos){
        text = text.substr(0, pos1);
    }

}



//========================================================================================



void TextSegmentWord::UpdateSize(bool _forceUpdate)
{

    if(word=="<<NL") return;
    if(content) return;

    assert(font);

    if(width<0 || _forceUpdate){
        width = font->GetPixelWidth(word);
    }
    if(height<0 || _forceUpdate){
        height = font->GetCommonPixelHeight() * fFontHeight;
    }


}


//========================================================================================

ColorRGBA TextSegment::defaultColor;
Font* TextSegment::defaultFont = nullptr;
ColorRGBA TextSegment::currentColor;
Font* TextSegment::currentFont;
std::string TextSegment::currentButton;
ColorRGBA TextSegment::currentButtonColor;
ColorRGBA TextSegment::currentButtonColor_CO{255,0,0};
//int TextSegment::currentOffset = 0;
int TextSegment::defaultIndent = 0;
int TextSegment::defaultTabSpacing = 30;
int TextSegment::currentIndent = 0;
int TextSegment::currentTabSpacing = 30;


TextSegment::~TextSegment()
{
    for(TextSegmentWord *tw : words){
        delete tw;
    }
}



void TextSegment::AddParagraph(const std::string &_text, ColorsLibrary &_colors, FontsLibrary &_fonts)
{

    assert(_fonts.GetSize()>0);


    std::string text = _text;


    VerifyAndFixUtf8string(text);
    std::vector<int>unicodes;
    GetStringUnicodes(text, unicodes);

    if(unicodes.empty()) return;


    //currentColor = defaultColor;                    // index 0
    //currentFont = defaultFont;                          // index 0
    //currentIndent = defaultIndent;
    //currentTabSpacing = defaultTabSpacing;




    TextSegmentWord * word = nullptr;


    //--- If text already exist, the new one will be added is added in new line
    if(words.empty()==false){
        word = new TextSegmentWord();
        word->word = "<<NL";            // nw line keyword !
        words.push_back(word);
        word = nullptr;
        //word = new TTextWord();
    }

    if(currentIndent > 0){

        word = new TextSegmentWord();
        word->width = currentIndent;
        word->height = 0;
        word->content = TextSegmentWord::INDENT;
        words.push_back(word);
        word = nullptr;
    }


    for(int i=0; i<unicodes.size(); i++){

        int code = unicodes[i];

        std::string letter;
        utf8::append(code, letter);

        bool whiteSpace = (letter.length()==1 && IsWhiteSpaceCharacter(letter[0]));

        //if(letter != " "){
        if(whiteSpace==false){

            if(word==nullptr){
                word = new TextSegmentWord();
            }

            word->word += letter;
            word->nCharacters++;

        //--- space
        }else if(letter[0]==' '){

            if(word){
                ProcessAddingWord(word, _colors, _fonts);
                word = nullptr;
            }


        //--- tab
        }else if(letter[0]=='\t'){

            word = new TextSegmentWord();
            word->width = currentTabSpacing;
            word->height = 0;
            word->content = TextSegmentWord::TAB_SPACING;
            words.push_back(word);
            word = nullptr;

        }

       // last character -> add last word if it's not empty.
        if(i == unicodes.size()-1){

            /*
            if(word->word != ""){
                word->color = currentColor;     // !
                word->font = currentFont;       // !
                words.push_back(word);
            }else{
                delete word;
            }
            word = nullptr;
            */

            if(word){
                ProcessAddingWord(word, _colors, _fonts);
                word = nullptr;
            }

            //word = ProcessAddingWord(word, colors, fonts);

            // if word was not added to words, delete it (it was
            //if(words.empty()==false && word!= words.back()){
            //    delete word;
            //}
        }
    }

    assert(word==nullptr);
}



void TextSegment::ProcessAddingWord(TextSegmentWord *word, ColorsLibrary &colors, FontsLibrary &fonts)
{


    if(word->word ==""){                        // Extra spaces are not registered!



    //--- KEYS

    //--- COLOR
    }else if(word->word.substr(0,7) == "<color:" && word->word.substr(word->nCharacters-1)==">"){        // keyword for color

        std::string value = word->word.substr(7, word->nCharacters-1-7);
        if(value.empty()==false){
            if(value.front()=='#'){
                value = value.substr(1);
                currentColor = ColorRGBA::ParseFromHexString(value);

            }else{
                int colorID = std::stof(value);
                currentColor = colors.At(colorID);
            }
        }

    }else if(word->word == "<defColor>"){        // keyword for defaultFont
        currentColor = defaultColor;


    //--- FONT
    }else if(word->word.substr(0,6) == "<font:" && word->word.substr(word->nCharacters-1)==">"){        // keyword for font

        int fontID = std::stoi(word->word.substr(6, word->nCharacters-1-6));
        if(fontID<0 || fontID>=fonts.GetSize()){
            fontID = 0;
        }
        currentFont = fonts.At(fontID);


    }else if(word->word == "<defFont>"){        // keyword for defaultFont
        currentFont = defaultFont;


    //--- BUTTON
    }else if(word->word.substr(0,8) == "<button:" && word->word.substr(word->nCharacters-1)==">"){        // keyword for button

        currentButton = word->word.substr(8, word->nCharacters-1-8);

    }else if(word->word == "</button>"){        // keyword for button end

        currentButton = "";


    //--- OFFSET
    }else if(word->word.substr(0,8) == "<offset:" && word->word.substr(word->nCharacters-1)==">"){      // horizontal spacing

        std::string value = word->word.substr(8, word->nCharacters-1-8);
        word->width = std::stof(value);
        word->height = 0;
        word->content = TextSegmentWord::OFFSET;
        words.push_back(word);


    //--- SPACING
    }else if(word->word.substr(0,12) == "<verSpacing:" && word->word.substr(word->nCharacters-1)==">"){      // vertical spacing

        std::string spacing = word->word.substr(12, word->nCharacters-1-12);
        word->height = std::stof(spacing);
        word->width = 0;
        word->content = TextSegmentWord::VER_SPACING;
        words.push_back(word);


    //--- TAB
    }else if(word->word.substr(0,12) == "<tabSpacing:" && word->word.substr(word->nCharacters-1)==">"){      // horizontal spacing

        std::string value = word->word.substr(12, word->nCharacters-1-12);
        currentTabSpacing = std::stof(value);


    }else if(word->word == "<defTabSpacing>"){      // horizontal spacing
        currentTabSpacing = defaultTabSpacing;


    //--- PARAGRAPH INDENT
    }else if(word->word.substr(0,8) == "<indent:" && word->word.substr(word->nCharacters-1)==">"){
        std::string value = word->word.substr(8, word->nCharacters-1-8);
        currentIndent = std::stof(value);

    }else if(word->word == "<defIndent>"){
        currentIndent = defaultIndent;




    //--- TEXT ------------------------------------------------------------
    }else{

        word->word += " ";                       // del vsake besede je razmak do naslednje
        word->nCharacters++;
        word->color = currentColor;
        word->font = currentFont;
        word->button = currentButton;
        if(word->button != ""){
            word->color = currentButtonColor;
            word->buttonColor_CO = currentButtonColor_CO;
        }
        //word->buttonColor = currentButtonColor;

        words.push_back(word);
    }


    //--- If the word was not stored, delete it !
    if(words.empty() || words.back()!=word){
        delete word;
    }

}





bool TextSegment::Contains(const std::string &_word, bool compareWithOriginalWord)
{

    std::string word = _word + " ";

    for(TextSegmentWord *tw : words){

        if(compareWithOriginalWord && tw->originalWord.length()>0){
            if(tw->originalWord==word){
                return true;
            }

        }else if(tw->word==word){
            return true;
        }
    }

    return false;
}


void TextSegment::SetWordSizesIfNeeded()
{
    for(TextSegmentWord *tw : words){
        tw->UpdateSize();
    }
}


bool TextSegment::Replace(const std::string &_searchedWord, const std::string &newWord)
{

    bool found = false;

    std::string searchedWord = _searchedWord + " ";

    for(TextSegmentWord* w : words){

        std::string s = (w->originalWord!="")? w->originalWord : w->word;
        if(s==searchedWord){
            if(w->originalWord==""){
                w->originalWord = w->word;
            }

            w->word = newWord + " ";
            w->nCharacters = GetStringNumberOfCharacters(w->word);
            w->UpdateSize(true);
            found = true;
        }
    }

    return found;

}



//========================================================================================



TextSequence::~TextSequence()
{

    for(TextSegment *ts : TextSegments){
        if(ts){
            delete ts;
        }
    }

}



//========================================================================================================================


std::string TextLibrary::path;
std::string TextLibrary::pathPrefix;


TextLibrary::~TextLibrary()
{

    for(TextBook* b : books){
        delete b;
    }

}


TextBook* TextLibrary::CreateNewBook(const std::string  &_bookName)
{

    for(TextBook* b : books){
        if(b->GetName()==_bookName){
            return b;
        }
    }

    TextBook *book = new TextBook(_bookName);
    books.push_back(book);

    return book;

}


TextBook* TextLibrary::FindBook(const std::string &_bookName)
{

    for(TextBook* b : books){
        if(b->GetName()==_bookName){
            return b;
        }
    }

    return nullptr;
}


TextSegment* TextLibrary::GetTextSegmentFromBook(const std::string &_bookName, int _segmentID)
{

    TextBook* book = FindBook(_bookName);

    if(book){
        if(_segmentID>=0 && _segmentID < book->GetTextSegments().size()){
            return book->GetTextSegments()[_segmentID];
        }
    }

    return nullptr;
}


TextLibrary textLibrary;
	



}




