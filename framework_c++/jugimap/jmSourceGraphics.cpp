#include "jmVectorShapes.h"
#include "jmVectorShapesUtilities.h"
#include "jmFrameAnimation.h"
#include "jmTimelineAnimation.h"
#include "jmSourceGraphics.h"




namespace jugimap {





void GraphicItem::Init()
{
    graphicFile->Init();
}



GraphicItem::~GraphicItem()
{
    for(VectorShape *vs : spriteShapes){
        delete vs;
    }
    for(VectorShape *vs : extraShapes){
        delete vs;
    }
}


//================================================================================================================


std::string GraphicFile::pathPrefix;


GraphicFile::~GraphicFile()
{
    for(GraphicItem *i : items){
        delete i;
    }
}


//================================================================================================================


SourceSprite::~SourceSprite()
{
    for(FrameAnimation* fa : frameAnimations){
        delete fa;
    }
    for(TimelineAnimation* ta : timelineAnimations){
        delete ta;
    }

}


void SourceSprite::Init()
{

    if(initialized) return;

    for(GraphicItem *im : graphicItems){
        im->Init();
    }


    if(name=="tilesA,3"){
        DummyFunction();
    }

    //--- status flags
    if(graphicItems.size()>1){

        for(int i=0; i<int(graphicItems.size())-1; i++){
            GraphicItem * gi = graphicItems[i];
            GraphicItem * gi2 = graphicItems[i+1];

            //if(gi->width!=gi2->width || gi->height!=gi2->height){
            if(gi->GetSize() != gi2->GetSize()){
                statusFlags |= Status::HAS_IMAGES_OF_DIFFERENT_SIZE;
            }

            if(gi->GetSpriteShapes().empty() && gi2->GetSpriteShapes().empty()){
                //empty

            }else if(gi->GetSpriteShapes().empty()==false && gi2->GetSpriteShapes().empty()==false){
                statusFlags |= Status::HAS_IMAGES_WITH_SPRITE_SHAPES;

                //if(GeometricShapesEqual(gi->collisionShape->GetGeometricShape(), gi2->collisionShape->GetGeometricShape())==false){
                if(SameGeometricShapes(gi->GetSpriteShapes(), gi2->GetSpriteShapes())==false){
                    statusFlags |= Status::HAS_IMAGES_WITH_DIFFERENT_SPRITE_SHAPES;
                }

            }else{

                statusFlags = statusFlags | Status::HAS_IMAGES_WITH_SPRITE_SHAPES | Status::HAS_IMAGES_WITH_DIFFERENT_SPRITE_SHAPES;

            }

            if(gi->GetSpriteShapes().size()>1){
                statusFlags |= Status::HAS_MULTIPLE_SPRITE_SHAPES_IN_IMAGE;
            }
        }


    }else if(graphicItems.size()==1){

        GraphicItem * gi = graphicItems[0];
        if(gi->GetSpriteShapes().empty()==false){
            statusFlags |= Status::HAS_IMAGES_WITH_SPRITE_SHAPES;

            if(gi->GetSpriteShapes().size()>1){
                statusFlags |= Status::HAS_MULTIPLE_SPRITE_SHAPES_IN_IMAGE;
            }
        }

    }

    initialized = true;
}



//================================================================================================================


SourceSprite * SourceSpriteReplacements::FindSourceSpriteForChild(const std::string &_childSpriteName)
{

    for(SourceSpriteReplacement &r : replacements){
        if(r.childSpriteName==_childSpriteName){
            return r.sourceSprite;
        }
    }

    return nullptr;
}


SourceSprite* SourceGraphics::dummyTextSourceSprite = new SourceSprite(SpriteKind::TEXT);


//================================================================================================================


SourceGraphics::~SourceGraphics()
{
    DeleteContent();
}



void SourceGraphics::DeleteContent()
{
    for(GraphicFile* imf : files) delete imf;
    files.clear();

    for(SourceSprite* ss : sourceSprites) delete ss;
    sourceSprites.clear();
}



void SourceGraphics::DeleteGlobalData()
{
    delete dummyTextSourceSprite;

}




}



