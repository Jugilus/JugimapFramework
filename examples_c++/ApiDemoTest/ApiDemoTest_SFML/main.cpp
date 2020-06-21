#include <algorithm>
#include "sceneSFML.h"




int main()
{

    sf::RenderWindow window(sf::VideoMode(1300, 800), "JugiMap API Demo Test - SFML version");
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(false);


    //---
    sf::Font font;
    if(font.loadFromFile("media/fonts/OpenSans-Regular.ttf")==false){
        jugimap::DbgOutput("Font load error!");
    }
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(14);


    // Set required jugimap global paramaters and objects
    //---------------------------------------------------
    jugimap::settings.SetScreenSize(jugimap::Vec2i(window.getSize().x, window.getSize().y));
    jugimap::objectFactory = new jugimap::ObjectFactorySFML();            // all jugimap elements are created via 'objectFactory'
    jugimap::sceneManager = new jugimap::SceneManager();
    jugimap::globSFML::CurrentWindow = &window;
    jugimap::globSFML::CurrentRenderTarget = &window;

    // path prefixes
    //jugimap::JugiMapBinaryLoader::pathPrefix = "";        // No prefix needed.
    //jugimap::GraphicFile::pathPrefix = "";                // No prefix needed.
    //jugimap::Font::pathPrefix = "";                       // No prefix needed.

    // Load shaders
    jugimap::shaders::ColorOverlay_SimpleMultiply.Load("media/shaders_SFML/spriteColorOverlay_simpleMultiply.frag");
    jugimap::shaders::ColorOverlay_Normal.Load("media/shaders_SFML/spriteColorOverlay_normal.frag");
    jugimap::shaders::ColorOverlay_Multiply.Load("media/shaders_SFML/spriteColorOverlay_multiply.frag");
    jugimap::shaders::ColorOverlay_LinearDodge.Load("media/shaders_SFML/spriteColorOverlay_linearDodge.frag");
    jugimap::shaders::ColorOverlay_Color.Load("media/shaders_SFML/spriteColorOverlay_color.frag");

    //---------------------------------------------------

    // Create scene
    jugimap::Scene* scene = jugimap::sceneManager->AddScene(new apiTestDemo::PlatformerSceneSFML());
    jugimap::sceneManager->SetCurrentScene(scene);


    //--- timing variables
    double passedTimeMSPrevious =  jugimap::time.UpdatePassedTimeMS();


	while (window.isOpen())
	{

        // EVENTS
        //----------------------------------------------------------
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
                window.close();
                return 0;

            }else if(event.type == sf::Event::MouseButtonPressed){
                apiTestDemo::mouse.SetHit(true);

            }else if(event.type == sf::Event::MouseButtonReleased){
                apiTestDemo::mouse.SetHit(false);

            }else if(event.type == sf::Event::MouseMoved){
                apiTestDemo::mouse.SetScreenPosition(jugimap::Vec2f(event.mouseMove.x, event.mouseMove.y));
            }
        }


        // ERROR
        //----------------------------------------------------------
        if(jugimap::settings.GetErrorMessage() != ""){
            window.clear();
            window.setView(window.getDefaultView());
            text.setString(jugimap::settings.GetErrorMessage());
            text.setPosition(30, 30);
            window.draw(text);
            //---
            window.display();
            continue;
        }


        // LOGIC
        //----------------------------------------------------------
        double passedTimeMS = jugimap::time.UpdatePassedTimeMS();
        double frameTimeMS = passedTimeMS - passedTimeMSPrevious;
        passedTimeMSPrevious = passedTimeMS;

        // Use this function for updating logic with a variable time step which is the same as the frame time...
        jugimap::sceneManager->Update(frameTimeMS);

        // ... Or use this to update logic with a fixed time step.
        //jugimap::settings.EnableLerpDrawing(true);                              // draw with interpolated values to reduce stutter when fixed rate logic is in place which is not in sync with frame rate
        //jugimap::sceneManager->UpdateViaFixedTimeStep(1000.0/30);            // fixed time step of 1000.0/30 miliseconds



        // DRAW
        //----------------------------------------------------------
        window.clear();
        jugimap::sceneManager->Draw();
		window.display();

        jugimap::globSFML::countDrawnFrames++;
    }


	return 0;
}



