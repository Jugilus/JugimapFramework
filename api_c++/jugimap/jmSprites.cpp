#include <algorithm>
#include <assert.h>
#include "jmSourceGraphics.h"
#include "jmVectorShapes.h"
#include "jmUtilities.h"
#include "jmFrameAnimation.h"
#include "jmMapBinaryLoader.h"
#include "jmCollision.h"
#include "jmMap.h"
#include "jmLayers.h"
#include "jmText.h"
#include "jmObjectFactory.h"
#include "jmSprites.h"



namespace jugimap {




Sprite* Sprite::MakePassiveCopy()
{

    Sprite *s = objectFactory->NewSprite(GetKind());
    s->sourceSprite = sourceSprite;
    s->layer = layer;               // Set layer link but do not insert sprite into layer yet!
    s->parentComposedSprite = parentComposedSprite;
    s->CopyProperties(this, Property::ALL);
    if(GetKind()==SpriteKind::COMPOSED){
        ComposedSprite::CopyLayers(static_cast<ComposedSprite*>(this), static_cast<ComposedSprite*>(s), nullptr);
    }

    return s;
}


Sprite* Sprite::MakeActiveCopy()
{
    Sprite *s = MakePassiveCopy();
    s->layer->AddSprite(s);
    s->InitEngineSprite();
    return s;
}


/*
void Sprite::Delete()
{
    layer->RemoveAndDeleteSprite(this);

}
*/

void Sprite::CopyProperties(Sprite *_sprite, int copyFlags)
{

    if(copyFlags & Property::POSITION){
        position = _sprite->position;
        animationFrameOffset = _sprite->animationFrameOffset;
    }

    if(copyFlags & Property::SCALE){
        scale = _sprite->scale;
    }

    if(copyFlags & Property::FLIP){
        flip = _sprite->flip;
    }

    if(copyFlags & Property::ROTATION){
        rotation = _sprite->rotation;
    }

    if(copyFlags & Property::HANDLE){
        handle = _sprite->handle;
    }

    if(copyFlags & Property::ALPHA){
        alpha = _sprite->alpha;
    }

    if(copyFlags & Property::BLEND){
        ownBlend = _sprite->ownBlend;
    }

    if(copyFlags & Property::OVERLAY_COLOR){
        colorOverlayActive = _sprite->colorOverlayActive;
        colorOverlayRGBA = _sprite->colorOverlayRGBA;
        colorOverlayBlend = _sprite->colorOverlayBlend;
    }

    if(copyFlags & Property::DATA){
        parameters = _sprite->parameters;
        dataFlags = _sprite->dataFlags;
    }

    if(copyFlags & Property::TAG){
        tag = _sprite->tag;
    }

    if(copyFlags & Property::LINK){
        linkObject = _sprite->linkObject;
    }


    changeFlags |= copyFlags;
    layer->SetSpritesChanged(true);
}



SpriteKind Sprite::GetKind()
{
    return sourceSprite->GetKind();
}


void Sprite::SetPosition(Vec2f _position)
{
    position = _position;
    changeFlags |= Property::POSITION;
    layer->SetSpritesChanged(true);
}


void Sprite::SetScale(Vec2f _scale)
{
    scale = _scale;
    changeFlags |= Property::SCALE;
    layer->SetSpritesChanged(true);
}


void Sprite::SetRotation(float _rotation)
{
    rotation = _rotation;
    changeFlags |= Property::ROTATION;
    layer->SetSpritesChanged(true);
}


void Sprite::SetFlip(Vec2i _flip)
{
    flip = _flip;
    changeFlags |= Property::FLIP;
    layer->SetSpritesChanged(true);
}


void Sprite::SetHandle(Vec2f _handle)
{
    handle = _handle;
    changeFlags |= Property::HANDLE;
    layer->SetSpritesChanged(true);
}


void Sprite::SetAnimationFrameOffset(Vec2f _frameOffset)
{
    animationFrameOffset = _frameOffset;
    changeFlags |= Property::POSITION;
    layer->SetSpritesChanged(true);
}


void Sprite::SetAlpha(float _alpha)
{
    alpha = _alpha;
    changeFlags |= Property::ALPHA;
    layer->SetSpritesChanged(true);
}


void Sprite::SetOwnBlend(Blend _blend)
{
    ownBlend = _blend;
    changeFlags |= Property::BLEND;
    layer->SetSpritesChanged(true);
}


void Sprite::SetColorOverlayActive(bool _colorOverlayActive)
{
    colorOverlayActive = _colorOverlayActive;
    changeFlags |= Property::OVERLAY_COLOR;
    layer->SetSpritesChanged(true);
}


void Sprite::SetColorOverlayRGBA(ColorRGBA _colorOverlayRGBA)
{
    colorOverlayRGBA = _colorOverlayRGBA;
    changeFlags |= Property::OVERLAY_COLOR;
    layer->SetSpritesChanged(true);
}


void Sprite::SetColorOverlayBlend(ColorOverlayBlend _colorOverlayBlend)
{
    colorOverlayBlend = _colorOverlayBlend;
    changeFlags |= Property::OVERLAY_COLOR;
    layer->SetSpritesChanged(true);
}


void Sprite::SetChangeFlags(int _changeFlags)
{
    changeFlags = _changeFlags;
    if(changeFlags !=0){
        layer->SetSpritesChanged(true);
    }
}


void Sprite::AppendToChangeFlags(int _changeFlags)
{
    changeFlags |= _changeFlags;
    if(changeFlags !=0 ){
        layer->SetSpritesChanged(true);
    }
}


Vec2f Sprite::GetGlobalPosition()
{
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldPos(position) : position;
}


Vec2f Sprite::GetFullGlobalPosition()
{
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldPos(position+animationFrameOffset) : position+animationFrameOffset;
}


Vec2f Sprite::GetGlobalScale()
{
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldScale(scale) : scale;
}


float Sprite::GetGlobalRotation()
{
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldRotation(rotation) : rotation;
}


Vec2i Sprite::GetGlobalFlip()
{
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldFlip(flip) : flip;
}


//=======================================================================================


StandardSprite::~StandardSprite ()
{

    if(collider){
        delete collider;
    }
}


void StandardSprite::UpdateBoundingBox()
{

    Vec2f posGlobal;
    Vec2i size;
    Vec2f handleTMP;

    if(IsEngineSpriteInitialized()){

        if(GetActiveImage()==nullptr) return;
        posGlobal = GetGlobalPosition() + GetLayer()->GetParallaxOffset();
        size = GetActiveImage()->GetSize();
        handleTMP = GetHandle();

    }else{
        if(GetSourceSprite()==nullptr) return;
        posGlobal = GetGlobalPosition();
        size = GetSourceSprite()->GetGraphicItems().front()->GetSize();
        handleTMP = GetSourceSprite()->GetGraphicItems().front()->GetHandle();
    }

    //----
    jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, GetGlobalScale(),  GetGlobalFlip(), GetGlobalRotation(), handleTMP);
    SetBoundingBox(TransformRectf(Rectf(0,0, size.x, size.y), m));
}


