
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


void StandardSpriteAGK::InitEngineObjects()
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


void StandardSpriteAGK::UpdateEngineObjects()
{
    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TRANSFORMATION){

        Vec2f posWorld = GetGlobalPosition();
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
            //if(GetSourceSprite()->GetName()=="buttonB"){
            //    DummyFunction();
            //    agk::SetSpriteOffset(agkId, 50, 30);
            //}else{
                agk::SetSpriteOffset(agkId, GetHandle().x*agk::GetSpriteScaleX(agkId), GetHandle().y*agk::GetSpriteScaleY(agkId));
            //}

        }

        if(flags & Property::SCALE){
            agk::SetSpriteScaleByOffset(agkId, scaleWorld.x, scaleWorld.y);
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

    if(GetSourceSprite()->GetName()=="buttonA-spr2"){
        DummyFunction();
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


    if(flags & Property::VISIBILITY){

        bool visibleMap = GetLayer()->GetMap()->IsVisible();

        if(IsVisible() && visibleMap){
            if(GetActiveImage()){
                agk::SetSpriteVisible(agkId, 1);
            }

        }else{
            agk::SetSpriteVisible(agkId, 0);
        }
    }

    //---
    SetChangeFlags(0);
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


TextSpriteAGK::~TextSpriteAGK()
{
    agk::DeleteText(agkId);
}


void TextSpriteAGK::InitEngineObjects()
{

    if(agkId != 0) return;     //already initialized

    TextSprite::InitEngineObjects();


    //----
    agkId = agk::CreateText(GetTextString().c_str());
    agk::SetTextFont(agkId, static_cast<FontAGK*>(GetFont())->GetAgkId());
    agk::SetTextSize(agkId, GetFont()->GetLoadFontSize());
    agk::SetTextDepth(agkId, GetLayer()->GetZOrder());

    if(GetLayer()->GetMap()->GetType()==MapType::SCREEN){
        agk::FixTextToScreen(agkId, 1);                         // layer belong to a screen map and as such will not be transformed via agk 'view' -> we need to fix texts on screen!
    }

    Vec2f size(agk::GetTextTotalWidth(agkId), agk::GetTextTotalHeight(agkId));
    _SetSize(size);

    //float w = agk::GetTextTotalWidth(agkId);
    //float h = agk::GetTextTotalHeight(agkId);
    //float h = GetFont()->GetCommonPixelHeight();
    //SetHandle(Vec2f(GetRelativeHandle().x*w,  GetRelativeHandle().y*h));
    SetHandle(GetRelativeHandle() * size);

    //---
    SetChangeFlags(Property::ALL);
}


void TextSpriteAGK::UpdateEngineObjects()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false && GetChangeFlags()==0) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();

    if(GetTextString()=="OPTIONS"){
        DummyFunction();
    }


    if(flags & Property::TEXT_STRING){

        agk::SetTextString(agkId, GetTextString().c_str());
        Vec2f size(agk::GetTextTotalWidth(agkId), agk::GetTextTotalHeight(agkId));
        _SetSize(size);
        SetHandle(size*GetRelativeHandle());
        flags |= Property::POSITION;

        //---- update bounding box
        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();
        jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, GetGlobalScale(), GetGlobalFlip(),  GetGlobalRotation(), GetHandle());
        SetBoundingBox(TransformRectf(Rectf(0,0, size.x, size.y), m));

        //----
        flags &= ~Property::TEXT_STRING;    // ! Because GetSize() used later in this function checks for this flag and call UpdateEngineSprite !
        SetChangeFlags(flags);
    }



    if(flags & Property::TRANSFORMATION){

        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();

        Vec2f scaleGlobal = GetGlobalScale();
        float rotationGlobal = GetGlobalRotation();
        Vec2i flipGlobal = GetGlobalFlip();

        //----
        jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, scaleGlobal,  flipGlobal, rotationGlobal, GetHandle());
        SetBoundingBox(TransformRectf(Rectf(0,0, GetSize().x, GetSize().y), m));


        // Sprites in screen maps are not affected by AGK view tranformation so we need to include camera transformation for every sprite here
        Camera *cam = GetLayer()->GetMap()->GetCamera();
        if(cam && cam->GetKind()==Camera::Kind::SCREEN){
            ScreenMapCamera *camera = static_cast<ScreenMapCamera*>(GetLayer()->GetMap()->GetCamera());
            posGlobal = camera->ScreenPointFromMapPoint(posGlobal);
            scaleGlobal = scaleGlobal * camera->GetScale();
            rotationGlobal = rotationGlobal + camera->GetRotation();
        }

        if (flags & Property::HANDLE){
            //agk::SetTextOffs
            //sfText->setOrigin(GetHandle().x, GetHandle().y);
        }
        if(flags & Property::POSITION){
            Vec2f h = GetHandle()*scaleGlobal;
            agk::SetTextPosition(agkId, posGlobal.x - h.x, posGlobal.y - h.y);

        }
        if(flags & Property::SCALE){
            float size = GetFont()->GetLoadFontSize() * scaleGlobal.x;
            agk::SetTextSize(agkId, size);
        }
        if(flags & Property::ROTATION){
            agk::SetTextAngle(agkId, -rotationGlobal);
        }


    }


    if(flags & Property::APPEARANCE){

        if(flags & Property::COLOR){
            agk::SetTextColor(agkId, GetColor().r, GetColor().g, GetColor().b, GetColor().a * GetAlpha());
        }
        if(flags & Property::ALPHA){
            agk::SetTextColor(agkId, GetColor().r, GetColor().g, GetColor().b, GetColor().a * GetAlpha());
        }
    }

    if(flags & Property::VISIBILITY){

        bool visibleMap = GetLayer()->GetMap()->IsVisible();

        if(IsVisible() && visibleMap){
            agk::SetTextVisible(agkId, 1);
        }else{
            agk::SetTextVisible(agkId, 0);
        }
    }

    //---
    SetChangeFlags(0);

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


