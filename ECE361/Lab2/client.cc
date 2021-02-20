#include <algorithm>
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "json.h"
#include "protocol.h"

namespace nl = nlohmann;

//===------------------------------===//
// CLASSES
//===------------------------------===//

class Session
{
    bool m_init;
    uint64_t m_ip;
    uint16_t m_port;
    std::string m_name;
    std::string m_password;

    int m_sock;

  public:
    bool valid() { return m_init; }

    bool connect(std::string host, uint16_t port)
    {
        if (m_init)
            destroy();

        struct sockaddr_in sv_addr;
        memset(&sv_addr, 0, sizeof(sv_addr));
        sv_addr.sin_family = AF_INET;

        ::inet_pton(AF_INET, host.data(), &sv_addr.sin_addr);
        sv_addr.sin_port = ::htons(port);

        m_sock = ::socket(AF_INET, SOCK_STREAM, 0);
        assert(m_sock >= 0);

        ::connect(m_sock, (struct sockaddr*)&sv_addr, sizeof(sv_addr));

        return true;
    }

    void destroy()
    {
        if (m_init == false)
            return;

        ::close(m_sock);

        // down init state
        m_init = false;
    }

    void send(const nl::json& j)
    {
        std::vector<uint8_t> v_cbor(nl::json::to_cbor(j));
        ::send(m_sock, v_cbor.data(), v_cbor.size(), 0);
    }

    void receive(nl::json& j)
    {
        char buf[MAXDATASIZE] = { 0 };
        int n = ::recv(m_sock, buf, MAXDATASIZE, 0 /* | MSG_WAITALL */);

        std::vector<uint8_t> v_cbor(buf, buf + n);
        j = nl::json::from_cbor(v_cbor);
    }
};

//===------------------------------===//
// GLOBALS
//===------------------------------===//
Session s;

//===------------------------------===//
// FUNCTIONS
//===------------------------------===//

void
assert_ack(const nl::json& resp, PacketType packet_type)
{
    assert(resp["type"].get<PacketType>() == packet_type);
}

void
handle_login(std::istream& is)
{
    std::string username, password, host;
    uint16_t port;
    is >> username >> password >> host >> port;

    // establish a connection
    if (s.connect(host, port) == false) {
        std::cout << "FAILED TO ESTABLISH A CONNECTION" << std::endl;
        return;
    }

    // send login information
    s.send({
      { "type", PacketType::LOGIN },
      { "username", username },
      { "password", password },
    });

    // wait for server response
    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::LOGIN_ACK);
}

void
handle_register(std::istream& is)
{
    std::string username, password, host;
    uint16_t port;
    is >> username >> password >> host >> port;

    // establish a connection
    if (s.connect(host, port) == false) {
        std::cout << "FAILED TO ESTABLISH A CONNECTION" << std::endl;
        return;
    }

    // send login information
    s.send({
      { "type", PacketType::REGISTER },
      { "username", username },
      { "password", password },
    });

    // wait for server response
    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::REGISTER_ACK);
}

void
handle_list(std::istream& is)
{
    // send request
    s.send({
      { "type", PacketType::LIST },
    });

    // wait for server response
    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::LIST_ACK);
}

void
handle_create(std::istream& is)
{
    s.send({
      { "type", PacketType::CREATE },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::CREATE_ACK);
}

void
handle_join(std::istream& is)
{
    uint32_t session_id;
    is >> session_id;

    s.send({
      { "type", PacketType::JOIN },
      { "session_id", session_id },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::JOIN_ACK);
}

void
handle_leave(std::istream& is)
{
    s.send({
      { "type", PacketType::LEAVE },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::LEAVE_ACK);
}

void
handle_logout(std::istream& is)
{
    s.send({
      { "type", PacketType::LOGOUT },
    });

    // destroy session and notify the server
    s.destroy();

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::LOGOUT_ACK);

    // exit
    exit(0);
}

void
handle_message(std::string message)
{
    s.send({
      { "type", PacketType::MESSAGE },
      { "message", message },
    });
}

//===------------------------------===//
// ENTRY POINT
//===------------------------------===//

typedef std::function<void(std::istream&)> callback_t;
std::map<std::string, callback_t> command_map{
    { "/login", handle_login },   //
    { "/list", handle_list },     //
    { "/create", handle_create }, //
    { "/join", handle_join },     //
    { "/leave", handle_leave },   //
    { "/logout", handle_logout }, //
    { "/register", handle_register }, //
};

int
main(void)
{
    std::string input;
    std::string line;

    while (1) {
        std::cout << "client> ";
        std::cin >> input;

        if (std::cin.eof()) {
            std::cout << std::endl;
            break;
        }

        if (input.front() != '/')
            goto L_HANDLE_MESSAGE;

    L_HANDLE_COMMAND:
        if (command_map.find(input) == command_map.end()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (std::cin.eof()) {
                std::cout << std::endl;
                break;
            }

            std::cout << "INVALID COMMAND" << std::endl;
            continue;
        }

        command_map[input](std::cin);

        goto L_END;

    L_HANDLE_MESSAGE:
        if (!s.valid()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (std::cin.eof()) {
                std::cout << std::endl;
                break;
            }

            std::cout << "INVALID SESSION" << std::endl;
            continue;
        }

        std::getline(std::cin, line);

        handle_message(input + line);

        goto L_END;

    L_END:;
    }
}