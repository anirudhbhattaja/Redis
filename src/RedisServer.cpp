#include "../include/RedisServer.h"
#include "../include/RedisCommandHandler.h"
#include <iostream>
#include <vector>
#include <thread>
#include <cstring>


#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define CLOSE_SOCKET closesocket
#else
#include <sys/socket.h>
#include <unistd.h>
#define CLOSE_SOCKET close
#include <netinet/in.h>
#endif



//The above is being used as some libraries such as sys socket and unistd dont work on windows
//currently im developing on windows, incase it needs to be run on linux the others can be used

static RedisServer* globalServer = nullptr;

RedisServer::RedisServer(int port) : port(port), server_socket(-1), running(true) {
    globalServer = this;
}

void RedisServer::shutdown(){
    running = false;
    if (server_socket != -1){
        CLOSE_SOCKET(server_socket);
    }
    std::cout<<"Shutting down server running on "<<server_socket<<"\n";
}

void RedisServer::run(){
    server_socket = socket(AF_INET,SOCK_STREAM, 0); //Use IPv4,use tcp, default is 0 for tcp
    if (server_socket < 0){
        std::cerr<<"Error creating server socket, less than 1\n";
        return;
    }

    int opt = 1;
    setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR, &opt ,sizeof(opt));
    //the socket you created,level of the option in this case socket, allows socket to bind to an address that was recently used, pointer to value to set, lenght of value
    
    //We are using the above because this way we can avoid errors if we restart server quickly
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;


    //We use this to tell that we want our socket to listen on this port and address
    if (bind(server_socket,(struct sockaddr*)&serverAddr, sizeof(serverAddr))<0){
        std::cerr<<"Error binding server socket\n";
        return;
    }

    if(listen(server_socket,10) < 0){
        std::cerr << "Error listening on Server socket \n";
        return;
    }

    std::cout << "Redis Server listening on port "<<port<<"\n";

    std::vector<std::thread> threads;
    RedisCommandHandler cmdHanlder;

    while (running){
        int client_socket = accept(server_socket,nullptr,nullptr);
        if (client_socket < 0){
            if(running){
                std::cerr<<"Error accepting client connection\n";
            }
            break;
        }
        threads.emplace_back([client_socket, &cmdHanlder](){
            char buffer[1024];
            while(true){
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer) - 1,0);
                if(bytes <= 0){
                    break;
                }
                std::string request(buffer, bytes);
                std::string response = cmdHanlder.processCommand(request);
                send(client_socket,response.c_str(),response.size(),0);
            }
            CLOSE_SOCKET(client_socket);
        });
    }

    for(auto& t : threads){
        if (t.joinable()) t.join();
    }




}