void SkeletonSpriteAGK::InitEngineObjects()
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


void SkeletonSpriteAGK::UpdateEngineObjects()
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


void SpriteLayerAGK::UpdateEngineObjects()
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

    SpriteLayer::UpdateEngineObjects();

}


//===================================================================================================


void MapAGK::SetVisible(bool _visible)
{

    if(IsVisible() == _visible) return;

    Map::SetVisible(_visible);

    for(Layer *l : GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer *sl = static_cast<SpriteLayer*>(l);
            sl->SetSpritesChangeFlag(Sprite::Property::VISIBILITY);
        }
        l->UpdateEngineObjects();
    }
}


void MapAGK::UpdateEngineObjects()
{

    if(IsVisible()==false) return;

    Map::UpdateEngineObjects();

    // Convert world map camera to AGK 'view'.

    Camera *cam = GetCamera();

    if(cam->GetKind()==Camera::Kind::WORLD){
        jugimap::Vec2f offset = cam->GetProjectedMapPosition();
        offset = offset / cam->GetScale();                           // this gives correct result
        agk::SetViewOffset(-offset.x, -offset.y);
        agk::SetViewZoom(cam->GetScale());
    }

}


//===================================================================================================


void EngineSceneAGK::PreUpdate()
{

    // MOUSE
    Vec2f pos(agk::GetRawMouseX(), agk::GetRawMouseY());
    mouse._SetPosition(Vec2i(pos.x, pos.y));
    mouse._SetWheel(Vec2i(0,agk::GetRawMouseWheelDelta()));

    int buttonDown = agk::GetRawMouseLeftState();               // 1 or 0
    mouse._SetButtonState(MouseButton::LEFT,  buttonDown);

    buttonDown = agk::GetRawMouseMiddleState();
    mouse._SetButtonState(MouseButton::MIDDLE, buttonDown);

    buttonDown = agk::GetRawMouseRightState();
    mouse._SetButtonState(MouseButton::RIGHT, buttonDown);

    if(mouse.GetCursorSprite()){
        mouse.GetCursorSprite()->SetPosition(pos);
    }


    // KEYBOARD
    for(int i=0; i<keyboardConversionTable.table.size(); i++){

        int keyDown = agk::GetRawKeyState(i);
        keyboard._SetKeyState(keyboardConversionTable.table[i], keyDown);

        //---
        if(i == AGK_KEY_SHIFT){
            keyboard._SetKeyState(KeyCode::RIGHT_SHIFT, keyDown);

        }else if(i == AGK_KEY_CONTROL){
            keyboard._SetKeyState(KeyCode::RIGHT_CONTROL, keyDown);

        }else if(i == AGK_KEY_ALT){
            keyboard._SetKeyState(KeyCode::RIGHT_ALT, keyDown);
        }
    }


    // TOUCH
    int touchID = agk::GetRawFirstTouchEvent(1);
    while(touchID > 0){
        Vec2i pos(agk::GetRawTouchCurrentX(touchID),  agk::GetRawTouchCurrentY(touchID));
        touch._SetFingerState(touchID, true, pos);
        touchID = agk::GetRawNextTouchEvent();
    }


    // JOYSTICK
    for(int i=0; i<jugimap::joysticks.size(); i++){
        if(jugimap::joysticks[i].IsConnected()){

            //--- buttons
            for(int j=0; j<joysticks[i].GetButtons().size(); j++){
                int down = agk::GetRawJoystickButtonState(i+1, j+1);
                joysticks[i]._SetButtonState(j, down);

                //if(agk::GetRawJoystickButtonPressed(i+1, j+1)==1){
                //    DbgOutput("Joystick pressed button:" + std::to_string(j));
                //}
            }

            //--- POV directions
            int pov =  agk::GetRawJoystickPOV(i+1, 0);
            int delta = 2300;

            if(pov < 0){                                                    // no angle
                joysticks[i]._SetPOV_X(Joystick::POV_X::NONE);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::NONE);

            }else if(pov > 36000-delta || pov < 0+delta){                   //  UP
                joysticks[i]._SetPOV_X(Joystick::POV_X::NONE);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::UP);

            }else if(pov > 4500-delta && pov < 4500+delta){                  // RIGHT, UP
                joysticks[i]._SetPOV_X(Joystick::POV_X::RIGHT);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::UP);

            }else if(pov > 9000-delta && pov < 9000+delta){                     // RIGHT
                joysticks[i]._SetPOV_X(Joystick::POV_X::RIGHT);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::NONE);

            }else if(pov > 13500-delta && pov < 13500+delta){                     // RIGHT, DOWN
                joysticks[i]._SetPOV_X(Joystick::POV_X::RIGHT);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::DOWN);

            }else if(pov > 18000-delta && pov < 18000+delta){                     // DOWN
                joysticks[i]._SetPOV_X(Joystick::POV_X::NONE);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::DOWN);

            }else if(pov > 22500-delta && pov < 22500+delta){                     // LEFT, DOWN
                joysticks[i]._SetPOV_X(Joystick::POV_X::LEFT);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::DOWN);

            }else if(pov > 27000-delta && pov < 27000+delta){                     // LEFT
                joysticks[i]._SetPOV_X(Joystick::POV_X::LEFT);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::NONE);

            }else if(pov > 31500-delta && pov < 31500+delta){                     // LEFT, UP
                joysticks[i]._SetPOV_X(Joystick::POV_X::LEFT);
                joysticks[i]._SetPOV_Y(Joystick::POV_Y::UP);
            }

            //DbgOutput("Joystick POV 0:" + std::to_string(pov));

            //--- axis values
            joysticks[i]._SetXaxis(agk::GetRawJoystickX(i+1));
            joysticks[i]._SetYaxis(agk::GetRawJoystickY(i+1));
            joysticks[i]._SetZaxis(agk::GetRawJoystickZ(i+1));

        }
    }
}


