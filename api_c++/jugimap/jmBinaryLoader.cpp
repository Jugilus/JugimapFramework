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
#include "jmBinaryLoader.h"


using namespace std;


namespace jugimap{




StdBinaryStreamReader::StdBinaryStreamReader(const std::string &fileName)
{
    fs.open(fileName, ios::binary);

    if(fs.is_open()){
        fs.seekg(0, ios::end);
        size = fs.tellg();
        fs.seekg(0, ios::beg);
    }
}


std::string StdBinaryStreamReader::ReadString(int length)
{
    char *buf = new char[length];
    fs.read(buf, length);
    std::string value(buf, length);
    delete[] buf;
    return value;
}


//====================================================================================================

std::string JugiMapBinaryLoader::pathPrefix = "";
std::string JugiMapBinaryLoader::error = "";
std::vector<std::string> JugiMapBinaryLoader::messages;
bool JugiMapBinaryLoader::usePixelCoordinatesForTileLayerSprites = true;
//bool JugiMapBinaryLoader::yCoordinateUp = false;
int JugiMapBinaryLoader::zOrderCounterStart = 1000;
int JugiMapBinaryLoader::zOrderCounter = 1000;         // starting depth value (the lowest layer)
int JugiMapBinaryLoader::zOrderCounterStep = 10;




bool JugiMapBinaryLoader::LoadSourceGraphics(string _filePath, SourceGraphics *_sourceGraphics)
{

    if(objectFactory==nullptr) objectFactory = &genericObjectFactory;

    _filePath = pathPrefix + _filePath;



    std::unique_ptr<BinaryStreamReader>stream(objectFactory->NewBinaryStreamReader(_filePath));
    //StreamBinaryReader *Stream = objectFactory->NewBinaryStreamReader(_filePath);
    if(stream->IsOpen()==false){
        error = "Can not open file: " + _filePath;
        return false;
    }

    //--- format signature
    int signature = stream->ReadInt();
    if(signature!=SaveSignature::EXPORTED_SOURCE_GRAPHICS_FORMAT){
        error = string("Wrong format!");
        return false;
    }

    int version = stream->ReadInt();
    if(version!=SaveSignature::EXPORTED_SOURCE_GRAPHICS_FORMAT_VERSION){
        error = string("Wrong format version!");
        return false;
    }

    //---

    JugiMapBinaryLoader loader;
    loader.sourceGraphics = _sourceGraphics;
    //loader.indexBeginImageFiles = _indexBeginImageFiles;
    //loader.indexBeginSourceSprites = _indexBeginSourceSprites;
    loader.map = nullptr;



    int posStart = stream->Pos();
    int sizeCounter = stream->Size() - posStart;
    while(sizeCounter>0){
        int signature = stream->ReadInt();
        int sizeChunk = stream->ReadInt();
        if(sizeChunk>sizeCounter){
            error = "Load error! ChunkSize error for signature: " + std::to_string(signature);
            break;
        }
        int posChunk = stream->Pos();

        if(signature==SaveSignature::SOURCE_SETS){
            if(loader._LoadSourceGraphics(*stream.get(), sizeChunk)==false){
                if(error=="") error = "LoadSourceGraphics error!";
                break;
            }

        }else{
            //cout << "Load - unknown signature:"<< signature;
            stream->Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream->Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "Load error! Wrong chunkSize for signature:" + std::to_string(signature);
            break;
        }
        sizeCounter-=(dSize+8);   //8 for signature and chunk size
    }


    if(error!=""){
        return false;
    }

    return true;

}


bool JugiMapBinaryLoader::LoadMap(std::string _filePath, Map* _map, SourceGraphics *_sourceGraphics)
{

    if(objectFactory==nullptr) objectFactory = &genericObjectFactory;

    Reset();

    _filePath = pathPrefix + _filePath;


    std::unique_ptr<BinaryStreamReader>stream(objectFactory->NewBinaryStreamReader(_filePath));

    //StreamBinaryReader Stream(_filePath);
    if(stream->IsOpen()==false){
        error = "Can not open file: " + _filePath;
        return false;
    }

    //--- format signature
    int signature = stream->ReadInt();
    if(signature!=SaveSignature::EXPORTED_MAP_FORMAT){
        error = string("Wrong format!");
        return false;
    }

    int version = stream->ReadInt();
    if(version!=SaveSignature::EXPORTED_MAP_FORMAT_VERSION){
        error = string("Wrong format version!");
        return false;
    }

    //---

    JugiMapBinaryLoader loader;
    loader.sourceGraphics = _sourceGraphics;
    //loader.indexBeginImageFiles = _indexBeginImageFiles;
    //loader.indexBeginSourceSprites = _indexBeginSourceSprites;
    if(objectFactory==nullptr) objectFactory = &genericObjectFactory;
    loader.map = _map;

    //----
    loader.map->name = _filePath;
    size_t indexLastSlash = loader.map->name.find_last_of("\\/");
    if (std::string::npos != indexLastSlash) loader.map->name.erase(0, indexLastSlash+1);
    size_t indexPeriod = loader.map->name.rfind('.');
    if (std::string::npos != indexPeriod) loader.map->name.erase(indexPeriod);

    //----
    int posStart = stream->Pos();
    int sizeCounter = stream->Size() - posStart;
    while(sizeCounter>0){
        int signature = stream->ReadInt();
        int sizeChunk = stream->ReadInt();
        if(sizeChunk>sizeCounter){
            error = "Load error! ChunkSize error for signature: " + std::to_string(signature);
            break;
        }
        int posChunk = stream->Pos();

        if(signature==SaveSignature::MAP){
            if(loader._LoadMap(*stream.get(), sizeChunk)==false){
                if(error=="") error = "Load/LoadMap error!";
                break;
            }

        }else{
            //cout << "Load - unknown signature:"<< signature;
            stream->Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream->Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "Load error! Wrong chunkSize for signature:" + std::to_string(signature);
            break;
        }
        sizeCounter-=(dSize+8);   //8 for signature and chunk size
    }

    if(error!=""){
        return false;
    }

    //loader.AdjustMapWorldLayers();

    return true;
}


void JugiMapBinaryLoader::Reset()
{
    error = "";
    messages.clear();
    zOrderCounter = zOrderCounterStart;
}


bool JugiMapBinaryLoader::_LoadSourceGraphics(BinaryStreamReader &stream, int size)
{

    int posStart = stream.Pos();
    int sizeCounter = size;

    //---
    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();
        if(sizeChunk>sizeCounter){
            error = "LoadSourceSets error! ChunkSize error for signature: " + std::to_string(signature);
            return false;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::SOURCE_IMAGE_FILES){

            int nGraphicFiles = stream.ReadInt();
            for(int i=0; i<nGraphicFiles; i++){

                int signature2 = stream.ReadInt();
                int sizeChunk2 = stream.ReadInt();
                int posChunk2 = stream.Pos();

                if(signature2==SaveSignature::SOURCE_IMAGE_FILE){

                    GraphicFile *gf = LoadGraphicFile(stream,sizeChunk2);
                    if(gf){
                        sourceGraphics->files.push_back(gf);
                    }else{
                        return false;
                    }

                }else{
                    messages.push_back("LoadSourceData - SOURCE_IMAGE_FILES - unknown signature: " + std::to_string(signature2));
                    stream.Seek(posChunk2+sizeChunk2);
                }
            }


        }else if(signature==SaveSignature::SOURCE_OBJECTS){

            int nSourceSprites = stream.ReadInt();
            for(int i=0; i<nSourceSprites; i++){

                int signature2 = stream.ReadInt();
                int sizeChunk2 = stream.ReadInt();
                int posChunk2 = stream.Pos();

                if(signature2==SaveSignature::SOURCE_OBJECT){
                    SourceSprite *ss = LoadSourceSprite(stream,sizeChunk2);
                    if(ss){
                        sourceGraphics->sourceSprites.push_back(ss);
                    }else{
                        return false;
                    }

                }else{
                    messages.push_back("LoadSourceData - SOURCE_OBJECTS - unknown signature: " + std::to_string(signature2));
                    stream.Seek(posChunk2+sizeChunk2);
                }
            }

        }else{
            messages.push_back("LoadSourceSets - unknown signature: " + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadSourceSets error! Wrong chunkSize for signature:" + std::to_string(signature);
            return false;
        }
        sizeCounter-=(dSize+8);   //8 for two integers - 'signature' and 'size'
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadSourceSets error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        return false;
    }

    return true;
}


GraphicFile * JugiMapBinaryLoader::LoadGraphicFile(BinaryStreamReader &stream, int size)
{

    GraphicFile *gf = objectFactory->NewFile();


    int posStart = stream.Pos();
    int sizeCounter = size;

    //---
    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();

        if(sizeChunk>sizeCounter){
            error = "LoadGraphicFile error! ChunkSize error for signature: " + std::to_string(signature);
            delete gf;
            return nullptr;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){

            int fileKind = stream.ReadByte();
            if(fileKind==jmSINGLE_IMAGE){
                gf->kind = FileKind::SINGLE_IMAGE;

            }else if(fileKind==jmTILE_SHEET_IMAGE){
                gf->kind = FileKind::TILE_SHEET_IMAGE;

            }else if(fileKind==jmSPRITE_SHEET_IMAGE){
                gf->kind = FileKind::SPRITE_SHEET_IMAGE;

            }else if(fileKind==jmSPINE_FILE){
                gf->kind = FileKind::SPINE_FILE;

            }else if(fileKind==jmSPRITER_FILE){
                gf->kind = FileKind::SPRITER_FILE;
            }

            //gf->kind = stream.ReadByte();
            gf->relativeFilePath = stream.ReadStringWithWrittenLength();
            gf->size.x = stream.ReadInt();
            gf->size.y = stream.ReadInt();
            gf->spineAtlasRelativeFilePath = stream.ReadStringWithWrittenLength();


        }else if(signature==SaveSignature::SOURCE_IMAGES){

            int nItems = stream.ReadInt();
            for(int i=0; i<nItems; i++){
                GraphicItem *im = objectFactory->NewItem();
                im->graphicFile = gf;
                im->name = stream.ReadStringWithWrittenLength();
                im->rect.min.x = stream.ReadInt();
                im->rect.min.y = stream.ReadInt();
                im->rect.max.x = im->rect.min.x + stream.ReadInt();     // width
                im->rect.max.y = im->rect.min.y + stream.ReadInt();     // height
                im->handle.x = stream.ReadInt();
                im->handle.y = stream.ReadInt();
                im->spineScale = stream.ReadFloat();

                if(settings.IsYCoordinateUp()){
                    im->handle.y = im->rect.Height() - im->handle.y;
                }

                int nCollisionShapes = stream.ReadInt();
                for(int k=0; k<nCollisionShapes; k++){

                    int signature2 = stream.ReadInt();
                    int sizeChunk2 = stream.ReadInt();
                    int posChunk2 = stream.Pos();
                    if(signature2==SaveSignature::VECTOR_SHAPE){
                        VectorShape *vs = LoadVectorShape(stream, sizeChunk2, false);
                        if(vs){
                            if(vs->probe){
                                im->probeShapes.push_back(vs);
                            }else{
                                // check out if shape geometry is ok for using in collision detection
                                bool skip = false;
                                if(vs->GetKind()==ShapeKind::POLYLINE){
                                    PolyLineShape* poly = static_cast<PolyLineShape*>(vs->GetGeometricShape());
                                    if(poly->vertices.size()<3){
                                        skip = true;
                                    }
                                    if(skip==false && poly->rectangle==false){
                                        // set 'convex' flag and make sure vertices order is CCW - both is required by 'box2d' library
                                        bool CW = false;
                                        poly->convex = IsPolygonConvex(poly->vertices,CW);
                                        if(poly->convex && CW){
                                            std::reverse(poly->vertices.begin(), poly->vertices.end());
                                        }
                                    }

                                }else if(vs->GetKind()==ShapeKind::ELLIPSE){
                                    // must be circle
                                    if(AreEqual(static_cast<EllipseShape*>(vs->GetGeometricShape())->a, static_cast<EllipseShape*>(vs->GetGeometricShape())->b)==false){
                                        skip = true;
                                    }
                                }else{
                                    skip = true;
                                }
                                if(skip==false){
                                    im->spriteShapes.push_back(vs);
                                }else{
                                    delete vs;
                                }

                            }
                            //im->collisionShapes.push_back(vs);
                        }else{
                            delete im;
                            delete gf;
                            return nullptr;
                        }

                    }else{
                        messages.push_back("LoadGraphicFile - VECTOR_SHAPE - unknown signature: " + std::to_string(signature2));
                        stream.Seek(posChunk2+sizeChunk2);
                    }
                }
                InitCollisionShapes(im);

                //----
                gf->items.push_back(im);
            }

        }else{
            messages.push_back("LoadGraphicFile - unknown signature: " + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadGraphicFile error! Wrong chunkSize for signature:" + std::to_string(signature);
            delete gf;
            return nullptr;
        }
        sizeCounter-=(dSize+8);   //8 for two integers - 'signature' and 'size'
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadGraphicFile error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        delete gf;
        return nullptr;
    }

    return gf;
}


SourceSprite * JugiMapBinaryLoader::LoadSourceSprite(BinaryStreamReader &stream, int size)
{

    SourceSprite *ss = objectFactory->NewSourceSprite();


    int posStart = stream.Pos();
    int sizeCounter = size;

    //---
    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();

        if(sizeChunk>sizeCounter){
            error = "LoadSourceSprite error! ChunkSize error for signature: " + std::to_string(signature);
            delete ss;
            return nullptr;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){

            ss->name = stream.ReadStringWithWrittenLength();


        }else if(signature==SaveSignature::SOURCE_IMAGES){

            int nItems = stream.ReadInt();
            for(int i=0; i<nItems; i++){
                int indexGraphicFile = stream.ReadInt();
                indexGraphicFile += indexBeginImageFiles;

                int indexGraphicItem = stream.ReadInt();

                if(indexGraphicFile==saveID_DUMMY_SIF){
                    if(indexGraphicItem==saveID_DUMMY_COMPOSED_SPRITE){
                        ss->kind = SpriteKind::COMPOSED;
                    }

                }else if(indexGraphicFile>=0 && indexGraphicFile<sourceGraphics->files.size()){

                    GraphicFile *sif = sourceGraphics->files[indexGraphicFile];

                    if(indexGraphicItem>=0 && indexGraphicItem<sif->items.size()){

                        ss->graphicItems.push_back(sif->items[indexGraphicItem]);

                        if(sif->kind==FileKind::SPINE_FILE){
                            ss->kind = SpriteKind::SPINE;

                        }else if(sif->kind==FileKind::SPRITER_FILE){
                            ss->kind = SpriteKind::SPRITER;

                        }else{
                            ss->kind = SpriteKind::STANDARD;
                        }

                    }else{
                        error = "LoadSourceSprite: "+ss->name+"  indexSourceImage: " + to_string(indexGraphicItem) + " out of range!";
                        assert(false);
                        delete ss;
                        return nullptr;
                    }
                }else{
                    error = "LoadSourceSprite: "+ss->name+" indexSourceImageFile: " + to_string(indexGraphicFile) + " out of range!";
                    assert(false);
                    delete ss;
                    return nullptr;
                }
            }


        }else if(signature==SaveSignature::PARAMETERS){

            int nParameters = stream.ReadInt();
            for(int i=0; i<nParameters; i++){
                Parameter pv;
                pv.kind = stream.ReadByte();
                pv.name = stream.ReadStringWithWrittenLength();
                //----
                ss->parametersTMP.push_back(pv);
            }


        }else if(signature==SaveSignature::CONSTANT_PARAMETERS){

            int nConstantParameters = stream.ReadInt();
            for(int i=0; i<nConstantParameters; i++){
                Parameter pv;
                pv.kind = stream.ReadByte();
                pv.name = stream.ReadStringWithWrittenLength();
                pv.value = stream.ReadStringWithWrittenLength();
                //----
                ss->constantParameters.push_back(pv);
            }

        }else if(signature==SaveSignature::COMPOSED_SPRITE){

            ss->SourceComposedSprite = LoadComposedSpriteData(stream,sizeChunk);
            if(ss->SourceComposedSprite){
                // empty

            }else{
                delete ss;
                return nullptr;
            }

        }else if(signature==SaveSignature::FRAME_ANIMATIONS){

            int nFrameAnimations = stream.ReadInt();

            for(int i=0; i<nFrameAnimations; i++){
                int signature2 = stream.ReadInt();
                int sizeChunk2 = stream.ReadInt();
                int posChunk2 = stream.Pos();

                if(signature2==SaveSignature::FRAME_ANIMATION){
                    FrameAnimation *fa = LoadFrameAnimation(stream, sizeChunk2, ss);
                    if(fa){
                        ss->frameAnimations.push_back(fa);
                    }
                }else{
                    messages.push_back("LoadSourceSprite - FRAME_ANIMATIONS - unknown signature: " + std::to_string(signature2));
                    stream.Seek(posChunk2+sizeChunk2);
                }
            }


        }else{
            messages.push_back("LoadSourceSprite - unknown signature: " + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadSourceSprite error! Wrong chunkSize for signature:" + std::to_string(signature);
            delete ss;
            return nullptr;
        }
        sizeCounter-=(dSize+8);   //8 for two integers - 'signature' and 'size'
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadSourceSprite error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        delete ss;
        return nullptr;
    }

    return ss;

}


ComposedSprite *JugiMapBinaryLoader::LoadComposedSpriteData(BinaryStreamReader &stream, int size)
{

    ComposedSprite *cs = objectFactory->NewComposedSprite();
    loadedComposedSprite = cs;

    int posStart = stream.Pos();
    int sizeCounter = size;

    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();
        if(sizeChunk>sizeCounter){
            error = "LoadComposedSprite error! ChunkSize error for signature: " + std::to_string(signature);
            delete cs;
            return nullptr;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){
            cs->tileSize.x = stream.ReadInt();
            cs->tileSize.y = stream.ReadInt();
            cs->nTiles.x = stream.ReadInt();
            cs->nTiles.y = stream.ReadInt();
            cs->size.x = stream.ReadInt();
            cs->size.y = stream.ReadInt();
            cs->handle.x = stream.ReadInt();
            cs->handle.y = stream.ReadInt();

            if(settings.IsYCoordinateUp()){
                cs->handle.y = cs->size.y - cs->handle.y;
            }

        }else if(signature==SaveSignature::LAYERS){

            int nLayers = stream.ReadInt();
            for(int i=0; i<nLayers; i++){

                int childSignature = stream.ReadInt();
                int sizeChildChunk = stream.ReadInt();
                int posChildChunk = stream.Pos();

                if(childSignature==SaveSignature::LAYER){
                    SpriteLayer * layer = LoadLayer(stream, sizeChildChunk);
                    if(layer){
                        cs->layers.push_back(layer);
                    }else{
                        delete cs;
                        return nullptr;
                    }

                }else if(childSignature==SaveSignature::VECTOR_LAYER){
                    VectorLayer *vectorLayer = LoadVectorLayer(stream, sizeChildChunk);
                    if(vectorLayer){
                        cs->layers.push_back(vectorLayer);
                    }else{
                        delete cs;
                        return nullptr;
                    }

                }else{
                    messages.push_back("LoadComposedSprite - unknown signature: " + std::to_string(signature));

                    if(sizeChildChunk>sizeCounter){
                        error = "LoadComposedSprite child error! Chunk size error for unknown signature:" + std::to_string(childSignature);
                        delete cs;
                        return nullptr;
                    }
                    stream.Seek(posChildChunk+sizeChildChunk);
                }
            }


        }else{
            messages.push_back("LoadComposedSprite - unknown signature: " + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadComposedSprite error! Wrong chunkSize for signature:" + std::to_string(signature);
            delete cs;
            return nullptr;
        }
        sizeCounter-=(dSize+8);   //8 za signature in size
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadComposedSprite error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        delete cs;
        return nullptr;
    }

    loadedComposedSprite = nullptr;
    return cs;

}


bool JugiMapBinaryLoader::_LoadMap(BinaryStreamReader &stream, int size)
{

    //loading = loadingMAP;

    int posStart = stream.Pos();
    int sizeCounter = size;

    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();
        if(sizeChunk>sizeCounter){
            error = "LoadMap error! ChunkSize error for signature: " + std::to_string(signature);
            return false;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){
            map->tileSize.x = stream.ReadInt();
            map->tileSize.y = stream.ReadInt();
            map->nTiles.x = stream.ReadInt();
            map->nTiles.y = stream.ReadInt();


        }else if(signature==SaveSignature::LAYERS){

            int nLayers = stream.ReadInt();
            for(int i=0; i<nLayers; i++){

                int childSignature = stream.ReadInt();
                int sizeChildChunk = stream.ReadInt();
                int posChildChunk = stream.Pos();

                if(childSignature==SaveSignature::LAYER){
                    SpriteLayer *layer = LoadLayer(stream, sizeChildChunk);
                    if(layer){
                        map->layers.push_back(layer);
                    }else{
                        return false;
                    }

                }else if(childSignature==SaveSignature::VECTOR_LAYER){
                    VectorLayer *vectorLayer = LoadVectorLayer(stream, sizeChildChunk);
                    if(vectorLayer){
                        //map->vectorLayers.push_back(vectorLayer);
                        map->layers.push_back(vectorLayer);
                    }else{
                        return false;
                    }

                    /*
                }else if(childSignature==SaveSignature::STATIC_LAYER){
                    SpriteLayer *sssl = LoadStaticSingleSpriteLayer(stream, sizeChildChunk);
                    if(sssl){
                        map->layers.push_back(sssl);
                    }else{
                        return false;
                    }
                    */

                }else{
                    messages.push_back("LoadMap - unknown signature: " + std::to_string(signature));

                    if(sizeChildChunk>sizeCounter){
                        error = "LoadMap board child error! Chunk size error for unknown signature:" + std::to_string(childSignature);
                        return false;
                    }
                    stream.Seek(posChildChunk+sizeChildChunk);
                }
            }

        }else if(signature==SaveSignature::PARAMETERS){

            LoadParameters(stream, map->parameters);


        }else{
            messages.push_back("LoadMap - unknown signature: " + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadMap error! Wrong chunkSize for signature:" + std::to_string(signature);
            return false;
        }
        sizeCounter-=(dSize+8);   //8 za signature in size
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadMap error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        return false;
    }

    return true;
}


SpriteLayer *JugiMapBinaryLoader::LoadLayer(BinaryStreamReader &stream, int size)
{

    SpriteLayer *layer = objectFactory->NewSpriteLayer();
    if(map){
        layer->map = map;
        layer->zOrder = zOrderCounter;
        zOrderCounter += zOrderCounterStep;
    }


    int posStart = stream.Pos();
    int sizeCounter = size;

    //---
    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();

        if(sizeChunk>sizeCounter){
            error = "LoadLayer error! ChunkSize error for signature: " + std::to_string(signature);
            return nullptr;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){

            layer->name = stream.ReadStringWithWrittenLength();
            int layerKind = stream.ReadByte();     // tile layer or sprite layer
            if(layerKind==jmTILE_LAYER){
                //layer->kind = LayerKind::TILE;
                layer->editorTileLayer = true;

            }else if(layerKind==jmSPRITE_LAYER){
                layer->editorTileLayer = false;
            //    layer->kind = LayerKind::SPRITE;
            }
            layer->boundingBox.min.x = stream.ReadFloat();
            layer->boundingBox.min.y = stream.ReadFloat();
            layer->boundingBox.max.x = stream.ReadFloat();
            layer->boundingBox.max.y = stream.ReadFloat();

            if(settings.IsYCoordinateUp()){
                float yMin,yMax;
                if(loadedComposedSprite){                           // a child of loadedComposedSprite
                    yMin = loadedComposedSprite->size.y - layer->boundingBox.max.y;
                    yMax = loadedComposedSprite->size.y - layer->boundingBox.min.y;
                }else if(map){
                    yMin = map->nTiles.y * map->tileSize.y - layer->boundingBox.max.y;
                    yMax = map->nTiles.y * map->tileSize.y - layer->boundingBox.min.y;
                }else{
                    assert(false);
                }
                layer->boundingBox.min.y = yMin;
                layer->boundingBox.max.y = yMax;
            }


        }else if(signature==SaveSignature::OBJECTS){

            int nSprites = stream.ReadInt();
            for(int i=0; i<nSprites; i++){

                int indexSourceSprite = stream.ReadInt();
                indexSourceSprite += indexBeginSourceSprites;

                if(indexSourceSprite<0 || indexSourceSprite>=sourceGraphics->sourceSprites.size()){
                    error = "LoadLayer indexSourceObject: " + to_string(indexSourceSprite) + " out of range!";
                    assert(false);
                    delete layer;
                    return nullptr;
                }
                SourceSprite *ss = sourceGraphics->sourceSprites[indexSourceSprite];


                Sprite *s = objectFactory->NewSprite(ss->kind);
                s->sourceSprite = ss;
                //s->map = map;
                s->layer = layer;
                s->parentComposedSprite = layer->parentComposedSprite;

                //---
                s->position.x = stream.ReadInt();
                s->position.y = stream.ReadInt();

                if(usePixelCoordinatesForTileLayerSprites && layer->editorTileLayer){
                    if(loadedComposedSprite){
                        s->position.x = s->position.x*loadedComposedSprite->tileSize.x + loadedComposedSprite->tileSize.x/2.0;
                        s->position.y = s->position.y*loadedComposedSprite->tileSize.y + loadedComposedSprite->tileSize.y/2.0;
                    }else if(map){
                        s->position.x = s->position.x*map->tileSize.x + map->tileSize.x/2.0;
                        s->position.y = s->position.y*map->tileSize.y + map->tileSize.y/2.0;
                    }
                }


                //---- PROPERTIES
                const int flagXflip = 1;
                const int flagYflip = 2;
                const int flagUniformScale = 4;
                const int flagSavedXscale = 8;
                const int flagSavedYscale = 16;
                const int flagSavedRotation = 32;
                const int flagSavedDataFlag = 64;
                const int flagSavedXdelta = 128;
                const int flagSavedYdelta = 256;
                const int flagSavedAlpha = 512;
                const int flagSavedExtraProperty_ColorBlend = 1024;

                const int transformSaveFlags = stream.ReadInt();

                if(transformSaveFlags & flagXflip) s->flip.x = true;
                if(transformSaveFlags & flagYflip) s->flip.y = true;
                if(transformSaveFlags & flagSavedXscale) s->scale.x = stream.ReadFloat();
                if(transformSaveFlags & flagSavedYscale) s->scale.y = stream.ReadFloat();
                if(transformSaveFlags & flagUniformScale) s->scale.y = s->scale.x;
                if(transformSaveFlags & flagSavedRotation) s->rotation = stream.ReadFloat();
                if(transformSaveFlags & flagSavedXdelta) s->animationFrameOffset.x = stream.ReadFloat();
                if(transformSaveFlags & flagSavedYdelta) s->animationFrameOffset.y = stream.ReadFloat();
                if(transformSaveFlags & flagSavedAlpha){
                    s->alpha = stream.ReadFloat();
                }
                if(transformSaveFlags & flagSavedExtraProperty_ColorBlend){
                    s->colorOverlayActive = true;
                    s->colorOverlayRGBA = ColorRGBA(stream.ReadInt());
                    int colorBlendMode = stream.ReadInt();
                    const int blendSIMPLE_MULTIPLY = 0;
                    const int blendNORMAL = 1;
                    const int blendMULTIPLY = 2;
                    const int blendLINEAR_DODGE = 3;
                    const int blendCOLOR = 4;

                    if(colorBlendMode==blendSIMPLE_MULTIPLY){
                        s->colorOverlayBlend = ColorOverlayBlend::SIMPLE_MULTIPLY;

                    }else if(colorBlendMode==blendNORMAL){
                        s->colorOverlayBlend = ColorOverlayBlend::NORMAL;

                    }else if(colorBlendMode==blendMULTIPLY){
                        s->colorOverlayBlend = ColorOverlayBlend::MULTIPLY;

                    }else if(colorBlendMode==blendLINEAR_DODGE){
                        s->colorOverlayBlend = ColorOverlayBlend::LINEAR_DODGE;

                    }else if(colorBlendMode==blendCOLOR){
                        s->colorOverlayBlend = ColorOverlayBlend::COLOR;
                    }

                }

                if(transformSaveFlags & flagSavedDataFlag) s->dataFlags = stream.ReadInt();


                if(settings.IsYCoordinateUp()){
                    if(loadedComposedSprite){   // sprite is child of loadedComposedSprite
                        //s->pos.y = loadedComposedSprite->nTiles.y * loadedComposedSprite->tileSize.y - s->pos.y;
                        s->position.y = loadedComposedSprite->size.y - s->position.y;
                    }else if(map){
                        s->position.y = map->nTiles.y * map->tileSize.y - s->position.y;
                    }
                    s->rotation = -s->rotation;
                }

                // sprite parameters
                int nParameters = stream.ReadInt();
                if(nParameters != ss->parametersTMP.size()){
                    error = "LoadLayer nParameters:" + to_string(nParameters) + " different then for the source object: " + to_string(ss->parametersTMP.size());
                    assert(false);
                    delete s;
                    delete layer;
                    return nullptr;
                }

                for(int i=0; i<nParameters; i++){

                    Parameter &pvSource = ss->parametersTMP[i];
                    Parameter pv;

                    pv.name = pvSource.name;
                    pv.kind = pvSource.kind;

                    bool pvActive = (bool)stream.ReadByte();

                    if(pv.kind==jmINTEGER){
                        int valueInt = stream.ReadInt();
                        pv.value = to_string(valueInt);

                    }else if(pv.kind==jmFLOAT){
                        float valueFloat = stream.ReadFloat();
                        pv.value = to_string(valueFloat);

                    }else if(pv.kind==jmBOOLEAN){
                        //empty

                    }else if(pv.kind==jmSTRING){
                        pv.value = stream.ReadStringWithWrittenLength();

                    }else if(pv.kind==jmVALUES_SET){
                        pv.value = stream.ReadStringWithWrittenLength();
                    }

                    if(pvActive){
                        s->parameters.push_back(pv);
                    }
                }


                //----
                if(ss->kind==SpriteKind::COMPOSED){
                    assert(ss->SourceComposedSprite);
                    if(ss->SourceComposedSprite==nullptr){
                        error = "LoadLayer error! Missing SourceComposedSprite for source sprite: " + ss->name;
                        delete layer;
                        return nullptr;
                    }

                    s->handle = ss->SourceComposedSprite->handle;
                    //if(configuration.IsYCoordinateUp()){
                    //    s->handle.y = ss->SourceComposedSprite->size.y - s->handle.y;
                    //}
                    if(map){
                        if(ss->name=="csRedDiamond"){
                            DummyFunction();
                        }
                    }

                    JugiMapBinaryLoader* loader = (map!=nullptr)? this : nullptr;
                    ComposedSprite::CopyLayers(ss->SourceComposedSprite, static_cast<ComposedSprite*>(s), loader);
                }

                //---
                layer->sprites.push_back(s);
            }

        }else if(signature==SaveSignature::PARAMETERS){
            LoadParameters(stream, layer->parameters);

        }else{
            messages.push_back("LoadLayer - unknown signature:" + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadLayer error! Wrong chunkSize for signature:" + std::to_string(signature);
            delete layer;
            return nullptr;
        }
        sizeCounter-=(dSize+8);   //8 for two integers - 'signature' and 'size'
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadLayer error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        delete layer;
        return nullptr;
    }

    return layer;

}



VectorLayer* JugiMapBinaryLoader::LoadVectorLayer(BinaryStreamReader &stream, int size)
{

    VectorLayer *vl = objectFactory->NewVectorLayer();
    vl->kind = LayerKind::VECTOR;
    if(map){
        vl->map = map;
        zOrderCounter += zOrderCounterStep;
    }


    int posStart = stream.Pos();
    int sizeCounter = size;

    //---
    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();

        if(sizeChunk>sizeCounter){
            error = "LoadVectorLayer error! ChunkSize error for signature: " + std::to_string(signature);
            delete vl;
            return nullptr;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){

            vl->name = stream.ReadStringWithWrittenLength();

        }else if(signature==SaveSignature::VECTOR_SHAPES){

            int nVectorShapes = stream.ReadInt();
            for(int i=0; i<nVectorShapes; i++){

                int signature2 = stream.ReadInt();
                int sizeChunk2 = stream.ReadInt();
                int posChunk2 = stream.Pos();
                if(signature2==SaveSignature::VECTOR_SHAPE){
                    VectorShape *vs = LoadVectorShape(stream, sizeChunk2);
                    if(vs){
                        vl->vectorShapes.push_back(vs);
                    }else{
                        delete vl;
                        return nullptr;
                    }

                }else{
                    messages.push_back("LoadVectorLayer - VECTOR_SHAPE - unknown signature: " + std::to_string(signature2));
                    stream.Seek(posChunk2+sizeChunk2);
                }
            }

        }else if(signature==SaveSignature::PARAMETERS){
            LoadParameters(stream, vl->parameters);

        }else{
            messages.push_back("LoadVectorLayer - unknown signature: " + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadVectorLayer error! Wrong chunkSize for signature:" +  std::to_string(signature);
            delete vl;
            return nullptr;
        }
        sizeCounter-=(dSize+8);   //8 for two integers - 'signature' and 'size'
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadVectorLayer error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        delete vl;
        return nullptr;
    }

    return vl;

}


VectorShape* JugiMapBinaryLoader::LoadVectorShape(BinaryStreamReader &stream, int size, bool vectorLayerShape)
{

    FVectorShape fvs;

    int posStart = stream.Pos();
    int sizeCounter = size;

    //---
    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();

        if(sizeChunk>sizeCounter){
            error = "LoadVectorShape error! ChunkSize error for signature: " + std::to_string(signature);
            return nullptr;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){

            fvs.kind = stream.ReadByte();
            fvs.closed = stream.ReadByte();
            fvs.dataFlags = stream.ReadInt();
            fvs.probe = stream.ReadByte();

        }else if(signature==SaveSignature::VECTOR_SHAPE_CONTROL_POINTS){

            int nVertices = stream.ReadInt();
            for(int i=0; i<nVertices; i++){
                FVectorVertex V;
                V.x = stream.ReadInt();
                V.y = stream.ReadInt();
                if(vectorLayerShape && settings.IsYCoordinateUp()){
                    V.y = map->nTiles.y*map->tileSize.y - V.y;
                }
                if(fvs.kind==jmBEZIER_POLYCURVE){
                    V.xBPprevious = stream.ReadFloat();
                    V.yBPprevious = stream.ReadFloat();
                    V.xBPnext = stream.ReadFloat();
                    V.yBPnext = stream.ReadFloat();

                    if(vectorLayerShape && settings.IsYCoordinateUp()){
                        V.yBPprevious = map->nTiles.y*map->tileSize.y - V.yBPprevious;
                        V.yBPnext = map->nTiles.y*map->tileSize.y - V.yBPnext;
                    }
                }
                fvs.vertices.push_back(V);
            }

        }else if(signature==SaveSignature::PARAMETERS){

            LoadParameters(stream, fvs.parameters);

        }else{
            cout << "LoadVectorShape - unknown signature:"<< signature;
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadVectorShape error! Wrong chunkSize for signature:" + std::to_string(signature);
            return nullptr;
        }
        sizeCounter-=(dSize+8);   //8 for two integers - 'signature' and 'size'
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadVectorShape error! Wrong size: " + to_string(stream.Pos()-posStart) + "  saved: " + to_string(size);
        return nullptr;
    }


