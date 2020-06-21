#include <algorithm>
#include <assert.h>
#include "jmSourceGraphics.h"
#include "jmVectorShapes.h"
#include "jmUtilities.h"
#include "jmFrameAnimation.h"
#include "jmAnimationCommon.h"
#include "jmMapBinaryLoader.h"
#include "jmCollision.h"
#include "jmMap.h"
#include "jmLayers.h"
#include "jmText.h"
#include "jmObjectFactory.h"
#include "jmSprites.h"



namespace jugimap {



Sprite::~Sprite()
{

    if(ap){
        delete ap;
    }

}


Sprite* Sprite::MakePassiveCopy()
{

    Sprite *s = objectFactory->NewSprite(GetKind());
    s->sourceSprite = sourceSprite;
    s->layer = layer;               // Set layer link but do not insert sprite into layer yet!
    s->parentComposedSprite = parentComposedSprite;
    s->CopyProperties(this, Property::ALL);
    if(GetKind()==SpriteKind::COMPOSED){
        //ComposedSprite::CopyLayers(static_cast<ComposedSprite*>(this), static_cast<ComposedSprite*>(s), nullptr);
        int zOrder = s->layer->GetZOrder();
        ComposedSprite::CopyLayers(static_cast<ComposedSprite*>(this), static_cast<ComposedSprite*>(s), zOrder);
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

    if(copyFlags & Property::ID){
        name = _sprite->name;
        id = _sprite->id;
    }

    if(copyFlags & Property::TAG){
        tag = _sprite->tag;
    }

    if(copyFlags & Property::DATA){
        parameters = _sprite->parameters;
        dataFlags = _sprite->dataFlags;
    }

    if(copyFlags & Property::LINK){
        linkObject = _sprite->linkObject;
    }


    changeFlags |= copyFlags;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
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
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetScale(Vec2f _scale)
{
    scale = _scale;
    changeFlags |= Property::SCALE;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetRotation(float _rotation)
{
    rotation = _rotation;
    changeFlags |= Property::ROTATION;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetFlip(Vec2i _flip)
{
    flip = _flip;
    changeFlags |= Property::FLIP;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetHandle(Vec2f _handle)
{
    handle = _handle;
    changeFlags |= Property::HANDLE;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetAlpha(float _alpha)
{
    alpha = _alpha;
    changeFlags |= Property::ALPHA;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetOwnBlend(Blend _blend)
{
    ownBlend = _blend;
    changeFlags |= Property::BLEND;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetColorOverlayActive(bool _colorOverlayActive)
{
    colorOverlayActive = _colorOverlayActive;
    changeFlags |= Property::OVERLAY_COLOR;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetColorOverlayRGBA(ColorRGBA _colorOverlayRGBA)
{
    colorOverlayRGBA = _colorOverlayRGBA;
    changeFlags |= Property::OVERLAY_COLOR;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetColorOverlayBlend(ColorOverlayBlend _colorOverlayBlend)
{
    colorOverlayBlend = _colorOverlayBlend;
    changeFlags |= Property::OVERLAY_COLOR;
    layer->SetSpritesChanged(true);
    if(parentComposedSprite){
        parentComposedSprite->SetChildrenSpritesChanged(true);
    }
}


void Sprite::SetChangeFlags(int _changeFlags)
{
    changeFlags = _changeFlags;
    if(changeFlags !=0){
        layer->SetSpritesChanged(true);
        if(parentComposedSprite){
            parentComposedSprite->SetChildrenSpritesChanged(true);
        }
    }
}


void Sprite::AppendToChangeFlags(int _changeFlags)
{
    changeFlags |= _changeFlags;
    if(changeFlags !=0 ){
        layer->SetSpritesChanged(true);
        if(parentComposedSprite){
            parentComposedSprite->SetChildrenSpritesChanged(true);
        }
    }
}



Vec2f Sprite::GetPosition(bool _includeAnimatedProperties)
{

    if(_includeAnimatedProperties && ap){
        return position + ap->translation;
    }
    return position;

}

/*
Vec2f Sprite::GetFullPosition()
{
    if(ap){
        return position+ap->translation;
    }

}
*/


Vec2f Sprite::GetGlobalPosition(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldPos(position+ap->translation) : position+ap->translation;
    }

    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldPos(position) : position;
}


/*
Vec2f Sprite::GetFullGlobalPosition()
{
    if(ap){
        return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldPos(position+ap->translation) : position+ap->translation;
    }

    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldPos(position) : position;
}
*/


//-----


Vec2f Sprite::GetScale(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        return scale * ap->scale;
    }
    return scale;
}


/*
Vec2f Sprite::GetScale()
{
    if(ap){
        return scale * ap->scale;
    }
    return scale;
}
*/

/*
Vec2f Sprite::GetlScaleWithAni()
{
    if(ap){
        return scale * ap->scale;
    }
    return scale;
}
*/


Vec2f Sprite::GetGlobalScale(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldScale(scale*ap->scale) : scale*ap->scale;
    }
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldScale(scale) : scale;
}


/*
Vec2f Sprite::GetGlobalScaleWithAni()
{
    if(ap){
        return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldScale(scale*ap->scale) : scale*ap->scale;
    }
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldScale(scale) : scale;
}
*/

//-----


float Sprite::GetRotation(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        return rotation + ap->rotation;
    }
    return rotation;
}

/*
float Sprite::GetGlobalRotation()
{
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldRotation(rotation) : rotation;
}
*/


float Sprite::GetGlobalRotation(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldRotation(rotation + ap->rotation) : rotation + ap->rotation;
    }
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldRotation(rotation) : rotation;
}


//-----


Vec2i Sprite::GetFlip(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        Vec2i f = flip;
        if(ap->flip.x) f.x = 1 - f.x;
        if(ap->flip.y) f.y = 1 - f.y;
        return f;

    }
    return flip;
}

/*
Vec2i Sprite::GetGlobalFlip()
{
    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldFlip(flip) : flip;
}

*/

Vec2i Sprite::GetGlobalFlip(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        Vec2i f = flip;
        if(ap->flip.x) f.x = 1 - f.x;
        if(ap->flip.y) f.y = 1 - f.y;
        return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldFlip(f) : f;
    }

    return (parentComposedSprite!=nullptr)? parentComposedSprite->ConvertToWorldFlip(flip) : flip;
}


//-----

float Sprite::GetAlpha(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap){
        return alpha * ap->alpha;
    }
    return alpha;
}


//-----


bool Sprite::IsOverlayColorActive()
{

    if(ap && ap->changeFlags & Property::OVERLAY_COLOR){
        return true;
    }
    return colorOverlayActive;
}



ColorRGBA Sprite::GetOverlayColorRGBA(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap && ap->changeFlags & Property::OVERLAY_COLOR){
        return ap->colorOverlayRGBA;
    }
    return colorOverlayRGBA;
}


ColorOverlayBlend Sprite::GetOverlayColorBlend(bool _includeAnimatedProperties)
{
    if(_includeAnimatedProperties && ap && ap->changeFlags & Property::OVERLAY_COLOR){
        return ap->colorOverlayBlend;
    }
    return colorOverlayBlend;
}



void Sprite::CreateAnimatedPropertiesIfNone()
{

    if(ap==nullptr){
        ap = new AnimatedProperties();
    }

}


void Sprite::ResetAnimatedProperties()
{

    if(ap){
        ap->Reset();
    }

}


void Sprite::AppendAnimatedProperties(AnimatedProperties& _ap)
{

    ap->Append(_ap);
    AppendToChangeFlags(_ap.changeFlags);

    //if(sourceSprite->GetName()=="rotatorA"){
    //    DbgOutput("SetAnimatedProperties _ap pos x:" + std::to_string(_ap.translation.x)+" y:" + std::to_string(_ap.translation.y));
    //    DbgOutput("SetAnimatedProperties rotatorA pos x:" + std::to_string(GetGlobalPosition().x)+" y:" + std::to_string(GetGlobalPosition().y));

    //}
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
        //if(GetHandle().Equals(activeImage->GetHandle())==false){
            SetHandle(activeImage->handle);
        //}
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


void StandardSprite::AppendAnimatedProperties(AnimatedProperties &_ap)
{


    Sprite::AppendAnimatedProperties(_ap);

    if(_ap.changeFlags & Property::TEXTURE){
        if(activeImage !=_ap.graphicItem){
            SetActiveImage(_ap.graphicItem);
        }
    }

    //if(GetNameID()=="batTest" && _ap.changeFlags & Property::TEXTURE){
        //DbgOutput("spiderA texture:" + GetActiveImage()->GetName());
    //    DbgOutput("batTest texture:"+ GetActiveImage()->GetName() + "  pos x:" + std::to_string(_ap.translation.x)+" y:"+ std::to_string(_ap.translation.y));

    //}

}


//=======================================================================================


void ComposedSprite::CopyLayers(ComposedSprite *csSource, ComposedSprite *csDest, int &zOrder)
{

    //--- SPRITE LAYERS
    for(Layer *l : csSource->layers){

        if(l->GetKind()==LayerKind::SPRITE){

            SpriteLayer * sl = static_cast<SpriteLayer*>(l);

            SpriteLayer *lCopy = objectFactory->NewSpriteLayer();
            lCopy->map = csDest->layer->map;                // Using csDest because csSource can be SourceComposedSprite where the map is null.
            lCopy->parentComposedSprite = csDest;
            lCopy->kind = sl->kind;
            lCopy->name = sl->name;
            lCopy->parameters = sl->parameters;

            //--- set depth when building
            lCopy->zOrder = zOrder;
            zOrder += settings.GetZOrderStep();

            //DbgOutput("CS Layer:"+lCopy->GetName() + " zOrder:" + std::to_string(lCopy->zOrder));

            //----
            for(Sprite *s : sl->sprites){

                Sprite *sCopy = objectFactory->NewSprite(s->GetKind());
                sCopy->sourceSprite = s->sourceSprite;
                sCopy->layer = lCopy;
                //sCopy->map = lCopy->map;
                sCopy->parentComposedSprite = lCopy->parentComposedSprite;
                sCopy->CopyProperties(s, Property::ALL);
                if(s->GetKind()==SpriteKind::COMPOSED){
                    CopyLayers(static_cast<ComposedSprite*>(s), static_cast<ComposedSprite*>(sCopy), zOrder);
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

            vlCopy->zOrder = zOrder;
            zOrder += settings.GetZOrderStep();


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

            tlCopy->zOrder = zOrder;
            zOrder += settings.GetZOrderStep();

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
    childrenSpritesChanged = true;

    if(sourceSprite->name=="rotatorA"){
        DummyFunction();
    }

    for(Layer *l : layers){
        l->InitEngineLayer();
    }
}


void ComposedSprite::UpdateEngineSprite()
{
    if(visible==false) return;

    if(changeFlags!=0){
        UpdateTransform();
    }

    if(childrenSpritesChanged || changeFlags!=0){
        for(Layer *l : layers){
            if(l->GetKind()==LayerKind::SPRITE){
                SpriteLayer * sl = static_cast<SpriteLayer*>(l);
                for(Sprite *s : sl->GetSprites()){
                    s->SetChangeFlags(s->GetChangeFlags() | changeFlags);
                    s->UpdateEngineSprite();
                }
            }
        }
    }

    SetChangeFlags(0);
    childrenSpritesChanged = false;

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


void ComposedSprite::ResetAnimatedProperties()
{

    Sprite::ResetAnimatedProperties();

    for(Layer *l : GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);
            for(Sprite *s : sl->GetSprites()){
                s->ResetAnimatedProperties();
            }
        }
    }

}


void ComposedSprite::UpdateTransform()
{

    Vec2f realPosition = GetPosition();
    Vec2f realScale = GetScale();
    float realRotation = GetRotation();
    Vec2i realFlip = GetFlip();
    Vec2f handle = GetHandle();


    if(changeFlags == Property::POSITION){    // changed only position

        cTransform = MakeTransformationMatrix(realPosition+GetLayer()->GetParallaxOffset(), realScale, realFlip, realRotation, handle);
        if(parentComposedSprite){
            cTransform = parentComposedSprite->cTransform * cTransform;
        }
        return;
    }

    cTransform = MakeTransformationMatrix(position+GetLayer()->GetParallaxOffset(), realScale, realFlip, realRotation, handle);
    cFlip = realFlip;
    cScale = realScale.x;      // composed sprites have uniform scale !
    cRotation = realRotation;

    //---
    if(parentComposedSprite){
        cTransform = parentComposedSprite->cTransform * cTransform;
        cScale *= parentComposedSprite->cScale;

        if(parentComposedSprite->cFlip.y + parentComposedSprite->cFlip.x == 1){     // one and only one flip changes rotation sign
            cRotation = parentComposedSprite->cRotation - realRotation;
        }else{
            cRotation = parentComposedSprite->cRotation + realRotation;
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



