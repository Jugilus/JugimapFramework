
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


StandardSpriteSFML::~StandardSpriteSFML ()
{
    if(sfSprite){
        delete sfSprite;
    }
}


void StandardSpriteSFML::InitEngineSprite()
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
    UpdateEngineSprite();
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


void StandardSpriteSFML::UpdateEngineSprite()
{

    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineSpriteUsedDirectly()) return;


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
            posGlobal = GetFullGlobalPosition();
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
    posGlobal = GetFullGlobalPosition();
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

        if(GetSourceSprite()->GetName()=="Shine"){
            DummyFunction();
        }

        if(drawChangeFlags & Property::POSITION){

            Vec2f pos = posGlobal;
            if(settings.LerpDrawingEnabled()){
                pos = posGlobalPrevious + (posGlobal-posGlobalPrevious)*settings.GetLerpDrawingFactor();
            }

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


FontSFML::FontSFML(const std::string &_relativeFilePath, int _size, FontKind _kind)
    : Font(_relativeFilePath, _size, _kind)
{

    sfFont = new sf::Font();
    if(sfFont->loadFromFile(GetRelativeFilePath())==false){
        assert(true);
    }
}


FontSFML::~FontSFML()
{
    delete sfFont;
}


//===================================================================================================


TextSFML::~TextSFML()
{
    delete sfText;
}



void TextSFML::InitEngineText()
{
    if(sfText) return;             //already initialized

    sfText = new sf::Text(GetTextString(), *static_cast<FontSFML*>(GetFont())->GetSFFont(), GetFont()->GetSize());

    //---
    SetChangeFlags(Property::ALL);
}


void TextSFML::UpdateEngineText()
{

    if(GetChangeFlags()==0) return;
    if(IsVisible()==false) return;
    if(IsEngineTextUsedDirectly()) return;

    int flags = GetChangeFlags();


    if(flags & Property::TEXT_STRING){
        sfText->setString(GetTextString());
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

    //---
    SetChangeFlags(0);
}



//===================================================================================================



void TextLayerSFML::DrawEngineLayer()
{

    for(Text *t : GetTexts()){
        globSFML::CurrentRenderTarget->draw(*static_cast<TextSFML*>(t)->GetSFText());
    }
}


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
    int n = std::round(2*pi / da + 0.5);

    if(outline.size()<n+1) outline.resize(n+1);

    for(int i=0; i<=n; i++){
        float angle = i * 2*pi/n;
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


void MapSFML::UpdateAndDrawEngineMap()
{


    UpdateEngineMap();


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


    DrawEngineMap();

}



//===================================================================================================



namespace shaders
{
    ShaderSFML_ColorOverlay_SimpleMultiply ColorOverlay_SimpleMultiply;
    ShaderSFML_ColorOverlay_Normal ColorOverlay_Normal;
    ShaderSFML_ColorOverlay_Multiply ColorOverlay_Multiply;
    ShaderSFML_ColorOverlay_LinearDodge ColorOverlay_LinearDodge;
    ShaderSFML_ColorOverlay_Color ColorOverlay_Color;

}



namespace globSFML {


sf::RenderTarget *CurrentRenderTarget = nullptr;
sf::Window *CurrentWindow = nullptr;


int countSpriteDrawCalls = 0;
int countDrawnSprites = 0;
int countDrawnFrames = 0;


}





}