    //---
    VectorShape *vs = new VectorShape(fvs.probe, fvs.dataFlags, fvs.parameters);

    if(fvs.kind==jmRECTANGLE){

        PolyLineShape *polyline = new PolyLineShape();
        polyline->rectangle = true;
        polyline->closed = true;
        for(FVectorVertex &v : fvs.vertices){
            polyline->vertices.push_back(Vec2f(v.x, v.y));
        }
        polyline->CalculatePathLength();
        polyline->UpdateBoundingBox();
        vs->shape = polyline;

    }else if(fvs.kind==jmPOLYLINE){
        PolyLineShape *polyline = new PolyLineShape();
        polyline->closed = fvs.closed;
        for(FVectorVertex &v : fvs.vertices){
            polyline->vertices.push_back(Vec2f(v.x, v.y));
        }
        polyline->CalculatePathLength();
        polyline->UpdateBoundingBox();
        vs->shape = polyline;

    }else if(fvs.kind==jmELLIPSE){
        EllipseShape *ellipse = new EllipseShape();
        ellipse->center.Set(fvs.vertices[0].x, fvs.vertices[0].y);
        ellipse->a = fvs.vertices[2].x - fvs.vertices[0].x;   // 0 center, 1 top, 2 right, 3 bottom, 4 left
        ellipse->b = fvs.vertices[3].y - fvs.vertices[0].y;
        if(vectorLayerShape && settings.IsYCoordinateUp()) ellipse->b = -ellipse->b;
        ellipse->CalculatePathLength();

        vs->shape = ellipse;

    }else if(fvs.kind==jmBEZIER_POLYCURVE){
        BezierShape *bezierCurve = new BezierShape();
        bezierCurve->closed = fvs.closed;
        for(FVectorVertex &v : fvs.vertices){
            bezierCurve->vertices.push_back(BezierVertex(Vec2f(v.x, v.y), Vec2f(v.xBPprevious, v.yBPprevious), Vec2f(v.xBPnext, v.yBPnext)));
        }
        BezierPolycurveToPolyline(bezierCurve->vertices, bezierCurve->polylineVertices);
        bezierCurve->CalculatePathLength();

        vs->shape = bezierCurve;

    }else if(fvs.kind==jmSINGLE_POINT){
        SinglePointShape *singlePoint = new SinglePointShape();
        singlePoint->point.Set(fvs.vertices[0].x, fvs.vertices[0].y);

        vs->shape = singlePoint;

    }