void StandardSprite::SetActiveImage(GraphicItem *_image)
{
    activeImage = _image;
    if(activeImage){
        if(GetHandle().Equals(activeImage->GetHandle())==false){
            SetHandle(activeImage->handle);
        }
    }
    UpdateBoundingBox();
    //changeFlags = Flags::ALL;
}



void StandardSprite::SetColliderShapes()
{

    if(collider==nullptr) return;

    if(activeImage==nullptr){

        // Possible situation when there is an empty frame image in frame animation!
        if(preserveShapesOnNullActiveImage==false){
            collider->ClearShapes();
        }
        return;
    }

    if(activeImage->GetSpriteShapes().empty()==false){

        if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_IMAGES_WITH_DIFFERENT_SPRITE_SHAPES){
            collider->ClearShapes();
        }

        if(collider->GetShapesCount()==0){              // empty collider
            collider->AddShapes();
        }

    }else{
        collider->ClearShapes();
    }
}


void StandardSprite::UpdateColliderAndBoundingBox()
{

    if(activeImage==nullptr) return;

    Vec2f posGlobal = GetGlobalPosition() + GetLayer()->GetParallaxOffset();
    Vec2f scaleGlobal = GetGlobalScale();
    float rotationGlobal = GetGlobalRotation();
    Vec2i flipGlobal = GetGlobalFlip();

    jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, scaleGlobal, flipGlobal, rotationGlobal);

    if(collider){
        collider->UpdateShapes(m);
    }

    m = m.Translate(-GetHandle().x, -GetHandle().y);
    SetBoundingBox(TransformRectf(Rectf(0,0, activeImage->GetWidth(),activeImage->GetHeight()), m));
}


bool StandardSprite::HasCollider()
{
    return collider!=nullptr;
}


bool StandardSprite::PointInside(Vec2f _pos)
{
    if(collider==nullptr) return false;

    if(GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        UpdateEngineSprite();                   // need to update engine sprite which also updates collider
    }

    return collider->PointInside(_pos);
}


