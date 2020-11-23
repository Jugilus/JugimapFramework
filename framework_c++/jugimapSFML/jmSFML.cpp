
//===================================================================================================
//      JugiMap API extension for SFML.
//===================================================================================================


#include <assert.h>
#include <algorithm>
#include "jmSFML.h"




namespace jugimap {



std::string BinaryFileStreamReaderSFML::ReadString()
{
    int length = ReadInt();
    char *buf = new char[length];
    fis.read(buf, length);
    std::string value(buf, length);
    delete[] buf;
    return value;
}


//===================================================================================================


bool GraphicFileSFML::textureSmoothFilter = true;


GraphicFileSFML::~GraphicFileSFML()
{
    if(sfTexture){
        delete sfTexture;
    }
}


void GraphicFileSFML::Init()
{
    if(sfTexture) return;            // already loaded

    GraphicFile::Init();


    if(GetKind()==FileKind::SPINE_FILE){

        // spine atlas file: GetSpineAtlasRelativeFilePath()
        // spine skeleton file: GetRelativeFilePath()

    }else if(GetKind()==FileKind::SPRITER_FILE){

        // spriter model file: GetRelativeFilePath()


    }else{      // image

        sfTexture = new sf::Texture();
        if(sfTexture->loadFromFile(GetRelativeFilePath())){
            if(textureSmoothFilter){
                sfTexture->setSmooth(true);
            }
        }else{

            delete sfTexture;
            sfTexture = nullptr;
            assert(false);
        }
    }
}


//===================================================================================================


/*
StandardSpriteSFML::StandardSpriteSFML(SourceSprite *_sourceSprite, Vec2f _position, SpriteLayer *_spriteLayer)
    : StandardSprite(_sourceSprite, _position, _spriteLayer)
{

    InitEngineSprite();
}
*/


StandardSpriteSFML::~StandardSpriteSFML ()
{
    if(sfSprite){
        delete sfSprite;
    }
}


void StandardSpriteSFML::InitEngineObjects()
{

    if(sfSprite) return;             //already initialized

    GetSourceSprite()->Init();

    sfSprite = new sf::Sprite();
    if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_IMAGES_WITH_SPRITE_SHAPES){
        if(GetSourceSprite()->GetStatusFlags() & SourceSprite::Status::HAS_MULTIPLE_SPRITE_SHAPES_IN_IMAGE){
            SetCollider(new MultiShapesCollider(this));
        }else{
            SetCollider(new SingleShapeCollider(this));
        }
    }

    //---
    GraphicItem *gi = GetSourceSprite()->GetGraphicItems().front();
    SetActiveImage(gi);


    //---
    SetChangeFlags(Property::ALL);

    //---
    //UpdateEngineObjects();
    CaptureForLerpDrawing();
}


void StandardSpriteSFML::SetActiveImage(GraphicItem *_image)
{

    if(GetActiveImage()==_image) return;

    if(GetSourceSprite()->GetName()=="Shine"){
        DummyFunction();
    }

    StandardSprite::SetActiveImage(_image);

    //----
    GraphicItem *image = GetActiveImage();
    if(image==nullptr){         // _image can be also nullptr!
        return;
    }

    GraphicFileSFML * gf = static_cast<GraphicFileSFML*>(image->GetFile());

    sfSprite->setTexture(*gf->GetSFTexture(), true);
    sfSprite->setTextureRect(sf::IntRect(GetActiveImage()->GetPos().x, GetActiveImage()->GetPos().y, GetActiveImage()->GetWidth(), GetActiveImage()->GetHeight()));
    sfSprite->setOrigin(GetHandle().x, GetHandle().y);

    //---
    SetColliderShapes();

}


void StandardSpriteSFML::UpdateEngineObjects()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false) return;
    if(IsEngineSpriteUsedDirectly()) return;

    if(GetSourceSprite()->GetName()=="bg_gradient_v2"){
        DummyFunction();
    }

    int flags = GetChangeFlags();


    if(flags & Property::TRANSFORMATION){

        Vec2f scaleGlobal = GetGlobalScale();
        float rotationGlobal = GetGlobalRotation();
        Vec2i flipGlobal = GetGlobalFlip();

        if(flags & Property::HANDLE){
            sfSprite->setOrigin(GetHandle().x, GetHandle().y);
        }
        if(flags & Property::POSITION){
            posGlobalPrevious = posGlobal;
            posGlobal = GetGlobalPosition();
            drawChangeFlags |= Property::POSITION;
        }
        if(flags & Property::SCALE){
            sfSprite->setScale(scaleGlobal.x, scaleGlobal.y);
        }
        if(flags & Property::ROTATION){
            sfSprite->setRotation(-rotationGlobal);
        }
        if(flags & Property::FLIP){
            sfSprite->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }

        UpdateColliderAndBoundingBox();
    }


    if(flags & Property::APPEARANCE){

        if(flags & Property::OVERLAY_COLOR){
            ManageShaders_OverlayColor();
        }

        if(flags & Property::ALPHA){
            sfSprite->setColor(sf::Color(255,255,255, GetAlpha()*GetLayer()->GetAlpha()*255));
        }

        if(flags & Property::BLEND){
            Blend blend = GetOwnBlend();
            if(blend==Blend::NOT_DEFINED) blend = GetLayer()->GetBlend();

            if(blend==Blend::ALPHA){
                 sfBlend = sf::BlendAlpha;

            }else if(blend==Blend::MULTIPLY){
                sfBlend = sf::BlendMultiply;

            }else if(blend==Blend::ADDITIVE){
                sfBlend = sf::BlendAdd;

            }else if(blend==Blend::SOLID){
                sfBlend = sf::BlendNone;

            }else{
                sfBlend = sf::BlendAlpha;
            }
        }
    }

    //---
    SetChangeFlags(0);
}


