#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include <iostream>
#include <set>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>


typedef websocketpp::server<websocketpp::config::asio> server;


class Game {

public:
    Game();
    ~Game();
    void start();
    void load(std::string save);

private:
    void start_game();
    void save();

    void on_open(websocketpp::connection_hdl hdl) {
        std::cout << "New client connection" << std::endl;
        client_connections.push_back(hdl);
    }

    void on_close(websocketpp::connection_hdl hdl) {
        std::cout << "Client disconnected!" << std::endl;
        //client_connections.(hdl);
    }

    void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::cout << "Received message: " << msg->get_payload() << std::endl;


        std::cout << m_endpoint.get_con_from_hdl(hdl)->get_state() << std::endl;

        websocketpp::lib::error_code ec;
        // Echo the message back to the sender
        m_endpoint.send(hdl, "General Kenobi", websocketpp::frame::opcode::text, ec);
        if (ec)
            std::cout << "Fail to reply " << ec << std::endl;
    }


    Map _map;
    bool _isGameOver;

    server m_endpoint;
    std::list<websocketpp::connection_hdl> client_connections;

    template<typename T>
    void printVector(sf::Vector2<T> vec) {
        std::cout << vec.x << " " << vec.y << std::endl;
    }
};