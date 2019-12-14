/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"
#include "sceneCOCOS2D-X.h"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
//static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1300, 800);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{

#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif


    apiTestDemo::entities.Delete();
    jugimap::shaders::DeleteShaders();
    jugimap::sceneManager->DeleteScenes();
    jugimap::DeleteGlobalObjects();

}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

        glview = GLViewImpl::createWithRect("JugiMap API Demo Test - Cocos2d-x version", cocos2d::Rect(0, 0, mediumResolutionSize.width, mediumResolutionSize.height));
#else
        glview = GLViewImpl::create("HelloCpp");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    //director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    //glview->setDesignResolutionSize(mediumResolutionSize.width, mediumResolutionSize.height, ResolutionPolicy::NO_BORDER);


    auto frameSize = glview->getFrameSize();
    glview->setDesignResolutionSize(frameSize.width, frameSize.height, ResolutionPolicy::SHOW_ALL);

    register_all_packages();



    // Set required jugimap global paramaters and objects
    //---------------------------------------------------

    jugimap::settings.SetScreenSize(jugimap::Vec2i(director->getVisibleSize().width, director->getVisibleSize().height));
    jugimap::settings.SetYCoordinateUp(true);
    jugimap::objectFactory = new jugimap::ObjectFactoryCC();                    // all jugimap elements are created via 'objectFactory'
    jugimap::sceneManager = new jugimap::SceneManager();

    // path prefixes
    jugimap::JugiMapBinaryLoader::pathPrefix = "Resources/";        // The map loader is not part of cocos2d loaders so we need to add prefix
    //jugimap::GraphicFile::pathPrefix = "";                        // No prefix as cocos2d loaders use prefix in their loaders
    //jugimap::Font::pathPrefix = "";                               // No prefix as cocos2d loaders use prefix in their loaders

    // Load shaders required for some jugimap sprite properties
    jugimap::shaders::glpColorOverlay_blendSimpleMultiply = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "SIMPLE_MULTIPLY_BLEND");
    if(jugimap::shaders::glpColorOverlay_blendSimpleMultiply){
        jugimap::shaders::glpColorOverlay_blendSimpleMultiply->retain();
    }
    jugimap::shaders::glpColorOverlay_blendNormal = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "NORMAL_PIXEL_BLEND");
    if(jugimap::shaders::glpColorOverlay_blendNormal){
        jugimap::shaders::glpColorOverlay_blendNormal->retain();
    }
    jugimap::shaders::glpColorOverlay_blendMultiply = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "MULTIPLY_PIXEL_BLEND");
    if(jugimap::shaders::glpColorOverlay_blendMultiply){
        jugimap::shaders::glpColorOverlay_blendMultiply->retain();
    }
    jugimap::shaders::glpColorOverlay_blendLinearDodge = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "LINEAR_DODGE_PIXEL_BLEND");
    if(jugimap::shaders::glpColorOverlay_blendLinearDodge){
        jugimap::shaders::glpColorOverlay_blendLinearDodge->retain();
    }
    jugimap::shaders::glpColorOverlay_blendColor = cocos2d::GLProgram::createWithFilenames("media/shaders_COCOS2D-X/spriteCommon.vert", "media/shaders_COCOS2D-X/spriteColorOverlay.frag", "COLOR_PIXEL_BLEND");
    if(jugimap::shaders::glpColorOverlay_blendColor){
        jugimap::shaders::glpColorOverlay_blendColor->retain();
    }

    assert(jugimap::shaders::glpColorOverlay_blendSimpleMultiply!=nullptr);
    assert(jugimap::shaders::glpColorOverlay_blendNormal!=nullptr);
    assert(jugimap::shaders::glpColorOverlay_blendMultiply!=nullptr);
    assert(jugimap::shaders::glpColorOverlay_blendLinearDodge!=nullptr);
    assert(jugimap::shaders::glpColorOverlay_blendColor!=nullptr);


    //---------------------------------------------------

    jugimap::Scene* scene = jugimap::sceneManager->AddScene(new apiTestDemo::PlatformerSceneCC());
    jugimap::sceneManager->SetCurrentScene(scene);

    if(scene->GetEngineSceneLink()==nullptr) return false;
    jugimap::SceneCCNode* sceneNode = static_cast<jugimap::SceneCCNode*>(scene->GetEngineSceneLink());

    // run
    director->runWithScene(sceneNode);

    return true;
}


// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif


}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif


}
