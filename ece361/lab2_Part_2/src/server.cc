#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <random>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <stdbool.h>

#include "channel.h"
#include "json.h"
#include "picosha2.h"
#include "protocol.h"

#define MAX_EVENTS 128
#define DB_FILE "user_info.txt"

namespace nl = nlohmann;

// channel name -> channel
std::map<std::string, Channel> channels;

// cookie -> username, channel*
struct uinfo
{
    std::string username;
    Channel* channel;
};

std::map<std::string, uinfo> users;

//keeps track of master of channel channel, user
std::map<std::string, std::string> master;

std::map<std::string, std::vector<std::string>> banned;

// rudimentary session identifier; obviously not cryptographically secure
std::string
generate_cookie()
{
    std::string s("0123456789"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "abcdefghijklmnopqrstuvwxyz");

    std::shuffle(
      std::begin(s), std::end(s), std::mt19937(std::random_device{}()));

    return s.substr(0, 31);
}

void
login_handler(const nl::json& client_request, nl::json& server_response)
{
    std::string username = client_request["username"].get<std::string>();
    std::string password = client_request["password"].get<std::string>();

    password = picosha2::hash256_hex_string(password);

    std::string line;

    std::string user_comparison;
    std::string pass_comparison;

    std::ifstream ifile(DB_FILE);
    if (ifile.is_open()) {
        // reads each line untill it finds the pass word
        while (ifile >> user_comparison >> pass_comparison) {
            if (user_comparison != username || password != pass_comparison)
                continue;

            // generate and register session key
            std::string cookie(generate_cookie());
            users[cookie].username = username;
            users[cookie].channel = nullptr;

            // construct response
            server_response = { { "type", PacketType::LOGIN_ACK },
                                { "success", true },
                                { "cookie", cookie } };
            return;
        }

        ifile.close();
    }

    server_response = { { "type", PacketType::LOGIN_ACK },
                        { "success", false } };
    return;
}

void
registration_handler(const nl::json& client_request, nl::json& server_response)
{
    std::string username = client_request["username"].get<std::string>();
    std::string password = client_request["password"].get<std::string>();

    password = picosha2::hash256_hex_string(password);

    std::string line;

    std::string user_comparison;
    std::string pass_comparison;

    std::ifstream ifile(DB_FILE);
    if (ifile.is_open()) {
        // check for duplicate username
        while (ifile >> user_comparison >> pass_comparison) {
            if (user_comparison != username)
                continue;

            server_response = { { "type", PacketType::REGISTER_ACK },
                                { "success", false },
                                { "error", "User already exists" } };
            return;
        }

        ifile.close();
    }

    std::ofstream ofile;
    ofile.open(DB_FILE, std::ios::app);
    ofile << username << " " << password << std::endl;

    // send back response
    server_response = {
        { "type", PacketType::REGISTER_ACK },
        { "success", true },
    };
}

void
create_handler(const nl::json& client_request, nl::json& server_response)
{
    auto channel_name = client_request["channel_name"].get<std::string>();
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::CREATE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // channel already exists
    if (channels.find(channel_name) != std::end(channels)) {
        server_response = { { "type", PacketType::CREATE_ACK },
                            { "success", false },
                            { "error", "Channel already exists" } };
        return;
    }

    channels.insert(
      { channel_name, Channel(channel_name, users[cookie].username) });

    //master
    master[channel_name] = users[cookie].username;

    // send back response
    server_response = {
        { "type", PacketType::CREATE_ACK },
        { "success", true },
    };
}

void
join_handler(const nl::json& client_request, nl::json& server_response)
{
    auto channel_name = client_request["channel_name"].get<std::string>();
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::JOIN_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // channel not found
    if (channels.find(channel_name) == std::end(channels)) {
        server_response = { { "type", PacketType::JOIN_ACK },
                            { "success", false },
                            { "error", "Channel not found" } };
        return;
    }

    //Check if user is banned
    for(int i = 0 ; i < banned[channel_name].size() ; i++){
        if(banned[channel_name][i] == users[cookie].username){
            server_response = { { "type", PacketType::JOIN_ACK },
                            { "success", false },
                            { "error", "You have been banned from this chatroom" } };
            return;
        }
    }


    // already joined in a channel, unsubscribe first
    if (users[cookie].channel != nullptr) {
        users[cookie].channel->unsubscribe(users[cookie].username);
    }

    // update the uinfo map
    users[cookie].channel = &channels.at(channel_name);

    // update the channel
    users[cookie].channel->subscribe(users[cookie].username);

    std::string message;
    message.append(users[cookie].username);
    message.append(" has joined the chat");

    users[cookie].channel->post_msg(users[cookie].username, message);

    // send back response
    server_response = {
        { "type", PacketType::JOIN_ACK },
        { "success", true },
    };
}

