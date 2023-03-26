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

void handleClient(int clientSocket, sockaddr_in client, Router & router) {
    std::cout << "`handleClient` successfully called" << std::endl;
    // handle messages
    const int MAX_BUF_SIZE = 4096;
    std::vector<char> buf(MAX_BUF_SIZE);
    std::vector<char> receivedData;
    while(true) {
        // clear the buffer
        std::memset(buf.data(), 0, MAX_BUF_SIZE);
        // wait for a message
        int bytesRecv = recv(clientSocket, buf.data(), MAX_BUF_SIZE, 0);
        if (bytesRecv == -1) {
            std::cerr << "Connection issue" << std::endl;
            break;
        }
        if(bytesRecv == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        // accumulate received data in a buffer
        receivedData.insert(receivedData.end(), buf.begin(), buf.begin() + bytesRecv);

        // check if we have received a complete message
        auto pos = std::search(receivedData.begin(), receivedData.end(), std::begin("/nof"), std::end("/nof"));
        std::string message;
        if (pos != receivedData.end()) {
            // extract the message and remove it from the buffer
            message = std::string(receivedData.begin(), pos);
            receivedData.erase(receivedData.begin(), pos + 4); // remove "/nof" from the buffer

            // display the message
            std::cout << "Received: " << message << std::endl;
        } else if (receivedData.size() > MAX_BUF_SIZE) {
            // if the buffer is full and we haven't received a complete message,
            // assume that the data is a partial message and display it
            message = std::string(receivedData.begin(), receivedData.end());
            receivedData.clear();

            // display the partial message
            std::cout << "Received partial message: " << message << std::endl;
        }

        //std::string message = std::string(buf.data(), bytesRecv);
        std::string messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - (message.find("/type ") + 6));
        if(messageType == "register") {
            std::cout << "Recognized `register` type" << std::endl;
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