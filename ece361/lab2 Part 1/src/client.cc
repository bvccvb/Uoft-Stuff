#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "json.h"
#include "linenoise.h"
#include "message.h"
#include "protocol.h"

namespace nl = nlohmann;

static std::mutex global_mutex;

//===------------------------------===//
// CLASSES
//===------------------------------===//

// a class for communication abstraction; this is different from "chat sessions"
class Session
{
    bool m_init;
    int m_sock;

    std::function<nl::json(nl::json)> m_auth = [](nl::json j) { return j; };

  public:
    bool valid() { return m_init; }

    bool connect(std::string host, uint16_t port)
    {
        if (m_init)
            destroy();

        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;

        ::inet_pton(AF_INET, host.data(), &server.sin_addr);
        server.sin_port = ::htons(port);

        m_sock = ::socket(AF_INET, SOCK_STREAM, 0);
        assert(m_sock >= 0);

        int c = ::connect(m_sock, (struct sockaddr*)&server, sizeof(server));
        assert(c == 0);

        m_init = true;

        return true;
    }

    void destroy()
    {
        assert(m_init);

        // close tcp connection
        ::close(m_sock);

        // down init state
        m_init = false;

        // reset other states
        m_sock = 0;
        m_auth = [](nl::json j) { return j; };
    }

    void send(const nl::json& j)
    {
        assert(m_init);

        // compress and send to server
        std::vector<uint8_t> v_cbor(nl::json::to_cbor(m_auth(j)));
        ::send(m_sock, v_cbor.data(), v_cbor.size(), 0);
    }

    void receive(nl::json& j)
    {
        assert(m_init);

        char buf[MAXDATASIZE] = { 0 };
        int n = ::recv(m_sock, buf, MAXDATASIZE, 0 /* | MSG_WAITALL */);

        std::vector<uint8_t> v_cbor(buf, buf + n);
        j = nl::json::from_cbor(v_cbor);
    }

    void auth(const std::function<nl::json(nl::json)>& f) { m_auth = f; }
};

//===------------------------------===//
// GLOBALS
//===------------------------------===//
Session s;

// simple program; define some global states here
bool JOIN_DONE = false;
bool LOGIN_DONE = false;

//===------------------------------===//
// FUNCTIONS
//===------------------------------===//

void
assert_ack(const nl::json& resp, PacketType packet_type)
{
    assert(resp["type"].get<PacketType>() == packet_type);
}

void
request_sync(bool init)
{
    static uint64_t last_sync = 0;

    s.send({
      { "type", PacketType::SYNC },
      { "timestamp", init ? 0 : last_sync },
    });

    // wait for server response
    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::SYNC_ACK);

    try {
        std::vector<Message> messages = resp["messages"];
        for (auto&& m : messages) {
            std::cout << '\r' << m.printable() << std::endl;
            std::cout << "> " << std::flush;
            last_sync = m.time() + 1;
        }
    } catch (std::exception e) {
    }
}

void
handle_login(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string username, password, host;
    uint16_t port;
    is >> username >> password >> host >> port;

    // establish a connection
    if (s.connect(host, port) == false) {
        std::cout << "CONNECTION FAILED" << std::endl;
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

    if (resp["success"].get<bool>() == false) {
        std::cout << "LOGIN FAILED" << std::endl;
        return;
    }

    std::cout << "LOGIN SUCCESS" << std::endl;

    auto cookie = resp["cookie"].get<std::string>();
    s.auth([=](nl::json j) {
        j["cookie"] = cookie;
        return j;
    });

    // update state
    LOGIN_DONE = true;
}

void
handle_register(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

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

    if (resp["success"].get<bool>() == false) {
        std::cout << "REGISTER FAILED" << std::endl;
        return;
    }

    std::cout << "REGISTER SUCCESS" << std::endl;
}

void
handle_list(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    // send request
    s.send({
      { "type", PacketType::LIST },
    });

    // wait for server response
    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::LIST_ACK);

    if (resp["success"].get<bool>() == false) {
        std::cout << "LIST FAILED" << std::endl;
        return;
    }

    std::cout << "LIST SUCCESS" << std::endl;
    std::cout << "Active Users :" << std::endl;

    for(int i = 0 ; i < resp["active users"].size() ; i++)
    	 std::cout << resp["active users"][i] << std::endl;

    std::cout << "Active Sessions :" << std::endl;

     for(int i = 0 ; i < resp["active channels"].size() ; i++)
    	 std::cout << resp["active channels"][i] << std::endl;

}