void
kick_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::KICK_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::KICK_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    // check admin privileges
    if (master[users[cookie].channel->name()] != users[cookie].username) {
        server_response = { { "type", PacketType::KICK_ACK },
                            { "success", false },
                            { "error", "Insufficient privileges" } };
        return;
    }

    auto username = client_request["username"].get<std::string>();

    //can't kick yourself
    if (username == users[cookie].username) {
        server_response = { { "type", PacketType::KICK_ACK },
                            { "success", false },
                            { "error", "Cannot kick yourself" } };
        return;
    }


    for(auto it = users.begin(); it != users.end(); it++){
        //Finds user online
        if(it->second.username == username){

            //User isn't in chat
            if (users[it-> first].channel == nullptr) {
                server_response = { { "type", PacketType::KICK_ACK },
                                    { "success", false },
                                    { "error", "User not in session" } };
                return;
            }
            int i;

            std::string kick_message;
            kick_message.append("Kicked ");
            kick_message.append(username);
            kick_message.append(" out of chat");

            users[it-> first].channel-> post_msg(users[cookie].username, kick_message);

            users[it-> first].channel-> unsubscribe(username);
            users[it-> first].channel = nullptr;

            server_response = { { "type", PacketType::KICK_ACK },
                            { "success", true },
                            { "kicked", username } };

            return;

        }
    }

    server_response = { { "type", PacketType::KICK_ACK },
                            { "success", false },
                            { "error", "User not logged in" } };
    return;
}


void
ban_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::BAN_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::BAN_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    // check admin privileges
    if (master[users[cookie].channel->name()] != users[cookie].username) {
        server_response = { { "type", PacketType::BAN_ACK },
                            { "success", false },
                            { "error", "Insufficient privileges" } };
        return;
    }

    auto username = client_request["username"].get<std::string>();

    //can't ban yourself
    if (username == users[cookie].username) {
        server_response = { { "type", PacketType::BAN_ACK },
                            { "success", false },
                            { "error", "Cannot ban yourself" } };
        return;
    }


    for(auto it = users.begin(); it != users.end(); it++){
        //Finds user online
        if(it->second.username == username){

            //User isn't in chat
            if (users[it-> first].channel == nullptr) {
                server_response = { { "type", PacketType::BAN_ACK },
                                    { "success", false },
                                    { "error", "User not in session" } };
                return;
            }

            std::string kick_message;
            kick_message.append("Banned ");
            kick_message.append(username);
            kick_message.append(" out of chat");

            //Ban the user from channel
            if (banned.find(users[it-> first].channel-> name()) == std::end(banned)) {
                //need to make new vector
                std::vector<std::string> temp;
                temp.push_back(username);
                banned[users[it-> first].channel-> name()] = temp;
            } else{
                banned[users[it-> first].channel-> name()].push_back(username);
            }

            users[it-> first].channel-> post_msg(users[cookie].username, kick_message);

            users[it-> first].channel-> unsubscribe(username);
            users[it-> first].channel = nullptr;

            
            server_response = { { "type", PacketType::BAN_ACK },
                            { "success", true },
                            { "banned", username } };

            return;

        }
    }

    server_response = { { "type", PacketType::BAN_ACK },
                            { "success", false },
                            { "error", "User not logged in" } };
    return;
}

void
unban_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::UNBAN_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::UNBAN_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    // check admin privileges
    if (master[users[cookie].channel->name()] != users[cookie].username) {
        server_response = { { "type", PacketType::UNBAN_ACK },
                            { "success", false },
                            { "error", "Insufficient privileges" } };
        return;
    }

    auto username = client_request["username"].get<std::string>();

    //can't unban yourself
    if (username == users[cookie].username) {
        server_response = { { "type", PacketType::UNBAN_ACK },
                            { "success", false },
                            { "error", "Cannot unban yourself" } };
        return;
    }

    std::string channel_name = users[cookie].channel->name();

    if(banned.find(channel_name) == std::end(banned)){
        server_response = { { "type", PacketType::UNBAN_ACK },
                            { "success", false },
                            { "error", "No one has been banned" } };
        return;
    } 

    for(int i = 0; i < banned[channel_name].size(); i++){
        if(banned[channel_name][i] == username){

            std::string unban_message;
            unban_message.append("Unbanned ");
            unban_message.append(username);

            users[cookie].channel-> post_msg(users[cookie].username, unban_message);

            banned[channel_name].erase(banned[channel_name].begin() + i);
            server_response = { { "type", PacketType::UNBAN_ACK },
                            { "success", true },
                            { "unbanned", username } };
            return;
        }
    }
            server_response = { { "type", PacketType::BAN_ACK },
                            { "success", false },
                            { "error", "User not on banned list" } };

    return;
}

