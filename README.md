# JugiMap API

JugiMap API is a library for using JugiMap maps in game development engines.

The API consists of two parts: the main library and the engine extensions.

### 1. Main library

The main library is an engine independent collection of classes and functions. It provides definitions for:

- Map elements from the editor (map, layers, sprites, vector shapes, ...).
- Other map elements (text, drawing layers for geometric primitives, ...).
- Supporting elements (cameras, colliders, tweens ...).
- Supporting tools (map loader, functions for seeking and collecting map elements, ...).
- Common objects (2d vector, rectangle, transformation matrix, ...).

The main library is currently available for the following languages:
- C++


### 2. Engine extensions

Engine extensions provide integration of the main library with the game development frameworks - engines. An engine extension is a collection of classes which extend classes from the main library and wrap the engine objects. 

Engine extensions are currently available for the following engines: 
- Cocos2d-x
- SFML
- AGK Tier 2
- nCine

If you are interested to make an extension for some engine, check out the existing extensions and use one as template.  


### 3. Using the API

The API library provides an engine independent interface for managing map elements. It is possible to manipulate engine objects directly but you should not mix direct manipulation and manipulation via interface as that would lead to synchronization issues between jugimap and engine objects. It is recommended to use direct manipulation only for things not supported by the library, in example sprites in physics simulation.


A game scene which uses a JugiMap map must manage it in the following way:

**scene initialization**
- ...
- Set some global values required by the JugiMap API (screen size, ... )
- Load shaders needed for some sprite effects. 
- Set object factory for the used engine.
- Load the map from files (source graphics file and map file exported from the editor).
- Initialize the map as a world, parallax or screen map.
- Initialize the map engine objects.
- Initialize a camera and assign it to the map. 
- ...

**scene update**
- Set some global runtime values required by the JugiMap API ( passed milliseconds, frame time, ...).
- ...
- (Game logic)
- ...
- Update the map engine objects. (This can be also done at the start of **scene draw** if the engine allows it.)

  
These are standard operations. Some engines may require few more things.

A practical example of setting up and using a scene from JugiMap maps is API Demo Test.


#### API Demo Test

API Demo Test is a testing program which is used in development.
It is also an extensive example of how to use the API. The source code is available for all supported engines and 
well documented. 

Features:

- A scene with world, parallax and screen maps.
	- Loading maps.
	- Initializing maps.
	- Initializing map cameras.
	- Creating and adding *text layers* and *drawing layers* to maps.
	- Manually creating a map with a layer and a sprite. 
	 
- Game entities with jugimap sprites as 'avatars'.
	- Managing sprites frame animation.
	- Using sprite colliders with a custom collision system.
	- Adding and removing entities dynamically.
	 
- A simple GUI.
	- Implementation of buttons with jugimap sprites and text.
	
- Physics test (for engines which provides a 2d physics system)
	- Using sprites as physics objects 


### 4. Documentation

The Jugimap API is fully documented with Doxygen.