void
handle_create(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string channel_name;
    is >> channel_name;

    s.send({
      { "type", PacketType::CREATE },
      { "channel_name", channel_name },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::CREATE_ACK);

    if (resp["success"].get<bool>() == false) {
        std::cout << "CREATE FAILED" << std::endl;
        return;
    }

    std::cout << "CREATE SUCCESS" << std::endl;
}

void
handle_join(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string channel_name;
    is >> channel_name;

    s.send({
      { "type", PacketType::JOIN },
      { "channel_name", channel_name },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::JOIN_ACK);

    if (resp["success"].get<bool>() == false) {
        std::cout << "JOIN FAILED" << std::endl;
        return;
    }

    std::cout << "JOIN SUCCESS" << std::endl;

    // initial sync
    request_sync(true);

    // update state
    JOIN_DONE = true;
}

void
handle_leave(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    s.send({
      { "type", PacketType::LEAVE },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::LEAVE_ACK);

    if (resp["success"].get<bool>() == false) {
        std::cout << "LEAVE FAILED" << std::endl;
        return;
    }

    std::cout << "LEAVE SUCCESS" << std::endl;
    std::cout << "Left " << resp["session left"] << std::endl;

    // update state
    JOIN_DONE = false;
}

void
handle_logout(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    s.send({
      { "type", PacketType::LOGOUT },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::LOGOUT_ACK);

    if (resp["success"].get<bool>() == false) {
        std::cout << "LOGOUT FAILED" << std::endl;
        return;
    }

    // destroy session and notify the server
    s.destroy();

    std::cout << "LOGOUT SUCCESS" << std::endl;

    LOGIN_DONE = false;

#if 0
    // exit
    exit(0);
#endif
}

void
handle_message(const std::string& message)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    // conform to the max packet size
    s.send({
      { "type", PacketType::MESSAGE },
      { "message", message.substr(0, 128) },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::MESSAGE_ACK);
}

//===------------------------------===//
// ENTRY POINT
//===------------------------------===//

typedef std::function<void(std::stringstream&)> callback_t;
std::map<std::string, callback_t> command_map{
    { "/login", handle_login },       //
    { "/list", handle_list },         //
    { "/create", handle_create },     //
    { "/join", handle_join },         //
    { "/leave", handle_leave },       //
    { "/logout", handle_logout },     //
    { "/register", handle_register }, //
};

int
main(void)
{
    // spawn a background thread for fetching data
    std::thread t([]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            if (s.valid() == false)
                continue;

            {
                std::lock_guard<std::mutex> lk(global_mutex);
                request_sync(false);
            }
        }
    });
    t.detach();

    // setup auto-complete
    linenoise::SetCompletionCallback(
      [](const char* buffer, std::vector<std::string>& completions) {
          const std::vector<std::string> candidates = //
            { "/login", "/list",   "/create",  "/join",
              "/leave", "/logout", "/register" };

          for (auto&& c : candidates) {
              if (std::string(buffer) == c.substr(0, strlen(buffer)))
                  completions.push_back(c);
          }
      });

    // Read Eval Print Loop (REPL)
    while (true) {
        std::string line;
        std::string word;
        auto quit = linenoise::Readline("> ", line);
        std::stringstream ss(line);
        ss >> word;

        if (quit)
            break;

        if (word.front() != '/')
            goto L_HANDLE_MESSAGE;

    L_HANDLE_COMMAND:
        if (command_map.find(word) == command_map.end()) {
            std::cout << "INVALID COMMAND" << std::endl;
            continue;
        }

        command_map[word](ss);

        goto L_END;

    L_HANDLE_MESSAGE:
        if (!LOGIN_DONE || !JOIN_DONE) {
            std::cout << "NOT IN A CHATROOM" << std::endl;
            continue;
        }

        {
            std::lock_guard<std::mutex> lk(global_mutex);
            std::cout << "\e[A" << '\r' << std::flush;
        }

        handle_message(line);

        goto L_END;

    L_END:;
    }

// FIXME
#if 0
    {
        std::stringstream ss;

        if (JOIN_DONE)
            handle_leave(ss);

        if (LOGIN_DONE)
            handle_logout(ss);
    }
#endif
}