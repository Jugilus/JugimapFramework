
//===================================================================================================
//      JugiMap API extension for SFML.
//===================================================================================================


#include <algorithm>
#include "jmAGK.h"




namespace jugimap {



GraphicItemAGK::~GraphicItemAGK()
{
    if(agkId!=0){
        agk::DeleteImage(agkId);
        agkId = 0;
    }
}


void GraphicItemAGK::Init()
{
    if(agkId != 0) return;     // safety - already initialized


    GraphicFileAGK *gf = static_cast<GraphicFileAGK*>(GetFile());

    gf->Init();

    if(gf->GetKind()==FileKind::SPINE_FILE ){
        //empty

    }else if(gf->GetKind()==FileKind::SPRITER_FILE){
        //empty)

    }else if(gf->GetKind()==FileKind::SINGLE_IMAGE){
        agkId = gf->GetAgkId();

    }else{
        agkId = agk::LoadSubImage(gf->GetAgkId(), GetName().c_str());
        assert(agkId!=0);
    }

}


//===================================================================================================


GraphicFileAGK::~GraphicFileAGK()
{
    if(agkId!=0){
        if(GetKind() != FileKind::SINGLE_IMAGE){
            agk::DeleteImage(agkId);
        }
    }
}


void GraphicFileAGK::Init()
{
    if(agkId != 0) return;     // safety - already initialized

    GraphicFile::Init();


    if(GetKind()==FileKind::SPINE_FILE){

        // spineAtlasRelativeFilePath is path to .atlas file
        // AGK require loaded atlas image file which must have the same name as .atlas file

        std::string imagePath = GetSpineAtlasRelativeFilePath();
        const size_t indexPeriod = imagePath.rfind('.');
        if (indexPeriod != std::string::npos){
            imagePath.erase(indexPeriod);
        }
        imagePath += ".png";

        agkId = agk::LoadImage(imagePath.c_str());
        assert(agkId != 0);

    }else if(GetKind()==FileKind::SPRITER_FILE){
        // empty

    }else{
        agkId = agk::LoadImage(GetRelativeFilePath().c_str());
        assert(agkId != 0);
    }
}


//===================================================================================================


StandardSpriteAGK::~StandardSpriteAGK ()
{
    if(agkId!=0){
        agk::DeleteSprite(agkId);
    }
}


void StandardSpriteAGK::InitEngineSprite()
{
    if(agkId != 0) return;     //already initialized


    GetSourceSprite()->Init();

    //----
    GraphicItemAGK *gi = static_cast<GraphicItemAGK*>(GetSourceSprite()->GetGraphicItems().front());    // Create sprite with the first image.
    agkId = agk::CreateSprite(gi->GetAgkId());

    agk::SetSpriteDepth(agkId, GetLayer()->GetZOrder());
    if(GetLayer()->GetMap()->GetType()==MapType::SCREEN){
        agk::FixSpriteToScreen(agkId, 1);
    }


    if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_IMAGES_WITH_SPRITE_SHAPES){

        if(GetLayer()->GetMap()->GetType()==MapType::SCREEN){

            // Agk collision doesn't work for screen maps so we use jugimap colliders;

            if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_MULTIPLE_SPRITE_SHAPES_IN_IMAGE){
                SetCollider(new MultiShapesCollider(this));
            }else{
                SetCollider(new SingleShapeCollider(this));
            }

        }else{

            // For world maps we make colliders which utilize AGK collision detection. This way we can
            // also use sprites with AGK physics and access AGK collision functions.

            SetCollider(new ColliderAGK(this));
        }

    }

    SetActiveImage(gi);

    //----
    SetChangeFlags(Property::ALL);
}


