#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class Connection {
public:
    typedef websocketpp::lib::shared_ptr<Connection> ptr;

    Connection(websocketpp::connection_hdl hdl, std::string uri)
        : m_hdl(hdl)
        , m_status("Connecting")
        , m_uri(uri)
        , m_server("N/A")
    {}

    void on_open(client* c, websocketpp::connection_hdl hdl) {
        m_status = "Open";
        std::cout << m_status << std::endl;

        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
        c->send(hdl, "Hello there", websocketpp::frame::opcode::text);
    }

    void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
        std::cout << "Received message: " << msg->get_payload() << std::endl;
    }

    void on_fail(client* c, websocketpp::connection_hdl hdl) {
        m_status = "Failed";
        std::cout << m_status << std::endl;
        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
        m_error_reason = con->get_ec().message();
    }

    //friend std::ostream& operator<< (std::ostream& out, Connection const& data);
private:
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
};

//std::ostream& operator<< (std::ostream& out, Connection const& data) {
//    out << "> URI: " << data.m_uri << "\n"
//        << "> Status: " << data.m_status << "\n"
//        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
//        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);
//
//    return out;
//}