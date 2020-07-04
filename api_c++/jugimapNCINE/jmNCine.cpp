
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
        if(ncSprite->isDrawEnabled()){
            ncSprite->setDrawEnabled(false);
        }
        return;

    }else{
        if(ncSprite->isDrawEnabled()==false && IsVisible()){
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

        Vec2f posWorld = GetGlobalPosition();
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
        //DeleteNCNode(static_cast<StandardSpriteNC*>(_sprite)->GetNCSprite());

        delete static_cast<StandardSpriteNC*>(_sprite)->GetNCSprite();

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

    //DeleteNCNode(static_cast<TextNC*>(_text)->GetNCTextNode());

    delete static_cast<TextNC*>(_text)->GetNCTextNode();

    TextLayer::RemoveAndDeleteText(_text);

}


//===================================================================================================


void DrawerNC::InitEngineDrawer()
{


}


void DrawerNC::UpdateEngineDrawer()
{

    //Camera *cam = GetDrawingLayer()->GetMap()->GetCamera();
    //ImGui::SetNextWindowPos(ImVec2(cam->GetViewport().min.x, cam->GetViewport().min.y));
    //ImGui::SetNextWindowSize(ImVec2(cam->GetViewport().Width(), cam->GetViewport().Height()));

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(jugimap::settings.GetScreenSize().x, jugimap::settings.GetScreenSize().y));

    ImGui::Begin("My shapes", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
    drawList = ImGui::GetWindowDrawList();

    GetDrawingLayer()->DrawEngineLayer();

    ImGui::End();
}


void DrawerNC::Clear()
{


}


void DrawerNC::SetOutlineColor(ColorRGBA _outlineColor)
{
    outlineColor = ImColor(_outlineColor.r, _outlineColor.g, _outlineColor.b, _outlineColor.a);
}


void DrawerNC::Line(Vec2f p1, Vec2f p2)
{

    // we must camera transform the points
    Camera *cam = GetDrawingLayer()->GetMap()->GetCamera();
    p1 = cam->ScreenPointFromMapPoint(p1);
    p2 = cam->ScreenPointFromMapPoint(p2);

    drawList->AddLine(ImVec2(p1.x, ImGui::GetIO().DisplaySize.y-p1.y), ImVec2(p2.x, ImGui::GetIO().DisplaySize.y-p2.y), outlineColor);

}


void DrawerNC::RectangleOutline(const Rectf &rect)
{

    Line(rect.min, Vec2f(rect.max.x, rect.min.y));
    Line(Vec2f(rect.max.x, rect.min.y), rect.max);
    Line(rect.max, Vec2f(rect.min.x, rect.max.y));
    Line(Vec2f(rect.min.x, rect.max.y), rect.min);

}


void DrawerNC::EllipseOutline(Vec2f c, Vec2f r)
{

    float ra = (std::fabs(r.x) + std::fabs(r.y)) / 2;
    float da = std::acos(ra / (ra + 0.125 /1.0)) * 2;
    int n = std::round(2*mathPI / da + 0.5);

    float pxPrev = c.x + std::cos(0) * r.x;
    float pyPrev = c.y + std::sin(0) * r.y;

    for(int i=1; i<=n; i++){
        float angle = i * 2*mathPI/n;
        float px = c.x + std::cos( angle ) * r.x;
        float py = c.y + std::sin( angle ) * r.y;
        Line(Vec2f(pxPrev,pyPrev), Vec2f(px,py));
        pxPrev = px;
        pyPrev = py;

    }
}


void DrawerNC::Dot(Vec2f p)
{

    p = GetDrawingLayer()->GetMap()->GetCamera()->ScreenPointFromMapPoint(p);
    drawList->AddCircleFilled(ImVec2(p.x, ImGui::GetIO().DisplaySize.y-p.y), 2, outlineColor, 6);

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


/*
void DeleteNCNode(ncine::SceneNode* _node)
{

    if(deleteNCNodesSpecial){
<<<<<<< HEAD
        nctl::Array<ncine::SceneNode *>children = _node->children();     // Children list as copy (not reference)!
=======
        nctl::Array<ncine::SceneNode *>children = _node->children();     // Children array as copy (not reference)!
>>>>>>> be1dcb3fb6016be6a2749d8b95db80c145dc1d70
        for(ncine::SceneNode* child : children){
            DeleteNCNode(child);
        }
        delete _node;

    }else{
        delete _node;
    }
}

bool deleteNCNodesSpecial = false;
*/

ncine::TextNode * errorMessageLabel = nullptr;


}