void StandardSpriteSFML::CaptureForLerpDrawing()
{
    posGlobal = GetGlobalPosition();
    posGlobalPrevious = posGlobal;
}


void StandardSpriteSFML::DrawEngineSprite()
{

    globSFML::countSpriteDrawCalls++;

    if(IsVisible()==false) return;
    if(GetActiveImage()==nullptr) return;
    if(GetLayer()->GetMap()->GetCamera()->GetMapVisibleRect().Intersects(GetBoundingBox())==false){
        //WorldMapCamera * camera = dynamic_cast<WorldMapCamera*>(GetLayer()->GetMap()->GetCamera());
        return;
    }



    if(drawChangeFlags!=0){

        //if(GetSourceSprite()->GetName()=="Shine"){
        //    DummyFunction();
        //}

        if(drawChangeFlags & Property::POSITION){

            Vec2f pos = posGlobal;
            //if(settings.LerpDrawingEnabled()){
            //    pos = posGlobalPrevious + (posGlobal-posGlobalPrevious)*settings.GetLerpDrawingFactor();
            //}

            //if(GetLayer()->GetName()=="Parallax3 - static"){
            //    DbgOutput("pos x:"+std::to_string(pos.x)+ "y:"+std::to_string(pos.y)+"  posWorld x:"+std::to_string(posWorld.x)+ "y:"+std::to_string(posWorld.y)
            //              +"  parallaxOffset.x"+std::to_string(GetLayer()->GetParallaxOffset().x)+ "y:"+std::to_string(GetLayer()->GetParallaxOffset().y));

            //}

            pos += GetLayer()->GetParallaxOffset();
            sfSprite->setPosition(pos.x, pos.y);
        }
        drawChangeFlags = 0;
    }

    sf::RenderStates renderStates(sfBlend);
    if(shader) renderStates.shader = shader;


    //----
    globSFML::CurrentRenderTarget->draw(*sfSprite, renderStates);
    globSFML::countDrawnSprites++;

}


void StandardSpriteSFML::ManageShaders_OverlayColor()
{


    if(IsOverlayColorActive()==false){
        shader = nullptr;
        return;

    }else{

        ColorRGBA colorRGBA = GetOverlayColorRGBA();
        ColorRGBAf c(colorRGBA);

        switch (GetOverlayColorBlend()) {

        case ColorOverlayBlend::SIMPLE_MULTIPLY:

            shader = shaders::ColorOverlay_SimpleMultiply.GetShader();
            if(shader==nullptr) return;

            if(colorRGBA != shaders::ColorOverlay_SimpleMultiply.GetColorRGBA()){
                shaders::ColorOverlay_SimpleMultiply.SetColorRGBA(colorRGBA);
                shader->setUniform(shaders::ColorOverlay_SimpleMultiply.GetColorRGBAname(),  sf::Glsl::Vec4(c.r, c.g, c.b, c.a));
            }

            break;

        case ColorOverlayBlend::NORMAL:
            shader = shaders::ColorOverlay_Normal.GetShader();
            if(shader==nullptr) return;

            if(colorRGBA!=shaders::ColorOverlay_Normal.GetColorRGBA()){
                shaders::ColorOverlay_Normal.SetColorRGBA(colorRGBA);
                shader->setUniform(shaders::ColorOverlay_Normal.GetColorRGBAname(),  sf::Glsl::Vec4(c.r, c.g, c.b, c.a));
            }

            break;

        case ColorOverlayBlend::MULTIPLY:
            shader = shaders::ColorOverlay_Multiply.GetShader();
            if(shader==nullptr) return;

            if(colorRGBA!=shaders::ColorOverlay_Multiply.GetColorRGBA()){
                shaders::ColorOverlay_Multiply.SetColorRGBA(colorRGBA);
                shader->setUniform(shaders::ColorOverlay_Multiply.GetColorRGBAname(),  sf::Glsl::Vec4(c.r, c.g, c.b, c.a));
            }

            break;

        case ColorOverlayBlend::LINEAR_DODGE:
            shader = shaders::ColorOverlay_LinearDodge.GetShader();
            if(shader==nullptr) return;

            if(colorRGBA!=shaders::ColorOverlay_LinearDodge.GetColorRGBA()){
                shaders::ColorOverlay_LinearDodge.SetColorRGBA(colorRGBA);
                shader->setUniform(shaders::ColorOverlay_LinearDodge.GetColorRGBAname(),  sf::Glsl::Vec4(c.r, c.g, c.b, c.a));
            }

            break;

        case ColorOverlayBlend::COLOR:
            shader = shaders::ColorOverlay_Color.GetShader();
            if(shader==nullptr) return;

            if(colorRGBA!=shaders::ColorOverlay_Color.GetColorRGBA()){
                shaders::ColorOverlay_Color.SetColorRGBA(colorRGBA);
                shader->setUniform(shaders::ColorOverlay_Color.GetColorRGBAname(),  sf::Glsl::Vec4(c.r, c.g, c.b, c.a));
            }

            break;

        default:
            shader = nullptr;
        }

    }

}



