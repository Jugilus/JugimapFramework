#ifndef JUGIMAP_API_H
#define JUGIMAP_API_H


#include "jmBinaryLoader.h"
#include "jmCommon.h"
#include "jmCommonFunctions.h"
#include "jmSourceGraphics.h"
#include "jmSprites.h"
#include "jmLayers.h"
#include "jmMap.h"
#include "jmScene.h"
#include "jmSceneManager.h"
#include "jmVectorShapes.h"
#include "jmVectorShapesUtilities.h"
#include "jmText.h"
#include "jmDrawing.h"
#include "jmCollision.h"
#include "jmFrameAnimation.h"
#include "jmObjectFactory.h"
#include "jmTween.h"
#include "jmCamera.h"
#include "jmGlobal.h"
#include "jmUtilities.h"


namespace jugimap
{









// DoxyGen

/// \defgroup Loaders

/// \defgroup MapElements

/// \defgroup Cameras

/// \defgroup Animation

/// \defgroup Globals

/// \defgroup SearchFunctions

/// \defgroup Common

/// \defgroup CommonFunctions

/// \defgroup EngineExtension_Cocos2d-x
///
/// ### Cocos2d-x nodes ownership
/// Jugimap extended classes which wrap Cocos2d-x node objects does **not** take ownership over them.
/// When deleting a jugimap scene first delete its Cocos2d scene node via
/// Cocos2d director. For deleting sprites or text objects use dedicated layer functions which also
/// remove related Cocos2d-x nodes.
///

/// \defgroup EngineExtension_SFML

/// \defgroup EngineExtension_AGK-tier2

/// \defgroup EngineExtension_nCine
///
/// ### nCine nodes ownership
/// Jugimap extended classes which wrap nCIne node objects does **not** take ownership over them. The exception are Scene objects.
/// For deleting sprites or text objects use dedicated layer functions which also
/// delete related nCine nodes.
///







}



#endif
