#ifndef JUGIMAP_DRAWING_H
#define JUGIMAP_DRAWING_H


#include "jmLayers.h"



namespace jugimap {



class DrawingLayer;


///\ingroup MapElements
///\brief The base drawer class.
///
/// A Drawer object draws geometric primitives on a drawing layer.
class Drawer
{
public:

    ///\brief  Destructor.
    virtual ~Drawer() {}


    ///\brief  Initialize engine objects related to this drawer.
    ///
    /// This function should usually not be used manually as it gets called in the DrawingLayer::InitEngineLayer() function.
    virtual void InitEngineDrawer() {}


    ///\brief  Update engine objects related to this drawer.
    ///
    /// This function should usually not be used manually as it gets called in the DrawingLayer::UpdateEngineLayer() function.
    virtual void UpdateEngineDrawer() {}


    ///\brief Set outline color to the given *_outlineColor*.
    virtual void SetOutlineColor(ColorRGBA _outlineColor){}


    ///\brief Clear this drawer (required by some engines).
    virtual void Clear(){}


    ///\brief Draw a line from the given position *p1* to *p2*.
    virtual void Line(jugimap::Vec2f p1, jugimap::Vec2f p2) {}


    ///\brief Draw the outline of the given rectangle *rect*.
    virtual void RectangleOutline(const jugimap::Rectf &rect) {}

    ///\brief Draw the outline of an ellipse defined by the given center point *c* and radius *r*.
    virtual void EllipseOutline(jugimap::Vec2f c, jugimap::Vec2f r) {}


    ///\brief Draw a dot at the given position *p*.
    virtual void Dot(jugimap::Vec2f p) {}


    ///\brief Set the drawing layer of this drawer.
    void SetDrawingLayer(DrawingLayer* _drawingLayer){ drawingLayer = _drawingLayer; }


    ///\brief Returns the drawing layer of this drawer.
    DrawingLayer* GetDrawingLayer(){ return drawingLayer; }


private:
    DrawingLayer* drawingLayer = nullptr;               // LINK
};



///\ingroup MapElements
///\brief A layer for drawing geometric primitives.
///
/// We use drawing layers by extending the DrawingLayer and reimplement the DrawingLayer::DrawEngineLayer function.
class DrawingLayer : public Layer
{
public:


    ///\brief  Constructor.
    DrawingLayer(const std::string &_name);

    ///\brief  Destructor.
    virtual ~DrawingLayer() override;

    virtual void InitEngineLayer() override;
    virtual void UpdateEngineLayer() override;


    ///\brief  Set the drawer of this drawing layer to the given *_drawer*.
    ///
    /// **Important:** This drawing layer will take over the ownership of the set drawer!
    void SetDrawer(Drawer* _drawer){ drawer = _drawer; drawer->SetDrawingLayer(this);}


    ///\brief  Returns the drawer of this drawing layer.
    Drawer* GetDrawer() { return drawer;}


protected:
    Drawer* drawer = nullptr;                 // OWNED

};



}


#endif
