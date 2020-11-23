
//===================================================================================================
//      JugiMap API extension for Cocos2d-x.
//===================================================================================================


#include <assert.h>
#include <algorithm>
#include "jmCocos2d-x.h"



namespace jugimap {




BinaryFileStreamReaderCC::BinaryFileStreamReaderCC(const std::string &fileName) : BinaryBufferStreamReader(nullptr, 0, true)
{

    cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(fileName);

    // The commented code bellow causes crash when destructor deletes the buffer. No idea why.
    //ssize_t len = 0;
    //buff = data.takeBuffer(&len);
    //length = len;


    unsigned char* b = data.getBytes();

    size = data.getSize();

    if(size>0){
        buff = new unsigned char[size];
        memcpy(buff, b, size);
    }

    ownedBuffer = true;

}


TextFileStreamReaderCC::TextFileStreamReaderCC(const std::string &fileName)
{

    std::string text = cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);

    //jugimap::DbgOutput(text);

    if(text==""){
        return;
    }

    VerifyAndFixUtf8string(text);


    int count = 0;

    std::string s;

    char *i = (char*)text.data();
    char *iEnd = (char*)text.data() + text.size();
    while(i<iEnd){
        int unicode = utf8::next(i, iEnd);

        bool newLine = false;

        if(unicode==0x0D){              // CR (carriage return)
            int unicodeNext = utf8::peek_next(i, iEnd);
            if(unicodeNext==0x0A){      // LF (line feed)
                unicode = utf8::next(i, iEnd);
            }
            newLine = true;

        }else if(unicode==0x0A){        // LF (line feed)
            newLine = true;

        }else{

            utf8::append(unicode, s);
        }

        if(newLine || i==iEnd){
            lines.push_back(s);
            s = "";
        }

        //qDebug()<<hex<<count<<" : "<< unicode;
        count++;
    }
}


bool TextFileStreamReaderCC::IsOpen()
{
    return !lines.empty();

}


void TextFileStreamReaderCC::ReadLine(std::string &s)
{
    assert(index>=0 && index<lines.size());

    s = lines[index];
    index++;
}


bool TextFileStreamReaderCC::Eof()
{
    return index>=lines.size();

}


void TextFileStreamReaderCC::Close()
{
    index = 0;
}



//===================================================================================================



int GraphicFileCC::textureSmoothFilter = true;


GraphicFileCC::~GraphicFileCC()
{
    if(texture){
        cocos2d::TextureCache *cache = cocos2d::Director::getInstance()->getTextureCache();
        cache->removeTexture(texture);
        texture = nullptr;
    }
}


void GraphicFileCC::Init()
{
    if(texture) return;            // already loaded

    GraphicFile::Init();


    if(GetKind()==FileKind::SPINE_FILE){

        // spine atlas file: GetSpineAtlasRelativeFilePath()
        // spine skeleton file: GetRelativeFilePath()

    }else if(GetKind()==FileKind::SPRITER_FILE){

        // spriter model file: GetRelativeFilePath()

    }else{      // image

        cocos2d::TextureCache *cache = cocos2d::Director::getInstance()->getTextureCache();
        texture = cache->addImage(GetRelativeFilePath());
        //assert(texture);

    }
}


//===================================================================================================



void StandardSpriteCC::InitEngineObjects()
{

    if(ccSprite) return;             //already initialized

    GetSourceSprite()->Init();

    //---
    ccSprite = cocos2d::Sprite::create();
    ccSprite->setOpacityModifyRGB(true);
    ccSprite->setLocalZOrder(GetLayer()->GetZOrder());

    static_cast<MapCC*>(GetLayer()->GetMap())->GetMapNode()->addChild(ccSprite);    // add sprite to map node


    if(settings.SpriteCollidersEnabled()){
        if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_IMAGES_WITH_SPRITE_SHAPES){

            // We use jugimap colliders as Cocos2d-x lacks functions for
            // quick collision detection (overlaping sprites, point over sprite etc...)
            // To use Cocos2d-x collision system via its physcis engine
            // use SetPhysicsMode

            if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_MULTIPLE_SPRITE_SHAPES_IN_IMAGE){
                SetCollider(new MultiShapesCollider(this));
            }else{
                SetCollider(new SingleShapeCollider(this));
            }
        }
    }

    //---
    GraphicItem *gi = GetSourceSprite()->GetGraphicItems().front();
    SetActiveImage(gi);

    //---
    SetChangeFlags(Property::ALL);
}


void StandardSpriteCC::SetActiveImage(GraphicItem *_image)
{
    if(GetActiveImage()==_image) return;

    StandardSprite::SetActiveImage(_image);

    //----
    GraphicItem *image = GetActiveImage();
    if(image==nullptr){                                     // image can also be nullptr!
        if(ccSprite->isVisible()){
            ccSprite->setVisible(false);
        }
        return;

    }else{

        if(ccSprite->isVisible()==false && IsVisible()){
            ccSprite->setVisible(true);
        }
    }


    GraphicFileCC * gf = static_cast<GraphicFileCC*>(image->GetFile());
    ccSprite->setTexture(gf->GetTexture());
    cocos2d::Rect rect(GetActiveImage()->GetPos().x, GetActiveImage()->GetPos().y, GetActiveImage()->GetWidth(), GetActiveImage()->GetHeight());
    ccSprite->setTextureRect(rect);
    cocos2d::Vec2 anchorPoint(GetHandle().x/GetActiveImage()->GetWidth(), GetHandle().y/GetActiveImage()->GetHeight());
    ccSprite->setAnchorPoint(anchorPoint);

    SetColliderShapes();

}


