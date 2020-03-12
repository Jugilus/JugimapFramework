
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


void StandardSpriteCC::InitEngineSprite()
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

                    cocos2d::Vec2 offset(r.Center().x - image->GetWidth()/2.0, image->GetHeight()/2.0-r.Center().y);
                    shape = cocos2d::PhysicsShapeBox::create(cocos2d::Size(r.Width(), r.Height()), physicsMaterial, offset);

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


void StandardSpriteCC::UpdateEngineSprite()
{

    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TRANSFORMATION){

        Vec2f posGlobal = GetFullGlobalPosition();
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
            ccSprite->setOpacity(GetAlpha()*GetLayer()->GetAlpha()*255);
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

    //---
    SetChangeFlags(0);
}


void StandardSpriteCC::SetVisible(bool _visible)
{
    if(IsVisible()==_visible) return;

    StandardSprite::SetVisible(_visible);

    if(_visible){
        if(GetActiveImage()){
            ccSprite->setVisible(true);
        }
        //    SetChangeFlags(Property::ALL);

    }else{
        ccSprite->setVisible(false);
    }

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



void SpriteLayerCC::RemoveAndDeleteSprite(Sprite* _sprite)
{

    if(_sprite->GetKind()==SpriteKind::STANDARD){
        static_cast<StandardSpriteCC*>(_sprite)->GetCCSprite()->removeFromParent();

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
    GetDrawingLayer()->DrawEngineLayer();

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
    int n = std::round(2*pi / da + 0.5);

    //if(outline.size()<n+1) outline.resize(n+1);

    float pxPrev = c.x + std::cos(0) * r.x;
    float pyPrev = c.y + std::sin(0) * r.y;

    for(int i=1; i<=n; i++){
        float angle = i * 2*pi/n;
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



void TextCC::InitEngineText()
{
    if(ccLabel) return;             //already initialized

    ccLabel = cocos2d::Label::createWithTTF(GetTextString(), GetFont()->GetRelativeFilePath(), GetFont()->GetSize());
    ccLabel->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    ccLabel->setLocalZOrder(GetLayer()->GetZOrder());

    static_cast<MapCC*>(GetLayer()->GetMap())->GetMapNode()->addChild(ccLabel);

    //---
    SetChangeFlags(Property::ALL);
}


void TextCC::UpdateEngineText()
{

    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineTextUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TEXT_STRING){
        ccLabel->setString(GetTextString());
        _SetSize(Vec2f(ccLabel->getContentSize().width, ccLabel->getContentSize().height));
    }
    if(flags & Property::POSITION){
        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();
        ccLabel->setPosition(posGlobal.x, posGlobal.y);
    }
    if(flags & Property::COLOR){
        ccLabel->setColor(cocos2d::Color3B(GetColor().r, GetColor().g, GetColor().b));
    }
    if(flags & Property::ALPHA){
        ccLabel->setOpacity(GetAlpha() * 255);
    }

    //---
    SetChangeFlags(0);
}


//===================================================================================================


void TextLayerCC::RemoveAndDeleteText(Text* _text)
{

    static_cast<TextCC*>(_text)->GeCCLabel()->removeFromParent();
    TextLayer::RemoveAndDeleteText(_text);

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
            cocos2d::Rect viewport(MapCC::currentCamera->GetViewport().min.x, MapCC::currentCamera->GetViewport().min.y, MapCC::currentCamera->GetViewport().Width(), MapCC::currentCamera->GetViewport().Height());
            //DbgOutput(" viewport x:" + std::to_string(viewport.origin.x)+" y:"+ std::to_string(viewport.origin.y)+" w:"+std::to_string(viewport.size.width));
            glView->setScissorInPoints(viewport.getMinX(), viewport.getMinY(), viewport.getMaxX()-viewport.getMinX(), viewport.getMaxY()-viewport.getMinY());
            renderer->render();
        }

        map->SetCamera(c);
        map->UpdateEngineMap();
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
        cocos2d::Rect viewport(MapCC::currentCamera->GetViewport().min.x, MapCC::currentCamera->GetViewport().min.y, MapCC::currentCamera->GetViewport().Width(), MapCC::currentCamera->GetViewport().Height());
        //DbgOutput(" viewport x:" + std::to_string(viewport.origin.x)+" y:"+ std::to_string(viewport.origin.y)+" w:"+std::to_string(viewport.size.width));
        glView->setScissorInPoints(viewport.getMinX(), viewport.getMinY(), viewport.getMaxX()-viewport.getMinX(), viewport.getMaxY()-viewport.getMinY());
        renderer->render();

        MapCC::currentCamera = nullptr;

        glDisable(GL_SCISSOR_TEST);
    }
}


//===================================================================================================


jugimap::Camera* MapCC::currentCamera = nullptr;


void MapCC::InitEngineMap()
{

    ccMapNode = MapCCNode::Create(this);     // First create cocos2d node as it will be used during sprite initialization!
    Map::InitEngineMap();
}


void MapCC::UpdateEngineMap()
{

    Map::UpdateEngineMap();

    ccMapNode->setRotation(GetCamera()->GetRotation());
    ccMapNode->setScale(GetCamera()->GetScale());
    ccMapNode->setPosition(GetCamera()->GetProjectedMapPosition().x, GetCamera()->GetProjectedMapPosition().y);

}


//===================================================================================================


int SceneCCNode::glDrawnBatchesString = 0;
int SceneCCNode::glDrawnVerticesString = 0;


SceneCCNode* SceneCCNode::Create(jugimap::Scene *_jmScene)
{

    SceneCCNode* sn = new SceneCCNode();
    if(sn){
        sn->jmScene = _jmScene;
        if(sn->init()){
            sn->autorelease();
            sn->jmScene->SetEngineSceneLink(sn);
            return sn;
        }
    }
    CC_SAFE_DELETE(sn);
    return nullptr;
}


SceneCCNode::~SceneCCNode()
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


bool SceneCCNode::init()
{

    if(jmScene->HasPhysicsEnabled()){
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


void SceneCCNode::update(float delta)
{

    //--- Manage errors
    if(jugimap::settings.GetErrorMessage() != ""){
        if(errorMessageLabel){
            errorMessageLabel->setString(jugimap::settings.GetErrorMessage());
        }
        return ;
    }

    assert(sceneManager->GetCurrentScene()==jmScene);
    if(sceneManager->GetCurrentScene()!=jmScene) return;

    sceneManager->Update(delta*1000);

}


//===================================================================================================


void SceneCCNode::render(cocos2d::Renderer *renderer, const cocos2d::Mat4 &eyeTransform, const cocos2d::Mat4 *eyeProjection)
{
    glEnable(GL_SCISSOR_TEST);

    cocos2d::Scene::render(renderer,eyeTransform,eyeProjection);

    glDrawnBatchesString = renderer->getDrawnBatches();
    glDrawnVerticesString = renderer->getDrawnVertices();

    glDisable(GL_SCISSOR_TEST);
}


namespace shaders
{
    cocos2d::GLProgram *glpColorOverlay_blendSimpleMultiply = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendNormal = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendMultiply = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendLinearDodge = nullptr;
    cocos2d::GLProgram *glpColorOverlay_blendColor = nullptr;

    std::string nameOverlayColor = "overlayColor";



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



