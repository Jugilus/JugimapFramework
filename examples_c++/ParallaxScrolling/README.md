# Parallax Scrolling

## About

Parallax Scrolling is an application which utilizes parallax map features of JugiMap Framework.

</br> 

A web version, the project files for JugiMap Editor and other information: https://jugilus.github.io/Jugimap-ParallaxScrolling/JugimapParallaxScrolling.html

## Setting up the engine project

Game development engines provide different ways of creating a new project and you should be familiar with that process.

Create a new project for engine 'ENGINE' (COCOS2D-X, SFML, AGK, nCINE ) and name it, in example, ParallaxScrolling.

Add the following source code to the project:
- JugiMap Framework common library - add directory *jugimap*
- Jugimap Framework engine extension - add directory *jugimapENGINE*
- Other libraries - add directory *utf8cpp*
- Engine independent application source files  - add directory: *jugiApp*
- Engine dependent application source files - add the **content** of directory: *ParallaxScrolling_ENGINE*

Copy directory *media* to:
- the working directory of the application if the engine is AGK or SFML.
- the *Resources* subdirectory of the working directory if the engine is COCOS2D-X 
- the *data* subdirectory of the working directory if the engine is nCINE


