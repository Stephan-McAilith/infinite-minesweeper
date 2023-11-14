#include "Map.hpp"
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

Map::Map()
{
    srand(time(NULL));

    create_new_chunk(sf::Vector2i(0, 0));
}

Map::~Map()
{
}

int Map::get_chunk_id(sf::Vector2i coord)
{
    //get the chunk id from its coordinates
    for (auto it = _ids.begin(); it != _ids.end(); ++it) {
        if (coord == std::get<0>(*it))
            return (std::get<1>(*it));
    }
    return (-1);
}

float Map::dist(sf::Vector2i vec1, sf::Vector2i vec2) {
    //calcul euclidian distance between 2 vectors
    sf::Vector2i tmp = vec1 - vec2;
    return sqrt(pow(tmp.x, 2) + pow(tmp.y, 2));
}


bool Map::reveal(sf::Vector2i chunk, sf::Vector2i coord) {
    //search for the chunked to reveal the tile in
    int id = get_chunk_id(chunk);

    // chunk not found
    if (_chunks.find(id) == _chunks.end())
        return false;
    for (int x = -1; x < 2; x += 1) {
        for (int y = -1; y < 2; y += 1) {
            if (get_chunk_id(sf::Vector2i(x + chunk.x, y + chunk.y)) == -1)
                create_new_chunk(sf::Vector2i(x + chunk.x, y + chunk.y));
        }
    }
    return _chunks[id]->reveal(coord.x, coord.y);
}


unsigned char Map::get_tile(sf::Vector2i chunk, sf::Vector2i coord) {
    //search for the chunked to reveal the tile in
    int id = get_chunk_id(chunk);

    // chunk not found
    if (_chunks.find(id) == _chunks.end())
        return false;
    return _chunks[id]->get_tile(coord);
}


bool Map::click(sf::Vector2f target, sf::Mouse::Button button)
{
    sf::Vector2i chunk_coord, tiles_coord;
    // convert coord
    chunk_coord.x = target.x < 0 ? (int)target.x / CHUNK_PX_SIZE - 1 : (int)target.x / CHUNK_PX_SIZE;
    chunk_coord.y = target.y < 0 ? (int)target.y / CHUNK_PX_SIZE - 1 : (int)target.y / CHUNK_PX_SIZE;
    tiles_coord.x = (target.x < 0 ? (int)target.x % CHUNK_PX_SIZE + CHUNK_PX_SIZE : (int)target.x % CHUNK_PX_SIZE) / TILE_PX_SIZE;
    tiles_coord.y = (target.y < 0 ? (int)target.y % CHUNK_PX_SIZE + CHUNK_PX_SIZE : (int)target.y % CHUNK_PX_SIZE) / TILE_PX_SIZE;


    //search for the clicked chunked
    int id = get_chunk_id(chunk_coord);
    // chunk not found
    if (_chunks.find(id) == _chunks.end())
        return false;
    for (int x = -1; x < 2; x += 1) {
        for (int y = -1; y < 2; y += 1) {
            if (get_chunk_id(sf::Vector2i(x + chunk_coord.x, y + chunk_coord.y)) == -1)
                create_new_chunk(sf::Vector2i(x + chunk_coord.x, y + chunk_coord.y));
        }
    }
    return _chunks[id]->click(tiles_coord, button);
}


void Map::load_chunk_in_generator(int id, int x, int y)
{
    if (_chunks.find(id) == _chunks.end())
        return;
    // load tiles with an offset
    auto& tiles = _chunks[id]->_tiles;
    for (int i = 0; i < CHUNK_SIZE; i += 1) {
        for (int j = 0; j < CHUNK_SIZE; j += 1) {
            _generator[i + x][j + y] = tiles[i + j * CHUNK_SIZE];
        }
    }
}

void Map::update_tiles(int id, int x, int y)
{
    if (_chunks.find(id) == _chunks.end())
        return;
    // update tiles in chunk
    for (int i = 0; i < CHUNK_SIZE; i += 1) {
        for (int j = 0; j < CHUNK_SIZE; j += 1) {
            _chunks[id]->_tiles[i + j * CHUNK_SIZE] = _generator[i + x][j + y];
        }
    }
}


