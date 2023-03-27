#include "include/Routing.h"
#include "include/messagesPassing.h"
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <thread>
// if (bytesRecv == -1) {
        //     std::cerr << "Connection issue" << std::endl;
        //     break;
        // }
        // if(bytesRecv == 0) {
        //     std::cout << "Client disconnected" << std::endl;
        //     break;
        // }
void handleClient(int clientSocket, sockaddr_in & client, Router & router) {
    std::cout << "`handleClient` successfully called" << std::endl;
    // handle messages
    const int MAX_BUF_SIZE = 4096;

    char buf[MAX_BUF_SIZE];
    while(true) {
        // clear the buffer
        std::memset(buf, 0, MAX_BUF_SIZE);
        // wait for a message
        int bytesRecv;
        
        std::string message = "";

        while ((bytesRecv = recv(clientSocket, buf, MAX_BUF_SIZE, 0)) > 0) {
            std::cout << "Received " << bytesRecv << " bytes: ";
            std::cout.write(buf, bytesRecv);
            std::cout << std::endl;
            message.append(buf, bytesRecv);
            std::memset(buf, 0, MAX_BUF_SIZE);
        }

        std::string messageType;
        try {
            messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - (message.find("/type ") + 6));
        } catch (const std::out_of_range& ex) {
            std::cerr << "Error: Out of range exception caught: " << ex.what() << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "Error: Exception caught: " << ex.what() << std::endl;
        }

        if(messageType == "register") {
            std::cout << "Recognized `register` type" << std::endl;
            // close socket
            close(clientSocket);
            Obj* obj = new Obj;
            obj->hostname = message.substr(message.find("/hst ") + 5, message.find("/topic ") -( message.find("/hst ") + 5));
            std::string topic = message.substr(message.find("/topic ") + 7, message.find("/type ") - (message.find("/topic ") + 7));
            obj->topics.push_back(topic);
            char ipAddr[NI_MAXHOST];
            inet_ntop(AF_INET, &client.sin_addr, ipAddr, NI_MAXHOST);
            obj->ipAddress = ipAddr;
            obj->port = ntohs(client.sin_port);
            std::cout << "Registered object:" << std::endl 
                      << "hostname: " << obj->hostname << std::endl
                      << "IPv4" << obj->ipAddress << std::endl
                      << "port: " << obj->port << std::endl
                      << "topics: " << std::endl;
            for(auto i: obj->topics) {
                std::cout << i << ", ";
            }
            std::cout << std::endl;
            router.addObjectToList(obj);
        } else if(messageType == "direct" || messageType == "broadcast") {
            std::cout << "Recognized `direct` or `broadcast` type" << std::endl;
            // close socket
            close(clientSocket);
            router.pushMessageTo(message);
        } else {
            std::cout << "invalid message:" << std::endl
                      << message << std::endl;
        }
        
    }

    // close socket
    close(clientSocket);
}

int main() {
    Router router;
    //create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return -1;
    }

    //bind the socket to an IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(4554);
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

    std::vector<std::thread> threads;  // container to store threads

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

        threads.emplace_back(handleClient, std::ref(clientSocketCopy), std::ref(client), std::ref(router));// add thread to container
    }

    // join all threads before exiting
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}