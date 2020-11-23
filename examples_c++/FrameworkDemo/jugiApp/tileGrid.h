#ifndef API_DEMO_TEST__TILE_GRID_H
#define API_DEMO_TEST__TILE_GRID_H


#include "../jugimap/jugimap.h"


namespace jugiApp{


class TileGrid
{
public:

    TileGrid(){}
    TileGrid(jugimap::Vec2i _nTiles, jugimap::Vec2i _tileSize);
    ~TileGrid();

    void Rebuild(jugimap::Vec2i _nTiles, jugimap::Vec2i _tileSize);
    void DeleteGrid();

    jugimap::Vec2i GetNTiles(){return nTiles;}
    jugimap::Vec2i GetTileSize(){return tileSize;}

    void AddSprite(jugimap::Sprite  *sprite);
    jugimap::Sprite * GetSprite(jugimap::Vec2f worldPosition);
    bool PointInTile(jugimap::Vec2f worldPosition);

    void ClearSprites();


private:
    jugimap::Sprite  ***tiles;               // SPRITE LINKS in OWNED array of arrays
    jugimap::Vec2i nTiles;
    jugimap::Vec2i tileSize;
};


void BuildCollidersTileGrid(jugimap::Map *_map);


extern TileGrid collidersTileGrid;



}




#endif // JUGITILEMAP_AGK_H