bool StandardSprite::Overlaps(Sprite *_sprite)
{
    if(collider==nullptr) return false;
    if(_sprite->HasCollider()==false) return false;

    if(GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        UpdateEngineSprite();
    }
    if(_sprite->GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        _sprite->UpdateEngineSprite();
    }

    if(_sprite->GetKind()==SpriteKind::STANDARD){
        StandardSprite *s = static_cast<StandardSprite*>(_sprite);

        return collider->Overlaps(s->GetCollider());

    }else if(_sprite->GetKind()==SpriteKind::COMPOSED){
        ComposedSprite *s = static_cast<ComposedSprite*>(_sprite);

        return s->Overlaps(this);
    }

    return false;
}


bool StandardSprite::RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos)
{
    if(collider==nullptr) return false;
    if(GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        UpdateEngineSprite();
    }

    return collider->RaycastHit(_rayStart, _rayEnd, _hitPos);

}


//=======================================================================================


void ComposedSprite::CopyLayers(ComposedSprite *csSource, ComposedSprite *csDest, JugiMapBinaryLoader* loader)
{

    int countLayers = 0;

    //--- SPRITE LAYERS
    for(Layer *l : csSource->layers){

        if(l->GetKind()==LayerKind::SPRITE){

            SpriteLayer * sl = static_cast<SpriteLayer*>(l);

            SpriteLayer *lCopy = objectFactory->NewSpriteLayer();
            //lCopy->map = csDest->map;                   // Using csDest because csSource can be SourceComposedSprite where the map is null.
            lCopy->map = csDest->layer->map;                // Using csDest because csSource can be SourceComposedSprite where the map is null.
            lCopy->parentComposedSprite = csDest;
            lCopy->kind = sl->kind;
            lCopy->name = sl->name;
            lCopy->parameters = sl->parameters;

            //--- set depth when building
            if(loader){

                lCopy->zOrder = csDest->GetLayer()->GetZOrder() + countLayers * loader->zOrderCounterStep;
                if(lCopy->zOrder <= loader->zOrderCounter){
                    loader->zOrderCounter = lCopy->zOrder + loader->zOrderCounterStep;
                }
                countLayers++;

            }else{
                lCopy->zOrder = l->zOrder;
            }

            //----
            for(Sprite *s : sl->sprites){

                Sprite *sCopy = objectFactory->NewSprite(s->GetKind());
                sCopy->sourceSprite = s->sourceSprite;
                sCopy->layer = lCopy;
                //sCopy->map = lCopy->map;
                sCopy->parentComposedSprite = lCopy->parentComposedSprite;
                sCopy->CopyProperties(s, Property::ALL);
                if(s->GetKind()==SpriteKind::COMPOSED){
                    CopyLayers(static_cast<ComposedSprite*>(s), static_cast<ComposedSprite*>(sCopy), loader);
                }
                lCopy->sprites.push_back(sCopy);

            }

            csDest->layers.push_back(lCopy);

        }else if(l->GetKind()==LayerKind::VECTOR){

            VectorLayer * vl = static_cast<VectorLayer*>(l);

            VectorLayer *vlCopy = objectFactory->NewVectorLayer();  // new VectorLayer();
            vlCopy->parentComposedSprite = csDest;
            vlCopy->kind = vl->kind;
            vlCopy->name = vl->name;
            vlCopy->parameters = vl->parameters;

            for(VectorShape *vs : vl->GetVectorShapes()){
                VectorShape *vsCopy = new VectorShape(*vs);
                vlCopy->AddVectorShape(vsCopy);
            }
            csDest->layers.push_back(vlCopy);


        }else if(l->GetKind()==LayerKind::TEXT){

            TextLayer * tl = static_cast<TextLayer*>(l);

            TextLayer *tlCopy = objectFactory->NewTextLayer(tl->GetName());  // new VectorLayer();
            tlCopy->parentComposedSprite = csDest;
            tlCopy->kind = tl->kind;
            tlCopy->parameters = tl->parameters;

            for(Text *t : tl->GetTexts()){
                Text* tCopy = objectFactory->NewText(tlCopy, t->GetFont(), t->GetTextString(), t->GetPosition(), t->GetColor());
                tCopy->SetAlpha(t->GetAlpha());
            }
            csDest->layers.push_back(tlCopy);

        }




    }

}


ComposedSprite::~ComposedSprite()
{
    for(Layer* l : layers){
        delete l;
    }
    layers.clear();

}


void ComposedSprite::InitEngineSprite()
{
    UpdateTransform();
    visible = true;
    changeFlags = Property::ALL;


    for(Layer *l : layers){
        l->InitEngineLayer();
    }
}


void ComposedSprite::UpdateEngineSprite()
{
    if(visible==false) return;
    if(changeFlags==0) return;

    UpdateTransform();

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            for(Sprite *s : sl->GetSprites()){
                s->SetChangeFlags(s->GetChangeFlags() | changeFlags);
                s->UpdateEngineSprite();
            }
        }
    }

    SetChangeFlags(0);
}