void
resign_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::RESIGN_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::RESIGN_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    // check admin privileges
    if (master[users[cookie].channel->name()] != users[cookie].username) {
        server_response = { { "type", PacketType::RESIGN_ACK },
                            { "success", false },
                            { "error", "Insufficient privileges" } };
        return;
    }

    auto username = client_request["successor"].get<std::string>();

    //can't resign to yourself
    if (username == users[cookie].username) {
        server_response = { { "type", PacketType::RESIGN_ACK },
                            { "success", false },
                            { "error", "Cannot resign to yourself" } };
        return;
    }

    std::string channel_name = users[cookie].channel->name();

    for(auto it = users.begin(); it != users.end(); it++){
        //found sucessor
        if(it-> second.username == username && it-> second.channel-> name() == channel_name){

            std::string resign_message;
            resign_message.append(master[channel_name]);
            resign_message.append(" resigned admin privileges to ");
            resign_message.append(username);

            users[cookie].channel-> post_msg(users[cookie].username, resign_message);

            master[channel_name] = username;

            server_response = { { "type", PacketType::RESIGN_ACK }, 
                            { "success", true },
                            { "resigned to", username } };
            return;
        }
    }

    server_response = { { "type", PacketType::RESIGN_ACK },
                            { "success", false },
                            { "error", "Sucessor not found" } };
    return;
}

void
leave_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::LEAVE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::LEAVE_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    auto channel = users[cookie].channel;
    auto channel_name = channel->name();

    std::string message;

    message.append(users[cookie].username);
    message.append(" has left the chat");

    users[cookie].channel->post_msg(users[cookie].username, message);


    // delist from channel subscriber list
    channel->unsubscribe(users[cookie].username);

    // destroy session if no one left
    if (channel->n_users() == 0){
        channels.erase(channel_name);
        banned.erase(channel_name);
        master.erase(channel_name);
    }

    // unlink uinfo
    users[cookie].channel = nullptr;

    server_response = { { "type", PacketType::LEAVE_ACK },
                        { "success", true },
                        { "session_left", channel_name } };
    return;
}

void
delete_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::DELETE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::DELETE_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    // check admin privileges
    if (master[users[cookie].channel->name()] != users[cookie].username) {
        server_response = { { "type", PacketType::DELETE_ACK },
                            { "success", false },
                            { "error", "Insufficient privileges" } };
        return;
    }

    auto channel = users[cookie].channel;
    auto channel_name = channel->name();

    for(auto it = users.begin(); it != users.end(); it++){

        if(it-> second.channel-> name() == channel_name){
            // delist from channel subscriber list
            channel->unsubscribe(it-> second.username);
            // unlink uinfo
            users[it-> first].channel = nullptr;
        }
    }

    channels.erase(channel_name);
    banned.erase(channel_name);
    master.erase(channel_name);

    server_response = { { "type", PacketType::DELETE_ACK },
                        { "success", true },
                        { "deleted", channel_name } };
    return;
}

void
logout_handler(const nl::json& client_request, nl::json& server_response)
{
    std::string left_channel;
    auto cookie = client_request["cookie"].get<std::string>();

    // Checks if the user is even logged in, should be true since login and
    // register connects client and server
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::LOGOUT_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // cannot log out if there is an active session
    if (users[cookie].channel != nullptr) {
        server_response = { { "type", PacketType::LOGOUT_ACK },
                            { "success", false },
                            { "error", "An active session exists" } };
        return;
    }

    // erases cookie and username
    users.erase(cookie);

    server_response = { { "type", PacketType::LOGOUT_ACK },
                        { "success", true } };

    return;
}

