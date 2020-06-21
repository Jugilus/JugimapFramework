# Sprite Timeline Animation 

## About

Sprite Timeline Animation is a demo application which displays various animated sprites using timeline animations in JugiMap API.

For more information, editor map files and to see a web version go here: 
https://jugilus.github.io/Jugimap-SpriteTimelineAnimation/JugimapSpriteTimelineAnimation.html

## Setting up the engine project

Game development engines provide different ways of creating a new project and you should be familiar with that process.

Create a new project for engine ENGINE (COCOS2D-X, SFML, AGK, nCINE ) and name it, in example, SpriteTimelineAnimation.

Add the following source code to the project:
- JugiMap API common library - add directory *jugimap*
- Jugimap API engine extension - add directory *jugimapENGINE*
- Engine independent source files  - add directory: *engineIndependent*
- Engine dependent source files - add the **content** of directory: *SpriteTimelineAnimation_ENGINE*

Copy directory *media* to:
- the working directory of the application if the engine is AGK or SFML.
- the *Resources* subdirectory of the working directory if the engine is COCOS2D-X 
- the *data* subdirectory of the working directory if the engine is nCINE


