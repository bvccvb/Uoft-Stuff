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

#include "channel.h"
#include "json.h"
#include "picosha2.h"
#include "protocol.h"

#define MAX_EVENTS 128
namespace nl = nlohmann;

// channel name, channel
std::map<std::string, Channel> channels;

// cookie, username
std::map<std::string, std::string> cookie_username;

// username, channel name
std::map<std::string, std::string> username_to_channel;

// cookie, channel name
std::map<std::string, std::string> cookie_channel_name;

// cookie, channel
std::map<std::string, Channel*> cookie_channel;

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

    std::ifstream ifile("user_info.txt");
    if (ifile.is_open()) {
        // reads each line untill it finds the pass word
        while (ifile >> user_comparison >> pass_comparison) {
            if (user_comparison != username || password != pass_comparison)
                continue;

            // generate and register session key
            std::string cookie(generate_cookie());
            cookie_username[cookie] = username;

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

    std::ifstream ifile("user_info.txt");
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
    ofile.open("user_info.txt", std::ios::app);
    ofile << username << " " << password << std::endl;

    // send back response
    server_response = {
        { "type", PacketType::REGISTER_ACK },
        { "success", true },
    };
}

void
join_handler(const nl::json& client_request, nl::json& server_response)
{
    auto channel_name = client_request["channel_name"].get<std::string>();
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (cookie_username.find(cookie) == cookie_username.end()) {
        server_response = { { "type", PacketType::JOIN_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // channel not found
    if (channels.find(channel_name) == channels.end()) {
        server_response = { { "type", PacketType::JOIN_ACK },
                            { "success", false },
                            { "error", "Channel not found" } };
        return;
    }

    for(std::map<std::string, std::string>::iterator it =  username_to_channel.begin(); it !=  username_to_channel.end(); it++){
    	if((*it).first == cookie_username[cookie] && (*it).second == channel_name){
    		server_response = { { "type", PacketType::JOIN_ACK },
                            { "success", false },
                            { "error", "Already in channel" } };
       		return;
    	}
	}

    cookie_channel[cookie] = &channels[channel_name];

    //add the user association to channel name for easy leave and logout
    //also added a cookie to channel name
    username_to_channel[cookie_username[cookie]] = channel_name;
    cookie_channel_name[cookie] = channel_name;

    // send back response
    server_response = {
        { "type", PacketType::JOIN_ACK },
        { "success", true },
    };
}

void
create_handler(const nl::json& client_request, nl::json& server_response)
{
    auto channel_name = client_request["channel_name"].get<std::string>();
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (cookie_username.find(cookie) == cookie_username.end()) {
        server_response = { { "type", PacketType::CREATE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // channel already exists
    if (channels.find(channel_name) != channels.end()) {
        server_response = { { "type", PacketType::CREATE_ACK },
                            { "success", false },
                            { "error", "Channel already exists" } };
        return;
    }

    channels[channel_name] = Channel();

    // send back response
    server_response = {
        { "type", PacketType::CREATE_ACK },
        { "success", true },
    };
}

void
leave_handler(const nl::json& client_request, nl::json& server_response)
{	
	std::string left_channel;
	auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (cookie_username.find(cookie) == cookie_username.end()) {
        server_response = { { "type", PacketType::LEAVE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // channel not found
    if (cookie_channel.find(cookie) == cookie_channel.end()) {
        server_response = { { "type", PacketType::LEAVE_ACK },
                            { "success", false },
                            { "error", "No active Session" } };
        return;
    }

    //find the channel left
	left_channel = cookie_channel_name[cookie];

    //Erases the cookie associated with the channel and user associated
    cookie_channel.erase(cookie);
    username_to_channel.erase(cookie_username[cookie]);
    cookie_channel_name.erase(cookie_channel_name[cookie]);

    int found = 0;

    //If no one is left in the channel
    for(std::map<std::string, std::string>::iterator it =  username_to_channel.begin(); it !=  username_to_channel.end(); it++){
    	if((*it).second == left_channel){
    		found++;
    	}
	}
    
    if(found == 0)
		channels.erase(left_channel);

    server_response = { { "type", PacketType::LEAVE_ACK  },
                        { "success", true },
                    	{ "session left", left_channel}};
    return;
}

void
logout_handler(const nl::json& client_request, nl::json& server_response)
{   
    std::string left_channel;
    auto cookie = client_request["cookie"].get<std::string>();

    //Checks if the user is even logged in, should be sincelogin and register connects client and server
    if (cookie_username.find(cookie) == cookie_username.end()) {
        server_response = { { "type", PacketType::LOGOUT_ACK },
                            { "success", false },
                            { "error", "Auth error" }};
        return;
    }

    //Delete user association to session if there is one
    if(cookie_channel.find(cookie) != cookie_channel.end()){

	    //find the channel left
	    left_channel = cookie_channel_name[cookie];

	    //Erases the cookie associated with the channel and user associated
	    cookie_channel.erase(cookie);
	    username_to_channel.erase(cookie_username[cookie]);
	    cookie_channel_name.erase(cookie_channel_name[cookie]);

	    int found = 0;

	    //If no one is left in the channel
	    for(std::map<std::string, std::string>::iterator it =  username_to_channel.begin(); it !=  username_to_channel.end(); it++){
	        if((*it).second == left_channel){
	            found++;
	        }
	    }
	     
	    if(found == 0)
	        channels.erase(left_channel);

	    //find which session or sessions client was connected to and delete that entry
	    if (cookie_channel.find(cookie) != cookie_channel.end()) 
	        cookie_channel.erase(cookie);

	}

    //erases cookie and username
    if (cookie_username.find(cookie) != cookie_username.end()) 
        cookie_username.erase(cookie);

    server_response = { { "type", PacketType::LOGOUT_ACK },
                        { "success", true }};

    return;
}

void
message_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (cookie_username.find(cookie) == cookie_username.end()) {
        server_response = { { "type", PacketType::MESSAGE_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // channel not found
    if (cookie_channel.find(cookie) == cookie_channel.end()) {
        server_response = { { "type", PacketType::MESSAGE_ACK },
                            { "success", false },
                            { "error", "Channel not found" } };
        return;
    }

    auto message = client_request["message"].get<std::string>();
    (*cookie_channel[cookie]).post(cookie_username[cookie], message);

    server_response = { { "type", PacketType::MESSAGE_ACK },
                        { "success", true } };
}

void
sync_handler(const nl::json& client_request, nl::json& server_response)
{
    auto cookie = client_request["cookie"].get<std::string>();

    // auth error
    if (cookie_username.find(cookie) == cookie_username.end()) {
        server_response = { { "type", PacketType::SYNC_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    // channel not found
    if (cookie_channel.find(cookie) == cookie_channel.end()) {
        server_response = { { "type", PacketType::SYNC_ACK },
                            { "success", false },
                            { "error", "Channel not found" } };
        return;
    }

    auto timestamp = client_request["timestamp"].get<uint64_t>();
    nl::json messages = (*cookie_channel[cookie]).get(timestamp);

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
    if (cookie_username.find(cookie) == cookie_username.end()) {
        server_response = { { "type", PacketType::LIST_ACK },
                            { "success", false },
                            { "error", "Auth error" } };
        return;
    }

    std::string list_message;

    for(std::map<std::string, std::string>::iterator it = cookie_username.begin(); it != cookie_username.end(); it++){
    	a_users.push_back((*it).second);
	}

    for(std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); it++){
    	a_channels.push_back((*it).first);
	}

    // send back response
    server_response = {
        { "type", PacketType::LIST_ACK },
        { "success", true },
        { "active users", a_users},
        { "active channels", a_channels}
    };
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
        case (PacketType::JOIN):
            join_handler(client_request, server_response);
            break;
        case (PacketType::CREATE):
            create_handler(client_request, server_response);
            break;
        case (PacketType::LIST):
            list_handler(client_request, server_response);
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
        std::cout << "Connected to " << client << "\n";
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

        std::cout << "Reading from socket " << epoll_ev->data.fd << "\n";

        // receive client request
        {
            char buf[MAXDATASIZE] = { 0 };
            int n = recv(epoll_ev->data.fd, buf, MAXDATASIZE, 0);

            std::cout << "Read " << n << " bytes" << std::endl;

            // discard spurious packets
           // discard spurious packets
            if (n < 0)
                return;

            if (n == 0){
            	std::cout << "Client : " << epoll_ev-> data.fd << " disconnected (EOF detected)" << std::endl;
            	close(epoll_ev-> data.fd);
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

            if(server_response["type"].get<PacketType>() == PacketType::LOGOUT_ACK && server_response["sucess"] == true){

            	std::cout << "Client : " << epoll_ev-> data.fd << " logged out" << std::endl;
               	close(epoll_ev->data.fd);
            }

            std::cout << "Finished sending ACK for socket " << epoll_ev->data.fd
                      << std::endl;
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

    std::cout << "Server socket is " << skfd << "\n";

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