void StandardSpriteAGK::SetActiveImage(GraphicItem *_image)
{

    if(GetActiveImage()==_image) return;


    //---- get current texture size
    Vec2i sizePrevImage(-1,-1);
    GraphicItemAGK *prevImage = dynamic_cast<GraphicItemAGK*>(GetActiveImage());
    if(prevImage){
        sizePrevImage.x = prevImage->GetWidth();
        sizePrevImage.y = prevImage->GetHeight();
    }


    StandardSprite::SetActiveImage(_image);

    //----
    GraphicItemAGK *image = dynamic_cast<GraphicItemAGK*>(GetActiveImage());

    if(image==nullptr){                         // _image can be also nullptr!
        if(agk::GetSpriteVisible(agkId)){
            agk::SetSpriteVisible(agkId, 0);
        }
        return;
    }


    //--- set AGK sprite properties
    if(agk::GetSpriteVisible(agkId)==0){
        agk::SetSpriteVisible(agkId, 1);
    }

    //----
    if(GetCollider() && GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_IMAGES_WITH_DIFFERENT_SPRITE_SHAPES){
        agk::SetSpriteShape(agkId, 0);          // remove any shape in advance to spare time of transfoming it by SetSpriteSize command
    }

    agk::SetSpriteImage(agkId, image->GetAgkId(), 0);

    if(image->GetWidth()!=sizePrevImage.x || image->GetHeight()!=sizePrevImage.y){
        agk::SetSpriteSize(agkId, image->GetWidth(), image->GetHeight());
    }

    agk::SetSpriteOffset(agkId, GetHandle().x*agk::GetSpriteScaleX(agkId), GetHandle().y*agk::GetSpriteScaleY(agkId));
    // changeFlags &= ~Property::HANDLE;
    SetChangeFlags(GetChangeFlags() & (~Property::HANDLE));     // clear the handle bit

    SetColliderShapes();

}


void StandardSpriteAGK::UpdateEngineSprite()
{
    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TRANSFORMATION){

        Vec2f posWorld = GetFullGlobalPosition();
        posWorld = posWorld + GetLayer()->GetParallaxOffset();

        Vec2f scaleWorld = GetGlobalScale();
        float rotationWorld = GetGlobalRotation();
        Vec2i flipWorld = GetGlobalFlip();

        // Sprites in screen maps are not affected by AGK view tranformation so we need to include camera transformation for every sprite here
        if(GetLayer()->GetMap()->GetCamera()->GetKind()==Camera::Kind::SCREEN){
            ScreenMapCamera *camera = static_cast<ScreenMapCamera*>(GetLayer()->GetMap()->GetCamera());
            posWorld = camera->ScreenPointFromMapPoint(posWorld);
            scaleWorld = scaleWorld * camera->GetScale();
            rotationWorld = rotationWorld + camera->GetRotation();
        }

        if(flags & Property::HANDLE){
            agk::SetSpriteOffset(agkId, GetHandle().x*agk::GetSpriteScaleX(agkId), GetHandle().y*agk::GetSpriteScaleY(agkId));
        }

        if(flags & Property::SCALE){
            agk::SetSpriteScale(agkId, scaleWorld.x, scaleWorld.y);
        }

        if(flags & Property::ROTATION){
            agk::SetSpriteAngle(agkId, -rotationWorld);
        }

        if(flags & Property::FLIP){
            agk::SetSpriteFlip(agkId, flipWorld.x, flipWorld.y);
        }

        if(flags & Property::POSITION){
            agk::SetSpritePositionByOffset(agkId, posWorld.x, posWorld.y);      // This must be the last (apparently other commands affect agk offset) - bug in AGK !!
        }


        UpdateColliderAndBoundingBox();
    }


    if(flags & Property::APPEARANCE){

        if(flags & Property::OVERLAY_COLOR){
            ManageShaders_OverlayColor();
        }

        if(flags & Property::ALPHA){
            agk::SetSpriteColorAlpha(agkId, GetAlpha() * GetLayer()->GetAlpha()*255);
        }

        if(flags & Property::BLEND){
            Blend blend = GetOwnBlend();
            if(blend==Blend::NOT_DEFINED) blend = GetLayer()->GetBlend();

            if(blend==Blend::ALPHA){
                agk::SetSpriteTransparency(agkId, 1);

            }else if(blend==Blend::ADDITIVE){
                agk::SetSpriteTransparency(agkId, 2);

            }else if(blend==Blend::SOLID){
                agk::SetSpriteTransparency(agkId, 0);

            }else{
                agk::SetSpriteTransparency(agkId, 1);
            }
        }
    }

    //---
    SetChangeFlags(0);
}