//===================================================================================================


void EngineAppAGK::SetSystemMouseCursorVisible(bool _visible)
{
    agk::SetRawMouseVisible(_visible);
}


//===================================================================================================


FontAGK::FontAGK(const std::string &_relativeFilePath, int _size, FontKind _kind)
    : Font(_relativeFilePath, _size, _kind)
{
    agkId = agk::LoadFont(GetRelativeFilePath().c_str());
    assert(agkId!=0);

    agkProbeTextId = agk::CreateText(textForHeight.c_str());
    agk::SetTextFont(agkProbeTextId, agkId);
    agk::SetTextSize(agkProbeTextId, _size);
    agk::SetTextVisible(agkProbeTextId, 0);

    pixelHeightCommon = GetPixelHeight(textForHeight.c_str());

}


FontAGK::~FontAGK()
{
    //agk::DeleteText(agkId);
    agk::DeleteText(agkProbeTextId);
    agk::DeleteFont(agkId);
}


int FontAGK::GetPixelWidth(const std::string &s)
{

    agk::SetTextString(agkProbeTextId, s.c_str());
    float width = agk::GetTextTotalWidth(agkProbeTextId);
    return width;
}


int FontAGK::GetPixelHeight(const std::string &s)
{

    agk::SetTextString(agkProbeTextId, s.c_str());
    float height = agk::GetTextTotalHeight(agkProbeTextId);
    return height;
}