void StandardSpriteCC::SetPhysicsMode(PhysicsMode _physicsMode)
{

    if(settings.EnginePhysicsEnabled()==false) return;

    //--- Sprite must have defined shapes; all frames should have equal shapes!
    GraphicItem *image = GetActiveImage();
    if(image==nullptr || image->GetSpriteShapes().empty()){
        return;
    }

    if(_physicsMode==GetPhysicsMode()) return;


    //---
    StandardSprite::SetPhysicsMode(_physicsMode);

    cocos2d::PhysicsBody * body = ccSprite->getPhysicsBody();


    //---- Set body
    if(_physicsMode==PhysicsMode::NO_PHYSICS){

        if(body){
            ccSprite->removeComponent(body);
            body = nullptr;
        }

    }else if(_physicsMode==PhysicsMode::STATIC || _physicsMode==PhysicsMode::KINEMATIC){

        if(body==nullptr){
            body = cocos2d::PhysicsBody::create();
            ccSprite->addComponent(body);
        }
        body->setDynamic(false);

    }else if(_physicsMode==PhysicsMode::DYNAMIC){

        if(body==nullptr){
            body = cocos2d::PhysicsBody::create();
            ccSprite->addComponent(body);
        }
        body->setDynamic(true);
    }


    //---- Make shapes for physics body
    if(body && body->getShapes().empty()){

        //cocos2d::PhysicsMaterial mat(1.0, 0.3, 0.7);
        //cocos2d::PHYSICSSHAPE_MATERIAL_DEFAULT

        for(VectorShape *vs : image->GetSpriteShapes()){

            GeometricShape *gs = vs->GetGeometricShape();
            cocos2d::PhysicsShape * shape = nullptr;

            if(gs->GetKind()==ShapeKind::POLYLINE){
                PolyLineShape * poly = static_cast<PolyLineShape*>(gs);

                Rectf r(poly->boundingBoxMin, poly->boundingBoxMax);

                if(poly->rectangle){

                    // The way cocos2d-x creates box shapes is weird. Instead of defining rectangle area relative to the image anchor point
                    // you specify just width and height. What if you have a smaller box inside image with some arbitrary anchor point?
                    // It seems the code bellow position shapes correctly (got by trial and error - no idea why x and y value for offset are calculated differently.)

                    cocos2d::Vec2 offset(r.GetCenter().x - image->GetWidth()/2.0, image->GetHeight()/2.0-r.GetCenter().y);
                    shape = cocos2d::PhysicsShapeBox::create(cocos2d::Size(r.GetWidth(), r.GetHeight()), physicsMaterial, offset);

                }else{
                    if(poly->convex){
                        std::vector<cocos2d::Vec2>vertices;
                        for(Vec2f v : poly->vertices){
                            vertices.push_back(cocos2d::Vec2(v.x, v.y));
                        }
                        // Unlike for box shape, it seems there is no offset required here !?!
                        shape = cocos2d::PhysicsShapePolygon::create(vertices.data(), vertices.size(), physicsMaterial);
                    }
                }

            }else if(gs->GetKind()==ShapeKind::ELLIPSE){
                EllipseShape * circle = static_cast<EllipseShape*>(gs);

                // It seems no offset is needed here but not tested with the off the center circles...
                shape = cocos2d::PhysicsShapeCircle::create(circle->a, physicsMaterial);
            }

            body->addShape(shape);
        }
    }

}


void StandardSpriteCC::SetPhysicsMaterial(cocos2d::PhysicsMaterial _physicsMaterial)
{

    physicsMaterial = _physicsMaterial;

    cocos2d::PhysicsBody* body = ccSprite->getPhysicsBody();
    if(body==nullptr){
        body = cocos2d::PhysicsBody::create();
        ccSprite->addComponent(body);
    }

    for(cocos2d::PhysicsShape *shape : body->getShapes()){
        shape->setMaterial(physicsMaterial);
    }
}


void StandardSpriteCC::CopyProperties(Sprite *_sprite, int copyFlags)
{
    StandardSpriteCC *s = dynamic_cast<StandardSpriteCC*>(_sprite);

    StandardSprite::CopyProperties(_sprite, copyFlags);



    int i=1;
    DummyFunction();


}


void StandardSpriteCC::UpdateEngineObjects()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TRANSFORMATION){

        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();

        Vec2f scaleGlobal = GetGlobalScale();
        float rotationGlobal = GetGlobalRotation();
        Vec2i flipGlobal = GetGlobalFlip();


        if (flags & Property::HANDLE){
            if(GetActiveImage()){
                cocos2d::Vec2 anchorPoint(GetHandle().x/GetActiveImage()->GetWidth(), GetHandle().y/GetActiveImage()->GetHeight());
                ccSprite->setAnchorPoint(anchorPoint);
            }
        }
        if(flags & Property::POSITION){
            ccSprite->setPosition(posGlobal.x, posGlobal.y);
        }
        if(flags & Property::SCALE){
            ccSprite->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }
        if(flags & Property::ROTATION){
            ccSprite->setRotation(rotationGlobal);
        }
        if(flags & Property::FLIP){
            ccSprite->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }

        UpdateColliderAndBoundingBox();
    }


    if(flags & Property::APPEARANCE){


        if(flags & Property::OVERLAY_COLOR){
            ManageShaders_OverlayColor();
        }


        if(flags & Property::ALPHA){
            ccSprite->setOpacity(GetAlpha() * GetLayer()->GetAlpha()*255);
        }


        if(flags & Property::BLEND){
            Blend blend = GetOwnBlend();
            if(blend==Blend::NOT_DEFINED) blend = GetLayer()->GetBlend();

            if(blend==Blend::ALPHA){
                //ccSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
                ccSprite->setBlendFunc({GL_ONE, GL_ONE_MINUS_SRC_ALPHA});               // that one is correct as cocos2d by default premultiply alpha

            }else if(blend==Blend::MULTIPLY){
                ccSprite->setBlendFunc({GL_DST_COLOR, GL_ZERO});

            }else if(blend==Blend::ADDITIVE){
                ccSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

            }else if(blend==Blend::SOLID){
                ccSprite->setBlendFunc({GL_ONE, GL_ZERO});

            }else{
                ccSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
            }
        }

    }

    if(flags & Property::VISIBILITY){

        if(IsVisible()){
            if(GetActiveImage()){
                ccSprite->setVisible(true);
            }

        }else{
            ccSprite->setVisible(false);
        }

    }

    //---
    SetChangeFlags(0);
}