    if(vs->shape==nullptr){
        delete vs;
        vs = nullptr;
    }

    return vs;
}


FrameAnimation* JugiMapBinaryLoader::LoadFrameAnimation(BinaryStreamReader &stream, int size, SourceSprite *ss)
{

    FrameAnimation *fa = new FrameAnimation();


    int posStart = stream.Pos();
    int sizeCounter = size;

    //---
    while(sizeCounter>0){
        int signature = stream.ReadInt();
        int sizeChunk = stream.ReadInt();

        if(sizeChunk>sizeCounter){
            error = "LoadFrameAnimation error! ChunkSize error for signature: " + std::to_string(signature);
            delete fa;
            return nullptr;
        }
        int posChunk = stream.Pos();

        if(signature==SaveSignature::VARIABLES){

            fa->name = stream.ReadStringWithWrittenLength();
            fa->loopCount = stream.ReadInt();
            fa->loopForever = stream.ReadByte();
            fa->scaleDuration = stream.ReadFloat();
            fa->dataFlags = stream.ReadInt();
            fa->repeatAnimation = stream.ReadByte();
            fa->repeat_PeriodStart = stream.ReadInt();
            fa->repeat_PeriodEnd = stream.ReadInt();
            fa->startAtRepeatTime = stream.ReadByte();

        }else if(signature==SaveSignature::ANIMATION_FRAMES){

            int nFrames = stream.ReadInt();
            for(int i=0; i<nFrames; i++){

                AnimationFrame *af = new AnimationFrame();
                int indexGraphicItem = stream.ReadInt();
                if(indexGraphicItem==saveID_DUMMY_EMPTY_FRAME){
                    af->image = nullptr;

                }else{

                    if(indexGraphicItem<0 || indexGraphicItem >= ss->graphicItems.size()){
                        error = "_BuildFrameAnimations error! Animation: "+fa->name+"  IndexGraphicItem:"+std::to_string(indexGraphicItem);
                        delete af;
                        delete fa;
                        return nullptr;
                    }
                    af->image = ss->graphicItems[indexGraphicItem];
                }

                af->duration = stream.ReadInt();
                af->offset.x = stream.ReadInt();
                af->offset.y = stream.ReadInt();
                af->flip.x = stream.ReadInt();
                af->flip.y = stream.ReadInt();
                af->dataFlags = stream.ReadInt();
                for(int i=0; i<3; i++){
                    stream.ReadInt();
                }

                //---
                fa->frames.push_back(af);
            }

        }else if(signature==SaveSignature::PARAMETERS){

            LoadParameters(stream, fa->parameters);

        }else{
            messages.push_back("LoadFrameAnimation - unknown signature: " + std::to_string(signature));
            stream.Seek(posChunk+sizeChunk);
        }

        //---
        int dSize = stream.Pos()-posChunk;
        if(dSize!=sizeChunk){
            error = "LoadFrameAnimation error! Wrong chunkSize for signature:" + std::to_string(signature);
            delete fa;
            return nullptr;
        }
        sizeCounter-=(dSize+8);   //8 for two integers - 'signature' and 'size'
    }

    if(stream.Pos()-posStart!=size){
        error = "LoadFrameAnimation error! Wrong size: " + std::to_string(stream.Pos()-posStart) + "  saved: " + std::to_string(size);
        delete fa;
        return nullptr;
    }

    return fa;

}


