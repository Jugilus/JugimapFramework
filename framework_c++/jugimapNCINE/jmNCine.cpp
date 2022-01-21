
//===================================================================================================
//      JugiMap API extension for nCIne.
//===================================================================================================

#include <assert.h>
#include <algorithm>
#include <ncine/Application.h>
#include <ncine/IInputManager.h>
#include <ncine/Viewport.h>
//#include <jugimapNCINE/jmNCine.h>
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


/*
StandardSpriteNC::StandardSpriteNC(SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer)
    : StandardSprite(_sourceSprite, _position, _spriteLayer)
{

    InitEngineSprite();
}
*/


void StandardSpriteNC::InitEngineObjects()
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
        if(ncSprite->isDrawEnabled()){            // update for new ncine version
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


void StandardSpriteNC::UpdateEngineObjects()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false) return;
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

    if(flags & Property::VISIBILITY){

        if(IsVisible()){
            if(GetActiveImage()){
                ncSprite->setDrawEnabled(true);
            }

        }else{
            ncSprite->setDrawEnabled(false);
        }

    }

    //---
    SetChangeFlags(0);
}


/*

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

*/


void StandardSpriteNC::ManageShaders_OverlayColor()
{


    if(IsOverlayColorActive()==false){

        ncSprite->setColor(255,255,255,255);


    }else{

        ColorRGBA colorRGBA = GetOverlayColorRGBA();
        ColorRGBAf c(colorRGBA);


        switch (GetOverlayColorBlend()) {


        case ColorOverlayBlend::SIMPLE_MULTIPLY:

            ncSprite->setColor(colorRGBA.r, colorRGBA.g, colorRGBA.b, colorRGBA.a);
            break;

        case ColorOverlayBlend::NORMAL:

            break;

        case ColorOverlayBlend::MULTIPLY:

            break;

        case ColorOverlayBlend::LINEAR_DODGE:

            break;

        case ColorOverlayBlend::COLOR:

            break;

        case ColorOverlayBlend::NOT_DEFINED:

            break;

        }

    }


}



//===================================================================================================


/*
TextSpriteNC::TextSpriteNC(const std::string &_textString, Font* _font, ColorRGBA _color, Vec2f _position, TextHandleVariant _textHandle, SpriteLayer *_spriteLayer) :
    TextSprite(_textString, _font, _color, _position, _textHandle, _spriteLayer)
{

    InitEngineSprite();

}
*/

void TextSpriteNC::InitEngineObjects()
{

    if(ncTextNode) return;             //already initialized


    TextSprite::InitEngineObjects();

    /*

    if(GetSourceSpriteReplacements()){
        _SetSourceSprite(GetSourceSpriteReplacements()->replacements.front().sourceSprite);
    }



    std::string _text = GetTextString();
    Font *_font = GetFont();
    ColorRGBA _color = GetColor();
    Vec2f _relativeHandle = GetRelativeHandle();


    //--- if text sprite is loaded from the editor -> string and style are obtained from the graphic item !
    if(GetSourceSprite() && GetSourceSprite()->GetGraphicItems().empty()==false){

        _text = GetSourceSprite()->GetGraphicItems().front()->GetLabel();

        if(_text=="OPTIONS"){
            DummyFunction();
        }
        int fontId = GetSourceSprite()->GetGraphicItems().front()->GetFontStyleId();
        if(fontId>=0 && fontId<fontLibrary.size()){
            _font = fontLibrary[fontId];
        }

        int colorId = GetSourceSprite()->GetGraphicItems().front()->GetColorStyleId();
        //if(colorId>=0 && colorId<textColorsLibrary.size()){
        //    _color = textColorsLibrary[colorId];
        //}
        _color = textColorsLibrary.At(colorId);

        TextHandleVariant thv = GetSourceSprite()->GetGraphicItems().front()->GetTextHandleVariant();
        _relativeHandle = GetRelativeHandleFromTextHandleVariant(thv);
    }


    if(_font==nullptr){
        assert(fontLibrary.empty()==false);
        _font = fontLibrary.front();            // Make sure that you have added at least one font to fontLibrary !!!
    }

    if(_text.empty()){
        _text = "text";
    }

    //----
    _SetFont(_font);
    SetTextString(_text);
    _SetRelativeHandle(_relativeHandle);
    SetColor(_color);

    */
    //----
    //ccLabel = cocos2d::Label::createWithTTF(GetTextString(), GetFont()->GetRelativeFilePath(), GetFont()->GetLoadFontSize());
    //ccLabel->setLocalZOrder(GetLayer()->GetZOrder());
    //static_cast<MapCC*>(GetLayer()->GetMap())->GetMapNode()->addChild(ccLabel);

    //float w = ccLabel->getContentSize().width;
    //float h = ccLabel->getContentSize().height;


    ncTextNode = new ncine::TextNode(static_cast<MapNC*>(GetLayer()->GetMap())->GetMapNCNode(), static_cast<FontNC*>(GetFont())->GetNCFont());
    //ncTextNode->enableKerning(false);
    ncTextNode->setString(GetTextString().c_str());
    //_SetSize(Vec2f(ncTextNode->width(), ncTextNode->height()));
    //ncTextNode->setAnchorPoint(ncine::DrawableNode::AnchorBottomLeft);
    ncTextNode->setLayer(GetLayer()->GetZOrder());

    float w = ncTextNode->width();
    float h = ncTextNode->height();
    SetHandle(Vec2f(GetRelativeHandle().x*w,  GetRelativeHandle().y*h));

    //---
    SetChangeFlags(Property::ALL);
}


