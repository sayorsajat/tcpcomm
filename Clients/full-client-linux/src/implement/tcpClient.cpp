#include "../include/tcpClient.h"
#include "../include/Routing.h"
#include "../include/messagesPassing.h"
#include "../include/messages.h"
#include "../include/mailman.h"
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>


TCPClientHandler::TCPClientHandler() {
    
}

void TCPClientHandler::handleMessage(int clientSocket, Router & router) {
    std::cout << "`handleMessage` successfully called" << std::endl;
    // handle messages
    char buf[4096];
    while(true) {
        // clear the buffer
        memset(buf, 0, 4096);
        // wait for a message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1) {
            std::cerr << "Connection issue" << std::endl;
            break;
        }
        if(bytesRecv == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        // display message
        std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;
    
    
        std::string message = std::string(buf, 0, bytesRecv);
        std::string messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - (message.find("/type ") + 6));
        if(messageType == "direct" || messageType == "broadcast") {
            router.pushMessageTo(message);
        }

        // close socket
        close(clientSocket);
    }
}

int TCPClientHandler::BuildClient(std::vector<std::string> topics, Router & router, Mailman & mailman) {    

    for(auto &i : topics) {
        mailman.subscribeToTopic(i);
    };

    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return -1;
    }

    //bind the socket to an IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(7111);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "can't bind to port" << std::endl;
        return -2;
    }
    
    //Mark socket for listening
    if(listen(listening, SOMAXCONN) == -1) {
        std::cerr << "can't listen" << std::endl;
    }

    //Accept a call
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    std::vector<std::thread> threads;

    while(true) {
        int clientSocket = accept(listening, 
                            (sockaddr*)&client, 
                            &clientSize);

        if(clientSocket == -1) {
            std::cerr << "problem with client connecting" << std::endl;
            return -4;
        }

        memset(host, 0, NI_MAXHOST);
        memset(svc, 0, NI_MAXSERV);

        int result = getnameinfo((sockaddr*)&client,
                            sizeof(client),
                            host,
                            NI_MAXHOST,
                            svc,
                            NI_MAXSERV,
                            0);
    
        if(result) {
            std::cout << host << " connected on: " << svc << std::endl;
        } else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
        }

        int clientSocketCopy = clientSocket;

        threads.emplace_back(handleMessage, std::ref(clientSocketCopy), std::ref(router));// add thread to container
    }

    // join all threads before exiting
    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
};

int TCPClientHandler::RunClient(std::vector<std::string> topics, Router & router, Mailman & mailman) {
    std::thread clientThread(BuildClient, topics, std::ref(router), std::ref(mailman));
    clientThread.join();
    return 0;
};