//===================================================================================================


Vec2f TextSpriteSFML::posOffset{0,1};


TextSpriteSFML::~TextSpriteSFML()
{
    if(sfText){
        delete sfText;
    }
}


void TextSpriteSFML::InitEngineObjects()
{

    if(sfText) return;             //already initialized


    TextSprite::InitEngineObjects();


    //----

    sfText = new sf::Text(GetTextString(), *static_cast<FontSFML*>(GetFont())->GetSFFont(), GetFont()->GetLoadFontSize());


    //ncTextNode = new ncine::TextNode(static_cast<MapNC*>(GetLayer()->GetMap())->GetMapNCNode(), static_cast<FontNC*>(GetFont())->GetNCFont());
    //ncTextNode->setString(GetTextString().c_str());
    //ncTextNode->setLayer(GetLayer()->GetZOrder());


    float w = sfText->getLocalBounds().width;
    //float h = sfText->getLocalBounds().height;
    float h = GetFont()->GetCommonPixelHeight();
    SetHandle(Vec2f(GetRelativeHandle().x*w,  GetRelativeHandle().y*h));

    //---
    SetChangeFlags(Property::ALL);
}


void TextSpriteSFML::UpdateEngineObjects()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false && GetChangeFlags()==0) return;
    if(IsEngineSpriteUsedDirectly()) return;

    int flags = GetChangeFlags();

    if(GetTextString()=="OPTIONS"){
        DummyFunction();
    }


    if(flags & Property::TEXT_STRING){

        std::basic_string<sf::Uint32> u32s =  FontSFML::conv.from_bytes(GetTextString());
        sfText->setString(u32s);
        //Vec2f size(sfText->getLocalBounds().width, sfText->getLocalBounds().height);
        Vec2f size(sfText->getLocalBounds().width, GetFont()->GetCommonPixelHeight());
        _SetSize(size);
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



    if(flags & Property::TRANSFORMATION){

        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();

        Vec2f scaleGlobal = GetGlobalScale();
        float rotationGlobal = GetGlobalRotation();
        Vec2i flipGlobal = GetGlobalFlip();

        if (flags & Property::HANDLE){
            sfText->setOrigin(GetHandle().x, GetHandle().y);
        }
        if(flags & Property::POSITION){
            sfText->setPosition(posGlobal.x + posOffset.x, posGlobal.y + posOffset.y);
        }
        if(flags & Property::SCALE){
            sfText->setScale(scaleGlobal.x, scaleGlobal.y);
        }
        if(flags & Property::ROTATION){
            sfText->setRotation(-rotationGlobal);
        }
        if(flags & Property::FLIP){
            sfText->setScale((flipGlobal.x==0)? scaleGlobal.x : -scaleGlobal.x, (flipGlobal.y==0)? scaleGlobal.y : -scaleGlobal.y);
        }

        //----
        jugimap::AffineMat3f m = MakeTransformationMatrix(posGlobal, scaleGlobal,  flipGlobal, rotationGlobal, GetHandle());
        SetBoundingBox(TransformRectf(Rectf(0,0, GetSize().x, GetSize().y), m));

    }


    if(flags & Property::APPEARANCE){

        if(flags & Property::COLOR){
            sfText->setFillColor(sf::Color(GetColor().r, GetColor().g, GetColor().b, GetColor().a * GetAlpha()));
        }
        if(flags & Property::ALPHA){
            sfText->setFillColor(sf::Color(GetColor().r, GetColor().g, GetColor().b,  GetColor().a * GetAlpha()));
        }
    }


    //---
    SetChangeFlags(0);

}


void TextSpriteSFML::DrawEngineSprite()
{

    globSFML::countSpriteDrawCalls++;

    if(IsVisible()==false) return;
    if(GetLayer()->GetMap()->GetCamera()->GetMapVisibleRect().Intersects(GetBoundingBox())==false){
        //WorldMapCamera * camera = dynamic_cast<WorldMapCamera*>(GetLayer()->GetMap()->GetCamera());
        return;
    }
    if(sfText==nullptr) return;


    globSFML::CurrentRenderTarget->draw(*sfText);

    //----
    globSFML::countDrawnSprites++;

}


//===================================================================================================


void FragmentShaderSFML_ColorOverlay::Load(const std::string &_filePath)
{

    shader = new sf::Shader();

    if(shader->loadFromFile(_filePath, sf::Shader::Type::Fragment)){
        ColorRGBAf c(colorRGBA);
        shader->setUniform(colorRGBAname, sf::Glsl::Vec4(c.r, c.g, c.b, c.a));

    }else{
        assert(false);
        delete shader;
        shader = nullptr;
    }

}


//===================================================================================================

std::wstring_convert<std::codecvt_utf8<sf::Uint32>, sf::Uint32> FontSFML::conv;


FontSFML::FontSFML(const std::string &_relativeFilePath, int _size, FontKind _kind)
    : Font(_relativeFilePath, _size, _kind)
{

    sfFont = new sf::Font();
    if(sfFont->loadFromFile(GetRelativeFilePath())==false){
        assert(true);
    }
    sfText = new sf::Text("text", *sfFont, GetLoadFontSize());
    pixelHeightCommon = GetPixelHeight(textForHeight);

    //pixelHeightCommon += sfFont->getLineSpacing(_size);
    pixelHeightCommon = sfFont->getLineSpacing(_size);
    /*
    pixelHeightCommon = 0;

    for(int i=0; i<textForHeight.length(); i++){
        std::string s = textForHeight.substr(i,1);
        char c = s.at(0);
        const sf::Glyph &g = sfFont->getGlyph(c, _size, false);
        pixelHeightCommon += g.bounds.height;
    }
    pixelHeightCommon /= textForHeight.length();
    */

}