bool JugiMapBinaryLoader::LoadParameters(BinaryStreamReader &stream, std::vector<Parameter> & parameters)
{

    int nParameters = stream.ReadInt();
    for(int i=0; i<nParameters; i++){

        Parameter pv;

        bool pvActive = (bool)stream.ReadByte();

        pv.kind = stream.ReadByte();
        pv.name = stream.ReadStringWithWrittenLength();
        pv.value = stream.ReadStringWithWrittenLength();

        if(pvActive){
            parameters.push_back(pv);
        }
    }

    return true;

}


void JugiMapBinaryLoader::InitCollisionShapes(GraphicItem *gi)
{

    // This function perform two operations:
    // 1) Store coolision shapes vertices relative to image's handle point (exported from the editor are stored relative to the image's top left corner)
    // 2) The first suitable shape will be stored as the default shape for collision detection
    /*
    for(VectorShape *vs : gi->collisionShapes){

        if(vs->GetKind()==ShapeKind::POLYLINE){
            PolyLineShape *poly = static_cast<PolyLineShape *>(vs->GetGeometricShape());

            for(Vec2f &v : poly->vertices){
                if(configuration.IsYCoordinateUp()){
                    v.y = gi->GetHeight()-v.y;
                }

                v = v - gi->handle;
            }

            //----
            if(poly->rectangle==false){
                bool CW = false;
                poly->convex = IsPolygonConvex(poly->vertices,CW);
                if(poly->convex && CW){
                    std::reverse(poly->vertices.begin(), poly->vertices.end());     // order must be CCW
                }
            }

            //----
            if(gi->collisionShape==nullptr){
                if(vs->probe==false && poly->convex && poly->vertices.size()>=3 && poly->vertices.size()<12){
                    gi->collisionShape = vs;
                }
            }


        }else if(vs->GetKind()==ShapeKind::ELLIPSE){
            EllipseShape *ellipse = static_cast<EllipseShape *>(vs->GetGeometricShape());
            if(configuration.IsYCoordinateUp()){
                ellipse->center.y = gi->GetHeight()-ellipse->center.y;
            }
            ellipse->center = ellipse->center - gi->handle;

            if(gi->collisionShape==nullptr){
                if(vs->probe==false && AreSame(ellipse->a, ellipse->b)){        // must be circle
                    gi->collisionShape = vs;
                }
            }

        }else if(vs->GetKind()==ShapeKind::SINGLE_POINT){
            SinglePointShape *sp = static_cast<SinglePointShape *>(vs->GetGeometricShape());
            if(configuration.IsYCoordinateUp()){
                sp->point.y = gi->GetHeight()-sp->point.y;
            }
            sp->point = sp->point - gi->handle;

        }
    }
    */

    std::vector<VectorShape*>allShapes;
    for(VectorShape *vs : gi->spriteShapes) allShapes.push_back(vs);
    for(VectorShape *vs : gi->probeShapes) allShapes.push_back(vs);

    //---
    for(VectorShape *vs : allShapes){

        if(vs->GetKind()==ShapeKind::POLYLINE){
            PolyLineShape *poly = static_cast<PolyLineShape *>(vs->GetGeometricShape());

            for(Vec2f &v : poly->vertices){
                if(settings.IsYCoordinateUp()){
                    v.y = gi->GetHeight()-v.y;
                }

                v = v - gi->handle;
            }

            //----
            //if(poly->rectangle==false){
            //    bool CW = false;
            //    poly->convex = IsPolygonConvex(poly->vertices,CW);
            //    if(poly->convex && CW){
            //        std::reverse(poly->vertices.begin(), poly->vertices.end());     // order must be CCW
            //    }
            //}

            //----
            //if(gi->collisionShape==nullptr){
            //    if(vs->probe==false && poly->convex && poly->vertices.size()>=3 && poly->vertices.size()<12){
            //        gi->collisionShape = vs;
            //    }
            //}


        }else if(vs->GetKind()==ShapeKind::ELLIPSE){
            EllipseShape *ellipse = static_cast<EllipseShape *>(vs->GetGeometricShape());
            if(settings.IsYCoordinateUp()){
                ellipse->center.y = gi->GetHeight()-ellipse->center.y;
            }
            ellipse->center = ellipse->center - gi->handle;

            //if(gi->collisionShape==nullptr){
            //    if(vs->probe==false && AreSame(ellipse->a, ellipse->b)){        // must be circle
            //        gi->collisionShape = vs;
            //    }
            //}

        }else if(vs->GetKind()==ShapeKind::SINGLE_POINT){
            SinglePointShape *sp = static_cast<SinglePointShape *>(vs->GetGeometricShape());
            if(settings.IsYCoordinateUp()){
                sp->point.y = gi->GetHeight()-sp->point.y;
            }
            sp->point = sp->point - gi->handle;

        }
    }
}




}
