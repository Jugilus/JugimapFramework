#ifndef API_DEMO_TEST__SHAPES_DRAWING_LAYER_H
#define API_DEMO_TEST__SHAPES_DRAWING_LAYER_H


#include "../jugimap/jugimap.h"




namespace apiTestDemo{



class ShapesDrawingLayer : public jugimap::DrawingLayer
{
public:

    ShapesDrawingLayer(const std::string &_name, jugimap::Map *_map);

    void SetDoDrawMapSpritesCollisionShapes(bool _value){ doDrawMapSpritesCollisionShapes = _value; }
    void SetDoDrawMapVectorShapes(bool _value){ doDrawMapVectorShapes = _value; }

    void DrawEngineLayer() override;


private:

    jugimap::Map *map = nullptr;                            // LINK

    bool doDrawMapSpritesCollisionShapes = false;
    bool doDrawMapVectorShapes = false;

    jugimap::ColorRGBA colorDrawVectorShape;
    jugimap::ColorRGBA colorDrawMouseOverSpriteShape;

    void DrawMapVectorShapes();
    void DrawMapSpritesCollisionShapes();

    jugimap::Sprite *DrawSpriteCollisionShapes(jugimap::Sprite *sprite, int &countDrawn);

};



}



#endif
