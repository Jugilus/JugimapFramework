#ifndef JUGIMAP_API_H
#define JUGIMAP_API_H


#include "jmMapBinaryLoader.h"
#include "jmCommon.h"
#include "jmCommonFunctions.h"
#include "jmSourceGraphics.h"
#include "jmSprites.h"
#include "jmLayers.h"
#include "jmMap.h"
#include "jmApp.h"
#include "jmScene.h"
#include "jmVectorShapes.h"
#include "jmVectorShapesUtilities.h"
#include "jmColor.h"
#include "jmFont.h"
#include "jmGuiText.h"
#include "jmGuiTextWidgets.h"
#include "jmGuiCommon.h"
#include "jmGuiWidgetsA.h"
#include "jmDrawing.h"
#include "jmCollision.h"
#include "jmFrameAnimation.h"
#include "jmAnimationCommon.h"
#include "jmTimelineAnimation.h"
#include "jmTimelineAnimationInstance.h"
#include "jmObjectFactory.h"
#include "jmCamera.h"
#include "jmGlobal.h"
#include "jmUtilities.h"
#include "jmStreams.h"
#include "jmInput.h"


namespace jugimap
{









// DoxyGen

/// \defgroup MapLoaders

/// \defgroup MapElements

/// \defgroup Cameras

/// \defgroup Animation

/// \defgroup Globals

/// \defgroup SearchFunctions

/// \defgroup Common

/// \defgroup CommonFunctions

/// \defgroup Streams

/// \defgroup Gui

/// \defgroup Input

/// \defgroup EngineExtension_Cocos2d-x
///
/// ### Cocos2d-x nodes ownership
/// Jugimap extended classes which wrap Cocos2d-x node objects does **not** take ownership over them.
/// When deleting a jugimap scene first delete its Cocos2d scene node via
/// Cocos2d director. For deleting jugimap sprites or text objects use dedicated layer functions which also
/// remove related Cocos2d-x nodes.
///

/// \defgroup EngineExtension_SFML

/// \defgroup EngineExtension_AGK-tier2

/// \defgroup EngineExtension_nCine
///
/// ### nCine nodes ownership
/// Jugimap extended classes which wrap nCIne node objects does **not** take ownership over them. The exception are Scene objects.
/// For deleting jugimap sprites or text objects use dedicated layer functions which also
/// delete related nCine nodes.
///








}



#endif
