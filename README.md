JugiMap API
=======================

<h2>About</h2>

- JugiMap Editor - an application for creating 2d game maps: http://jugimap.com
- JugiMap API - a library for using JugiMap maps in game development engines: https://github.com/Jugilus/jugimapAPI
- JugiMap API documentation: https://jugilus.github.io/JugiMapAPIdoc


<h2>Main library</h2>

The main library is an engine independent part of JugiMap API. It provides support for:

- Map elements from the editor: map, layers, sprites, vector shapes, ...
- Other map elements: text, drawing layers for geometric primitives, ...
- Supporting elements: cameras, colliders, tweens ...
- Supporting tools: map loader, functions for seeking and collecting map elements, ...
- Common objects: 2d vector, rectangle, transformation matrix, ...

The main library is currently available for the following languages:
- C++


<h2>Engine extensions</h2>

Engine extensions provide integration of the main library with game development engines (frameworks). An engine extension is a collection of classes which extend classes from the main library and wrap the engine objects. 

Engine extensions are currently available for the following engines: 
- Cocos2d-x
- SFML
- AGK Tier 2
- nCine

Extensions are possible for engines with the following features:
- Programming language is c++.
- Game can be fully programmed by hand (as opposed to engines which require visual editors).
- Sufficient 2D functionality for supporting jugimap features.


<h2>Using the API</h2>

A game which uses scenes made from one or more JugiMap maps should be set up in the following way: 

**application initialization**
- ...
- Set required global jugimap parameters: screen size, engine specific parameters, path prefixes.
- Load shaders which are needed for some sprite properties.
- Make *object factory* for creating objects of the engine extended classes.
- Make *scene manager* (optional) 
- ...

**scene initialization**
- ...
- Load maps.
- Initialize maps as world, parallax or screen maps.
- Initialize engine objects of the maps.
- Make cameras and assign them to the maps. 
- ...

**application update**
- ...
- Update scene (called directly or via scene manager)
- ...


**scene update**
- Set required global jugimap time parameters: *logic time* and *passed time*.
- ...
- (Game logic)
- ...
- Update engine objects of the maps.

  
These are standard operations. Some engines may require more things. In general JugiMap API provides a consistent way to set up and use
scenes based on JugiMap maps.

**Manipulation of map elements**

The API library provides an engine independent interface for managing map elements. It is possible to manipulate engine objects directly but you should not mix direct manipulation and manipulation via interface as that would lead to synchronization issues between jugimap and engine objects. It is recommended to use direct manipulation only for things not supported by the library, e.g. for sprites in physics simulation.


<h3>Programming Examples</h3>

The best way to get acquainted with practical usage of JugiMap API is by examining programming examples which use it. They can be found at: https://github.com/Jugilus/jugimapAPI