FontSFML::~FontSFML()
{
    delete sfText;
    delete sfFont;
}


int FontSFML::GetPixelWidth(const std::string &s)
{

    std::basic_string<sf::Uint32> u32s = conv.from_bytes(s);

    sfText->setString(u32s);
    sf::FloatRect size = sfText->getLocalBounds();

    return size.width;
}


int FontSFML::GetPixelHeight(const std::string &s)
{

    std::basic_string<sf::Uint32> u32s = conv.from_bytes(s);

    sfText->setString(u32s);
    sf::FloatRect size = sfText->getLocalBounds();
    return size.height;
}


Vec2f FontSFML::GetPixelSize(const std::string &s)
{
    std::basic_string<sf::Uint32> u32s = conv.from_bytes(s);

    sfText->setString(u32s);
    sf::FloatRect size = sfText->getLocalBounds();
    return Vec2f(size.width, size.height);
}



/*
//===================================================================================================

std::wstring_convert<std::codecvt_utf8<sf::Uint32>, sf::Uint32> TextSFML::conv;

TextSFML::TextSFML(TextLayer* _textLayer, Font *_font, const std::string &_textString, Vec2f _position, ColorRGBA _color)
        : Text(_textLayer, _font, _textString, _position, _color)
{

    // We must convert string to std::basic_string<sf::Uint32> because sfml string can not be constructed from std::string with utf8 encoding !
    std::basic_string<sf::Uint32> u32s = conv.from_bytes(GetTextString());
    sfText = new sf::Text(u32s, *static_cast<FontSFML*>(GetFont())->GetSFFont(), GetFont()->GetLoadFontSize());

    //---
    SetChangeFlags(Property::ALL);

}


TextSFML::~TextSFML()
{
    delete sfText;
}



/*
void TextSFML::InitEngineText()
{
    if(sfText) return;             //already initialized

    sfText = new sf::Text(GetTextString(), *static_cast<FontSFML*>(GetFont())->GetSFFont(), GetFont()->GetLoadFontSize());

    //---
    SetChangeFlags(Property::ALL);
}
*/

/*

void TextSFML::UpdateEngineText()
{

    if(GetChangeFlags()==0) return;
    //if(IsVisible()==false) return;
    if(IsEngineTextUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TEXT_STRING){
        std::basic_string<sf::Uint32> u32s = conv.from_bytes(GetTextString());
        sfText->setString(u32s);
        _SetSize(Vec2f(sfText->getLocalBounds().width, sfText->getLocalBounds().height));
    }
    if(flags & Property::POSITION){
        Vec2f posGlobal = GetGlobalPosition();
        posGlobal += GetLayer()->GetParallaxOffset();
        sfText->setPosition(posGlobal.x, posGlobal.y);
    }
    if(flags & Property::COLOR){
        sfText->setFillColor(sf::Color(GetColor().r, GetColor().g, GetColor().b, GetColor().a * GetAlpha()));
    }
    if(flags & Property::ALPHA){
        sfText->setFillColor(sf::Color(GetColor().r, GetColor().g, GetColor().b,  GetColor().a * GetAlpha()));
    }
    //if(flags & Property::VISIBILITY){
    //    sfText->setFillColor(sf::Color(GetColor().r, GetColor().g, GetColor().b,  GetColor().a * GetAlpha()));
    //}

    //---
    SetChangeFlags(0);
}


//===================================================================================================


void TextLayerSFML::DrawEngineLayer()
{

    for(Text *t : GetTexts()){
        if(t->IsVisible()==false) continue;

        TextSFML* textSFML = static_cast<TextSFML*>(t);
        if(textSFML->GetSFText()==nullptr) continue;

        globSFML::CurrentRenderTarget->draw(*textSFML->GetSFText());
    }
}

*/

//===================================================================================================


void DrawerSFML::SetOutlineColor(ColorRGBA _outlineColor)
{
    sfOutlineColor = sf::Color(_outlineColor.r, _outlineColor.g, _outlineColor.b, _outlineColor.a);
}


void DrawerSFML::Line(jugimap::Vec2f p1, jugimap::Vec2f p2)
{

    sf::Vertex v1(sf::Vector2f(p1.x, p1.y), sfOutlineColor);
    sf::Vertex v2(sf::Vector2f(p2.x, p2.y), sfOutlineColor);
    sf::Vertex line[] = {v1, v2};
    globSFML::CurrentRenderTarget->draw(line, 2, sf::Lines);

}


void DrawerSFML::RectangleOutline(const jugimap::Rectf &rect)
{

    sf::Vertex outline[5];
    outline[0] = sf::Vertex(sf::Vector2f(rect.min.x, rect.min.y), sfOutlineColor);
    outline[1] = sf::Vertex(sf::Vector2f(rect.max.x, rect.min.y), sfOutlineColor);
    outline[2] = sf::Vertex(sf::Vector2f(rect.max.x, rect.max.y), sfOutlineColor);
    outline[3] = sf::Vertex(sf::Vector2f(rect.min.x, rect.max.y), sfOutlineColor);
    outline[4] = outline[0];

    globSFML::CurrentRenderTarget->draw(outline, 5, sf::LineStrip);

}


