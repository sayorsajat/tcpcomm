#include "include/Routing.h"
#include "include/messagesPassing.h"
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

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

    int clientSocket = accept(listening, 
                            (sockaddr*)&client, 
                            &clientSize);

    if(clientSocket == -1) {
        std::cerr << "problem with client connecting" << std::endl;
        return -4;
    }

    close(listening);

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
        std::string messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - message.find("/type ") + 6);
        if(messageType == "register") {
            Obj* obj;
            obj->hostname = message.substr(message.find("/hst ") + 5, message.find("/topic ") - message.find("/hst ") + 5);
            std::string topic = message.substr(message.find("/topic ") + 7, message.find("/type ") - message.find("/topic ") + 7);
            obj->topics.push_back(topic);
            char ipAddr[NI_MAXHOST];
            inet_ntop(AF_INET, &client.sin_addr, ipAddr, NI_MAXHOST);
            obj->ipAddress = ipAddr;
            obj->port = ntohs(client.sin_port);
            router.addObjectToList(obj);
        } else if(messageType == "direct" || messageType == "broadcast") {
            router.pushMessageTo(message);
        } else {
            std::cout << "invalid message" << std::endl;
        }
        
    }

    // close socket
    close(clientSocket);

    return 0;
}