void StandardSpriteCC::ManageShaders_OverlayColor()
{

    if(IsOverlayColorActive()==false){
        if(glProgramState){
            GetCCSprite()->setGLProgramState(cocos2d::GLProgramState::getOrCreateWithGLProgramName(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
            glProgramState = nullptr;
        }

    }else{

        ColorRGBA colorRGBA = GetOverlayColorRGBA();
        ColorRGBAf c(colorRGBA);


        switch (GetOverlayColorBlend()) {


        case ColorOverlayBlend::SIMPLE_MULTIPLY:

            if(shaders::glpColorOverlay_blendSimpleMultiply==nullptr) return;

            if(glProgramState==nullptr){
                glProgramState = cocos2d::GLProgramState::create(shaders::glpColorOverlay_blendSimpleMultiply);
                GetCCSprite()->setGLProgramState(glProgramState);
            }
            if(glProgramState->getGLProgram()!=shaders::glpColorOverlay_blendSimpleMultiply){
                glProgramState->setGLProgram(shaders::glpColorOverlay_blendSimpleMultiply);
            }
            glProgramState->setUniformVec4(shaders::nameOverlayColor,  cocos2d::Vec4(c.r, c.g, c.b, c.a));

            break;

        case ColorOverlayBlend::NORMAL:

            if(shaders::glpColorOverlay_blendNormal==nullptr) return;

            if(glProgramState==nullptr){
                glProgramState = cocos2d::GLProgramState::create(shaders::glpColorOverlay_blendNormal);
                GetCCSprite()->setGLProgramState(glProgramState);
            }
            if(glProgramState->getGLProgram()!=shaders::glpColorOverlay_blendNormal){
                glProgramState->setGLProgram(shaders::glpColorOverlay_blendNormal);
            }
            glProgramState->setUniformVec4(shaders::nameOverlayColor,  cocos2d::Vec4(c.r, c.g, c.b, c.a));

            break;

        case ColorOverlayBlend::MULTIPLY:

            if(shaders::glpColorOverlay_blendMultiply==nullptr) return;

            if(glProgramState==nullptr){
                glProgramState = cocos2d::GLProgramState::create(shaders::glpColorOverlay_blendMultiply);
                GetCCSprite()->setGLProgramState(glProgramState);
            }
            if(glProgramState->getGLProgram()!=shaders::glpColorOverlay_blendMultiply){
                glProgramState->setGLProgram(shaders::glpColorOverlay_blendMultiply);
            }
            glProgramState->setUniformVec4(shaders::nameOverlayColor,  cocos2d::Vec4(c.r, c.g, c.b, c.a));

            break;

        case ColorOverlayBlend::LINEAR_DODGE:
            if(shaders::glpColorOverlay_blendLinearDodge==nullptr) return;

            if(glProgramState==nullptr){
                glProgramState = cocos2d::GLProgramState::create(shaders::glpColorOverlay_blendLinearDodge);
                GetCCSprite()->setGLProgramState(glProgramState);
            }
            if(glProgramState->getGLProgram()!=shaders::glpColorOverlay_blendLinearDodge){
                glProgramState->setGLProgram(shaders::glpColorOverlay_blendLinearDodge);
            }
            glProgramState->setUniformVec4(shaders::nameOverlayColor,  cocos2d::Vec4(c.r, c.g, c.b, c.a));

            break;

        case ColorOverlayBlend::COLOR:

            if(shaders::glpColorOverlay_blendColor==nullptr) return;

            if(glProgramState==nullptr){
                glProgramState = cocos2d::GLProgramState::create(shaders::glpColorOverlay_blendColor);
                GetCCSprite()->setGLProgramState(glProgramState);
            }
            if(glProgramState->getGLProgram()!=shaders::glpColorOverlay_blendColor){
                glProgramState->setGLProgram(shaders::glpColorOverlay_blendColor);
            }
            glProgramState->setUniformVec4(shaders::nameOverlayColor,  cocos2d::Vec4(c.r, c.g, c.b, c.a));

            break;
        }

    }

}


//===================================================================================================



void TextSpriteCC::InitEngineObjects()
{

    if(ccLabel) return;             //already initialized

    TextSprite::InitEngineObjects();

    //----
    if(GetFont()->GetKind()==FontKind::TRUE_TYPE){
        ccLabel = cocos2d::Label::createWithTTF(GetTextString(), GetFont()->GetRelativeFilePath(), GetFont()->GetLoadFontSize());
    }else if(GetFont()->GetKind()==FontKind::BITMAP_FONT__FNT){
        ccLabel = cocos2d::Label::createWithBMFont(GetFont()->GetRelativeFilePath(), GetTextString());
    }

    //ccLabel->setAnchorPoint(cocos2d::Vec2(GetHandle().x, GetHandle().y));
    ccLabel->setLocalZOrder(GetLayer()->GetZOrder());
    static_cast<MapCC*>(GetLayer()->GetMap())->GetMapNode()->addChild(ccLabel);

    float w = ccLabel->getContentSize().width;
    float h = ccLabel->getContentSize().height;

    /*
    float hx = w * _relativeHandle.x;
    float hy = h * _relativeHandle.y;

    if(hy>0.001){
        float hyRounded = std::roundf(hy);
        _relativeHandle.y = hyRounded / h;
    }

    _SetRelativeHandle(_relativeHandle);
    */

    SetHandle(Vec2f(GetRelativeHandle().x*w,  GetRelativeHandle().y*h));

    //---
    SetChangeFlags(Property::ALL);

}


void TextSpriteCC::UpdateEngineObjects()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false && GetChangeFlags()==0) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();

    if(GetTextString()=="OPTIONS"){
        DummyFunction();
    }


    if(flags & Property::TEXT_STRING){

        ccLabel->setString(GetTextString());
        Vec2f size(ccLabel->getContentSize().width, ccLabel->getContentSize().height);
        _SetSize(size);
        SetHandle(size*GetRelativeHandle());

        //----
        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();
        jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, GetGlobalScale(), GetGlobalFlip(),  GetGlobalRotation(), GetHandle());
        SetBoundingBox(TransformRectf(Rectf(0,0, size.x, size.y), m));

        //----
        flags &= ~Property::TEXT_STRING;    // ! Because GetSize() used later in this function checks for this flag and call UpdateEngineSprite !
        SetChangeFlags(flags);
    }


    if(flags & Property::VISIBILITY){
        ccLabel->setVisible(IsVisible());


    }


    if(flags & Property::TRANSFORMATION){

        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();

        Vec2f scaleGlobal = GetGlobalScale();
        float rotationGlobal = GetGlobalRotation();
        Vec2i flipGlobal = GetGlobalFlip();

        if (flags & Property::HANDLE){
            //cocos2d::Vec2 anchor = cocos2d::Vec2(GetHandle().x/ccLabel->getContentSize().width, GetHandle().y/ccLabel->getContentSize().height);     // handle must be in range 0 - 1
            cocos2d::Vec2 anchor = cocos2d::Vec2(GetRelativeHandle().x, GetRelativeHandle().y);
            ccLabel->setAnchorPoint(anchor);
        }
        if(flags & Property::POSITION){
            ccLabel->setPosition(posGlobal.x, posGlobal.y);
        }
        if(flags & Property::SCALE){
            ccLabel->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }
        if(flags & Property::ROTATION){
            ccLabel->setRotation(rotationGlobal);
        }
        if(flags & Property::FLIP){
            ccLabel->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }

        //----
        jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, scaleGlobal,  flipGlobal, rotationGlobal, GetHandle());
        SetBoundingBox(TransformRectf(Rectf(0,0, GetSize().x, GetSize().y), m));

    }


    if(flags & Property::APPEARANCE){

        if(flags & Property::COLOR){
            ccLabel->setColor(cocos2d::Color3B(GetColor().r, GetColor().g, GetColor().b));
        }
        if(flags & Property::ALPHA){
            ccLabel->setOpacity(GetAlpha() * 255);
        }
    }


    //---
    SetChangeFlags(0);
}