void TextSpriteNC::UpdateEngineObjects()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false && GetChangeFlags()==0) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();

    if(GetTextString()=="OPTIONS"){
        DummyFunction();
    }


    if(flags & Property::TEXT_STRING){

        ncTextNode->setString(GetTextString().c_str());
        Vec2f size(ncTextNode->width(), ncTextNode->height());
        _SetSize(size);

        if(GetTextString().empty()==false && emptyString){          // required as ncine reset anchor point if assigning empty string
            //ncTextNode->setAnchorPoint(ncine::DrawableNode::AnchorBottomLeft);
            ncTextNode->setAnchorPoint(GetRelativeHandle().x, GetRelativeHandle().y);
        }
        emptyString = GetTextString().empty();

        SetHandle(size*GetRelativeHandle());

        //---- update bounding box
        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();
        jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, GetGlobalScale(), GetGlobalFlip(),  GetGlobalRotation(), GetHandle());
        SetBoundingBox(TransformRectf(Rectf(0,0, size.x, size.y), m));

        //----
        flags &= ~Property::TEXT_STRING;    // ! Because GetSize() used later in this function checks for this flag and call UpdateEngineSprite !
        SetChangeFlags(flags);
    }


    if(flags & Property::VISIBILITY){
        ncTextNode->setDrawEnabled(IsVisible());
    }


    if(flags & Property::TRANSFORMATION){

        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();

        Vec2f scaleGlobal = GetGlobalScale();
        float rotationGlobal = GetGlobalRotation();
        Vec2i flipGlobal = GetGlobalFlip();

        if (flags & Property::HANDLE){
            //ncine::Vector2f anchorPoint(GetHandle().x - GetWidth() * 0.5f, GetHandle().y - GetHeight()*0.5f);
            //ncTextNode->setAbsAnchorPoint(anchorPoint);
            ncTextNode->setAnchorPoint(GetRelativeHandle().x, GetRelativeHandle().y);
        }
        if(flags & Property::POSITION){
            ncTextNode->setPosition(posGlobal.x, posGlobal.y);
        }
        if(flags & Property::SCALE){
            ncTextNode->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }
        if(flags & Property::ROTATION){
            ncTextNode->setRotation(rotationGlobal);
        }
        if(flags & Property::FLIP){
            ncTextNode->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }

        //----
        jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, scaleGlobal,  flipGlobal, rotationGlobal, GetHandle());
        SetBoundingBox(TransformRectf(Rectf(0,0, GetSize().x, GetSize().y), m));

    }


    if(flags & Property::APPEARANCE){

        if(flags & Property::COLOR){
            ncTextNode->setColor(GetColor().r, GetColor().g, GetColor().b, GetColor().a);
        }
        if(flags & Property::ALPHA){
            ncTextNode->setAlphaF(GetAlpha());
        }
    }


    //---
    SetChangeFlags(0);

}



