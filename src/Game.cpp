#include "Game.hpp"
#include <iostream>
#include <fstream>

Game::Game() : _window(sf::VideoMode(1600, 900), "Infinite Minesweeper!"), _isGameOver(false), _zoom_level(1)
{
    _window.setFramerateLimit(30);
    _camera = sf::View(sf::Vector2f(CHUNK_PX_SIZE / 2, CHUNK_PX_SIZE / 2), sf::Vector2f(1920, 1080));
}

Game::~Game()
{
}

void Game::start() {


    //Mouse positions
    sf::Vector2f oldPos, newPos;
    //Did the camera moved
    bool moved = false;
    //Can the camera move
    bool moveCamera = false;
        _window.setView(_camera);

    //Game main loop
    while (_window.isOpen()) {
        sf::Event event;
        while (_window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                _window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                oldPos.x = event.mouseButton.x;
                oldPos.y = event.mouseButton.y;
                moved = false;
                if (event.mouseButton.button == sf::Mouse::Left) {
                    moveCamera = true;
                }
            }

            if (moveCamera && event.type == sf::Event::MouseMoved) {
                moved = true;
                newPos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                _camera.setCenter(_camera.getCenter() - (newPos - oldPos) * _zoom_level);
                oldPos = newPos;
                _window.setView(_camera);
            }

            if (event.type == sf::Event::MouseButtonReleased) {
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
    save();
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