//===================================================================================================


void SpriteLayerCC::RemoveAndDeleteSprite(Sprite* _sprite)
{

    if(_sprite->GetKind()==SpriteKind::STANDARD){
        if(settings.IsAppTerminated()==false){
            static_cast<StandardSpriteCC*>(_sprite)->GetCCSprite()->removeFromParent();
        }

    }else if(_sprite->GetKind()==SpriteKind::TEXT){

        if(settings.IsAppTerminated()==false){
            TextSpriteCC * tsCC = static_cast<TextSpriteCC*>(_sprite);
            cocos2d::Label* ccLabel = tsCC->GetCCLabel();
            ccLabel->removeFromParent();
        }

    }else if(_sprite->GetKind()==SpriteKind::COMPOSED){

    }else if(_sprite->GetKind()==SpriteKind::SPINE){

    }else if(_sprite->GetKind()==SpriteKind::SPRITER){

    }

    SpriteLayer::RemoveAndDeleteSprite(_sprite);

}


//===================================================================================================



void DrawerCC::InitEngineDrawer()
{

    drawNode = cocos2d::DrawNode::create(1);


    if(GetDrawingLayer()->IsMapLayer()){
        static_cast<MapCC*>(GetDrawingLayer()->GetMap())->GetMapNode()->addChild(drawNode);
        drawNode->setLocalZOrder(GetDrawingLayer()->GetZOrder());

    }else{
        assert(false);
        settings.SetErrorMessage("DrawerCC::InitEngineDrawer() error - drawing layer can not be stored in a composed sprite!");
    }
}



void DrawerCC::UpdateEngineDrawer()
{
    GetDrawingLayer()->DrawEngineObjects();

}



void DrawerCC::Clear()
{
    Drawer::Clear();
    drawNode->clear();
}


void DrawerCC::SetOutlineColor(ColorRGBA _outlineColor)
{
    outlineColor = cocos2d::Color4F(_outlineColor.r/255.0, _outlineColor.g/255.0, _outlineColor.b/255.0, _outlineColor.a/255.0);
}


void DrawerCC::Line(Vec2f p1, Vec2f p2)
{
    drawNode->drawLine(cocos2d::Vec2(p1.x,p1.y), cocos2d::Vec2(p2.x,p2.y), outlineColor);
}


void DrawerCC::RectangleOutline(const Rectf &rect)
{
    drawNode->drawRect(cocos2d::Vec2(rect.min.x, rect.min.y),
                       cocos2d::Vec2(rect.max.x, rect.min.y),
                       cocos2d::Vec2(rect.max.x, rect.max.y),
                       cocos2d::Vec2(rect.min.x, rect.max.y),
                       outlineColor);
}


void DrawerCC::EllipseOutline(Vec2f c, Vec2f r)
{

    float ra = (std::fabs(r.x) + std::fabs(r.y)) / 2;
    float da = std::acos(ra / (ra + 0.125 /1.0)) * 2;
    int n = std::round(2*mathPI / da + 0.5);

    //if(outline.size()<n+1) outline.resize(n+1);

    float pxPrev = c.x + std::cos(0) * r.x;
    float pyPrev = c.y + std::sin(0) * r.y;

    for(int i=1; i<=n; i++){
        float angle = i * 2*mathPI/n;
        float px = c.x + std::cos( angle ) * r.x;
        float py = c.y + std::sin( angle ) * r.y;
        //outline[i] =  cocos2d::Vec2(px, py);
        Line(Vec2f(pxPrev,pyPrev), Vec2f(px,py));
        pxPrev = px;
        pyPrev = py;

    }

}


void DrawerCC::Dot(Vec2f p)
{

    drawNode->drawDot(cocos2d::Vec2(p.x, p.y), 2, outlineColor);
}


//===================================================================================================




FontCC::FontCC(const std::string &_relativeFilePath, int _loadFontSize, FontKind _kind) : Font(_relativeFilePath, _loadFontSize, _kind)
{

    if(_kind==FontKind::TRUE_TYPE){
        ccLabel = cocos2d::Label::createWithTTF("test", GetRelativeFilePath(), GetLoadFontSize());
        ccLabel->retain();

    }else if(_kind==FontKind::BITMAP_FONT__FNT){
        ccLabel = cocos2d::Label::createWithBMFont(_relativeFilePath, "test");
        ccLabel->retain();

    }


    pixelHeightCommon = GetPixelHeight(textForHeight);


}


FontCC::~FontCC()
{
    if(ccLabel){
        delete ccLabel;
    }
}


int FontCC::GetPixelWidth(const std::string &s)
{

    ccLabel->setString(s);
    ccLabel->updateContent();
    int width = ccLabel->getWidth();
    cocos2d::Size size = ccLabel->getContentSize();

    return size.width;
}


