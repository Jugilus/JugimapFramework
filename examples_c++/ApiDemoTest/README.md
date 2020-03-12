# JugiMap API Demo Test

## About

JugiMap API Demo Test is an application which utilizes many features of JugiMap API and serves as an extensive programming example of how to set up and manage a relatively complex scene. 

The application source code includes:

- Programming a scene with world, parallax and screen maps.
   - Loading maps.
   - Initializing maps.
   - Initializing map cameras.
   - Creating and adding *text layers* and *drawing layers* to maps.
   - Manually creating a map with a layer and a sprite.
   - Starting scene.
   - Updating scene.
	 
- Programming game entities using jugimap sprites.
   - Setting up and updating entities.
   - Managing sprite frame animation.
   - Using sprite colliders with a custom collision system.
   - Dynamically adding and removing entities.
	 
- Programming a simple GUI.
   - Creating and using buttons from sprites and text objects.
	
- Physics test (for engines which provide a 2d physics system).
   - Using sprites as dynamic objects in physics simulation. 

</br>
   
For more information, editor map files and a web version go here: https://jugilus.github.io/Jugimap-ApiDemoTest/JugimapApiDemoTest.html


## Setting up the engine project

Game development engines provide different ways of creating a new project and you should be familiar with that process.

Create a new project for engine ENGINE (COCOS2D-X, SFML, AGK, nCINE ) and name it, in example, JugiMapAPIDemoTest.

Add the following source code to the project:
- JugiMap API common library - add directory *jugimap*
- Jugimap API engine extension - add directory *jugimapENGINE*
- Engine independent source files  - add directory: *engineIndependent*
- Engine dependent source files - add the **content** of directory: *ApiDemoTest_ENGINE*

Copy directory *media* to:
- the working directory of the application if the engine is AGK or SFML.
- the *Resources* subdirectory of the working directory if the engine is COCOS2D-X 
- the *data* subdirectory of the working directory if the engine is nCINE