void DrawerSFML::EllipseOutline(jugimap::Vec2f c, jugimap::Vec2f r)
{

    static std::vector<sf::Vertex>outline;

    float ra = (std::fabs(r.x) + std::fabs(r.y)) / 2;
    float da = std::acos(ra / (ra + 0.125 /1.0)) * 2;
    int n = std::round(2*mathPI / da + 0.5);

    if(outline.size()<n+1) outline.resize(n+1);

    for(int i=0; i<=n; i++){
        float angle = i * 2*mathPI/n;
        float px = c.x + std::cos( angle ) * r.x;
        float py = c.y + std::sin( angle ) * r.y;
        outline[i] =  sf::Vertex (sf::Vector2f(px, py), sfOutlineColor);
    }
    globSFML::CurrentRenderTarget->draw(outline.data(), n+1, sf::LineStrip);

}


void DrawerSFML::Dot(jugimap::Vec2f p)
{

    float r = 2;

    sf::CircleShape circle(r,6);
    circle.setOrigin(r, r);
    circle.setFillColor(sfOutlineColor);
    circle.setOutlineThickness(0);
    circle.setPosition(p.x, p.y);
    globSFML::CurrentRenderTarget->draw(circle);

}


//===================================================================================================


void MapSFML::DrawEngineObjects()
{


    //--- convert jugimap 'camera' to SFML 'view'
    Camera *c = GetCamera();
    sf::FloatRect rect(c->GetViewport().min.x, c->GetViewport().min.y, c->GetViewport().GetWidth(), c->GetViewport().GetHeight());

    sf::View view(rect);
    view.setRotation(c->GetRotation());
    view.zoom(1.0/c->GetScale());

    if(c->GetKind()==Camera::Kind::WORLD){
        WorldMapCamera *wc = static_cast<WorldMapCamera*>(c);
        Vec2f center = wc->GetPointedPosition();
        view.setCenter(center.x ,center.y);

    }else if(c->GetKind()==Camera::Kind::SCREEN){
        ScreenMapCamera *sc = static_cast<ScreenMapCamera*>(c);
        view.setCenter(sc->GetScreenMapDesignSize().x/2.0 , sc->GetScreenMapDesignSize().y/2.0);
    }

    sf::FloatRect sfViewport(rect.left/settings.GetScreenSize().x, rect.top/settings.GetScreenSize().y, rect.width/settings.GetScreenSize().x, rect.height/settings.GetScreenSize().y);
    view.setViewport(sfViewport);
    globSFML::CurrentRenderTarget->setView(view);


    Map::DrawEngineObjects();

}


/*

void MapSFML::UpdateAndDrawEngineMap()
{


    UpdateEngineObjects();


    //--- convert jugimap 'camera' to SFML 'view'
    Camera *c = GetCamera();
    sf::FloatRect rect(c->GetViewport().min.x, c->GetViewport().min.y, c->GetViewport().Width(), c->GetViewport().Height());

    sf::View view(rect);
    view.setRotation(c->GetRotation());
    view.zoom(1.0/c->GetScale());

    if(c->GetKind()==Camera::Kind::WORLD){
        WorldMapCamera *wc = static_cast<WorldMapCamera*>(c);
        Vec2f center = wc->GetPointedPosition();
        view.setCenter(center.x ,center.y);

    }else if(c->GetKind()==Camera::Kind::SCREEN){
        ScreenMapCamera *sc = static_cast<ScreenMapCamera*>(c);
        view.setCenter(sc->GetScreenMapDesignSize().x/2.0 , sc->GetScreenMapDesignSize().y/2.0);
    }

    sf::FloatRect sfViewport(rect.left/settings.GetScreenSize().x, rect.top/settings.GetScreenSize().y, rect.width/settings.GetScreenSize().x, rect.height/settings.GetScreenSize().y);
    view.setViewport(sfViewport);
    globSFML::CurrentRenderTarget->setView(view);


    DrawEngineObjects();

}

*/


//===================================================================================================


void EngineSceneSFML::PreUpdate()
{

    // JOYSTICK - lets manage joystick connection here because events are not trigered when app is out of focus
    for(int i=0; i<jugimap::joysticks.size(); i++){

        jugimap::Joystick &joystick = jugimap::joysticks[i];

        //----
        bool connected = sf::Joystick::isConnected(i);
        if(connected){
            if(joystick.IsConnected()==false){
                joystick._SetConnected(true);
                //sf::Joystick::Identification identification = sf::Joystick::getIdentification(0);
                //std::string name = identification.name.toAnsiString();
                //joystick._SetName(name);
            }
        }else{
            joystick._SetConnected(false);
        }
    }
}


//===================================================================================================


void EngineAppSFML::SetSystemMouseCursorVisible(bool _visible)
{
    globSFML::CurrentWindow->setMouseCursorVisible(_visible);
}



