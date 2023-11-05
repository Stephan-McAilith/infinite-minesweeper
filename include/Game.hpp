#include <SFML/Graphics.hpp>
#include "Map.hpp"

class Game {

public:
    Game();
    ~Game();
    void start();
    void load(std::string save);

private:
    void save();
    sf::RenderWindow _window;
    Map _map;
    bool _isGameOver;
};