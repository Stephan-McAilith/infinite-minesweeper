#include <SFML/Graphics.hpp>
#include <functional>

#define MINES_AROUND 0b00001111
#define MINE 0b00010000
#define REVEALED 0b00100000
#define FLAG 0b01000000


class Chunk {
public:
    Chunk(int id, sf::Vector2i pos, std::function<bool(sf::Vector2i, sf::Vector2i)> reveal_in_chunk,
        std::function<unsigned char(sf::Vector2i, sf::Vector2i)> get_tile_in_chunk);
    ~Chunk();
    void draw(sf::RenderWindow& window, sf::Sprite(&sprites)[13], bool over);
    bool click(sf::Vector2i pos, sf::Mouse::Button button);
    sf::Vector2i const& getPos();
    bool reveal(int x, int y);
    unsigned char get_tile(sf::Vector2i coord);

    unsigned char _tiles[256];
private:
    bool reveal_in_other_chunk(int x, int y);
    bool addMine(unsigned int pos);
    bool check_flag(int x, int y);
    void get_coord_in_other_chunk(int x, int y, sf::Vector2i& chunk, sf::Vector2i& coord);

    int _id;
    const sf::Vector2i _pos;
    //first 4 bits for number of mines around
    //5th bit for mine or not
    //6th bit for reveal or not
    //7th bit for flag or not
    std::function<bool(sf::Vector2i, sf::Vector2i)> _reveal_in_chunk;
    std::function<unsigned char(sf::Vector2i, sf::Vector2i)> _get_tile_in_chunk;
};