void
message_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::MESSAGE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::MESSAGE_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    auto message = client_request["message"].get<std::string>();
    users[cookie].channel->post_msg(users[cookie].username, message);

    server_response = { { "type", PacketType::MESSAGE_ACK },
                        { "success", true } };
}

void
private_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();
    std::string for_user = client_request["username"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::PRIVATE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::PRIVATE_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    for(auto it = users.begin() ; it != users.end() ; it ++){
        if(it-> second.username == for_user && it-> second.channel-> name() == users[cookie].channel-> name()){
            auto message = client_request["message"].get<std::string>();
            users[cookie].channel->post_private_msg(users[cookie].username, message, for_user);

            server_response = { { "type", PacketType::PRIVATE_ACK },
                                { "success", true } };
        }
    }

    server_response = { { "type", PacketType::PRIVATE_ACK },
                        { "success", false },
                        { "error", "User not found in chatroom"}};
}

void
sync_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::SYNC_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // no active session
    if (users[cookie].channel == nullptr) {
        server_response = { { "type", PacketType::SYNC_ACK },
                            { "success", false },
                            { "error", "No active session" } };
        return;
    }

    auto timestamp = client_request["timestamp"].get<uint64_t>();
    nl::json messages = users[cookie].channel->get_msg(timestamp, users[cookie].username);

    // message styling
    auto username = users[cookie].username;

    for (auto&& m : messages) {
        auto s = m["message"].get<std::string>();

        if (s.find("@" + username) == std::string::npos)
            continue;

        m["message"] = "\x1B[36m" + s + "\x1B[0m";
    }

    server_response = { { "type", PacketType::SYNC_ACK },
                        { "success", true },
                        { "messages", messages } };
}

void
list_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();
    std::vector<std::string> a_users;
    std::vector<std::string> a_channels;

    // auth error
    if (users.find(cookie) == std::end(users)) {
        server_response = { { "type", PacketType::LIST_ACK },
                            { "success", false },
                            { "error", "Auth error" } };

        return;
    }

    std::string list_message;

    for (auto&& i : users)
        a_users.push_back(i.second.username);

    for (auto&& i : channels)
        a_channels.push_back(i.first);

    // send back response
    server_response = { { "type", PacketType::LIST_ACK },
                        { "success", true },
                        { "active users", a_users },
                        { "active channels", a_channels } };
}



void
process_request(const nl::json& client_request, nl::json& server_response)
{
    switch (client_request["type"].get<PacketType>()) {
        case (PacketType::LOGIN):
            login_handler(client_request, server_response);
            break;
        case (PacketType::LEAVE):
            leave_handler(client_request, server_response);
            break;
        case (PacketType::LOGOUT):
            logout_handler(client_request, server_response);
            break;
        case (PacketType::LIST):
            list_handler(client_request, server_response);
            break;
        case (PacketType::CREATE):
            create_handler(client_request, server_response);
            break;
        case (PacketType::JOIN):
            join_handler(client_request, server_response);
            break;
        case (PacketType::PRIVATE):
            private_handler(client_request, server_response);
            break;
        case (PacketType::KICK):
            kick_handler(client_request, server_response);
            break;
        case (PacketType::BAN):
            ban_handler(client_request, server_response);
            break;
        case (PacketType::UNBAN):
            unban_handler(client_request, server_response);
            break;
        case (PacketType::DELETE):
            delete_handler(client_request, server_response);
            break;
        case (PacketType::RESIGN):
            resign_handler(client_request, server_response);
            break;
        case (PacketType::MESSAGE):
            message_handler(client_request, server_response);
            break;
        case (PacketType::REGISTER):
            registration_handler(client_request, server_response);
            break;
        case (PacketType::SYNC):
            sync_handler(client_request, server_response);
            break;
        default:
            std::cout << "INVALID REQUEST" << std::endl;
            break;
    }
}

void
accept_connection(int skfd, int epoll_fd)
{
    struct sockaddr_in peer_addr;
    int peer_addrlen = sizeof(peer_addr);

    struct epoll_event epoll_ev;
    int client;

    // Creates a new socket creating a new descriptor for a socket
    // that connects to the client
    if ((client = accept(skfd,
                         (struct sockaddr*)&peer_addr,
                         (socklen_t*)&peer_addrlen)) < 0) {
        perror("[!] accept() failed");
        exit(-1);
    } else {
        std::cout << "Connected to " << client << std::endl;
    }

    // Set the client socket to non blocking
    int flags = fcntl(client, F_GETFL, 0);
    fcntl(client, F_SETFL, flags | O_NONBLOCK);

    epoll_ev.data.fd = client;

    // if the client is readable
    epoll_ev.events = EPOLLIN | EPOLLET;

    if ((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &epoll_ev)) < 0) {
        perror("[!] epoll_ctl() failed");
        exit(-1);
    }
}

