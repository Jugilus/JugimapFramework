#ifndef JUGIMAP_STREAMS_H
#define JUGIMAP_STREAMS_H

#include <string>
#include <iostream>
#include <fstream>

#include "jmCommon.h"



namespace jugimap{



class SourceGraphics;
class Map;
class GraphicFile;
class GraphicItem;
class SourceSprite;
class VectorShape;
class FrameAnimation;
class SpriteLayer;
class VectorLayer;
class ComposedSprite;
class ObjectFactory;




///\ingroup Streams
///\brief The base abstract class for reading binary streams.
class BinaryStreamReader
{
public:

    /// Destructor
    virtual ~BinaryStreamReader() {}

    /// \brief Returns *true* if the stream of this object is open for reading; if not it returns *false*.
    virtual bool IsOpen() = 0;

    /// \brief Returns the current reading position of the stream.
    virtual int Pos() = 0;

    /// \brief Set the current reading position of the stream.
    virtual void Seek(int _pos) = 0;

    /// \brief Returns the size of the stream.
    virtual int Size() = 0;

    /// \brief Close the stream.
    virtual void Close() = 0;

    /// \brief Read and returns the byte number.
    virtual unsigned char ReadByte() = 0;

    /// \brief Read and returns the integer number (4 bytes).
    virtual int ReadInt() = 0;

    /// \brief Read and returns the float number (4 bytes).
    virtual float ReadFloat() = 0;

    /// \brief Read and returns the string. The length of string is written as the integer at the start.
    virtual std::string ReadString() = 0;

};



/// \ingroup Streams
/// \brief Extended BinaryStreamReader class which utilizes the standard library *ifstream* class for reading.
class StdBinaryFileStreamReader : public BinaryStreamReader
{
public:

    ///\brief Constructor
    ///
    /// Creates a new StdBinaryFileStreamReader object and open the file *fileName* for reading.
    StdBinaryFileStreamReader(const std::string &fileName);


    bool IsOpen() override {return fs.is_open();}
    int Pos() override {return fs.tellg();}
    void Seek(int _pos) override {fs.seekg(_pos);}
    int Size() override {return size;}
    void Close() override {fs.close();}


    unsigned char ReadByte()  override
    {
        unsigned char value;
        fs.read(reinterpret_cast<char*>(&value), 1);
        return value;
    }

    int ReadInt()  override
    {
        int value;
        fs.read(reinterpret_cast<char*>(&value), 4);
        return value;
    }

    float ReadFloat()  override
    {
        float value;
        fs.read(reinterpret_cast<char*>(&value), 4);
        return value;
    }

    std::string ReadString() override;




private:
    std::ifstream fs;
    int size;
};



/// \ingroup Streams
/// \brief Extended BinaryStreamReader class which reads from a memory buffer.
class BinaryBufferStreamReader : public BinaryStreamReader
{
public:


    ///\brief Constructor
    ///
    /// Creates a new BinaryBufferStreamReader object with the given memory buffer *_buff* of the size *_size*.
    /// If parameter *_ownedBuffer* is *true* the reader takes ownership of the buffer and will delete it on destruction.
    BinaryBufferStreamReader(unsigned char *_buff, int _size, bool _ownedBuffer) : buff(_buff), size(_size), ownedBuffer(_ownedBuffer){}


    ~BinaryBufferStreamReader() override
    {
        if(ownedBuffer && buff!=nullptr){
            delete [] buff;
        }
    }

    bool IsOpen() override { return size > 0; }
    int Pos() override {return pos;}
    void Seek(int _pos) override { pos = _pos; }
    int Size() override {return size;}
    void Close() override {}
    unsigned char* GetBuffer() {return buff;}


    unsigned char ReadByte()  override
    {
        unsigned char value = (buff+pos)[0];
        pos += 1;
        return value;
    }

    int ReadInt()  override
    {
        int value = reinterpret_cast<int*>(buff+pos)[0];
        pos +=4;
        return value;
    }

    float ReadFloat()  override
    {
        float value = reinterpret_cast<float*>(buff+pos)[0];
        pos +=4;
        return value;
    }


    std::string ReadString() override
    {
        int length = ReadInt();
        std::string value(reinterpret_cast<char*>(buff+pos), length);
        pos += length;
        return value;
    }

protected:

    unsigned char* buff = nullptr;
    int size = 0;
    int pos = 0;
    bool ownedBuffer = false;

};


///\ingroup Streams
///\brief The base abstract class for reading text streams.
class TextStreamReader
{
public:

    /// Destructor
    virtual ~TextStreamReader() {}

    /// \brief Returns *true* if the stream of this object is open for reading; if not it returns *false*.
    virtual bool IsOpen() = 0;

    /// \brief Returns *true* if the stream of this object is open for reading; if not it returns *false*.
    virtual void ReadLine(std::string &s) = 0;

    /// \brief Returns *true* if the end of file is reached; if not it returns *false*.
    virtual bool Eof() = 0;

    /// \brief Close the stream.
    virtual void Close() = 0;

};


/// \ingroup Streams
/// \brief Extended TextStreamReader class which utilizes the standard library *ifstream* class for reading.
class StdTextFileStreamReader : public TextStreamReader
{
public:

    StdTextFileStreamReader(const std::string &fileName);

    virtual ~StdTextFileStreamReader() {}


    virtual bool IsOpen() override {return fs.is_open();}


    virtual void ReadLine(std::string &s) override {  std::getline(fs, s); };


    virtual bool Eof() override { return fs.eof(); }


    virtual void Close() override { fs.close(); }


private:
    std::ifstream fs;
    int size;
};




}





#endif
