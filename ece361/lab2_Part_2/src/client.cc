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

#define INFO(x) std::cout << "\x1B[31m" << x << "\x1B[0m" << std::endl

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
    bool valid()
    {
        return m_init;
    }

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

    void auth(const std::function<nl::json(nl::json)>& f)
    {
        m_auth = f;
    }
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

    // if (resp["success"].get<bool>() == false) {
    //     INFO("SYNC FAILED");
    // }

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
        INFO("CONNECTION FAILED");
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
        INFO("LOGIN FAILED");
        return;
    }

    INFO("LOGIN SUCCESS");

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
        INFO("FAILED TO ESTABLISH A CONNECTION");
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
        INFO("REGISTER FAILED");
        return;
    }

    INFO("REGISTER SUCCESS");
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
        INFO("LIST FAILED");
        return;
    }

    INFO("LIST SUCCESS");

    std::cout << "Active Users :" << std::endl;
    for (int i = 0; i < resp["active users"].size(); i++)
        std::cout << resp["active users"][i] << std::endl;

    std::cout << "Active Sessions :" << std::endl;
    for (int i = 0; i < resp["active channels"].size(); i++)
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
        INFO("CREATE FAILED");
        return;
    }

    INFO("CREATE SUCCESS");
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
        INFO("JOIN FAILED");
        INFO(resp["error"]);
        return;
    }

    INFO("JOIN SUCCESS");

    // initial sync
    request_sync(true);

    // update state
    JOIN_DONE = true;
}

void
handle_kick(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string username;
    is >> username;

    s.send({
      { "type", PacketType::KICK },
      { "username", username },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::KICK_ACK);

    if (resp["success"].get<bool>() == false) {
        INFO("KICK FAILED");
        std::cout << resp["error"] << std::endl;
        return;
    }

    INFO("KICK SUCCESS");
}

void
handle_ban(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string username;
    is >> username;

    s.send({
      { "type", PacketType::BAN },
      { "username", username },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::BAN_ACK);

    if (resp["success"].get<bool>() == false) {
        INFO("BAN FAILED");
        std::cout << resp["error"] << std::endl;
        return;
    }

    INFO("BAN SUCCESS");
}

void
handle_unban(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string username;
    is >> username;

    s.send({
      { "type", PacketType::UNBAN },
      { "username", username },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::UNBAN_ACK);

    if (resp["success"].get<bool>() == false) {
        INFO("UNBAN FAILED");
        std::cout << resp["error"] << std::endl;
        return;
    }

    INFO("UNBAN SUCCESS");
}

void
handle_delete(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    s.send({
      { "type", PacketType::DELETE },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::DELETE_ACK);

    if (resp["success"].get<bool>() == false) {
        INFO("DELETE FAILED");
        return;
    }

    INFO("DELETE SUCCESS");
}

void
handle_resign(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string successor;
    is >> successor;

    s.send({
      { "type", PacketType::RESIGN },
      { "successor", successor },
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::RESIGN_ACK);

    if (resp["success"].get<bool>() == false) {
        INFO("RESIGN FAILED");
        std::cout << resp["error"] << std::endl;
        return;
    }

    INFO("RESIGN SUCCESS");
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
        INFO("LEAVE FAILED");
        return;
    }

    INFO("LEAVE SUCCESS");

    std::cout << "Left " << resp["session_left"] << std::endl;

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
        INFO("LOGOUT FAILED");
        INFO(resp["error"]);
        return;
    }

    // destroy session and notify the server
    s.destroy();

    INFO("LOGOUT SUCCESS");

    LOGIN_DONE = false;

#if 0
    // exit
    exit(0);
#endif
}

void
handle_help(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::cout << "/register [username] [password] [host name] [port number]\n";
    std::cout << "/login [username] [password] [host name] [port number]\n";
    std::cout << "/list\n";
    std::cout << "/create [channel name]\n";
    std::cout << "/join [channel name]\n";
    std::cout << "/leave\n";
    std::cout << "/logout\n";
    std::cout << std::flush;
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

void
handle_private(std::stringstream& is)
{
    std::lock_guard<std::mutex> lk(global_mutex);

    std::string user;
    is >> user;

    std::string message = is.str();

    s.send({
      { "type", PacketType::PRIVATE },
      { "username" , user},
      { "message", message.substr(0, 128)},
    });

    nl::json resp;
    s.receive(resp);

    // check if correct ACK was received
    assert_ack(resp, PacketType::PRIVATE_ACK);

    if (resp["success"].get<bool>() == false) {
        INFO("PRIVATE MESSAGE FAILED");
        return;
    }

    INFO("LEAVE SUCCESS");

    std::cout << "Left " << resp["session_left"] << std::endl;

    // update state
    JOIN_DONE = false;
}

//===------------------------------===//
// ENTRY POINT
//===------------------------------===//

typedef std::function<void(std::stringstream&)> callback_t;
std::map<std::string, callback_t> command_map{
    // init
    { "/?", handle_help },    //
    { "/help", handle_help }, //

    // account management
    { "/register", handle_register }, //
    { "/login", handle_login },       //

    // session management
    { "/list", handle_list },     //
    { "/create", handle_create }, //
    { "/join", handle_join },     //

    // session administration
    { "/kick", handle_kick },     //
    { "/ban", handle_ban },       //
    { "/unban", handle_unban },     //
    { "/delete", handle_delete }, //
    { "/resign", handle_resign }, //
    { "/private", handle_private }, //

    // cleanup
    { "/leave", handle_leave },   //
    { "/logout", handle_logout }, //
};

std::vector<std::string> init_commands = {
    "/?",
    "/help",
    "/login",
    "/register",
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
    std::vector<std::string> auto_complete_candidates;

    // extract keys
    std::transform(std::begin(command_map),
                   std::end(command_map),
                   std::back_inserter(auto_complete_candidates),
                   [](auto&& pair) { return pair.first; });

    linenoise::SetCompletionCallback(
      [&](const char* buffer, std::vector<std::string>& completions) {
          for (auto&& c : auto_complete_candidates) {
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
            INFO("INVALID COMMAND: /? or /help");
            continue;
        }

        if (!LOGIN_DONE && std::find(std::begin(init_commands),
                                     std::end(init_commands),
                                     word) == std::end(init_commands)) {
            INFO("NOT LOGGED IN");
            continue;
        }

        command_map[word](ss);

        goto L_END;

    L_HANDLE_MESSAGE:
        if (!LOGIN_DONE || !JOIN_DONE) {
            INFO("NOT IN A CHATROOM");
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
#if 1
    {
        std::stringstream ss;

        if (JOIN_DONE)
            handle_leave(ss);

        if (LOGIN_DONE)
            handle_logout(ss);
    }
#endif
}