void StandardSpriteAGK::SetVisible(bool _visible)
{

    if(IsVisible()==_visible) return;

    StandardSprite::SetVisible(_visible);

    if(_visible){
        if(GetActiveImage()){
            agk::SetSpriteVisible(agkId, 1);
        }
        //    SetChangeFlags(Change::ALL);

    }else{
        agk::SetSpriteVisible(agkId, 0);
    }
}


void StandardSpriteAGK::ManageShaders_OverlayColor()
{


    if(IsOverlayColorActive()==false){
        shaderAgkId = 0;
        agk::SetSpriteShader(agkId, 0);
        return;

    }else{
        ColorRGBA colorRGBA = GetOverlayColorRGBA();

        switch (GetOverlayColorBlend()) {

        case ColorOverlayBlend::SIMPLE_MULTIPLY:

            if(shaders::colorBlend_SimpleMultiply->agkId==0) return;

            if(colorRGBA!=shaders::colorBlend_SimpleMultiply->colorRGBA || shaderAgkId!=shaders::colorBlend_SimpleMultiply->agkId){
                shaderAgkId = shaders::colorBlend_SimpleMultiply->agkId;
                shaders::colorBlend_SimpleMultiply->colorRGBA = colorRGBA;
                ColorRGBAf c(colorRGBA);
                agk::SetShaderConstantByName(shaderAgkId, shaders::colorBlend_SimpleMultiply->colorRGBAname.c_str(), c.r, c.g, c.b, c.a);
                agk::SetSpriteShader(agkId, shaderAgkId);
            }

            break;

        case ColorOverlayBlend::NORMAL:

            if(shaders::colorBlend_Normal->agkId==0) return;

            if(colorRGBA!=shaders::colorBlend_Normal->colorRGBA || shaderAgkId!=shaders::colorBlend_Normal->agkId){
                shaderAgkId = shaders::colorBlend_Normal->agkId;
                shaders::colorBlend_Normal->colorRGBA = colorRGBA;
                ColorRGBAf c(colorRGBA);
                agk::SetShaderConstantByName(shaderAgkId, shaders::colorBlend_Normal->colorRGBAname.c_str(), c.r, c.g, c.b, c.a);
                agk::SetSpriteShader(agkId, shaderAgkId);
            }

            break;

        case ColorOverlayBlend::MULTIPLY:

            if(shaders::colorBlend_Multiply->agkId==0) return;

            if(colorRGBA!=shaders::colorBlend_Multiply->colorRGBA || shaderAgkId!=shaders::colorBlend_Multiply->agkId){
                shaderAgkId = shaders::colorBlend_Multiply->agkId;
                shaders::colorBlend_Multiply->colorRGBA = colorRGBA;
                ColorRGBAf c(colorRGBA);
                agk::SetShaderConstantByName(shaderAgkId, shaders::colorBlend_Multiply->colorRGBAname.c_str(), c.r, c.g, c.b, c.a);
                agk::SetSpriteShader(agkId, shaderAgkId);
            }

            break;

        case ColorOverlayBlend::LINEAR_DODGE:

            if(shaders::colorBlend_LinearDodge->agkId==0) return;

            if(colorRGBA!=shaders::colorBlend_LinearDodge->colorRGBA || shaderAgkId!=shaders::colorBlend_LinearDodge->agkId){
                shaderAgkId = shaders::colorBlend_LinearDodge->agkId;
                shaders::colorBlend_LinearDodge->colorRGBA = colorRGBA;
                ColorRGBAf c(colorRGBA);
                agk::SetShaderConstantByName(shaderAgkId, shaders::colorBlend_LinearDodge->colorRGBAname.c_str(), c.r, c.g, c.b, c.a);
                agk::SetSpriteShader(agkId, shaderAgkId);
            }

            break;

        case ColorOverlayBlend::COLOR:

            if(shaders::colorBlend_Color->agkId==0) return;

            if(colorRGBA!=shaders::colorBlend_Color->colorRGBA || shaderAgkId!=shaders::colorBlend_Color->agkId){
                shaderAgkId = shaders::colorBlend_Color->agkId;
                shaders::colorBlend_Color->colorRGBA = colorRGBA;
                ColorRGBAf c(colorRGBA);
                agk::SetShaderConstantByName(shaderAgkId, shaders::colorBlend_Color->colorRGBAname.c_str(), c.r, c.g, c.b, c.a);
                agk::SetSpriteShader(agkId, shaderAgkId);
            }

            break;

        default:
            shaderAgkId==0;
            agk::SetSpriteShader(agkId, 0);
        }

    }

}


