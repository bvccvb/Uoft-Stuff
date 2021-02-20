#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <stdbool.h>

#include "json.h"
#include "picosha2.h"
#include "protocol.h"

#define MAX_EVENTS 128
namespace nl = nlohmann;

void
login_handler(int fd, nl::json client_request, nl::json& server_response)
{
    std::string username = client_request["username"].get<std::string>();
    std::string password = client_request["password"].get<std::string>();

    password = picosha2::hash256_hex_string(password);

    std::string line;

    std::string user_comparison;    
    std::string pass_comparison;

    std::ifstream file("user_info.txt");
    if(file.is_open()){
    	//reads each line untill it finds the pass word
    	while(file >> user_comparison >> pass_comparison){
    		// std::cout << "Login handler user : " << user_comparison << " pass : " << pass_comparison << "\n";
    		if(user_comparison == username && password == pass_comparison){
    			server_response = {{"type", PacketType::LOGIN_ACK}/*, {"sucess", true}*/};
    			return;
    		}
    	}
    	file.close();
    }

    server_response = {{"type", PacketType::LOGIN_ACK}/*, {"sucess", false}*/};
    return;
}

// void logout(){

// }

// void join_session(){

// }

// void leave_session(){

// }

// void create_session(){

// }

// void list(){

// }

// void quit(){

// }

// void text(){

// }

void registration_handler(int fd, nl::json client_request, nl::json& server_response)
{
	std::string username = client_request["username"].get<std::string>();
    std::string password = client_request["password"].get<std::string>();

    password = picosha2::hash256_hex_string(password);

    std::string line;

    std::string user_comparison;    
    std::string pass_comparison;

    std::ifstream file("user_info.txt");
    if(file.is_open()){
    	//reads each line untill it finds the pass word
    	while(file >> user_comparison >> pass_comparison){
    		// std::cout << "Login handler user : " << user_comparison << " pass : " << pass_comparison << "\n";
    		if(user_comparison == username && password == pass_comparison){
    			server_response = {{"type", PacketType::REGISTER_ACK}/*, {"sucess", false}*/};
    			return;
    		}

    		file.close();
    	}
    }

    std::ofstream file_;
    file_.open("user_info.txt", std::ios::app);
    file_ << username << " " << password << std::endl;
    server_response = {{"type", PacketType::REGISTER_ACK}/*, {"sucess", true}*/};
    return;

}

// void change_pass(){

// }

void
parser(int fd, nl::json client_request, nl::json& server_response)
{
	// std::cout << "type is " << client_request["type"] << "\n";
	// std::cout << "user is " << client_request["username"] << "\n";
	// std::cout << "pass is " << client_request["password"] << "\n";


    if (client_request["type"]== PacketType::LOGIN) {
        login_handler(fd, client_request, server_response);
    } else if (client_request["type"] == PacketType::LOGOUT) {

    } else if (client_request["type"] == PacketType::LEAVE) {

    } else if (client_request["type"] == PacketType::JOIN) {

    } else if (client_request["type"] == PacketType::CREATE) {

    } else if (client_request["type"] == PacketType::LIST) {

    } else if (client_request["type"] == PacketType::MESSAGE) {

    } else if (client_request["type"] == PacketType::REGISTER) {
    	registration_handler(fd, client_request, server_response);
    } else {

    	std::cout << "INVALID COMMAND" << std::endl;

    }

    return;
}

int
main(int argc, char** argv)
{
    // Connection Establishment variables
    int port = std::atoi(argv[1]);
    int skfd;
    int client;
    struct sockaddr_in serv_addr;
    struct sockaddr_in peer_addr;
    int addrlen = sizeof(serv_addr);
    int peer_addrlen = sizeof(peer_addr);

    // Epoll variables
    int epoll_fd;
    struct epoll_event event;
    struct epoll_event epoll_event_vector[MAX_EVENTS];

    nl::json client_request;
    nl::json server_response;

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

    if ((listen(skfd, 30)) < 0) {
        perror("[!] listen() failed");
        exit(-1);
    }

    // Create epoll instance
    if ((epoll_fd = epoll_create1(0)) < 0) {
        perror("[!] epoll_create() failed");
        exit(-1);
    }

    std::cout << "Server socket is " << skfd << "\n";
    // Listen Socket is ready
    event.data.fd = skfd;

    // If the skfd is readable
    event.events = EPOLLIN | EPOLLET;

    if ((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, skfd, &event)) < 0) {
        perror("[!] epoll_ctl() failed");
        exit(-1);
    }

    // Now the program will continuously accept new connections and put them
    // into the event vector
    while (1) {

        // Wait for an event to happen, since argument 4 is unsigned -1 is super
        // large
        auto n = epoll_wait(epoll_fd, epoll_event_vector, MAX_EVENTS, -1);

        for (int i = 0; i < n; i++) {
            if (skfd == epoll_event_vector[i].data.fd) {

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

                event.data.fd = client;

                // if the client is readable
                event.events = EPOLLIN | EPOLLET;

                if ((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &event)) < 0) {
                    perror("[!] epoll_ctl() failed");
                    exit(-1);
                }
            } else {
                // The client socket is ready
                // Parse and read data

                if (epoll_event_vector[i].events && EPOLLIN) {

                    // skip empty
                    if (epoll_event_vector[i].data.ptr == NULL)
                        continue;

                    std::cout << "Reading from socket "
                              << epoll_event_vector[i].data.fd << "\n";

                    {
                        char buf[MAXDATASIZE] = { 0 };
                        int n = recv(
                          epoll_event_vector[i].data.fd, buf, MAXDATASIZE, 0);

                        std::cout << "Read " << n << " bytes" << std::endl;;
                        std::vector<uint8_t> v_cbor(buf, buf + n);
                        nl::json j = nl::json::from_cbor(v_cbor);

                        // // TODO: do stuff with j
                        // std::cout << j["username"].get<std::string>()
                        //           << std::endl;

                        // std::cout << picosha2::hash256_hex_string(
                        //                j["password"].get<std::string>())
                        //           << std::endl;

                        client_request = j;
                    }

                    {

                        parser(epoll_event_vector[i].data.fd, client_request, server_response);
 
                        std::vector<uint8_t> v_cbor(nl::json::to_cbor(server_response));

                        send(epoll_event_vector[i].data.fd,
                             v_cbor.data(),
                             v_cbor.size(),
                             0);

                        std::cout << "Finished sending ACK for socket "
                                  << epoll_event_vector[i].data.fd << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}