void Map::create_new_chunk(sf::Vector2i coord) {
    //reset generator map
    for (int i = 0; i < CHUNK_SIZE * 3; i += 1) {
        for (int j = 0; j < CHUNK_SIZE * 3; j += 1)
            _generator[i][j] = 0;
    }

    //create new chunk
    int new_id = _ids.size();

    _chunks.emplace(new_id, new Chunk(new_id, coord, [&](sf::Vector2i chunk, sf::Vector2i coord2) {return this->reveal(chunk, coord2); },
        [&](sf::Vector2i chunk, sf::Vector2i coord2) {return this->get_tile(chunk, coord2); }));
    _ids.push_back(std::make_tuple(coord, new_id));

    //load the chunk's tiles in the map generator
    for (int x = -1; x < 2; x += 1) {
        for (int y = -1; y < 2; y += 1) {
            int id = get_chunk_id(sf::Vector2i(x + coord.x, y + coord.y));
            load_chunk_in_generator(id, (x + 1) * CHUNK_SIZE, (y + 1) * CHUNK_SIZE);
        }
    }
    if (new_id == 0)
        return;
    for (int i = 0; i < MINE_PER_CHUNK; i += 1) {
        int x = std::rand() % CHUNK_SIZE + CHUNK_SIZE;
        int y = std::rand() % CHUNK_SIZE + CHUNK_SIZE;
        //if already is a mine
        if ((_generator[x][y] & MINE) != 0) {
            i -= 1;
            continue;
        }
        _generator[x][y] |= MINE;
    }

    //count and update mines indicator
    for (int i = CHUNK_SIZE - 1; i <= CHUNK_SIZE * 2; i += 1) {
        for (int j = CHUNK_SIZE - 1; j <= CHUNK_SIZE * 2; j += 1) {
            _generator[i][j] &= 0b11110000;
            for (int x = -1; x < 2; x += 1) {
                for (int y = -1; y < 2; y += 1) {
                    if ((_generator[x + i][y + j] & MINE) != 0)
                        _generator[i][j] += 1;
                }
            }
        }
    }

    // update tiles in chunks
    for (int x = -1; x < 2; x += 1) {
        for (int y = -1; y < 2; y += 1) {
            int id = get_chunk_id(sf::Vector2i(x + coord.x, y + coord.y));
            update_tiles(id, (x + 1) * CHUNK_SIZE, (y + 1) * CHUNK_SIZE);
        }
    }
}

void Map::save(std::ofstream& file)
{
    int size = _ids.size();
    file.write((char*)&size, 4);

    for (auto it = _ids.begin(); it != _ids.end(); ++it) {
        file.write((char*)&(std::get<0>(*it).x), 4);
        file.write((char*)&(std::get<0>(*it).y), 4);
    }
    for (auto it = _chunks.begin(); it != _chunks.end(); it++) {
        file.write((char*)(*it).second->_tiles, CHUNK_NB_TILES);
    }
}

void Map::load(std::ifstream& file)
{
    int size;
    int x, y;
    file.read((char*)&size, 4);
    _chunks.clear();
    _ids.clear();
    for (int i = 0; i < size; i += 1) {
        file.read((char*)&x, 4);
        file.read((char*)&y, 4);
        _ids.push_back(std::make_tuple(sf::Vector2i(x, y), i));
    }
    for (int i = 0; i < size; i += 1) {
        _chunks.emplace(i, new Chunk(i, std::get<0>(_ids[i]), [&](sf::Vector2i chunk, sf::Vector2i coord2) {return this->reveal(chunk, coord2); },
            [&](sf::Vector2i chunk, sf::Vector2i coord2) {return this->get_tile(chunk, coord2); }));
        file.read((char*)_chunks[i]->_tiles, CHUNK_NB_TILES);
    }
}