//===================================================================================================


void SpriteLayerNC::RemoveAndDeleteSprite(Sprite* _sprite)
{

    if(_sprite->GetKind()==SpriteKind::STANDARD){
        //static_cast<MapNC*>(GetMap())->GetMapNCNode()->removeChildNode(static_cast<StandardSpriteNC*>(_sprite)->GetNCSprite());
        //DeleteNCNode(static_cast<StandardSpriteNC*>(_sprite)->GetNCSprite());

        delete static_cast<StandardSpriteNC*>(_sprite)->GetNCSprite();

    }else if(_sprite->GetKind()==SpriteKind::TEXT){
        delete static_cast<TextSpriteNC*>(_sprite)->GetNCTextNode();


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

    ncTextNode = new ncine::TextNode(nullptr, ncFont);
    ncTextNode->setDrawEnabled(false);
    pixelHeightCommon = GetPixelHeight(textForHeight);

}


FontNC::~FontNC()
{
    delete ncTextNode;      // because it has no parent
    delete ncFont;

}


int FontNC::GetPixelWidth(const std::string &s)
{

    text = s;
    ncTextNode->setString(text.c_str());
    float width = ncTextNode->width();

    return width;
}


int FontNC::GetPixelHeight(const std::string &s)
{

    text = s;
    ncTextNode->setString(text.c_str());
    float height = ncTextNode->height();

    return height;
}


Vec2f FontNC::GetPixelSize(const std::string &s)
{
    text = s;
    ncTextNode->setString(text.c_str());
    float width = ncTextNode->width();
    float height = ncTextNode->height();

    return Vec2f(width, height);
}



//===================================================================================================

/*
TextNC::TextNC(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
    : Text(_textLayer, _font, _textString, _position, _color)
{

    assert(GetLayer()->GetMap());
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
    if(IsEngineTextUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TEXT_STRING){
        ncTextNode->setString(GetTextString().c_str());
        _SetSize(Vec2f(ncTextNode->width(), ncTextNode->height()));
        //ncTextNode->setAnchorPoint(ncine::DrawableNode::AnchorBottomLeft);
        if(GetTextString().empty()==false && emptyString){          // required as ncine reset anchor point if assigning empty string
            ncTextNode->setAnchorPoint(ncine::DrawableNode::AnchorBottomLeft);
        }
        emptyString = GetTextString().empty();
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
    if(flags & Property::VISIBILITY){
        ncTextNode->setDrawEnabled(IsVisible());
    }

    //---
    SetChangeFlags(0);
}


//===================================================================================================


void TextLayerNC::RemoveAndDeleteText(Text* _text)
{

    //DeleteNCNode(static_cast<TextNC*>(_text)->GetNCTextNode());
    if(settings.IsAppTerminated()==false){
        ncine::TextNode* textNodeNC = static_cast<TextNC*>(_text)->GetNCTextNode();
        delete textNodeNC;
    }

    TextLayer::RemoveAndDeleteText(_text);

}

*/

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

    GetDrawingLayer()->DrawEngineObjects();

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
    ncine::SceneNode::visit(renderQueue);

}


//===================================================================================================


void MapNC::InitEngineObjects(MapType _mapType)
{

    mMapNCNode = new MapNCNode(this);     // First create node as it will be used during sprite initialization!
    Map::InitEngineObjects(_mapType);

    ncine::Viewport &rootViewport = ncine::theApplication().rootViewport();
    mViewport.setNextViewport(rootViewport.nextViewport());
    rootViewport.setNextViewport(&mViewport);
    mViewport.setCamera(&mCamera);
    mViewport.setRootNode(mMapNCNode);

}