void StandardSpriteAGK::SetPhysicsMode(PhysicsMode _physicsMode)
{
    if(_physicsMode==GetPhysicsMode()) return;
    StandardSprite::SetPhysicsMode(_physicsMode);


    if(_physicsMode==PhysicsMode::NO_PHYSICS){
        agk::SetSpritePhysicsOff(agkId);

    }else if(_physicsMode==PhysicsMode::STATIC){
        agk::SetSpritePhysicsOn(agkId, 1);

    }else if(_physicsMode==PhysicsMode::DYNAMIC){
        agk::SetSpritePhysicsOn(agkId, 2);

    }else if(_physicsMode==PhysicsMode::KINEMATIC){
        agk::SetSpritePhysicsOn(agkId, 3);

    }

}


//===================================================================================================


ColliderAGK::ColliderAGK(StandardSprite* _standardSprite) : Collider(_standardSprite)
{
    agkId = static_cast<StandardSpriteAGK*>(GetStandardSprite())->GetAgkId();
}


void ColliderAGK::AddShapes()
{

    nShapes = 0;
    std::vector<VectorShape*>&shapes = GetStandardSprite()->GetActiveImage()->GetSpriteShapes();


    //--- Add shape
    GeometricShape *geomShape = shapes.front()->GetGeometricShape();

    if(geomShape->GetKind()==ShapeKind::POLYLINE){
        PolyLineShape *poly = static_cast<PolyLineShape *>(geomShape);
        if(poly->convex){

            if(poly->rectangle){
                agk::SetSpriteShapeBox(agkId, poly->vertices[0].x , poly->vertices[0].y, poly->vertices[2].x, poly->vertices[2].y, 0);

            }else{
                for(int i=0; i<poly->vertices.size(); i++){
                    agk::SetSpriteShapePolygon(agkId, poly->vertices.size(), i,  poly->vertices[i].x, poly->vertices[i].y);
                }
            }
            nShapes++;
        }

    }else if(geomShape->GetKind()==ShapeKind::ELLIPSE){
        EllipseShape *ellipse = static_cast<EllipseShape *>(geomShape);
        agk::SetSpriteShapeCircle(agkId, ellipse->center.x, ellipse->center.y, ellipse->a);
        nShapes++;
    }



    //--- Add additional shapes
    for(int i=1; i<int(shapes.size()); i++){

        geomShape = shapes[i]->GetGeometricShape();

        if(geomShape->GetKind()==ShapeKind::POLYLINE){
            PolyLineShape *poly = static_cast<PolyLineShape *>(geomShape);
            if(poly->convex){

                if(poly->rectangle){
                    agk::AddSpriteShapeBox(agkId, poly->vertices[0].x , poly->vertices[0].y, poly->vertices[2].x, poly->vertices[2].y, 0);

                }else{
                    for(int i=0; i<poly->vertices.size(); i++){
                        agk::AddSpriteShapePolygon(agkId, poly->vertices.size(), i,  poly->vertices[i].x, poly->vertices[i].y);
                    }
                }
                nShapes++;
            }

        }else if(geomShape->GetKind()==ShapeKind::ELLIPSE){
            EllipseShape *ellipse = static_cast<EllipseShape *>(geomShape);
            agk::AddSpriteShapeCircle(agkId, ellipse->center.x, ellipse->center.y, ellipse->a);
            nShapes++;
        }
    }

}