Vec2f FontAGK::GetPixelSize(const std::string &s)
{

    agk::SetTextString(agkProbeTextId, s.c_str());
    Vec2f size(agk::GetTextTotalWidth(agkProbeTextId), agk::GetTextTotalHeight(agkProbeTextId));
    return size;
}


//===================================================================================================


DrawerAGK::DrawerAGK()
{
    agkOutlineColor = agk::MakeColor(255,255,0);
}


void DrawerAGK::UpdateEngineDrawer()
{

    GetDrawingLayer()->DrawEngineObjects();

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


void LoadJugimapShaders()
{

    colorBlend_SimpleMultiply->Load("media/shaders_AGK/spriteColorOverlay_simpleMultiply.ps");
    colorBlend_Normal->Load("media/shaders_AGK/spriteColorOverlay_normal.ps");
    colorBlend_Multiply->Load("media/shaders_AGK/spriteColorOverlay_multiply.ps");
    colorBlend_LinearDodge->Load("media/shaders_AGK/spriteColorOverlay_linearDodge.ps");
    colorBlend_Color->Load("media/shaders_AGK/spriteColorOverlay_color.ps");
}


void DeleteShaders()
{
    delete colorBlend_SimpleMultiply;
    delete colorBlend_Normal;
    delete colorBlend_Multiply;
    delete colorBlend_LinearDodge;
    delete colorBlend_Color;
}

}


//===================================================================================================


