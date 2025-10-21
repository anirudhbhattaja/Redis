#include "../include/RedisServer.h"
#include <iostream>


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
    server_socket = socket(AF_INET,SOCK_STREAM, 0);
    if (server_socket < 0){
        std::cerr<<"Error creating server socket, less than 1\n";
        return;
    }

    int opt = 1;
    setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR, &opt ,sizeof(opt));
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket,(struct sockaddr*)&serverAddr, sizeof(serverAddr))<0){
        std::cerr<<"Error binding server socket\n";
        return;
    }

    if(listen(server_socket,10) < 0){
        std::cerr << "Error listening on Server socket \n";
        return;
    }

    std::cout << "Redis Server listening on port "<<port<<"\n";




}