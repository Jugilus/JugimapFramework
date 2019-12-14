
//===================================================================================================
//      JugiMap API extension for nCIne.
//===================================================================================================

#include <assert.h>
#include <algorithm>
#include <jugimapNCINE/jmNCine.h>
#include "jmNCine.h"



namespace jugimap {




GraphicFileNC::~GraphicFileNC()
{
    if(ncTexture) delete ncTexture;
}


void GraphicFileNC::Init()
{
    if(ncTexture) return;            // already loaded

    GraphicFile::Init();


    if(GetKind()==FileKind::SPINE_FILE){

        // spine atlas file: GetSpineAtlasRelativeFilePath()
        // spine skeleton file: GetRelativeFilePath()

    }else if(GetKind()==FileKind::SPRITER_FILE){

        // spriter model file: GetRelativeFilePath()

    }else{      // image

        ncTexture = new ncine::Texture(GetRelativeFilePath().c_str());
        assert(ncTexture);
    }
}


//===================================================================================================



void StandardSpriteNC::InitEngineSprite()
{

    if(ncSprite) return;             //already initialized

    GetSourceSprite()->Init();


    //---
    GraphicItem *gi = GetSourceSprite()->GetGraphicItems().front();    // Create sprite with the first image.
    GraphicFileNC *gf = static_cast<GraphicFileNC*>(gi->GetFile());

    ncSprite = new ncine::Sprite(gf->GetNCTexture());
    ncSprite->setLayer(GetLayer()->GetZOrder());

    static_cast<MapNC*>(GetLayer()->GetMap())->GetMapNCNode()->addChildNode(ncSprite);      // add ccSprite node to map node

    if(settings.SpriteCollidersEnabled()){
        if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_IMAGES_WITH_SPRITE_SHAPES){
            if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_MULTIPLE_SPRITE_SHAPES_IN_IMAGE){
                SetCollider(new MultiShapesCollider(this));
            }else{
                SetCollider(new SingleShapeCollider(this));
            }
        }
    }

    //---
    SetActiveImage(gi);

    UpdateEngineSprite();
    //---
    SetChangeFlags(Property::ALL);

}


void StandardSpriteNC::SetActiveImage(GraphicItem *_image)
{

    if(GetActiveImage()==_image) return;

    StandardSprite::SetActiveImage(_image);

    //----
    GraphicItem *image = GetActiveImage();
    if(image==nullptr){                                     // image can also be nullptr!
        if(ncSprite->drawEnabled()){
            ncSprite->setDrawEnabled(false);
        }
        return;

    }else{
        if(ncSprite->drawEnabled()==false && IsVisible()){
            ncSprite->setDrawEnabled(true);
        }
    }


    GraphicFileNC * gf = static_cast<GraphicFileNC*>(image->GetFile());
    ncSprite->setTexture(gf->GetNCTexture());
    ncine::Recti rect(GetActiveImage()->GetPos().x, GetActiveImage()->GetPos().y, GetActiveImage()->GetWidth(), GetActiveImage()->GetHeight());
    ncSprite->setTexRect(rect);

    SetColliderShapes();

}


void StandardSpriteNC::UpdateEngineSprite()
{

    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();



    if(flags & Property::TRANSFORMATION){

        Vec2f posWorld = GetFullGlobalPosition();
        posWorld += GetLayer()->GetParallaxOffset();

        Vec2f scaleWorld = GetGlobalScale();
        float rotationWorld = GetGlobalRotation();
        Vec2i flipWorld = GetGlobalFlip();

        if(flags & Property::HANDLE){
            if(GetActiveImage()){
                ncine::Vector2f anchorPoint(GetHandle().x - GetActiveImage()->GetWidth() * 0.5f, GetHandle().y - GetActiveImage()->GetHeight()*0.5f);
                ncSprite->setAbsAnchorPoint(anchorPoint);
            }
        }
        if(flags & Property::POSITION){
            ncSprite->setPosition(posWorld.x, posWorld.y);
        }
        if(flags & Property::SCALE){
            ncSprite->setScale(scaleWorld.x, scaleWorld.y);
        }
        if(flags & Property::ROTATION){
            ncSprite->setRotation(-rotationWorld);
        }
        if(flags & Property::FLIP){
            ncSprite->setScale((flipWorld.x==1)? -scaleWorld.x : scaleWorld.x, (flipWorld.y==1)? -scaleWorld.y : scaleWorld.y);
        }

        UpdateColliderAndBoundingBox();
    }


    if(flags & Property::APPEARANCE){

        if(flags & Property::OVERLAY_COLOR){
            ManageShaders_OverlayColor();
        }

        if(flags & Property::ALPHA){
            ncSprite->setAlpha(GetAlpha()*GetLayer()->GetAlpha()*255);
        }

        if(flags & Property::BLEND){
            Blend blend = GetOwnBlend();
            if(blend==Blend::NOT_DEFINED) blend = GetLayer()->GetBlend();

            if(blend==Blend::ALPHA){
                ncSprite->setBlendingFactors(ncine::DrawableNode::BlendingFactor::SRC_ALPHA,  ncine::DrawableNode::BlendingFactor::ONE_MINUS_SRC_ALPHA);

            }else if(blend==Blend::MULTIPLY){
                ncSprite->setBlendingFactors(ncine::DrawableNode::BlendingFactor::DST_COLOR,  ncine::DrawableNode::BlendingFactor::ZERO);

            }else if(blend==Blend::ADDITIVE){
                ncSprite->setBlendingFactors(ncine::DrawableNode::BlendingFactor::SRC_ALPHA,  ncine::DrawableNode::BlendingFactor::ONE);

            }else if(blend==Blend::SOLID){
                ncSprite->setBlendingFactors(ncine::DrawableNode::BlendingFactor::ONE,  ncine::DrawableNode::BlendingFactor::ZERO);

            }
        }
    }

    //---
    SetChangeFlags(0);
}


