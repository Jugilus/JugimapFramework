JugiMap Framework
=======================

<h2>About</h2>

- JugiMap Editor - an application for creating 2d game maps: http://jugimap.com
- JugiMap Framework - a framework for using JugiMap maps in game development engines: https://github.com/Jugilus/JugimapFramework

</br>

<h2>User documentation</h2>

- JugiMap Editor: documentation included with the application.
- JugiMap Framework: https://jugilus.github.io/JugimapFrameworkDoc
- Markup codes for writing game texts: http://jugimap.com/extraDocumentation/text_markup_codes.html

</br>

<h2>Main library</h2>

The main library consists of:

- Data structure for all map elements from the editor ( layers, sprites, vector shapes, ... ).
- Supporting data structure ( cameras, colliders, loaders, drawing of graphic primitives, ... ).
- Animation module.
- GUI module.
- Core application and scene system.
- ...

The main library is engine independent part of JugiMap Framework. It is currently available for the following languages:
- C++

</br>

<h2>Engine extensions</h2>

Engine extensions provide integration of the main library with game development engines. An engine extension is a collection of classes which extend classes from the main library and wrap the engine objects. 

Engine extensions are currently available for the following engines: 
- Cocos2d-x
- SFML
- AGK Tier 2
- nCine

</br>

<h2>Using the framework</h2>

The framework provides interface for programming games in engine independent way. If an engine provides features which are not supported in JugiMap framework you can still access and manipulate the engine objects directly.


**Examples**

Check out at: https://github.com/Jugilus/JugimapFramework for various demo examples which were designed with JugiMap Editor and developed with JugiMap framework.