int FontCC::GetPixelHeight(const std::string &s)
{

    ccLabel->setString(s);
    ccLabel->updateContent();
    cocos2d::Size size = ccLabel->getContentSize();
    return size.height;
}


Vec2f FontCC::GetPixelSize(const std::string &s)
{

    ccLabel->setString(s);
    ccLabel->updateContent();
    cocos2d::Size size = ccLabel->getContentSize();
    return Vec2f(size.width, size.height);
}



//===================================================================================================


MapCCNode* MapCCNode::Create(MapCC *_map)
{

    MapCCNode* mn = new MapCCNode();
    if(mn && mn->init()){
        mn->map = _map;
        mn->autorelease();
        return mn;
    }
    CC_SAFE_DELETE(mn);
    return nullptr;

}


void MapCCNode::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{

    cocos2d:: GLView* glView = cocos2d::Director::getInstance()->getOpenGLView();

    if(map->GetCameras().empty()){
        if(map->GetCamera()){
            map->GetCameras().push_back(map->GetCamera());
        }else{
            return;
        }
    }


    for(Camera *c : map->GetCameras()){

        // if camera changes render everything in render queue to currentCamera viewport
        if(c!=MapCC::currentCamera && MapCC::currentCamera){
            cocos2d::Rect viewport(MapCC::currentCamera->GetViewport().min.x, MapCC::currentCamera->GetViewport().min.y, MapCC::currentCamera->GetViewport().GetWidth(), MapCC::currentCamera->GetViewport().GetHeight());
            //DbgOutput(" viewport x:" + std::to_string(viewport.origin.x)+" y:"+ std::to_string(viewport.origin.y)+" w:"+std::to_string(viewport.size.width));
            glView->setScissorInPoints(viewport.getMinX(), viewport.getMinY(), viewport.getMaxX()-viewport.getMinX(), viewport.getMaxY()-viewport.getMinY());
            renderer->render();
        }

        map->SetCamera(c);
        map->UpdateEngineObjects();
        cocos2d::Node::visit(renderer, transform, flags);
        //---
        MapCC::currentCamera = c;
    }


    // render the last child
    // --------------------------------------
    // find the last map node
    MapCCNode *lastMapNode = nullptr;
    for(int i = int(getParent()->getChildren().size())-1; i>=0; i--){
        lastMapNode = dynamic_cast<MapCCNode*>(getParent()->getChildren().at(i));
        if(lastMapNode) break;
    }

    if(this==lastMapNode){
        cocos2d::Rect viewport(MapCC::currentCamera->GetViewport().min.x, MapCC::currentCamera->GetViewport().min.y, MapCC::currentCamera->GetViewport().GetWidth(), MapCC::currentCamera->GetViewport().GetHeight());
        //DbgOutput(" viewport x:" + std::to_string(viewport.origin.x)+" y:"+ std::to_string(viewport.origin.y)+" w:"+std::to_string(viewport.size.width));
        glView->setScissorInPoints(viewport.getMinX(), viewport.getMinY(), viewport.getMaxX()-viewport.getMinX(), viewport.getMaxY()-viewport.getMinY());
        renderer->render();

        MapCC::currentCamera = nullptr;

        glDisable(GL_SCISSOR_TEST);
    }
}


//===================================================================================================


jugimap::Camera* MapCC::currentCamera = nullptr;


void MapCC::InitEngineObjects(MapType _mapType)
{

    ccMapNode = MapCCNode::Create(this);     // First create cocos2d node as it will be used during sprite initialization!
    Map::InitEngineObjects(_mapType);
}


void MapCC::UpdateEngineObjects()
{

    Map::UpdateEngineObjects();

    ccMapNode->setRotation(GetCamera()->GetRotation());
    ccMapNode->setScale(GetCamera()->GetScale());
    ccMapNode->setPosition(GetCamera()->GetProjectedMapPosition().x, GetCamera()->GetProjectedMapPosition().y);

}


void MapCC::SetVisible(bool _visible)
{
    Map::SetVisible(_visible);

    ccMapNode->setVisible(_visible);

}


//===================================================================================================


EngineSceneCC::EngineSceneCC(Scene *_scene) : EngineScene(_scene)
{

    ccNode = cocos2d::Node::create();
    assert(ccNode);

}




void EngineSceneCC::PostInit()
{

    // Add map cocos nodes to the cocos scene node

    for(Map* m : GetScene()->GetMaps()){
        MapCC * ccMap = static_cast<MapCC*>(m);
        ccNode->addChild(static_cast<jugimap::MapCC*>(ccMap)->GetMapNode());
    }

}


void EngineSceneCC::PostUpdate()
{

    for(Map* m : GetScene()->GetMaps()){
        MapCC * ccMap = static_cast<MapCC*>(m);
        for(Camera *c : ccMap->GetCameras()){
            c->ClearChangeFlags();
        }
    }

}


//==================================================================================================


EngineAppCC::EngineAppCC(App *_app) : EngineApp(_app)
{

    //appCCNode = AppCCNode::Create(GetApp());
    //assert(appCCNode);

}


void EngineAppCC::PreInit()
{


}


void EngineAppCC::PostInit()
{

    appCCNode = AppCCNode::Create(GetApp());

    EventsLayer* eventsLayer = EventsLayer::Create();
    appCCNode->addChild(eventsLayer);

    for(Scene* s : GetApp()->GetScenes()){
        EngineSceneCC *engineSceneCC = static_cast<EngineSceneCC *>(s->GetEngineScene());
        appCCNode->addChild(engineSceneCC->GetEngineNode());
    }

    if(fontLibrary.GetSize()>0){
        appCCNode->errorMessageLabel = cocos2d::Label::createWithTTF("Label", fontLibrary.At(0)->GetRelativeFilePath(), 14);
        appCCNode->errorMessageLabel->setAnchorPoint(cocos2d::Vec2(0,1));
        appCCNode->errorMessageLabel->setPosition(10, cocos2d::Director::getInstance()->getVisibleSize().height-10);
        appCCNode->errorMessageLabel->setString("");
        int zOrder = settings.GetMapZOrderStart() + 100 * settings.GetMapZOrderStep();      // a big z-order -> drawn above other maps
        appCCNode->errorMessageLabel->setLocalZOrder(zOrder);

        appCCNode->addChild(appCCNode->errorMessageLabel);
    }
}