void ColliderAGK::UpdateShapes(const AffineMat3f &m)
{
    // empty
}


void ColliderAGK::ClearShapes()
{
    StandardSpriteAGK* sprAGK = static_cast<StandardSpriteAGK*>(GetStandardSprite());
    int agkId = sprAGK->GetAgkId();

    agk::SetSpriteShape(agkId, 0);
    nShapes = 0;

}


int ColliderAGK::GetShapesCount()
{
    return nShapes;
}


bool ColliderAGK::PointInside(Vec2f _pos)
{

    if(nShapes==0) return false;

    return (bool) agk::GetSpriteHitTest(agkId, _pos.x, _pos.y);

}


bool ColliderAGK::Overlaps(Collider *_otherCollider)
{
    if(nShapes==0 || static_cast<ColliderAGK*>(_otherCollider)->nShapes==0) return false;

    return (bool)agk::GetSpriteCollision(agkId,  static_cast<ColliderAGK*>(_otherCollider)->agkId);

}


bool ColliderAGK::RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos)
{
    if(nShapes==0) return false;

    if(agk::SpriteRayCastSingle(agkId, _rayStart.x, _rayStart.y, _rayEnd.x, _rayEnd.y)){
        _hitPos.x = agk::GetRayCastX();
        _hitPos.y = agk::GetRayCastY();
        return true;
    }

    return false;
}


//===================================================================================================

SkeletonSpriteAGK::~SkeletonSpriteAGK(){

    if(agkId!=0){
        agk::DeleteSkeleton2D(agkId);
    }
}


void SkeletonSpriteAGK::InitEngineSprite()
{
    if(agkId != 0) return;     //already initialized


    GetSourceSprite()->Init();


    GraphicItemAGK *gi = static_cast<GraphicItemAGK*>(GetSourceSprite()->GetGraphicItems().front());
    GraphicFileAGK *gf = static_cast<GraphicFileAGK*>(gi->GetFile());

    if(GetKind()==SpriteKind::SPINE){

        std::string spineFilePath = gf->GetRelativeFilePath();
        unsigned int agkAtlasId = gf->GetAgkId();
        float scale = gi->GetSpineScale();
        agkId = agk::LoadSkeleton2DFromSpineFile(spineFilePath.c_str(), scale, agkAtlasId, 0);
        assert(agkId != 0);

    }else if(GetKind()==SpriteKind::SPRITER){

        std::string spriterFilePath = gf->GetRelativeFilePath();
        spriterFilePath = "media/graphics/GreyGuy/player.scon";
        //unsigned int agkAtlasId = sourceSprite->graphicItems[0]->graphicFile->agkID;
        float scale = gi->GetSpineScale();
        agkId = agk::LoadSkeleton2DFromSpriterFile(spriterFilePath.c_str(), scale, 0);
        assert(agkId != 0);
    }

    agk::SetSkeleton2DDepth(agkId, GetLayer()->GetZOrder());
    agk::SetSkeleton2DFlip(agkId, GetFlip().x, GetFlip().y);

}


void SkeletonSpriteAGK::UpdateEngineSprite()
{
    agk::SetSkeleton2DPosition(agkId, GetPosition().x, GetPosition().y);
}


void SkeletonSpriteAGK::SetVisible(bool _visible)
{
    SkeletonSprite::SetVisible(_visible);
    if(_visible){
        agk::SetSkeleton2DVisible(agkId, 1);
    }else{
        agk::SetSkeleton2DVisible(agkId, 0);
    }
}