/*

void CommandsSFML::PreAppUpdate()
{

    // MOUSE
    if(mouse.GetCursorSprite()){
        mouse.GetCursorSprite()->SetPosition(Vec2f(mouse.GetX(), mouse.GetY()));
    }

    // JOYSTICK - lets manage joystick connection here because events are not trigered when app is out of focus
    for(int i=0; i<jugimap::joysticks.size(); i++){

        jugimap::Joystick &joystick = jugimap::joysticks[i];

        //----
        bool connected = sf::Joystick::isConnected(i);
        if(connected){
            if(joystick.IsConnected()==false){
                joystick._SetConnected(true);
                //sf::Joystick::Identification identification = sf::Joystick::getIdentification(0);
                //std::string name = identification.name.toAnsiString();
                //joystick._SetName(name);
            }
        }else{
            joystick._SetConnected(false);
        }
    }

}



void CommandsSFML::PostAppUpdate()
{

    mouse.ResetPerUpdateFlags();
    keyboard.ResetPerUpdateFlags();
    touch.ResetPerUpdateFlags();

    for(Joystick &gc: joysticks) gc.ResetPerUpdateFlags();
}


void CommandsSFML::SetSystemMouseCursorVisible(bool _visible)
{
    globSFML::CurrentWindow->setMouseCursorVisible(_visible);
}

*/


//===================================================================================================


KeyboardConversionTable::KeyboardConversionTable()
{

    table.resize((int)sf::Keyboard::KeyCount, jugimap::KeyCode::UNKNOWN);

    table[(int)sf::Keyboard::Backspace] = jugimap::KeyCode::BACKSPACE;
    table[(int)sf::Keyboard::Tab] = jugimap::KeyCode::TAB;
    table[(int)sf::Keyboard::Return] = jugimap::KeyCode::ENTER;
    table[(int)sf::Keyboard::Pause] = jugimap::KeyCode::PAUSE;
    table[(int)sf::Keyboard::Escape] = jugimap::KeyCode::ESCAPE;
    table[(int)sf::Keyboard::Space] = jugimap::KeyCode::SPACE;

    table[(int)sf::Keyboard::PageUp] = jugimap::KeyCode::PAGEUP;
    table[(int)sf::Keyboard::PageDown] = jugimap::KeyCode::PAGEDOWN;
    table[(int)sf::Keyboard::End] = jugimap::KeyCode::END;
    table[(int)sf::Keyboard::Home] = jugimap::KeyCode::HOME;
    table[(int)sf::Keyboard::Left] = jugimap::KeyCode::LEFT;
    table[(int)sf::Keyboard::Up] = jugimap::KeyCode::UP;
    table[(int)sf::Keyboard::Right] = jugimap::KeyCode::RIGHT;
    table[(int)sf::Keyboard::Down] = jugimap::KeyCode::DOWN;
    table[(int)sf::Keyboard::Insert] = jugimap::KeyCode::INSERT;
    table[(int)sf::Keyboard::Delete] = jugimap::KeyCode::DELETEkey;

    table[(int)sf::Keyboard::Num0] = jugimap::KeyCode::NUM_0;
    table[(int)sf::Keyboard::Num1] = jugimap::KeyCode::NUM_1;
    table[(int)sf::Keyboard::Num2] = jugimap::KeyCode::NUM_2;
    table[(int)sf::Keyboard::Num3] = jugimap::KeyCode::NUM_3;
    table[(int)sf::Keyboard::Num4] = jugimap::KeyCode::NUM_4;
    table[(int)sf::Keyboard::Num5] = jugimap::KeyCode::NUM_5;
    table[(int)sf::Keyboard::Num6] = jugimap::KeyCode::NUM_6;
    table[(int)sf::Keyboard::Num7] = jugimap::KeyCode::NUM_7;
    table[(int)sf::Keyboard::Num8] = jugimap::KeyCode::NUM_8;
    table[(int)sf::Keyboard::Num9] = jugimap::KeyCode::NUM_9;

    table[(int)sf::Keyboard::Numpad0] = jugimap::KeyCode::NUM_0;
    table[(int)sf::Keyboard::Numpad1] = jugimap::KeyCode::NUM_1;
    table[(int)sf::Keyboard::Numpad2] = jugimap::KeyCode::NUM_2;
    table[(int)sf::Keyboard::Numpad3] = jugimap::KeyCode::NUM_3;
    table[(int)sf::Keyboard::Numpad4] = jugimap::KeyCode::NUM_4;
    table[(int)sf::Keyboard::Numpad5] = jugimap::KeyCode::NUM_5;
    table[(int)sf::Keyboard::Numpad6] = jugimap::KeyCode::NUM_6;
    table[(int)sf::Keyboard::Numpad7] = jugimap::KeyCode::NUM_7;
    table[(int)sf::Keyboard::Numpad8] = jugimap::KeyCode::NUM_8;
    table[(int)sf::Keyboard::Numpad9] = jugimap::KeyCode::NUM_9;

    table[(int)sf::Keyboard::A] = jugimap::KeyCode::A;
    table[(int)sf::Keyboard::B] = jugimap::KeyCode::B;
    table[(int)sf::Keyboard::C] = jugimap::KeyCode::C;
    table[(int)sf::Keyboard::D] = jugimap::KeyCode::D;
    table[(int)sf::Keyboard::E] = jugimap::KeyCode::E;
    table[(int)sf::Keyboard::F] = jugimap::KeyCode::F;
    table[(int)sf::Keyboard::G] = jugimap::KeyCode::G;
    table[(int)sf::Keyboard::H] = jugimap::KeyCode::H;
    table[(int)sf::Keyboard::I] = jugimap::KeyCode::I;
    table[(int)sf::Keyboard::J] = jugimap::KeyCode::J;
    table[(int)sf::Keyboard::K] = jugimap::KeyCode::K;
    table[(int)sf::Keyboard::L] = jugimap::KeyCode::L;
    table[(int)sf::Keyboard::M] = jugimap::KeyCode::M;
    table[(int)sf::Keyboard::N] = jugimap::KeyCode::N;
    table[(int)sf::Keyboard::O] = jugimap::KeyCode::O;
    table[(int)sf::Keyboard::P] = jugimap::KeyCode::P;
    table[(int)sf::Keyboard::Q] = jugimap::KeyCode::Q;
    table[(int)sf::Keyboard::R] = jugimap::KeyCode::R;
    table[(int)sf::Keyboard::S] = jugimap::KeyCode::S;
    table[(int)sf::Keyboard::T] = jugimap::KeyCode::T;
    table[(int)sf::Keyboard::U] = jugimap::KeyCode::U;
    table[(int)sf::Keyboard::V] = jugimap::KeyCode::V;
    table[(int)sf::Keyboard::W] = jugimap::KeyCode::W;
    table[(int)sf::Keyboard::X] = jugimap::KeyCode::X;
    table[(int)sf::Keyboard::Y] = jugimap::KeyCode::Y;
    table[(int)sf::Keyboard::Z] = jugimap::KeyCode::Z;

    table[(int)sf::Keyboard::F1] = jugimap::KeyCode::F1;
    table[(int)sf::Keyboard::F2] = jugimap::KeyCode::F2;
    table[(int)sf::Keyboard::F3] = jugimap::KeyCode::F3;
    table[(int)sf::Keyboard::F4] = jugimap::KeyCode::F4;
    table[(int)sf::Keyboard::F5] = jugimap::KeyCode::F5;
    table[(int)sf::Keyboard::F6] = jugimap::KeyCode::F6;
    table[(int)sf::Keyboard::F7] = jugimap::KeyCode::F7;
    table[(int)sf::Keyboard::F8] = jugimap::KeyCode::F8;
    table[(int)sf::Keyboard::F9] = jugimap::KeyCode::F9;
    table[(int)sf::Keyboard::F10] = jugimap::KeyCode::F10;
    table[(int)sf::Keyboard::F11] = jugimap::KeyCode::F11;
    table[(int)sf::Keyboard::F12] = jugimap::KeyCode::F12;

    table[(int)sf::Keyboard::Hyphen] = jugimap::KeyCode::HYPHEN;                    // -
    table[(int)sf::Keyboard::Equal] = jugimap::KeyCode::EQUAL;                    // =
    table[(int)sf::Keyboard::LBracket] = jugimap::KeyCode::LEFT_BRACKET;
    table[(int)sf::Keyboard::RBracket] = jugimap::KeyCode::RIGHT_BRACKET;
    table[(int)sf::Keyboard::Backslash] = jugimap::KeyCode::BACKSLASH;
    table[(int)sf::Keyboard::Semicolon] = jugimap::KeyCode::SEMICOLON;
    table[(int)sf::Keyboard::Quote] = jugimap::KeyCode::QUOTE;
    table[(int)sf::Keyboard::Comma] = jugimap::KeyCode::COMMA;
    table[(int)sf::Keyboard::Period] = jugimap::KeyCode::PERIOD;
    table[(int)sf::Keyboard::Slash] = jugimap::KeyCode::SLASH;

    table[(int)sf::Keyboard::LShift] = jugimap::KeyCode::LEFT_SHIFT;
    table[(int)sf::Keyboard::RShift] = jugimap::KeyCode::RIGHT_SHIFT;
    table[(int)sf::Keyboard::LControl] = jugimap::KeyCode::LEFT_CONTROL;
    table[(int)sf::Keyboard::RControl] = jugimap::KeyCode::RIGHT_CONTROL;
    table[(int)sf::Keyboard::LAlt] = jugimap::KeyCode::LEFT_ALT;
    table[(int)sf::Keyboard::RAlt] = jugimap::KeyCode::RIGHT_ALT;

}


