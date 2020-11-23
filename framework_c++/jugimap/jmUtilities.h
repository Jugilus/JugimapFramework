#ifndef JUGIMAP_UTILITIES_H
#define JUGIMAP_UTILITIES_H

#include <cmath>
#include <vector>
#include <string>
#include "jmCommon.h"
#include "jmObjectFactory.h"



namespace jugimap {


class Layer;
class Sprite;
class VectorShape;
class FrameAnimation;
class TimelineAnimation;


/// \addtogroup SearchFunctions
/// @{


/// \brief Collect layers which contain the given parameter.
///
/// \param map The map where we collect the layers.
/// \param collectedLayers A reference to a vector where the collected layers are stored .
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
/// \param layerKind The required layer kind. If it is not specified the function collects among all layers.
void CollectLayersWithParameter(Map *map, std::vector<jugimap::Layer *> &collectedLayers, const std::string &pName, const std::string &pValue="", LayerKind layerKind=LayerKind::NOT_DEFINED);


/// \brief Find a layer with the given name.
///
/// The function returns the searched layer if is found; if it is not found it returns nullptr.
/// \param map The map where we search for the layer.
/// \param name The searched name of the layer.
/// \param layerKind The required layer kind. If it is not specified the function searches among all layers.
Layer* FindLayerWithName(Map *map, const std::string &name, LayerKind layerKind=LayerKind::NOT_DEFINED);


//--- COLLECT SPRITES


/// \brief Collect sprites with the given id.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param id The searched id.
void CollectSpritesWithId(Map *map, std::vector<Sprite*>&collectedSprites,  int id);


/// \brief Collect child sprites of the given composed sprite with the given id.
///
/// \param composedSprite The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param id The searched id.
void CollectSpritesWithId(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  int id);


/// \brief Collect sprites with the given name.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesWithName(Map *map, std::vector<Sprite*>&collectedSprites,  const std::string &name);


/// \brief Collect child sprites of the given composed sprite for the given name.
///
/// \param composedSprite The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesWithName(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  const std::string &name);


/// \brief Collect sprites for the given kind.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesOfKind(Map *map, std::vector<Sprite*>&collectedSprites,  SpriteKind kind);


/// \brief Collect child sprites of the given composed sprite for the given kind.
///
/// \param composedSprite The composed sprite in which we seek the child sprites. The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesOfKind(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  SpriteKind kind);


/// \brief Collect sprites which contain the given parameter.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithParameter(Map *map, std::vector<jugimap::Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue="");


/// \brief Collect child sprites of the given composed sprite which contain the given parameter.
///
/// \param composedSprite The composed sprite in which we seek the child sprites. The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithParameter(ComposedSprite *composedSprite, std::vector<jugimap::Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue="");


/// \brief Collect sprites with a source sprite containing the given constant parameter.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithConstParameter(Map *map, std::vector<jugimap::Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue="");


/// \brief Collect child sprites of the given composed sprite which contain the given constant parameter.
///
/// \param composedSprite The composed sprite in which we seek the child sprites. The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithConstParameter(ComposedSprite *composedSprite, std::vector<jugimap::Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue="");


/// \brief Collect sprites with an original source sprite containing the given constant parameter.
///
/// This function is applicable for sprites which have replaced source sprite to access original source sprite parameters.
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithOrigConstParameter(Map *map, std::vector<jugimap::Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue="");


/// \brief Collect child sprites of the given composed sprite with an original source sprite containing the given constant parameter.
///
/// This function is applicable for sprites which have replaced source sprite to access original source sprite parameters.
/// \param composedSprite The composed sprite in which we seek the child sprites. The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithOrigConstParameter(ComposedSprite *composedSprite, std::vector<jugimap::Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue="");


/// \brief Collect sprites which contain the given dataFlags.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param dataFlags The searched dataFlag.
/// \param compareDataFlagsAsBitmask If *true* the dataFlags will be compared bitwise; if *false* it will be compared as integer.
void CollectSpritesWithDataFlags(Map *map, std::vector<Sprite*>&collectedSprites,  int dataFlags, bool compareDataFlagsAsBitmask);


/// \brief Collect child sprites of the given composed sprite which contain the given dataFlags.
///
/// \param composedSprite The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param dataFlags The searched dataFlag.
/// \param compareDataFlagsAsBitmask If *true* the dataFlags will be compared bitwise; if *false* it will be compared as integer.
void CollectSpritesWithDataFlags(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  int dataFlags, bool compareDataFlagsAsBitmask);


/// \brief Collect sprites which have the source sprite with the given name.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesWithSourceSpriteName(Map *map, std::vector<Sprite*>&collectedSprites,  const std::string &name);


/// \brief Collect child sprites of the given composed sprite with the given name.
///
/// \param composedSprite The composed sprite in which we seek the child sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesWithSourceSpriteName(ComposedSprite *composedSprite, std::vector<Sprite*>&collectedSprites,  const std::string &name);

//--------------------------------------


/// \brief Collect sprites with the given id.
///
/// \param layer The sprite layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param id The searched id.
void CollectSpritesWithId(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  int id);


/// \brief Collect sprites with the given name.
///
/// \param layer The sprite layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesWithName(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  const std::string &name);


/// \brief Collect child sprites of the given composed sprite for the given kind.
///
/// \param layer The sprite layer where we seek the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesOfKind(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  SpriteKind kind);



/// \brief Collect sprites which contain the given parameter.
///
/// \param layer The layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithParameter(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, const std::string &pName, const std::string &pValue="");


/// \brief Collect sprites with a source sprite containing the given constant parameter.
///
/// \param layer The layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithConstParameter(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, const std::string &pName, const std::string &pValue="");


/// \brief Collect sprites with an original source sprite containing the given constant parameter.
///
/// This function is applicable for sprites which have replaced source sprite to access original source sprite parameters.
/// \param layer The layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithOrigConstParameter(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, const std::string &pName, const std::string &pValue="");



/// \brief Collect sprites which contain the given dataFlags.
///
/// \param layer The layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param dataFlags The searched dataFlag.
/// \param compareDataFlagsAsBitmask If *true* the dataFlags will be compared bitwise; if *false* it will be compared as integer.
void CollectSpritesWithDataFlags(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  int dataFlags, bool compareDataFlagsAsBitmask);


/// \brief Collect sprites which have the source sprite with the given name.
///
/// \param layer The sprite layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesWithSourceSpriteName(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  const std::string &name);


//--- FIND VECTOR SHAPE


/// \brief Find a vector shape with the given *name*.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param map The map where we search for the vector shape.
/// \param name The searched name of the vector shape.
VectorShape* FindVectorShapeWithName(Map *map, const std::string &name);


/// \brief Find a vector shape with the given parameters.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param map The map where we search for the vector shape.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If the empty string *""* is given the function searches for the name only.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindVectorShapeWithParameter(Map *map, const std::string &pName, const std::string &pValue, ShapeKind kind = ShapeKind::NOT_DEFINED);



/// \brief Find a vector shape with the given properties.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param map The map where we search for the vector shape.
/// \param probe The *probe* flag of a vector shape. Use 1 if the probe is *true*; use 0 if the probe is *false*;
///              Use -1 to skip comparing by the *probe*.
/// \param dataFlags The *dataFlags* of the vector shape. Use -1 to skip comparing by the *dataFlags*.
/// \param compareDataFlagsAsBitmask If *true* the *dataFlags* will be compared bitwise; if *false* it will be compared as integer.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindVectorShapeWithProperties(Map *map, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);


/// \brief Find a vector shape with the given *name*.
///
/// The function returns the searched vector shape if is found; if it is not found it returns nullptr.
/// \param vectorLayer The vector layer where we search for the vector shape.
/// \param name The searched name of the vector shape.
VectorShape* FindVectorShapeWithName(VectorLayer * vectorLayer, const std::string &name);


/// \brief Find a vector shape with the given parameters.
///
/// The function returns the searched vector shape if is found; if it is not found it returns nullptr.
/// \param vectorLayer The vector layer where we search for the vector shape.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If the empty string *""* is given the function searches for the name only.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindVectorShapeWithParameter(VectorLayer * vectorLayer, const std::string &pName, const std::string &pValue, ShapeKind kind = ShapeKind::NOT_DEFINED);


/// \brief Find a vector shape with the given properties.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param vectorLayer A vector layer where we search for the vector shape.
/// \param probe The *probe* of the vector shape. Use 1 if the probe is *true*; use 0 if the probe is *false*;
///              Use -1 to skip comparing by the *probe*.
/// \param dataFlags The *dataFlags* of the vector shape. Use -1 to skip comparing by the *dataFlags*.
/// \param compareDataFlagsAsBitmask If *true* the *dataFlags* will be compared bitwise; if *false* it will be compared as integer.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindVectorShapeWithProperties(VectorLayer * vectorLayer, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);


/// \brief Find a vector shape with the given *name*.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param vectorShapes A reference to stored vector shapes where we search for the vector shape.
/// \param name The searched name of the vector shape.
VectorShape* FindVectorShapeWithName(std::vector<VectorShape*>&vectorShapes, const std::string &name);


/// \brief Find a vector shape with the given parameters.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param vectorShapes A reference to stored vector shapes where we search for the vector shape.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindVectorShapeWithParameter(std::vector<VectorShape*>&vectorShapes, const std::string &pName, const std::string &pValue, ShapeKind kind = ShapeKind::NOT_DEFINED);


/// \brief Find a vector shape with the given properties.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param vectorShapes A reference to the vector shapes where we search for the vector shape.
/// \param probe The *probe* of the vector shape. Use 1 if the probe is *true*; use 0 if the probe is *false*;
///              Use -1 to skip comparing by the *probe*.
/// \param dataFlags The *dataFlags* of the vector shape. Use -1 to skip comparing by the *dataFlags*.
/// \param compareDataFlagsAsBitmask If *true* the *dataFlags* will be compared bitwise; if *false* it will be compared as integer.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindVectorShapeWithProperties(std::vector<VectorShape *>&vectorShapes, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);


/*
/// \brief Find a probe vector shape within the *probe shapes* with the given properties.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param standardSprite The standard sprite where we search for the probe vector shape.
/// \param dataFlags The *dataFlags* of the vector shape. Use -1 to skip comparing by the *dataFlags*.
/// \param compareDataFlagsAsBitmask If *true* the *dataFlags* will be compared bitwise; if *false* it will be compared as integer.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindSpriteExtraShapeWithProperties(StandardSprite * standardSprite, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);
*/


/// \brief Find a sprite extra vector shape with the given properties.
///
/// The function returns the first vector shape it founds; if none is found it returns nullptr.
/// \param graphicItem The graphic item of a standard sprite where we search for the shape.
/// \param dataFlags The *dataFlags* of the vector shape. Use -1 to skip comparing by the *dataFlags*.
/// \param compareDataFlagsAsBitmask If *true* the *dataFlags* will be compared bitwise; if *false* it will be compared as integer.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindSpriteExtraShapeWithProperties(GraphicItem * graphicItem, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);


/// \brief Find a sprite collision vector shape with the given properties.
///
/// The function returns the first vector shape it founds; if none is found it returns nullptr.
/// \param graphicItem The graphic item of a standard sprite where we search for the shape.
/// \param dataFlags The *dataFlags* of the vector shape. Use -1 to skip comparing by the *dataFlags*.
/// \param compareDataFlagsAsBitmask If *true* the *dataFlags* will be compared bitwise; if *false* it will be compared as integer.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindSpriteCollisionShapeWithProperties(GraphicItem * graphicItem, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);



//--- SOURCE SPRITES


/// \brief Collect source sprites which contain the given constant parameter.
///
/// \param sourceGraphics The SourceGraphics object where we search for the source sprites.
/// \param collectedSprites A reference to a vector where the collected source sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSourceSpritesWithConstParameter(SourceGraphics *sourceGraphics, std::vector<SourceSprite*>&collectedSourceSprites, const std::string &pName, const std::string &pValue="");


/// \brief Find a source sprite with the given name.
///
/// The function returns the searched source sprite if is found; if it is not found it returns nullptr.
/// \param sourceGraphics The SourceGraphics object where we search for the source sprite.
/// \param name The searched name of the source sprite.
SourceSprite* FindSourceSpriteWithName(SourceGraphics *sourceGraphics, const std::string &name);


/// \brief Find a source sprite which contain a graphic item (image) with the given file path.
///
/// The function returns the first found source sprite; if none is found it returns nullptr.
/// \param sourceGraphics The SourceGraphics object where we search for the source sprite.
/// \param filePath The searched file path.
SourceSprite* FindSourceSpriteByGraphicFilePath(SourceGraphics *sourceGraphics, const std::string &filePath);






//--- FIND ANIMATIONS


/// \brief Find a frame animation with the given name.
///
/// The function returns the searched frame animation if it is found; if it is not found it returns nullptr.
/// \param standardSprite The standard sprite which we search for the frame animation.
/// \param name The searched name of the frame animation.
FrameAnimation* FindFrameAnimationWithName(StandardSprite * standardSprite, const std::string &name);


/// \brief Find a frame animation with the given name.
///
/// The function returns the searched frame animation if it is found; if it is not found it returns nullptr.
/// \param animations A reference to stored frameAnimations where we search for the frame animation.
/// \param name The searched name of the frame animation.
FrameAnimation* FindFrameAnimationWithName(std::vector<FrameAnimation*>&animations, const std::string &name);


/// \brief Find a frame animation with the given parameter.
///
/// The function returns the searched frame animation if it is found; if it is not found it returns nullptr.
/// \param animations A reference to stored frameAnimations where we search for the frame animation.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
FrameAnimation* FindFrameAnimationWithParameter(std::vector<FrameAnimation *> &animations, const std::string &pName, const std::string &pValue="");



/// \brief Find a timeline animation with the given name.
///
/// The function returns the searched timeline animation if it is found; if it is not found it returns nullptr.
/// \param standardSprite The standard sprite which we search for the timeline animation.
/// \param name The searched name of the timeline animation.
TimelineAnimation* FindTimelineAnimationWithName(StandardSprite * standardSprite, const std::string &name);


/// \brief Find a frame animation with the given name.
///
/// The function returns the searched timeline animation if is found; if it is not found it returns nullptr.
/// \param animations A reference to stored frameAnimations where we search for the timeline animation.
/// \param name The searched name of the timeline animation.
TimelineAnimation* FindTimelineAnimationWithName(std::vector<TimelineAnimation*>&animations, const std::string &name);


/// \brief Find a timeline animation with the given parameter.
///
/// The function returns the searched timeline animation if is found; if it is not found it returns nullptr.
/// \param animations A reference to stored frameAnimations where we search for the timeline animation.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
TimelineAnimation* FindTimelineAnimationWithParameter(std::vector<TimelineAnimation *> &animations, const std::string &pName, const std::string &pValue="");



//---
void GatherSpritesWithSetNameID(ComposedSprite *_composedSprite, std::vector<std::vector<Sprite*>>&spritesPerNameID);









/// @}




}

#endif


