#include "Game.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

Game::Game() : _window(sf::VideoMode(1600, 900), "Infinite Minesweeper!"), _isGameOver(false), _zoom_level(1)
{
    _window.setFramerateLimit(30);
    _camera = sf::View(sf::Vector2f(0,0), sf::Vector2f(1600, 900));

    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);
    m_endpoint.init_asio();
    m_endpoint.start_perpetual();
    m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
}

Game::~Game()
{
}

void Game::start(std::string uri) {
    connect(uri);
    bool drawClickSprite = true;
 
     _window.setView(_camera);
     sf::Texture background;
     background.loadFromFile(".\\assets\\Welcome Screen.png");
     sf::Sprite backgroundSprite(background);

     sf::Texture click_to_start;
     click_to_start.loadFromFile(".\\assets\\click_to_start.png");
     sf::Sprite click_to_startSprite(click_to_start);
     _camera.setCenter(sf::Vector2f(background.getSize()) / 2.f);

     auto lastToggleTime = std::chrono::high_resolution_clock::now();
     const std::chrono::milliseconds toggleInterval(400);

    //Game main loop
     while (_window.isOpen()) {

         sf::Event event;
         while (_window.pollEvent(event)) {

             if (event.type == sf::Event::Closed)
                 _window.close();

             if (event.type == sf::Event::MouseButtonPressed) {
                 start_game();
             }



             if (event.type == sf::Event::MouseButtonReleased) {

             }

         }


         _window.clear();

         _camera.setSize(sf::Vector2f(_window.getSize()) * _zoom_level);
         _window.setView(_camera);
         _window.draw(backgroundSprite);

         // Calculate the time since the last toggle
         auto currentTime = std::chrono::high_resolution_clock::now();
         auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTime);
         if (elapsed >= toggleInterval) {
             drawClickSprite = !drawClickSprite;
             lastToggleTime = currentTime;
         }
        if (drawClickSprite)
        _window.draw(click_to_startSprite);


        _window.display();
    }
    save();
}


void Game::start_game() {
    
    //Mouse positions
    sf::Vector2i oldPos, newPos;
    //Did the camera moved
    bool moved = false;

    bool pressed = false;

    //Can the camera move
    bool moveCamera = false;

    _camera.setCenter(sf::Vector2f(CHUNK_PX_SIZE / 2, CHUNK_PX_SIZE / 2));

    sf::Event event;

    //Game main loop
    while (_window.isOpen()) {
        while (_window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                _window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                pressed = true;
                oldPos.x = event.mouseButton.x;
                oldPos.y = event.mouseButton.y;
                moved = false;
                if (event.mouseButton.button == sf::Mouse::Left) {
                    moveCamera = true;
                }
            }

            if (moveCamera && event.type == sf::Event::MouseMoved && (moved || Map::dist(oldPos, sf::Vector2i(event.mouseMove.x, event.mouseMove.y)) > 10)) {
                moved = true;
                newPos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
                _camera.setCenter(_camera.getCenter() - sf::Vector2f((newPos - oldPos)) * _zoom_level);
                oldPos = newPos;
                _window.setView(_camera);
            }

            if (pressed && event.type == sf::Event::MouseButtonReleased) {
                pressed = false;
                moveCamera = false;
                if (_isGameOver)
                    continue;
                if (moved == false) {
                    _isGameOver = _map.click(_window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)), event.mouseButton.button);
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled)
                scroll(event);
        }
        // clear previous draw
        _window.clear();
        // Draw game state
        _camera.setSize(sf::Vector2f(_window.getSize()) * _zoom_level);
        _window.setView(_camera);
        _map.draw(_window, _camera.getCenter(), _isGameOver, _zoom_level);
        _window.display();
    }
}

void Game::connect(std::string uri) {
    std::cout << "Connecting to " + uri << std::endl;
    websocketpp::lib::error_code ec;

    client::connection_ptr con = m_endpoint.get_connection(uri, ec);

    if (ec) {
        std::cout << "> Connect initialization error: " << ec.message() << std::endl;
        return;
    }

    Connection::ptr metadata_ptr(new Connection(con->get_handle(), uri));

    con->set_open_handler(websocketpp::lib::bind(
        &Connection::on_open,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_fail_handler(websocketpp::lib::bind(
        &Connection::on_fail,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));

    con->set_message_handler(websocketpp::lib::bind(&Connection::on_message, metadata_ptr, &m_endpoint, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    m_endpoint.connect(con);
    //std::cout << *metadata_ptr << std::endl;

}

void Game::save() {
    std::ofstream file("save.mine", std::ios::out | std::ios::trunc | std::ios::binary);
    _map.save(file);
    file.close();
}

void Game::load(std::string save) {
    std::ifstream file(save, std::ios::binary);
    _map.load(file);
    file.close();
}

void Game::scroll(sf::Event event) {
    float scroll = SCROLL_STEP * event.mouseWheelScroll.delta;
    float new_zoom = _zoom_level + scroll;
    new_zoom = new_zoom < MAX_ZOOM_IN ? MAX_ZOOM_IN : new_zoom;
    new_zoom = new_zoom > MAX_ZOOM_OUT ? MAX_ZOOM_OUT : new_zoom;
    float diff = _zoom_level - new_zoom;

    sf::Vector2f size = sf::Vector2f(_window.getSize());
    sf::Vector2f ratio((float)event.mouseWheelScroll.x / (float)_window.getSize().x, (float)event.mouseWheelScroll.y / (float)_window.getSize().y);
    ratio = ratio - sf::Vector2f(0.5, 0.5);
    sf::Vector2f px_diff = size * diff;
    sf::Vector2f camera_offset(ratio.x * px_diff.x, ratio.y * px_diff.y);
 
    _camera.setCenter(_camera.getCenter() + camera_offset);
    _zoom_level = new_zoom;
}
