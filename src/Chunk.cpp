#include "Chunk.hpp"
#include <iostream>

Chunk::Chunk(int id, sf::Vector2i pos,
    std::function<bool(sf::Vector2i, sf::Vector2i)> reveal_in_chunk,
    std::function<unsigned char(sf::Vector2i, sf::Vector2i)> get_tile_in_chunk)
    : _id(id), _pos(pos), _reveal_in_chunk(reveal_in_chunk), _get_tile_in_chunk(get_tile_in_chunk)
{
    //set tiles to default
    for (int i = 0; i < CHUNK_NB_TILES; i += 1)
        _tiles[i] = 0;

}

Chunk::~Chunk()
{

}

bool Chunk::addMine(unsigned int pos) {
    // if already is a mine return
    if ((_tiles[pos] & MINE) != 0)
        return false;
    //add a mine
    _tiles[pos] |= MINE;

    // add +1 to the mine counter of the tiles arounds
    int x = pos % CHUNK_SIZE;
    int y = (pos - x) / CHUNK_SIZE;
    for (int i = x - 1; i < x + 2; i += 1) {
        for (int j = y - 1; j < y + 2; j += 1) {
            if (i < 0 || i > CHUNK_SIZE -1 || j < 0 || j > CHUNK_SIZE - 1)
                continue;
            _tiles[j * CHUNK_SIZE + i] += 1;
        }
    }
    return true;
}

sf::Vector2i const& Chunk::getPos() {
    return _pos;
}


void Chunk::draw(sf::RenderWindow& window, sf::Sprite(&sprites)[13], bool over) {
    int sp = 0;

    //draw eatch tiles
    for (int i = 0; i < CHUNK_NB_TILES; i += 1) {
        //if not reveal
        if ((_tiles[i] & REVEALED) == 0) {
            sp = 12;
            //if game is over and it's a mine
            if (over && _tiles[i] & MINE)
                sp = 10;
            //if it's a flag
            if (_tiles[i] & FLAG)
                sp = 9;
        }
        //if the mine was click on 
        else if (_tiles[i] & MINE)
            sp = 11;
        //draw number of mines around
        else
            sp = _tiles[i] & MINES_AROUND;
        //set sprite position and draw
        sprites[sp].setPosition(i % CHUNK_SIZE * TILE_PX_SIZE + _pos.x * CHUNK_PX_SIZE, i / CHUNK_SIZE * TILE_PX_SIZE + _pos.y * CHUNK_PX_SIZE);
        window.draw(sprites[sp]);
    }
}

bool Chunk::click(sf::Vector2i pos, sf::Mouse::Button button)
{
    // if right click for flag and not reveal then change flag state
    if (button == sf::Mouse::Right && (_tiles[pos.x + pos.y * CHUNK_SIZE] & REVEALED) == 0) {
        _tiles[pos.x + pos.y * CHUNK_SIZE] ^= FLAG;
        return false;
    }

    //if revealed;
    if ((_tiles[pos.x + pos.y * CHUNK_SIZE] & REVEALED) != 0)
        return check_flag(pos.x, pos.y);
    // else reveal
    return reveal(pos.x, pos.y);
}

bool Chunk::reveal(int x, int y) {
    // if already revealed or flag do nothing
    if ((_tiles[x + y * CHUNK_SIZE] & REVEALED) != 0 || (_tiles[x + y * CHUNK_SIZE] & FLAG) != 0)
        return false;
    //reveal
    _tiles[x + y * CHUNK_SIZE] |= REVEALED;

    // if it's a mine
    if ((_tiles[x + y * CHUNK_SIZE] & MINE) != 0)
        return true;

    if ((_tiles[x + y * CHUNK_SIZE] & MINES_AROUND) != 0)
        return false;

    //if it's a 0 reveal around
    for (int i = x - 1; i < x + 2; i += 1) {
        for (int j = y - 1; j < y + 2; j += 1) {
            if (i < 0 || i > CHUNK_SIZE - 1 || j < 0 || j > CHUNK_SIZE - 1) {
                reveal_in_other_chunk(i, j);
                continue; // reveal in other chunk
            }
            //already revealed
            if ((_tiles[i + j * CHUNK_SIZE] & FLAG) != 0)
                continue;
            // else reveal
            reveal(i, j);
        }
    }
    return false;
}

bool Chunk::check_flag(int x, int y) {
    sf::Vector2i chunk, tile;
    unsigned char flag = 0;
    bool res = false;

    //count number of flag around
    for (int i = x - 1; i < x + 2; i += 1) {
        for (int j = y - 1; j < y + 2; j += 1) {
            if (i < 0 || i > CHUNK_SIZE - 1 || j < 0 || j > CHUNK_SIZE - 1) {
                get_coord_in_other_chunk(i, j, chunk, tile);
                flag += (_get_tile_in_chunk(chunk, tile) & FLAG) >> 6;
            }
            else
                flag += (_tiles[i + j * 16] & FLAG) >> 6;
        }
    }

    //not good number of flag return
    if (flag != (_tiles[x + y * 16] & MINES_AROUND))
        return false;

    // reveal tiles around when enough flags
    for (int i = x - 1; i < x + 2; i += 1) {
        for (int j = y - 1; j < y + 2; j += 1) {
            if (i < 0 || i > CHUNK_SIZE - 1 || j < 0 || j > CHUNK_SIZE - 1)
                res |= reveal_in_other_chunk(i, j);
            else
                res |= reveal(i, j);
        }
    }
    return res;
}

bool Chunk::reveal_in_other_chunk(int x, int y) {
    sf::Vector2i chunk, tile;

    //request to reveal a tile in an other chunk
    get_coord_in_other_chunk(x, y, chunk, tile);
    return _reveal_in_chunk(chunk, tile);
}

void Chunk::get_coord_in_other_chunk(int x, int y, sf::Vector2i& chunk, sf::Vector2i& coord) {
    //calculate the tiles position in an other chunk
    chunk = _pos;
    coord = sf::Vector2i(x, y);

    if (-1 == x) {
        chunk.x -= 1;
        coord.x = CHUNK_SIZE - 1;
    }
    if (16 == x) {
        chunk.x += 1;
        coord.x = 0;
    }
    if (-1 == y) {
        chunk.y -= 1;
        coord.y = CHUNK_SIZE - 1;

    }
    if (16 == y) {
        chunk.y += 1;
        coord.y = 0;
    }
}

unsigned char Chunk::get_tile(sf::Vector2i coord) {
    //return the requested tile
    return _tiles[coord.x + coord.y * CHUNK_SIZE];
}
