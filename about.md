JugiMap API {#mainpage}
=======================


<h2>About</h2>

- JugiMap Editor - an application for creating 2d game maps - http://jugimap.com/
- JugiMap API - a library for using JugiMap maps in game development engines - https://github.com/Jugilus/jugimapAPI


JugiMap API consists of two parts: the main library and the engine extensions.


<h2>Main library</h2>


The main library is an engine independent collection of classes and functions. It provides support for:

- Map elements from the editor (map, layers, sprites, vector shapes, ...).
- Other map elements (text, drawing layers for geometric primitives, ...).
- Supporting elements (cameras, colliders, tweens ...).
- Supporting tools (map loader, functions for seeking and collecting map elements, ...).
- Common objects (2d vector, rectangle, transformation matrix, ...).

The main library is currently available for the following languages:
- C++


<h2>Engine extensions</h2>

Engine extensions provide integration of the main library with the game development frameworks - engines. An engine extension is a collection of classes which extend classes from the main library and wrap the engine objects. 

Engine extensions are currently available for the following engines: 
- Cocos2d-x
- SFML
- AGK Tier 2
- nCine

Extensions are possible for engines which meets the following requirements:
- Programming language is c++ (support for more languages will follow in future) .
- They allow full programming approach (as opposed to engines which require visual editors).
- They provide required 2D functionality.


<h2>Using the API</h2>


A game which utilizes JugiMap API should be set up in the following way: 


**application initialization**
- ...
- Set required global jugimap parameters: screen size, engine specific parameters
- Load shaders needed for some sprite properties.
- Make object factory for creating objects from engine extended classes.
- Make scene manager (optional) 
- ...

**scene initialization**
- ...
- Load one or more maps from files (source graphics files and map files exported from the editor).
- Initialize maps as world, parallax or screen maps.
- Initialize map engine objects.
- Make cameras and assign them to maps. 
- ...

**scene update**
- Set required global jugimap time parameters - logic update period and passed time.
- ...
- (Game logic)
- ...
- Update map engine objects.

  
These are standard operations. Some engines may require more things. In general JugiMap API provides consistent way to set up and use
scenes based on JugiMap maps.

**Manipulation of map elements**

The API library provides an engine independent interface for managing map elements. It is possible to manipulate engine objects directly but you should not mix direct manipulation and manipulation via interface as that would lead to synchronization issues between jugimap and engine objects. It is recommended to use direct manipulation only for things not supported by the library, e.g. for sprites in physics simulation.


<h2>API Demo Test</h2>

A good and extensive example of setting up and using a scene from JugiMap maps is API Demo Test. Developers who plan to use JugiMap maps should examine it closely and set up jugimap support in their games in similar way. 
The source code is documented and available for all supported engines.

Features:

- A scene with world, parallax and screen maps. Covered tasks include:
	- Loading maps.
	- Initializing maps.
	- Initializing map cameras.
	- Creating and adding *text layers* and *drawing layers* to maps.
	- Manually creating a map with a layer and a sprite.
	- Starting scene.
	- Updating scene.
	 
- Game entities with jugimap sprites as 'avatars'.  Covered tasks include:
	- Managing sprites frame animation.
	- Using sprite colliders with a custom collision system.
	- Dynamically adding and removing entities.
	 
- A simple GUI.
	- Implementation of buttons with jugimap sprites and text.
	
- Physics test (for engines which provides a 2d physics system)
	- Using sprites as physics objects. 


<h2>Documentation</h2>

JugiMap API is fully documented with Doxygen.

