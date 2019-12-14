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


/// \brief Collect sprites which contain the given parameter.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithParameter(Map *map, std::vector<jugimap::Sprite *> &collectedSprites,  const std::string &pName, const std::string &pValue="");


/// \brief Collect sprites which contain the given dataFlags.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param dataFlags The searched dataFlag.
/// \param compareDataFlagsAsBitmask If *true* the dataFlags will be compared bitwise; if *false* it will be compared as integer.
void CollectSpritesWithDataFlags(Map *map, std::vector<Sprite*>&collectedSprites,  int dataFlags, bool compareDataFlagsAsBitmask);


/// \brief Collect sprites which have the source sprite with the given name.
///
/// \param map The map where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param name The searched name.
void CollectSpritesWithName(Map *map, std::vector<Sprite*>&collectedSprites,  const std::string &name);


/// \brief Collect sprites which contain the given parameter.
///
/// \param layer The layer where we collect the sprites.
/// \param collectedSprites A reference to a vector where the collected sprites are stored.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
void CollectSpritesWithParameter(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites, const std::string &pName, const std::string &pValue="");


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
void CollectSpritesWithName(SpriteLayer *layer, std::vector<Sprite*>&collectedSprites,  const std::string &name);


//--- FIND VECTOR SHAPE


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
VectorShape* FindVectorShapeWithProperties(Map *map, int probe, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);


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
VectorShape* FindVectorShapeWithProperties(VectorLayer * vectorLayer, int probe, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);


/// \brief Find a vector shape with the given parameters.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param vectorShapes A reference to stored vector shapes where we search for the vector shape.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
/// /// \param pValue The searched value of the parameter. If the empty string *""* is given the function searches for the name only.
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
VectorShape* FindVectorShapeWithProperties(std::vector<VectorShape *>&vectorShapes, int probe, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);


/// \brief Find a probe vector shape within the *probe shapes* with the given properties.
///
/// The function returns the first found vector shape; if none is found it returns nullptr.
/// \param standardSprite The standard sprite where we search for the probe vector shape.
/// \param dataFlags The *dataFlags* of the vector shape. Use -1 to skip comparing by the *dataFlags*.
/// \param compareDataFlagsAsBitmask If *true* the *dataFlags* will be compared bitwise; if *false* it will be compared as integer.
/// \param kind The *kind* of the vector shape. Use ShapeKind::NOT_DEFINED to skip comparing by the *shape*.
VectorShape* FindProbeShapeWithProperties(StandardSprite * standardSprite, int dataFlags, bool compareDataFlagsAsBitmask, ShapeKind kind = ShapeKind::NOT_DEFINED);



//--- FIND SOURCE SPRITE

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
/// The function returns the searched frame animation if is found; if it is not found it returns nullptr.
/// \param standardSprite The standard sprite which we search for the frame animation.
/// \param name The searched name of the frame animation.
FrameAnimation* FindFrameAnimationWithName(StandardSprite * standardSprite, const std::string &name);


/// \brief Find a frame animation with the given name.
///
/// The function returns the searched frame animation if is found; if it is not found it returns nullptr.
/// \param frameAnimations A reference to stored frameAnimations where we search for the frame animation.
/// \param name The searched name of the frame animation.
FrameAnimation* FindFrameAnimationWithName(std::vector<FrameAnimation*>&frameAnimations, const std::string &name);


/// \brief Find a frame animation with the given parameter.
///
/// The function returns the searched frame animation if is found; if it is not found it returns nullptr.
/// \param frameAnimations A reference to stored frameAnimations where we search for the frame animation.
/// \param pName The searched name of the parameter.
/// \param pValue The searched value of the parameter. If it is not specified the function searches for the name only.
FrameAnimation* FindFrameAnimationWithParameter(std::vector<FrameAnimation *> &frameAnimations, const std::string &pName, const std::string &pValue="");


/// @}




}

#endif