void ProcessEvents(sf::Event &event)
{

    static KeyboardConversionTable conv;


    //--- MOUSE
    if(event.type == sf::Event::MouseButtonPressed){

        if(event.mouseButton.button == sf::Mouse::Button::Left){
            jugimap::mouse._SetButtonState(jugimap::MouseButton::LEFT, true);

        }else if(event.mouseButton.button == sf::Mouse::Button::Middle){
            jugimap::mouse._SetButtonState(jugimap::MouseButton::MIDDLE, true);

        }else if(event.mouseButton.button == sf::Mouse::Button::Right){
            jugimap::mouse._SetButtonState(jugimap::MouseButton::RIGHT, true);
        }


    }else if(event.type == sf::Event::MouseButtonReleased){

        if(event.mouseButton.button == sf::Mouse::Button::Left){
            jugimap::mouse._SetButtonState(jugimap::MouseButton::LEFT, false);

        }else if(event.mouseButton.button == sf::Mouse::Button::Middle){
            jugimap::mouse._SetButtonState(jugimap::MouseButton::MIDDLE, false);

        }else if(event.mouseButton.button == sf::Mouse::Button::Right){
            jugimap::mouse._SetButtonState(jugimap::MouseButton::RIGHT, false);
        }

    }else if(event.type == sf::Event::MouseMoved){

        jugimap::mouse._SetPosition(jugimap::Vec2i(event.mouseMove.x, event.mouseMove.y));


    }else if(event.type == sf::Event::MouseWheelScrolled){

        if(event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel){
            jugimap::mouse._SetWheel(jugimap::Vec2i(0, event.mouseWheelScroll.delta));
        }


    //--- KEYBOARD
    }else if(event.type == sf::Event::KeyPressed){
        if((int)event.key.code>=0 && (int)event.key.code<conv.table.size()){
            jugimap::keyboard._SetKeyState(conv.table[(int)event.key.code], true);
        }

    }else if(event.type == sf::Event::KeyReleased){
        if((int)event.key.code>=0 && (int)event.key.code<conv.table.size()){
            jugimap::keyboard._SetKeyState(conv.table[(int)event.key.code], false);
        }


    //--- JOYSTICK
    }else if(event.type == sf::Event::JoystickButtonPressed){

        int joyID = event.joystickButton.joystickId;

        if(joyID>=0 && joyID<jugimap::joysticks.size()){
            jugimap::joysticks[joyID]._SetButtonState(event.joystickButton.button, true);
        }

    }else if(event.type == sf::Event::JoystickButtonReleased){

        int joyID = event.joystickButton.joystickId;

        if(joyID>=0 && joyID<jugimap::joysticks.size()){
            jugimap::joysticks[joyID]._SetButtonState(event.joystickButton.button, false);
        }


    }else if(event.type == sf::Event::JoystickMoved){

        int joyID = event.joystickMove.joystickId;

        if(joyID>=0 && joyID<jugimap::joysticks.size()){

            if(event.joystickMove.axis==sf::Joystick::Axis::X){
                joysticks[joyID]._SetXaxis(event.joystickMove.position/100.0);

            }else if(event.joystickMove.axis==sf::Joystick::Axis::Y){
                joysticks[joyID]._SetYaxis(event.joystickMove.position/100.0);

            }else if(event.joystickMove.axis==sf::Joystick::Axis::Z){
                joysticks[joyID]._SetZaxis(event.joystickMove.position/100.0);

            }else if(event.joystickMove.axis==sf::Joystick::Axis::PovX){

                //--- POV directions
                float x =  event.joystickMove.position;     // -100 ... 100

                if(x<-25){
                    //joysticks[joyID]._SetPOV_X(Joystick::POV::LEFT);
                    joysticks[joyID]._SetPOV_X(Joystick::POV_X::LEFT);

                }else if(x>25){
                    //joysticks[joyID]._SetPOV_X(Joystick::POV::RIGHT);
                    joysticks[joyID]._SetPOV_X(Joystick::POV_X::RIGHT);

                }else{
                    //joysticks[joyID]._SetPOV_X(Joystick::POV::NONE);
                    joysticks[joyID]._SetPOV_X(Joystick::POV_X::NONE);
                }

            }else if(event.joystickMove.axis==sf::Joystick::Axis::PovY){

                //--- POV directions
                float y =  event.joystickMove.position;     // -100 ... 100

                if(y<-25){
                    //joysticks[joyID]._SetPOV_Y(Joystick::POV::DOWN);
                    joysticks[joyID]._SetPOV_Y(Joystick::POV_Y::DOWN);

                }else if(y>25){
                    //joysticks[joyID]._SetPOV_Y(Joystick::POV::UP);
                    joysticks[joyID]._SetPOV_Y(Joystick::POV_Y::UP);

                }else{
                    //joysticks[joyID]._SetPOV_Y(Joystick::POV::NONE);
                    joysticks[joyID]._SetPOV_Y(Joystick::POV_Y::NONE);
                }
            }

            jugimap::joysticks[joyID]._SetButtonState(event.joystickButton.button, false);
        }


    //--- TOUCH
    }else if(event.type == sf::Event::TouchBegan){

        touch._SetFingerState(event.touch.finger, true, Vec2i(event.touch.x, event.touch.y));


    }else if(event.type == sf::Event::TouchEnded){

        touch._SetFingerState(event.touch.finger, false, Vec2i(event.touch.x, event.touch.y));

    }else if(event.type == sf::Event::TouchMoved){

        touch._SetFingerState(event.touch.finger, true, Vec2i(event.touch.x, event.touch.y));

    }

}



