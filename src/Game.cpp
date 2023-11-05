#include "Game.hpp"
#include <iostream>
#include <fstream>

Game::Game() : _window(sf::VideoMode(1920, 1080), "Infinite Minesweeper!"), _isGameOver(false)
{
    _window.setFramerateLimit(30);
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
    //
    sf::View camera(sf::Vector2f(256, 256), sf::Vector2f(1920, 1080));

    camera.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    _window.setView(camera);

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
                camera.setCenter(camera.getCenter() - (newPos - oldPos));
                oldPos = newPos;
                _window.setView(camera);
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                moveCamera = false;
                if (_isGameOver)
                    continue;
                if (moved == false) {
                    _isGameOver = _map.click(_window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)), event.mouseButton.button);
                }
            }
        }
        _window.clear();
        // draw game state
        _map.draw(_window, camera.getCenter() - sf::Vector2f(640, 360), _isGameOver);
    
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