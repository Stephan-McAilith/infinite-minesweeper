#include <SFML/Graphics.hpp>
#include "Chunk.hpp"
#include <map>
#include <vector>
#include <tuple>
#include <fstream>

#define MINE_PER_CHUNK 30

class Map {
public:
    Map();
    ~Map();
    void draw(sf::RenderWindow& window, sf::Vector2f camera, bool over, float zoom_level);
    bool click(sf::Vector2f target, sf::Mouse::Button button);
    void save(std::ofstream& file);
    void load(std::ifstream& file);
    static float dist(sf::Vector2i, sf::Vector2i);

private:
    void create_new_chunk(sf::Vector2i coord);
    bool reveal(sf::Vector2i chunk, sf::Vector2i coord);
    unsigned char get_tile(sf::Vector2i chunk, sf::Vector2i coord);
    int get_chunk_id(sf::Vector2i coord);
    void load_chunk_in_generator(int id, int x, int y);
    void update_tiles(int id, int x, int y);


    sf::Sprite _sprites[13];
    sf::Texture _textures[13];
    std::map<unsigned int, Chunk*> _chunks;
    std::vector<std::tuple<sf::Vector2i, unsigned int>> _ids;
    unsigned char _generator[48][48];
};