//===================================================================================================



namespace shaders
{

    ShaderSFML_ColorOverlay_SimpleMultiply ColorOverlay_SimpleMultiply;
    ShaderSFML_ColorOverlay_Normal ColorOverlay_Normal;
    ShaderSFML_ColorOverlay_Multiply ColorOverlay_Multiply;
    ShaderSFML_ColorOverlay_LinearDodge ColorOverlay_LinearDodge;
    ShaderSFML_ColorOverlay_Color ColorOverlay_Color;


    void LoadJugimapShaders()
    {
        ColorOverlay_SimpleMultiply.Load("media/shaders_SFML/spriteColorOverlay_simpleMultiply.frag");
        ColorOverlay_Normal.Load("media/shaders_SFML/spriteColorOverlay_normal.frag");
        ColorOverlay_Multiply.Load("media/shaders_SFML/spriteColorOverlay_multiply.frag");
        ColorOverlay_LinearDodge.Load("media/shaders_SFML/spriteColorOverlay_linearDodge.frag");
        ColorOverlay_Color.Load("media/shaders_SFML/spriteColorOverlay_color.frag");
    }

}





namespace globSFML {


sf::RenderTarget *CurrentRenderTarget = nullptr;
sf::Window *CurrentWindow = nullptr;


int countSpriteDrawCalls = 0;
int countDrawnSprites = 0;
int countDrawnFrames = 0;


}





}