void StandardSpriteNC::SetVisible(bool _visible)
{
    if(IsVisible()==_visible) return;

    StandardSprite::SetVisible(_visible);

    if(_visible){
        if(GetActiveImage()){
            ncSprite->setDrawEnabled(true);
        }
        //    SetChangeFlags(Change::ALL);

    }else{
        ncSprite->setDrawEnabled(false);
    }

}


void StandardSpriteNC::ManageShaders_OverlayColor()
{


}




//===================================================================================================


void SpriteLayerNC::RemoveAndDeleteSprite(Sprite* _sprite)
{

    if(_sprite->GetKind()==SpriteKind::STANDARD){
        //static_cast<MapNC*>(GetMap())->GetMapNCNode()->removeChildNode(static_cast<StandardSpriteNC*>(_sprite)->GetNCSprite());
        DeleteNCNode(static_cast<StandardSpriteNC*>(_sprite)->GetNCSprite());

    }else if(_sprite->GetKind()==SpriteKind::COMPOSED){


    }else if(_sprite->GetKind()==SpriteKind::SPINE){


    }else if(_sprite->GetKind()==SpriteKind::SPRITER){

    }

    SpriteLayer::RemoveAndDeleteSprite(_sprite);

}



//===================================================================================================



FontNC::FontNC(const std::string &_relativeFilePath, int _size, FontKind _kind)
    : Font(_relativeFilePath, _size, _kind)
{
    ncFont = new ncine::Font(GetRelativeFilePath().c_str());
    if(ncFont==nullptr){
        assert(true);
    }
}


FontNC::~FontNC()
{
    delete ncFont;
}



//===================================================================================================



void TextNC::InitEngineText()
{
    if(ncTextNode) return;             //already initialized

    ncTextNode = new ncine::TextNode(static_cast<MapNC*>(GetLayer()->GetMap())->GetMapNCNode(), static_cast<FontNC*>(GetFont())->GetNCFont());
    ncTextNode->setString(GetTextString().c_str());
    _SetSize(Vec2f(ncTextNode->width(), ncTextNode->height()));
    ncTextNode->setAnchorPoint(ncine::DrawableNode::AnchorBottomLeft);
    ncTextNode->setLayer(GetLayer()->GetZOrder());

    //---
    SetChangeFlags(Property::ALL);
}


void TextNC::UpdateEngineText()
{

    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineTextUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TEXT_STRING){
        ncTextNode->setString(GetTextString().c_str());
        _SetSize(Vec2f(ncTextNode->width(), ncTextNode->height()));
        //ncTextNode->setAnchorPoint(ncine::DrawableNode::AnchorBottomLeft);
    }
    if(flags & Property::POSITION){
        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();
        ncTextNode->setPosition(posGlobal.x, posGlobal.y);
    }
    if(flags & Property::COLOR){
        ncTextNode->setColor(GetColor().r, GetColor().g, GetColor().b, GetColor().a);
    }
    if(flags & Property::ALPHA){
        ncTextNode->setAlphaF(GetAlpha());
    }

    //---
    SetChangeFlags(0);
}


//===================================================================================================


void TextLayerNC::RemoveAndDeleteText(Text* _text)
{

    DeleteNCNode(static_cast<TextNC*>(_text)->GetNCTextNode());

    TextLayer::RemoveAndDeleteText(_text);

}


//===================================================================================================


void MapNCNode::visit(ncine::RenderQueue &renderQueue)
{

    //map->UpdateEngineMap();
    ncine::SceneNode::visit(renderQueue);

}


//===================================================================================================


void MapNC::InitEngineMap()
{

    mMapNCNode = new MapNCNode(this);     // First create cocos2d node as it will be used during sprite initialization!
    Map::InitEngineMap();

}



void MapNC::UpdateEngineMap()
{

    Map::UpdateEngineMap();

    mMapNCNode->setRotation(-GetCamera()->GetRotation());
    mMapNCNode->setScale(GetCamera()->GetScale());
    mMapNCNode->setPosition(GetCamera()->GetProjectedMapPosition().x, GetCamera()->GetProjectedMapPosition().y);

}

//===================================================================================================



void DeleteNCNode(ncine::SceneNode* _node)
{

    if(deleteNCNodesSpecial){
        nctl::List<ncine::SceneNode *>children = _node->children();     // Children list as copy (not reference)!
        for(ncine::SceneNode* child : children){
            DeleteNCNode(child);
        }
        delete _node;

    }else{
        delete _node;
    }
}

bool deleteNCNodesSpecial = false;

ncine::TextNode * errorMessageLabel = nullptr;


}