void MapNC::UpdateEngineObjects()
{

    Map::UpdateEngineObjects();

    if (GetCamera()->GetChangeFlags() != Camera::Change::NONE ||
        GetCamera()->GetKind() == Camera::Kind::SCREEN)
    {
        ncine::Camera::ViewValues viewValues = mCamera.viewValues();
        viewValues.rotation = -GetCamera()->GetRotation();
        viewValues.scale = GetCamera()->GetScale();
        viewValues.position.set(GetCamera()->GetProjectedMapPosition().x, GetCamera()->GetProjectedMapPosition().y);
        mCamera.setView(viewValues);
    }

}


void MapNC::SetVisible(bool _visible)
{
    Map::SetVisible(_visible);

    mMapNCNode->setEnabled(_visible);

}


//===================================================================================================


EngineSceneNC::EngineSceneNC(Scene *_scene) : EngineScene(_scene)
{

    //ccSceneNode = SceneCCNode::Create(_scene);
    //assert(ccSceneNode);

    //AddErrorMessageTextNode();

    ncNode = new ncine::SceneNode();
    assert(ncNode);

}


void EngineSceneNC::PostInit()
{

}


//==================================================================================================


EngineAppNC::EngineAppNC(App *_app) : EngineApp(_app)
{

    appNCNode = new ncine::SceneNode();

}



void EngineAppNC::PostInit()
{

    for(Scene* s : GetApp()->GetScenes()){
        EngineSceneNC *engineSceneNC = static_cast<EngineSceneNC *>(s->GetEngineScene());
        appNCNode->addChildNode(engineSceneNC->GetEngineNode());
    }


    ncine::theApplication().rootNode().addChildNode(appNCNode);

}


void EngineAppNC::SetSystemMouseCursorVisible(bool _visible)
{
    if(_visible){
        ncine::theApplication().inputManager().setMouseCursorMode(ncine::IInputManager::MouseCursorMode::NORMAL);
    }else{
        ncine::theApplication().inputManager().setMouseCursorMode(ncine::IInputManager::MouseCursorMode::HIDDEN);
    }
}




//===================================================================================================


/*

AppNCNode::~AppNCNode()
{
    DummyFunction();

}




void AppNCNode::update(float delta)
{

    //--- Manage errors
    if(jugimap::settings.GetErrorMessage() != ""){
        if(errorMessageLabel){
            errorMessageLabel->setString(jugimap::settings.GetErrorMessage().c_str());
        }
        return ;
    }

    //assert(sceneManager->GetCurrentScene()==jmApp);
    //if(sceneManager->GetCurrentScene()!=jmApp) return;

    jmApp->Update(delta);

    //sceneManager->Update(delta*1000);

}





void AppNCNode::visit(ncine::RenderQueue &renderQueue)
{

    //map->UpdateEngineMap();
    ncine::SceneNode::visit(renderQueue);

}

*/


//===================================================================================================


/*

void CommandsNC::PreAppUpdate()
{

    // Parameters of all input devices are obtained with cocos2d events listeners!

    if(mouse.GetCursorSprite()){
        mouse.GetCursorSprite()->SetPosition(Vec2f(mouse.GetX(), mouse.GetY()));
    }

}


void CommandsNC::PostAppUpdate()
{

    mouse.ResetPerUpdateFlags();
    keyboard.ResetPerUpdateFlags();
    touch.ResetPerUpdateFlags();

    for(Joystick &gc: joysticks) gc.ResetPerUpdateFlags();

}


void CommandsNC::SetSystemMouseCursorVisible(bool _visible)
{
    if(_visible){
        ncine::theApplication().inputManager().setMouseCursorMode(ncine::IInputManager::MouseCursorMode::NORMAL);
    }else{
        ncine::theApplication().inputManager().setMouseCursorMode(ncine::IInputManager::MouseCursorMode::DISABLED);
    }

}

*/
//===================================================================================================




/*
void DeleteNCNode(ncine::SceneNode* _node)
{

    if(deleteNCNodesSpecial){
        nctl::Array<ncine::SceneNode *>children = _node->children();     // Children list as copy (not reference)!
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



