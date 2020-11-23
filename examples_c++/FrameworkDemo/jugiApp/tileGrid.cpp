#include "tileGrid.h"


using namespace jugimap;


namespace jugiApp{



TileGrid::TileGrid(Vec2i _nTiles, Vec2i _tileSize)
{
    Rebuild(_nTiles,_tileSize);

}


TileGrid::~TileGrid()
{
    DeleteGrid();
}



void TileGrid::Rebuild(Vec2i _nTiles, Vec2i _tileSize)
{

    if(nTiles != _nTiles){

        DeleteGrid();

        nTiles = _nTiles;

        tiles = new Sprite **[nTiles.x];
        for(int ix=0; ix<nTiles.x; ix++){
            tiles[ix] = new Sprite *[nTiles.y];
        }
    }

    tileSize = _tileSize;
    ClearSprites();

}


void TileGrid::DeleteGrid()
{
    if(tiles==nullptr) return;

    for(int xt=0; xt<nTiles.x; xt++){
        delete[] tiles[xt];
    }
    delete[] tiles;

}


void TileGrid::AddSprite(Sprite *sprite)
{
    int xt = sprite->GetPosition().x/tileSize.x;
    int yt = sprite->GetPosition().y/tileSize.y;

    if(xt<0 || xt>=nTiles.x || yt<0 || yt>=nTiles.y) return;

    tiles[xt][yt] = sprite;
}


Sprite *TileGrid::GetSprite(Vec2f worldPosition)
{

    int xt = worldPosition.x/tileSize.x;
    int yt = worldPosition.y/tileSize.y;

    if(xt<0 || xt>=nTiles.x || yt<0 || yt>=nTiles.y) return nullptr;

    return tiles[xt][yt];

}


bool TileGrid::PointInTile(Vec2f worldPosition)
{
    int xt = worldPosition.x/tileSize.x;
    int yt = worldPosition.y/tileSize.y;

    if(xt<0 || xt>=nTiles.x || yt<0 || yt>=nTiles.y) return false;

    Sprite  *sprite = tiles[xt][yt];

    if(sprite){
        //bool inside = sprite->PointInside(worldPosition);
        return sprite->PointInside(worldPosition);
    }

    return false;
}


void TileGrid::ClearSprites()
{
    for(int xt=0; xt<nTiles.x; xt++){
        for(int yt=0; yt<nTiles.y; yt++){
            tiles[xt][yt] = nullptr;
        }
    }

}

//---------------------------------------------------------------------------------------

void BuildCollidersTileGrid(Map *_map)
{

    collidersTileGrid.Rebuild(_map->GetTilesCount(), _map->GetTileSize());

    for(Layer *l : _map->GetLayers()){
        if(l->GetKind()==LayerKind::SPRITE){
            SpriteLayer * sl = static_cast<SpriteLayer*>(l);

            if(Parameter::Exists(sl->GetParameters(), "blockingColliderTiles")){
                for(Sprite * s : sl->GetSprites()){
                    if(s->GetKind()==SpriteKind::STANDARD){
                        if(s->HasCollider()){
                            collidersTileGrid.AddSprite(s);
                        }
                    }
                }
            }
        }
    }

}


TileGrid collidersTileGrid;


}


