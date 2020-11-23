#include <algorithm>
#include "jugimapSFML/jmSFML.h"
#include "jugiApp/app.h"




int main()
{

    sf::RenderWindow window(sf::VideoMode(1300, 800), jugiApp::jugimapAppName + " - SFML version");
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(true);


    //---
    sf::Font font;
    if(font.loadFromFile("media/fonts/OpenSans-Regular.ttf")==false){
        jugimap::DbgOutput("Font load error!");
    }
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(14);


    // JUGIMAP CORE INITIALIZATION
    //---------------------------------------------------
    jugimap::settings.SetEngine(jugimap::Engine::SFML);
    jugimap::settings.SetScreenSize(jugimap::Vec2i(window.getSize().x, window.getSize().y));
    jugimap::objectFactory = new jugimap::ObjectFactorySFML();            // all jugimap elements are created via 'objectFactory'
    jugimap::globSFML::CurrentWindow = &window;
    jugimap::globSFML::CurrentRenderTarget = &window;

    // path prefixes
    // jugimap::JugiMapBinaryLoader::pathPrefix = "";        // No prefix needed.
    // jugimap::GraphicFile::pathPrefix = "";                // No prefix needed.
    // jugimap::Font::pathPrefix = "";                       // No prefix needed.
    // jugimap::TextBook::pathPrefix = "";                   // No prefix needed.

    // path where text files are stored
    jugimap::TextLibrary::path = "media/texts/";

    // shaders
    jugimap::shaders::LoadJugimapShaders();


    //---------------------------------------------------
    jugiApp::application = new jugiApp::DemoApp();
    jugiApp::application->Init();


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

                jugimap::settings.SetAppTerminated(true);
                delete  jugiApp::application;
                jugimap::DeleteGlobalObjects();

                window.close();
                return 0;

            }else{
                jugimap::ProcessEvents(event);
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


        jugiApp::application->Update(frameTimeMS/1000.0);

        // ... Or use this to update logic with a fixed time step.
        //jugimap::settings.EnableLerpDrawing(true);                              // draw with interpolated values to reduce stutter when fixed rate logic is in place which is not in sync with frame rate
        //jugimap::sceneManager->UpdateViaFixedTimeStep(1000.0/30);            // fixed time step of 1000.0/30 miliseconds


        // DRAW
        //----------------------------------------------------------
        window.clear();
        jugiApp::application->Draw();
		window.display();

        jugimap::globSFML::countDrawnFrames++;
    }


	return 0;
}






