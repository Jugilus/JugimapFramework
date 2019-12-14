#include "jmObjectFactory.h"
#include "jmDrawing.h"



namespace jugimap {



DrawingLayer::DrawingLayer(const std::string &_name)
{
    _SetName(_name);
    _SetKind(LayerKind::DRAWING);
    drawer = objectFactory->NewDrawer();
    drawer->SetDrawingLayer(this);

}


DrawingLayer::~DrawingLayer()
{
    if(drawer){
        delete drawer;
    }

}



void DrawingLayer::InitEngineLayer()
{
    drawer->InitEngineDrawer();
}


void DrawingLayer::UpdateEngineLayer()
{
    drawer->UpdateEngineDrawer();
}



}