void EngineAppCC::SetSystemMouseCursorVisible(bool _visible)
{
    cocos2d::Director::getInstance()->getOpenGLView()->setCursorVisible(_visible);
}



//===================================================================================================


int AppCCNode::glDrawnBatchesString = 0;
int AppCCNode::glDrawnVerticesString = 0;


AppCCNode* AppCCNode::Create(App *_jmApp)
{

    AppCCNode* sn = new AppCCNode();
    if(sn){
        sn->jmApp = _jmApp;
        if(sn->init()){
            sn->autorelease();
            //sn->jmScene->SetEngineSceneLink(sn);
            return sn;
        }
    }
    CC_SAFE_DELETE(sn);
    return nullptr;
}


AppCCNode::~AppCCNode()
{
    /*
    if(jmScene){
        sceneManager->RemoveScene(jmScene);
        delete jmScene;
        jmScene = nullptr;
    }
    */

    DummyFunction();

}


bool AppCCNode::init()
{


    if(jmApp->HasPhysicsEnabled()){
        if(cocos2d::Scene::initWithPhysics()==false){
            return false;
        }
    }else{
        if(cocos2d::Scene::init()==false){         //--- run scene without physics - everything works apart the crystals physics test
            return false;
        }
    }

    scheduleUpdate();

    return true;
}



void AppCCNode::update(float delta)
{

    //--- Manage errors
    if(jugimap::settings.GetErrorMessage() != ""){
        if(errorMessageLabel){
            errorMessageLabel->setString(jugimap::settings.GetErrorMessage());
        }
        return ;
    }

    //assert(sceneManager->GetCurrentScene()==jmApp);
    //if(sceneManager->GetCurrentScene()!=jmApp) return;

    jmApp->Update(delta);

    //sceneManager->Update(delta*1000);

}



//===================================================================================================


void AppCCNode::render(cocos2d::Renderer *renderer, const cocos2d::Mat4 &eyeTransform, const cocos2d::Mat4 *eyeProjection)
{
    glEnable(GL_SCISSOR_TEST);

    cocos2d::Scene::render(renderer,eyeTransform,eyeProjection);

    glDrawnBatchesString = renderer->getDrawnBatches();
    glDrawnVerticesString = renderer->getDrawnVertices();

    glDisable(GL_SCISSOR_TEST);
}




//=====================================================================================



EventsLayer* EventsLayer::Create()
{
    EventsLayer* el = new (std::nothrow) EventsLayer();
    if (el && el->Init())
    {
        el->autorelease();
        return el;
    }
    CC_SAFE_DELETE(el);
    return nullptr;
}


bool EventsLayer::Init()
{

    cocos2d::EventListenerMouse* mouseListener = cocos2d::EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(EventsLayer::OnMouseMove, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(EventsLayer::OnMouseUp, this);
    mouseListener->onMouseDown = CC_CALLBACK_1(EventsLayer::OnMouseDown, this);
    mouseListener->onMouseScroll = CC_CALLBACK_1(EventsLayer::OnMouseScroll, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    cocos2d::EventListenerKeyboard* keyboardListener = cocos2d::EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(EventsLayer::OnKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(EventsLayer::OnKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    cocos2d::EventListenerTouchOneByOne* touchListener = cocos2d::EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(EventsLayer::OnTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(EventsLayer::OnTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(EventsLayer::OnTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


    cocos2d::EventListenerController* controllerListener = cocos2d::EventListenerController::create();
    controllerListener->onConnected = CC_CALLBACK_2(EventsLayer::OnJoystickConnected, this);
    controllerListener->onKeyDown = CC_CALLBACK_3(EventsLayer::OnJoystickButtonDown, this);
    controllerListener->onKeyUp = CC_CALLBACK_3(EventsLayer::OnJoystickButtonUp, this);
    controllerListener->onAxisEvent = CC_CALLBACK_3(EventsLayer::OnJoystickAxisEvent, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(controllerListener, this);



    keysConversionTable.resize(256, jugimap::KeyCode::UNKNOWN);

    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_BACKSPACE] = jugimap::KeyCode::BACKSPACE;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_TAB] = jugimap::KeyCode::TAB;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_ENTER] = jugimap::KeyCode::ENTER;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_PAUSE] = jugimap::KeyCode::PAUSE;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE] = jugimap::KeyCode::ESCAPE;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_SPACE] = jugimap::KeyCode::SPACE;

    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_PG_UP] = jugimap::KeyCode::PAGEUP;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_PG_DOWN] = jugimap::KeyCode::PAGEDOWN;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_END] = jugimap::KeyCode::END;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_HOME] = jugimap::KeyCode::HOME;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW] = jugimap::KeyCode::LEFT;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW] = jugimap::KeyCode::UP;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] = jugimap::KeyCode::RIGHT;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW] = jugimap::KeyCode::DOWN;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_PRINT] = jugimap::KeyCode::PRINT;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_INSERT] = jugimap::KeyCode::INSERT;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_DELETE] = jugimap::KeyCode::DELETEkey;

    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_0] = jugimap::KeyCode::NUM_0;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_1] = jugimap::KeyCode::NUM_1;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_2] = jugimap::KeyCode::NUM_2;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_3] = jugimap::KeyCode::NUM_3;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_4] = jugimap::KeyCode::NUM_4;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_5] = jugimap::KeyCode::NUM_5;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_6] = jugimap::KeyCode::NUM_6;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_7] = jugimap::KeyCode::NUM_7;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_8] = jugimap::KeyCode::NUM_8;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_9] = jugimap::KeyCode::NUM_9;

    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_A] = jugimap::KeyCode::A;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_B] = jugimap::KeyCode::B;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_C] = jugimap::KeyCode::C;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_D] = jugimap::KeyCode::D;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_E] = jugimap::KeyCode::E;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F] = jugimap::KeyCode::F;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_G] = jugimap::KeyCode::G;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_H] = jugimap::KeyCode::H;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_I] = jugimap::KeyCode::I;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_J] = jugimap::KeyCode::J;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_K] = jugimap::KeyCode::K;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_L] = jugimap::KeyCode::L;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_M] = jugimap::KeyCode::M;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_N] = jugimap::KeyCode::N;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_O] = jugimap::KeyCode::O;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_P] = jugimap::KeyCode::P;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_Q] = jugimap::KeyCode::Q;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_R] = jugimap::KeyCode::R;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_S] = jugimap::KeyCode::S;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_T] = jugimap::KeyCode::T;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_U] = jugimap::KeyCode::U;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_V] = jugimap::KeyCode::V;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_W] = jugimap::KeyCode::W;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_X] = jugimap::KeyCode::X;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_Y] = jugimap::KeyCode::Y;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_Z] = jugimap::KeyCode::Z;

    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F1] = jugimap::KeyCode::F1;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F2] = jugimap::KeyCode::F2;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F3] = jugimap::KeyCode::F3;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F4] = jugimap::KeyCode::F4;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F5] = jugimap::KeyCode::F5;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F6] = jugimap::KeyCode::F6;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F7] = jugimap::KeyCode::F7;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F8] = jugimap::KeyCode::F8;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F9] = jugimap::KeyCode::F9;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F10] = jugimap::KeyCode::F10;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F11] = jugimap::KeyCode::F11;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_F12] = jugimap::KeyCode::F12;

    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_TILDE] = jugimap::KeyCode::TILDE;                     // ~
    //keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_HYPER] = jugimap::KeyCode::KEY_HYPHEN;                    // -
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_EQUAL] = jugimap::KeyCode::EQUAL;                    // =
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_BRACKET] = jugimap::KeyCode::LEFT_BRACKET;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_BRACKET] = jugimap::KeyCode::RIGHT_BRACKET;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_BACK_SLASH] = jugimap::KeyCode::BACKSLASH;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_SEMICOLON] = jugimap::KeyCode::SEMICOLON;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_QUOTE] = jugimap::KeyCode::QUOTE;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_COMMA] = jugimap::KeyCode::COMMA;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_PERIOD] = jugimap::KeyCode::PERIOD;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_SLASH] = jugimap::KeyCode::SLASH;

    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_SHIFT] = jugimap::KeyCode::LEFT_SHIFT;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_SHIFT] = jugimap::KeyCode::RIGHT_SHIFT;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_CTRL] = jugimap::KeyCode::LEFT_CONTROL;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_CTRL] = jugimap::KeyCode::RIGHT_CONTROL;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ALT] = jugimap::KeyCode::LEFT_ALT;
    keysConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ALT] = jugimap::KeyCode::RIGHT_ALT;
    //keyCodesConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_SYS] = jugimap::KeyCode::KEY_NONE;           // none
    //keyCodesConversionTable[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGT_SYS] = jugimap::KeyCode::KEY_NONE;           // none
    return true;
}


