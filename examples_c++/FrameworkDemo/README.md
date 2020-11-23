# Framework demo

## About


JugiMap Framework Demo is an application which utilizes many features of JugiMap framework and serves as an extensive programming example of how to initialize and manage a scene. It also shows how to include engine specific features into a scene in a way that makes easier porting to other engines. 

</br>   

A web version, the project files for JugiMap Editor and other information: https://jugilus.github.io/Jugimap-FrameworkDemo/JugimapFrameworkDemo.html


## Setting up the engine project

Game development engines provide different ways of creating a new project and you should be familiar with that process.

Create a new project for engine 'ENGINE' (COCOS2D-X, SFML, AGK, nCINE ) and name it, in example, FrameworkDemo.

Add the following source code to the project:
- JugiMap Framework common library - add directory *jugimap*
- Jugimap Framework engine extension - add directory *jugimapENGINE*
- Other libraries - add directory *utf8cpp*
- Engine independent application source files  - add directory: *jugiApp*
- Engine dependent application source files - add the **content** of directory: *FrameworkDemo_ENGINE*

Copy directory *media* to:
- the working directory of the application if the engine is AGK or SFML.
- the *Resources* subdirectory of the working directory if the engine is COCOS2D-X 
- the *data* subdirectory of the working directory if the engine is nCINE

