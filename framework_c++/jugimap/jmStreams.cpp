#include <assert.h>
#include <utility>
#include <memory>
#include "jmCommonFunctions.h"
#include "jmSourceGraphics.h"
#include "jmMap.h"
#include "jmLayers.h"
#include "jmSprites.h"
#include "jmVectorShapes.h"
#include "jmVectorShapesUtilities.h"
#include "jmFrameAnimation.h"
#include "jmObjectFactory.h"
#include "jmStreams.h"


using namespace std;


namespace jugimap{




StdBinaryFileStreamReader::StdBinaryFileStreamReader(const std::string &fileName)
{
    fs.open(fileName, ios::binary);

    if(fs.is_open()){
        fs.seekg(0, ios::end);
        size = fs.tellg();
        fs.seekg(0, ios::beg);
    }
}



std::string StdBinaryFileStreamReader::ReadString()
{
    int length = ReadInt();
    char *buf = new char[length];
    fs.read(buf, length);
    std::string value(buf, length);
    delete[] buf;
    return value;
}


//-----------------------------------------------------------------------------------------------


StdTextFileStreamReader::StdTextFileStreamReader(const std::string &fileName)
{

    fs.open(fileName);

    if(fs.is_open()){
        fs.seekg(0, ios::end);
        size = fs.tellg();
        fs.seekg(0, ios::beg);
    }

}




}