void
send_data(struct epoll_event* epoll_ev)
{
    nl::json client_request;
    nl::json server_response;

    // The client socket is ready; parse and read data
    if (epoll_ev->events && EPOLLIN) {
        // skip empty
        if (epoll_ev->data.ptr == NULL)
            return;

        //std::cout << "Reading from socket " << epoll_ev->data.fd << std::endl;

        // receive client request
        {
            char buf[MAXDATASIZE] = { 0 };
            int n = recv(epoll_ev->data.fd, buf, MAXDATASIZE, 0);

            //std::cout << "Read " << n << " bytes" << std::endl;

            // discard spurious packets
            // discard spurious packets
            if (n < 0)
                return;

            if (n == 0) {
                std::cout << "Client : " << epoll_ev->data.fd
                          << " disconnected (EOF detected)" << std::endl;
                close(epoll_ev->data.fd);
                return;
            }

            std::vector<uint8_t> v_cbor(buf, buf + n);
            nl::json j = nl::json::from_cbor(v_cbor);

            client_request = j;
        }

        // process request
        {
            process_request(client_request, server_response);

            std::vector<uint8_t> v_cbor(nl::json::to_cbor(server_response));
            send(epoll_ev->data.fd, v_cbor.data(), v_cbor.size(), 0);

            if (server_response["type"].get<PacketType>() ==
                  PacketType::LOGOUT_ACK &&
                server_response["sucess"] == true) {

                std::cout << "Client : " << epoll_ev->data.fd << " logged out"
                          << std::endl;
                close(epoll_ev->data.fd);
            }

            // std::cout << "Finished sending ACK for socket " << epoll_ev->data.fd
            //           << std::endl;
        }
    }
}

void
work(int skfd, int epoll_fd, struct epoll_event* epoll_event_vector)
{
    // Wait for an event to happen; argument 4 is unsigned -1 is super large
    auto n = epoll_wait(epoll_fd, epoll_event_vector, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++) {
        if (skfd == epoll_event_vector[i].data.fd)
            accept_connection(skfd, epoll_fd);
        else
            send_data(&epoll_event_vector[i]);
    }
}

int
main(int argc, char** argv)
{
    if (argc != 2)
        return -1;

    // Connection Establishment variables
    int port = std::atoi(argv[1]);
    int skfd;
    struct sockaddr_in serv_addr;
    int addrlen = sizeof(serv_addr);

    // Epoll variables
    int epoll_fd;
    struct epoll_event epoll_ev;
    struct epoll_event epoll_event_vector[MAX_EVENTS];

    // set addressing scheme to AF_INET (IP)
    serv_addr.sin_family = AF_INET;

    // listen on port
    serv_addr.sin_port = htons(port);

    // allow any IP to connect
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);

    // create a server socket
    if ((skfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[!] socket() failed");
        exit(-1);
    }

    // prepare to listen from port specified in sockaddr (server_address)
    if ((bind(skfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("[!] bind() failed");
        exit(-1);
    }

    // Set the socket to non blocking
    int flags = fcntl(skfd, F_GETFL, 0);
    fcntl(skfd, F_SETFL, flags | O_NONBLOCK);

    if ((listen(skfd, SOMAXCONN)) < 0) {
        perror("[!] listen() failed");
        exit(-1);
    }

    // Create epoll instance
    if ((epoll_fd = epoll_create1(0)) < 0) {
        perror("[!] epoll_create() failed");
        exit(-1);
    }

    std::cout << "Server socket is " << skfd << std::endl;

    // Listen socket is ready
    epoll_ev.data.fd = skfd;

    // If the skfd is readable
    epoll_ev.events = EPOLLIN | EPOLLET;

    if ((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, skfd, &epoll_ev)) < 0) {
        perror("[!] epoll_ctl() failed");
        exit(-1);
    }

    // Now the program will continuously accept new connections and put them
    // into the event vector
    while (1) {
        // Vi sitter här i venten och spelar lite DotA
        work(skfd, epoll_fd, epoll_event_vector);
    }

    return 0;
}