jugimap::MouseButton ConvertMouseButton(cocos2d::EventMouse::MouseButton b)
{

    switch (b){

    case cocos2d::EventMouse::MouseButton::BUTTON_LEFT :
        return jugimap::MouseButton::LEFT;

    case cocos2d::EventMouse::MouseButton::BUTTON_MIDDLE :
        return jugimap::MouseButton::MIDDLE;

    case cocos2d::EventMouse::MouseButton::BUTTON_RIGHT :
        return jugimap::MouseButton::RIGHT;

    default:
        return jugimap::MouseButton::NONE;

    }

}


void EventsLayer::OnMouseDown(cocos2d::Event *event)
{
    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //std::string str = "Mouse Down detected, Key: ";
    //str += std::to_string((int) e->getMouseButton());
    //jm::DbgOutput(str);

    //sceneMouse.SetPressed(true);
    //sceneMouse.SetHit(true);

    jugimap::MouseButton b = ConvertMouseButton(e->getMouseButton());
    jugimap::mouse._SetButtonState(b, true);

}


void EventsLayer::OnMouseUp(cocos2d::Event *event)
{

    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //std::string str = "Mouse Up detected, Key: ";
    //str += std::to_string((int) e->getMouseButton());
    //jm::DbgOutput(str);

    //sceneMouse.SetPressed(false);
    //sceneMouse.SetHit(false);

    jugimap::MouseButton b = ConvertMouseButton(e->getMouseButton());
    jugimap::mouse._SetButtonState(b, false);

}


void EventsLayer::OnMouseMove(cocos2d::Event *event)
{

    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;

    float x = e->getCursorX();
    float y = e->getCursorY();

    //sceneMouse.SetScreenPosition(jugimap::Vec2f(x,y));

    /*
    cocos2d::Scene *scene = cocos2d::Director::getInstance()->getRunningScene();
    if(dynamic_cast<SceneNode*>(scene)){
        SceneNode* sceneNode = static_cast<SceneNode*>(scene);
        sceneNode->GetJMScene()->UpdateProjectedMousePositions();
    }
    */

    jugimap::mouse._SetPosition(jugimap::Vec2i(x,y));

}


void EventsLayer::OnMouseScroll(cocos2d::Event *event)
{
    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //std::string str = "Mouse Scroll detected, X: ";
    //str = str + std::to_string(e->getScrollX()) + " Y: " + std::to_string(e->getScrollY());
    //jugimap::DbgOutput(str);

    float x = e->getScrollX();
    float y = e->getScrollY();

    jugimap::mouse._SetWheel(jugimap::Vec2i(x,y));


}


void EventsLayer::OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    //std::string str = "Pressed key: " + std::to_string((int)jugimap::KeyCode);
    //jugimap::DbgOutput(str);

    jugimap::KeyCode k = keysConversionTable[(int)keyCode];
    if(k==jugimap::KeyCode::UNKNOWN) return;

    jugimap::keyboard._SetKeyState(k, true);

}


void EventsLayer::OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    //std::string str = "Released key: " + std::to_string((int)jugimap::KeyCode);
    //jugimap::DbgOutput(str);

    jugimap::KeyCode k = keysConversionTable[(int)keyCode];
    if(k==jugimap::KeyCode::UNKNOWN) return;

    jugimap::keyboard._SetKeyState(k, false);

}