KeyboardConversionTable::KeyboardConversionTable()
{

    table.resize(256, jugimap::KeyCode::UNKNOWN);

    table[AGK_KEY_BACK] = jugimap::KeyCode::BACKSPACE;
    table[AGK_KEY_TAB] = jugimap::KeyCode::TAB;
    table[AGK_KEY_ENTER] = jugimap::KeyCode::ENTER;
    table[AGK_KEY_ESCAPE] = jugimap::KeyCode::ESCAPE;
    table[AGK_KEY_SPACE] = jugimap::KeyCode::SPACE;

    table[AGK_KEY_PAGEUP] = jugimap::KeyCode::PAGEUP;
    table[AGK_KEY_PAGEDOWN] = jugimap::KeyCode::PAGEDOWN;
    table[AGK_KEY_END] = jugimap::KeyCode::END;
    table[AGK_KEY_HOME] = jugimap::KeyCode::HOME;
    table[AGK_KEY_LEFT] = jugimap::KeyCode::LEFT;
    table[AGK_KEY_UP] = jugimap::KeyCode::UP;
    table[AGK_KEY_RIGHT] = jugimap::KeyCode::RIGHT;
    table[AGK_KEY_DOWN] = jugimap::KeyCode::DOWN;
    table[AGK_KEY_INSERT] = jugimap::KeyCode::INSERT;
    table[AGK_KEY_DELETE] = jugimap::KeyCode::DELETEkey;

    table[AGK_KEY_0] = jugimap::KeyCode::NUM_0;
    table[AGK_KEY_1] = jugimap::KeyCode::NUM_1;
    table[AGK_KEY_2] = jugimap::KeyCode::NUM_2;
    table[AGK_KEY_3] = jugimap::KeyCode::NUM_3;
    table[AGK_KEY_4] = jugimap::KeyCode::NUM_4;
    table[AGK_KEY_5] = jugimap::KeyCode::NUM_5;
    table[AGK_KEY_6] = jugimap::KeyCode::NUM_6;
    table[AGK_KEY_7] = jugimap::KeyCode::NUM_7;
    table[AGK_KEY_8] = jugimap::KeyCode::NUM_8;
    table[AGK_KEY_9] = jugimap::KeyCode::NUM_9;

    table[AGK_KEY_A] = jugimap::KeyCode::A;
    table[AGK_KEY_B] = jugimap::KeyCode::B;
    table[AGK_KEY_C] = jugimap::KeyCode::C;
    table[AGK_KEY_D] = jugimap::KeyCode::D;
    table[AGK_KEY_E] = jugimap::KeyCode::E;
    table[AGK_KEY_F] = jugimap::KeyCode::F;
    table[AGK_KEY_G] = jugimap::KeyCode::G;
    table[AGK_KEY_H] = jugimap::KeyCode::H;
    table[AGK_KEY_I] = jugimap::KeyCode::I;
    table[AGK_KEY_J] = jugimap::KeyCode::J;
    table[AGK_KEY_K] = jugimap::KeyCode::K;
    table[AGK_KEY_L] = jugimap::KeyCode::L;
    table[AGK_KEY_M] = jugimap::KeyCode::M;
    table[AGK_KEY_N] = jugimap::KeyCode::N;
    table[AGK_KEY_O] = jugimap::KeyCode::O;
    table[AGK_KEY_P] = jugimap::KeyCode::P;
    table[AGK_KEY_Q] = jugimap::KeyCode::Q;
    table[AGK_KEY_R] = jugimap::KeyCode::R;
    table[AGK_KEY_S] = jugimap::KeyCode::S;
    table[AGK_KEY_T] = jugimap::KeyCode::T;
    table[AGK_KEY_U] = jugimap::KeyCode::U;
    table[AGK_KEY_V] = jugimap::KeyCode::V;
    table[AGK_KEY_W] = jugimap::KeyCode::W;
    table[AGK_KEY_X] = jugimap::KeyCode::X;
    table[AGK_KEY_Y] = jugimap::KeyCode::Y;
    table[AGK_KEY_Z] = jugimap::KeyCode::Z;

    table[AGK_KEY_F1] = jugimap::KeyCode::F1;
    table[AGK_KEY_F2] = jugimap::KeyCode::F2;
    table[AGK_KEY_F3] = jugimap::KeyCode::F3;
    table[AGK_KEY_F4] = jugimap::KeyCode::F4;
    table[AGK_KEY_F5] = jugimap::KeyCode::F5;
    table[AGK_KEY_F6] = jugimap::KeyCode::F6;
    table[AGK_KEY_F7] = jugimap::KeyCode::F7;
    table[AGK_KEY_F8] = jugimap::KeyCode::F8;

    table[AGK_KEY_SHIFT] = jugimap::KeyCode::LEFT_SHIFT;
    table[AGK_KEY_CONTROL] = jugimap::KeyCode::LEFT_CONTROL;
    table[AGK_KEY_ALT] = jugimap::KeyCode::LEFT_ALT;

}

KeyboardConversionTable keyboardConversionTable;



}