//===================================================================================================


void SpriteLayerAGK::UpdateEngineLayer()
{

    // Sprites on screen maps are not affected by the AGK view tranformation so we need to include camera transformation into engine sprites.
    ScreenMapCamera *cam = dynamic_cast<ScreenMapCamera*>(GetMap()->GetCamera());

    if(cam && cam->GetChangeFlags()!=0){

        if(cam->GetChangeFlags() & Camera::Change::POSITION){
            AppendToSpritesChangeFlag(Sprite::Property::POSITION);
        }

        if(cam->GetChangeFlags() & Camera::Change::SCALE){
            AppendToSpritesChangeFlag(Sprite::Property::SCALE);
        }

        if(cam->GetChangeFlags() & Camera::Change::ROTATION){
            AppendToSpritesChangeFlag(Sprite::Property::ROTATION);
        }
    }

    SpriteLayer::UpdateEngineLayer();

}


//===================================================================================================


FontAGK::FontAGK(const std::string &_relativeFilePath, int _size, FontKind _kind)
    : Font(_relativeFilePath, _size, _kind)
{
    agkId = agk::LoadFont(GetRelativeFilePath().c_str());
    assert(agkId!=0);
}


FontAGK::~FontAGK()
{
    agk::DeleteFont(agkId);
}


//===================================================================================================



TextAGK::~TextAGK()
{
    agk::DeleteText(agkId);
}


void TextAGK::InitEngineText()
{
    if(agkId != 0) return;             //already initialized

    agkId = agk::CreateText(GetTextString().c_str());
    agk::SetTextFont(agkId, static_cast<FontAGK*>(GetFont())->GetAgkId());
    agk::SetTextSize(agkId, GetFont()->GetSize());
    agk::SetTextDepth(agkId, GetLayer()->GetZOrder());

    if(GetLayer()->GetMap()->GetType()==MapType::SCREEN){
        agk::FixTextToScreen(agkId, 1);                 // layer belong to a screen map and as such will not be transformed via agk 'view' -> we need to fix texts on screen!
    }

    //---
    SetChangeFlags(Property::ALL);
}


void TextAGK::UpdateEngineText()
{

    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineTextUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TEXT_STRING){
         agk::SetTextString(agkId, GetTextString().c_str());
        _SetSize(Vec2f(agk::GetTextTotalWidth(agkId), agk::GetTextTotalHeight(agkId)));
    }
    if(flags & Property::POSITION){
        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();
        agk::SetTextPosition(agkId, posGlobal.x, posGlobal.y);
    }
    if(flags & Property::COLOR){
        agk::SetTextColor(agkId, GetColor().r, GetColor().g, GetColor().b, GetColor().a * GetAlpha());
    }
    if(flags & Property::ALPHA){
        agk::SetTextColor(agkId, GetColor().r, GetColor().g, GetColor().b, GetColor().a * GetAlpha());
    }

    //---
    SetChangeFlags(0);
}


//===================================================================================================


void TextLayerAGK::UpdateEngineLayer()
{

    ScreenMapCamera *cam = dynamic_cast<ScreenMapCamera*>(GetMap()->GetCamera());

    if(cam && cam->GetChangeFlags()!=0){
        for(Text* t : GetTexts()){
            int labelAgkId = static_cast<TextAGK*>(t)->GetAgkId();

            if(cam->GetChangeFlags() & Camera::Change::POSITION){
                Vec2f pos = cam->ScreenPointFromMapPoint(t->GetPosition());
                //agk::SetTextPosition(labelAgkId, pos.x, pos.y);
                t->SetPosition(pos);
            }

            if(cam->GetChangeFlags() & Camera::Change::SCALE){
                float size = t->GetFont()->GetSize() * cam->GetScale();
                //agk::SetTextSize(labelAgkId, size);
                agk::SetTextSize(labelAgkId, size);
            }

            if(cam->GetChangeFlags() & Camera::Change::ROTATION){
                float rotation = cam->GetRotation();
                agk::SetTextAngle(labelAgkId, -rotation);
            }
        }
    }

    TextLayer::UpdateEngineLayer();
}