bool EventsLayer::OnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{

    cocos2d::EventTouch* e = (cocos2d::EventTouch*)event;

    int id = touch->getID();

    if(id<0 || id>9) return true;

    float x = touch->getLocationInView().x;
    float y = touch->getLocationInView().y;

    jugimap::touch._SetFingerState(id, true, jugimap::Vec2i(x,y));

    return true;
}


void EventsLayer::OnTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{

    cocos2d::EventTouch* e = (cocos2d::EventTouch*)event;

    int id = touch->getID();

    float x = touch->getLocationInView().x;
    float y = touch->getLocationInView().y;

    jugimap::touch._SetFingerState(id, true, jugimap::Vec2i(x,y));

}


void EventsLayer::OnTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{

    cocos2d::EventTouch* e = (cocos2d::EventTouch*)event;

    int id = touch->getID();

    float x = touch->getLocationInView().x;
    float y = touch->getLocationInView().y;

    jugimap::touch._SetFingerState(id, false, jugimap::Vec2i(x,y));

}


//===========================================================================================================


void EventsLayer::OnJoystickConnected(cocos2d::Controller* controller, cocos2d::Event* event)
{

    int joyId = controller->getDeviceId();
    if(joyId<0 || joyId>jugimap::joysticks.size()) return;

    jugimap::joysticks[joyId]._SetConnected(true);

}


void EventsLayer::OnJoystickDisconnected(cocos2d::Controller *controller, cocos2d::Event *event)
{
    int joyId = controller->getDeviceId();
    if(joyId<0 || joyId>jugimap::joysticks.size()) return;

    jugimap::joysticks[joyId]._SetConnected(false);

}


void EventsLayer::OnJoystickButtonDown(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{

    int joyId = controller->getDeviceId();
    if(joyId<0 || joyId>jugimap::joysticks.size()) return;

    cocos2d::EventController *e = static_cast<cocos2d::EventController *>(event);

    //int keyCodeB = e->getKeyCode();
    //jugimap::DbgOutput("OnJoystickButtonDown keycode :" + std::to_string(keyCode));

    // Key codes does not fit the Controller::Keys mapping !!!
    // There is no way to be sure when POV direction keys (dpad) were pressed.

    /*
    jugimap::DbgOutput("Controller key BUTTON_A :" + std::to_string(cocos2d::Controller::Key::BUTTON_A));
    jugimap::DbgOutput("Controller key BUTTON_Z :" + std::to_string(cocos2d::Controller::Key::BUTTON_Z));
    jugimap::DbgOutput("Controller key BUTTON_DPAD_RIGHT :" + std::to_string(cocos2d::Controller::Key::BUTTON_DPAD_RIGHT));
    jugimap::DbgOutput("Controller key BUTTON_DPAD_LEFT :" + std::to_string(cocos2d::Controller::Key::BUTTON_DPAD_LEFT));
    jugimap::DbgOutput("Controller key BUTTON_DPAD_UP :" + std::to_string(cocos2d::Controller::Key::BUTTON_DPAD_UP));
    jugimap::DbgOutput("Controller key BUTTON_DPAD_DOWN :" + std::to_string(cocos2d::Controller::Key::BUTTON_DPAD_DOWN));
    */

    //jugimap::joysticks[joyId]._SetButtonState(keyCode, true);

}


void EventsLayer::OnJoystickButtonUp(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{

    int joyId = controller->getDeviceId();
    if(joyId<0 || joyId>jugimap::joysticks.size()) return;

    cocos2d::EventController *e = static_cast<cocos2d::EventController *>(event);

}


void EventsLayer::OnJoystickAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{
    int joyId = controller->getDeviceId();
    if(joyId<0 || joyId>jugimap::joysticks.size()) return;

    cocos2d::EventController *e = static_cast<cocos2d::EventController *>(event);

}





namespace shaders
{
    cocos2d::GLProgram *glpColorOverlay_blendSimpleMultiply = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendNormal = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendMultiply = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendLinearDodge = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendColor = nullptr;

    std::string nameOverlayColor = "overlayColor";


    void LoadJugimapShaders()
    {

        glpColorOverlay_blendSimpleMultiply = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "SIMPLE_MULTIPLY_BLEND");
        if(glpColorOverlay_blendSimpleMultiply){
            glpColorOverlay_blendSimpleMultiply->retain();
        }
        glpColorOverlay_blendNormal = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "NORMAL_PIXEL_BLEND");
        if(glpColorOverlay_blendNormal){
            glpColorOverlay_blendNormal->retain();
        }
        glpColorOverlay_blendMultiply = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "MULTIPLY_PIXEL_BLEND");
        if(glpColorOverlay_blendMultiply){
            glpColorOverlay_blendMultiply->retain();
        }
        glpColorOverlay_blendLinearDodge = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "LINEAR_DODGE_PIXEL_BLEND");
        if(glpColorOverlay_blendLinearDodge){
            glpColorOverlay_blendLinearDodge->retain();
        }
        glpColorOverlay_blendColor = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "COLOR_PIXEL_BLEND");
        if(glpColorOverlay_blendColor){
            glpColorOverlay_blendColor->retain();
        }

        assert(glpColorOverlay_blendSimpleMultiply!=nullptr);
        assert(glpColorOverlay_blendNormal!=nullptr);
        assert(glpColorOverlay_blendMultiply!=nullptr);
        assert(glpColorOverlay_blendLinearDodge!=nullptr);
        assert(glpColorOverlay_blendColor!=nullptr);

    }



    void DeleteShaders()
    {

        if(glpColorOverlay_blendSimpleMultiply){
            glpColorOverlay_blendSimpleMultiply->release();
            glpColorOverlay_blendSimpleMultiply = nullptr;
        }

        if(glpColorOverlay_blendNormal){
            glpColorOverlay_blendNormal->release();
            glpColorOverlay_blendNormal = nullptr;
        }

        if(glpColorOverlay_blendMultiply){
            glpColorOverlay_blendMultiply->release();
            glpColorOverlay_blendMultiply = nullptr;
        }

        if(glpColorOverlay_blendLinearDodge){
            glpColorOverlay_blendLinearDodge->release();
            glpColorOverlay_blendLinearDodge = nullptr;
        }

        if(glpColorOverlay_blendColor){
            glpColorOverlay_blendColor->release();
            glpColorOverlay_blendColor = nullptr;
        }
    }

}



}



