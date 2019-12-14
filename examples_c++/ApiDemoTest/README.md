# JugiMap API Demo Test

## About

JugiMap API Demo Test is an extensive example of how to set up and use JugiMap API in an actual project.
It's source code is available for all supported engines.

You can check out a web build of API Demo test here: https://jugilus.github.io/ApiDemoTestWeb


## Setting the engine project

Jugimap API Demo Test uses only engine dependencies.

To prepare an engine project for a platform:

### Cocos2d-x

1. Create an empty Cocos2d-x project called *ApiDemoTest* in your IDE.
2. Copy the **content** of *ApiDemoTest_Cocos2d-x* from *examples_c++/ApiDemoTest* into the project directory.
3. Copy directory *engineIndependent* from *examples_c++/ApiDemoTest* into the *Classes* sub-directory of the project directory.
4. Copy directory *jugimap* from *api_c++* into the *Classes* sub-directory of the project directory.
5. Copy directory *jugimapCOCOS2D-X* from from *api_c++* into the *Classes* sub-directory of the project directory.
6. If the target platform is **not** Windows OS you should delete Windows specific *main.cpp* and *main.h* and add platform specific *main.cpp* and/or any other requried file. These files can be found in Cocos2d-x templates.
7. Add all source files to the project in the IDE.
8. Copy directory *media* into *Resources* directory which should reside in the output directory.
9. Add Cocos2d-x dynamic libraries to output directory if needed.

To set up multi-platform building use Cocos2d-x template project.

### AGK tier 2

1. Create an empty AGK project called *ApiDemoTest* in your IDE.
2. Copy the **content** of *ApiDemoTest_AGK* from *examples_c++/ApiDemoTest* into the project directory.
3. Copy directory *engineIndependent* from *examples_c++/ApiDemoTest* into the project directory.
4. Copy directory *jugimap* from *api_c++* into the project directory.
5. Copy directory *jugimapAGK* from from *api_c++* into the project directory.
6. If the target platform is **not** Windows OS you should delete Windows specific *Core.cpp* and *resource.h* and add platform specific source files. These files can be found in AGK templates.
7. Add all source files to the project in the IDE.
8. Copy directory *media* from *examples_c++/ApiDemoTest* to the project output directory.

### SFML

1. Create an empty SFML project called *ApiDemoTest* in your IDE.
2. Copy the **content** of *ApiDemoTest_SFML* from *examples_c++/ApiDemoTest* into the project directory.
3. Copy directory *engineIndependent* from *examples_c++/ApiDemoTest* into the project directory.
4. Copy directory *jugimap* from *api_c++* into the project directory.
5. Copy directory *jugimapSFML* from from *api_c++* into the project directory.
6. Add all source files to the project in the IDE.
7. Copy directory *media* from *examples_c++/ApiDemoTest* to the project output directory.
8. Add SFML dynamic libraries to output directory if needed.

### NCine

1. Create an empty nCIne project called *ApiDemoTest* in your IDE.
2. Copy the **content** of *ApiDemoTest_nCine* from *examples_c++/ApiDemoTest* into the project directory.
3. Copy directory *engineIndependent* from *examples_c++/ApiDemoTest* into the project directory.
4. Copy directory *jugimap* from *api_c++* into the project directory.
5. Copy directory *jugimapNCINE* from from *api_c++* into the project directory.
6. Add all source files to the project in the IDE.
8. Copy directory *media* into *data* directory which should reside in the output directory.
9. Add nCine dynamic libraries to output directory if needed.

To set up multi-platform building use nCine's template project. 

