#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "Connection.hpp"
#include <iostream>



#define SCROLL_STEP -0.20f
#define MAX_ZOOM_IN 0.5f
#define MAX_ZOOM_OUT 4.f

class Game {

public:
    Game();
    ~Game();
    void start(std::string uri);
    void load(std::string save);

private:
    void start_game();
    void save();
    void connect(std::string uri);
    void scroll(sf::Event event);
    sf::RenderWindow _window;
    sf::View _camera;
    Map _map;
    bool _isGameOver;
    float _zoom_level;

    template<typename T>
    void printVector(sf::Vector2<T> vec) {
        std::cout << vec.x << " " << vec.y << std::endl;
    }

    client m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
};