void ComposedSprite::DrawEngineSprite()
{
    if(visible==false) return;

    for(Layer *l : layers){
        l->DrawEngineLayer();
    }

}


void ComposedSprite::SetVisible(bool _visible)
{

    if(IsVisible()==_visible) return;

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            for(Sprite *s : sl->GetSprites()){
                s->SetVisible(_visible);
            }
        }
    }
    SetVisible(_visible);
    if(IsVisible()){
        SetChangeFlags(Property::TRANSFORMATION | Property::APPEARANCE);
    }
}


bool ComposedSprite::HasCollider()
{

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            for(Sprite *s : sl->GetSprites()){
                bool hasShape = s->HasCollider();
                if(hasShape) return true;
            }
        }
    }
    return false;
}


bool ComposedSprite::PointInside(Vec2f _pos)
{

    if(GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        UpdateEngineSprite();
    }

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            for(Sprite *s : sl->GetSprites()){
                bool inside = s->PointInside(_pos);
                if(inside) return true;
            }
        }
    }
    return false;
}


bool ComposedSprite::Overlaps(Sprite *_sprite)
{

    if(GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        UpdateEngineSprite();
    }
    if(_sprite->GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        _sprite->UpdateEngineSprite();
    }


    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            for(Sprite *s : sl->GetSprites()){
                if(s==_sprite) continue;
                bool overlaps = s->Overlaps(_sprite);
                if(overlaps) return true;
            }
        }
    }
    return false;
}


bool ComposedSprite::RaycastHit(Vec2f _rayStart, Vec2f _rayEnd, Vec2f &_hitPos)
{

    if(GetChangeFlags() & Sprite::Property::TRANSFORMATION){
        UpdateEngineSprite();                   // need to update engine sprite so that collider is in sysnc with the sprite
    }

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            for(Sprite *s : sl->GetSprites()){
                bool hit = s->RaycastHit(_rayStart, _rayEnd, _hitPos);
                if(hit) return true;
            }
        }
    }
    return false;
}


void ComposedSprite::UpdateBoundingBox()
{

    UpdateTransform();

    boundingBox = Rectf(999999.0, 999999.0, -999999.0, -999999.0);

    for(Layer *l : layers){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            sl->UpdateBoundingBox();
            boundingBox = boundingBox.Unite(sl->boundingBox);
        }
    }
}


void ComposedSprite::CaptureForLerpDrawing()
{

    for(Layer *l : GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            static_cast<SpriteLayer*>(l)->CaptureForLerpDrawing();
        }
    }
}



void ComposedSprite::UpdateTransform()
{

    if(changeFlags == Property::POSITION){    // changed only position

        cTransform = MakeTransformationMatrix(position+GetLayer()->GetParallaxOffset(), scale, flip, rotation, handle);
        if(parentComposedSprite){
            cTransform = parentComposedSprite->cTransform * cTransform;
        }
        return;
    }

    cTransform = MakeTransformationMatrix(position+GetLayer()->GetParallaxOffset(), scale, flip, rotation, handle);
    cFlip = flip;
    cScale = scale.x;      // composed sprites have uniform scale !
    cRotation = rotation;

    //---
    if(parentComposedSprite){
        cTransform = parentComposedSprite->cTransform * cTransform;
        cScale *= parentComposedSprite->cScale;

        if(parentComposedSprite->cFlip.y + parentComposedSprite->cFlip.x == 1){     // one and only one flip changes rotation sign
            cRotation = parentComposedSprite->cRotation - rotation;
        }else{
            cRotation = parentComposedSprite->cRotation + rotation;
        }


        cFlip.x = parentComposedSprite->cFlip.x;
        if(flip.x){
            cFlip.x = 1 - cFlip.x;
        }

        cFlip.y = parentComposedSprite->cFlip.y;
        if(flip.y){
            cFlip.y = 1 - cFlip.y;
        }


    }

}


//==========================================================================================================


void SkeletonSprite::InitEngineSprite()
{

    GetSourceSprite()->Init();


    GraphicItem *gi = GetSourceSprite()->GetGraphicItems().front();
    GraphicFile *gf = gi->GetFile();

    if(GetKind()==SpriteKind::SPINE){

        // spine atlas file: gf->GetSpineAtlasRelativeFilePath()
        // spine skeleton file: gf->GetRelativeFilePath()
        // spine skeleton skin name: gi->name()


    }else if(GetKind()==SpriteKind::SPRITER){

        // spriter model file: gf->GetRelativeFilePath()
        // spriter entity name: gi->name()

    }

}




}