//===============================================================================================


DrawerAGK::DrawerAGK()
{
    agkOutlineColor = agk::MakeColor(255,255,0);
}


void DrawerAGK::SetOutlineColor(ColorRGBA _outlineColor)
{
    agkOutlineColor = agk::MakeColor(_outlineColor.r, _outlineColor.g, _outlineColor.b);
}


void DrawerAGK::Line(jugimap::Vec2f p1, jugimap::Vec2f p2)
{

    Camera *cam = GetDrawingLayer()->GetMap()->GetCamera();
    p1 = cam->ScreenPointFromMapPoint(p1);     // AGK draw primitives functions are not affected by the 'view' tranformation so we need to transform manually
    p2 = cam->ScreenPointFromMapPoint(p2);

    agk::DrawLine(p1.x, p1.y, p2.x, p2.y, agk::GetColorRed(agkOutlineColor), agk::GetColorGreen(agkOutlineColor),agk::GetColorBlue(agkOutlineColor));
}


void DrawerAGK::RectangleOutline(const jugimap::Rectf &rect)
{
    Line(rect.min, Vec2f(rect.max.x, rect.min.y));
    Line(Vec2f(rect.max.x, rect.min.y), rect.max);
    Line(rect.max, Vec2f(rect.min.x, rect.max.y));
    Line(Vec2f(rect.min.x, rect.max.y), rect.min);
}


void DrawerAGK::EllipseOutline(jugimap::Vec2f c, jugimap::Vec2f r)
{
    c = GetDrawingLayer()->GetMap()->GetCamera()->ScreenPointFromMapPoint(c);
    r = r * GetDrawingLayer()->GetMap()->GetCamera()->GetScale();
    agk::DrawEllipse(c.x, c.y, r.x, r.y, agkOutlineColor, agkOutlineColor, 0);
}


void DrawerAGK::Dot(jugimap::Vec2f p)
{
    p = GetDrawingLayer()->GetMap()->GetCamera()->ScreenPointFromMapPoint(p);
    agk::DrawEllipse(p.x, p.y, 2, 2, agkOutlineColor, agkOutlineColor, 1);

}


//===================================================================================================


FragmentShaderAGK_ColorBlend::~FragmentShaderAGK_ColorBlend()
{
    if(agkId!=0) agk::DeleteShader(agkId);
}


void FragmentShaderAGK_ColorBlend::Load(const std::string &_filePath)
{

    agkId = agk::LoadSpriteShader(_filePath.c_str());

    if(agkId!=0){
        ColorRGBAf c(colorRGBA);
        agk::SetShaderConstantByName(agkId, _filePath.c_str(), c.r, c.g, c.b, c.a);
    }

}


namespace shaders
{

ShaderAGK_ColorBlend_SimpleMultiply* colorBlend_SimpleMultiply = new ShaderAGK_ColorBlend_SimpleMultiply();
ShaderAGK_ColorBlend_Normal* colorBlend_Normal = new ShaderAGK_ColorBlend_Normal();
ShaderAGK_ColorBlend_Multiply* colorBlend_Multiply = new ShaderAGK_ColorBlend_Multiply();
ShaderAGK_ColorBlend_LinearDodge* colorBlend_LinearDodge = new ShaderAGK_ColorBlend_LinearDodge();
ShaderAGK_ColorBlend_Color* colorBlend_Color = new ShaderAGK_ColorBlend_Color();


void DeleteShaders()
{
    delete colorBlend_SimpleMultiply;
    delete colorBlend_Normal;
    delete colorBlend_Multiply;
    delete colorBlend_LinearDodge;
    delete colorBlend_Color;
}


}



}



