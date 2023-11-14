#include "Game.hpp"
#include <iostream>
#include <fstream>


Game::Game() : _isGameOver(false)
{
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);
    m_endpoint.init_asio();


    m_endpoint.set_open_handler(websocketpp::lib::bind(
        &Game::on_open,
        this,
        websocketpp::lib::placeholders::_1
    ));

    m_endpoint.set_close_handler(websocketpp::lib::bind(
        &Game::on_close,
        this,
        websocketpp::lib::placeholders::_1
    ));

    m_endpoint.set_message_handler(std::bind(
        &Game::on_message, this,
        std::placeholders::_1, std::placeholders::_2
    ));
}

Game::~Game()
{
}

void Game::start() {

    // Listen on port 9002
    m_endpoint.listen(9002);

    // Queues a connection accept operation
    m_endpoint.start_accept();

    // Start the Asio io_service run loop
    m_endpoint.run();
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
