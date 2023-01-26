#include "include/Routing.h"
#include "include/messagesPassing.h"
#include "messages.h"
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
// define name for this host
#define HOSTNAME "firstObj"

void secondReceiver(Message_T message) {
    std::cout << "receiver works!" << std::endl;

    
    //create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return;
    }

    // create a hint structure for the server
    int port = 4554; // you can change the port, but you will need to change router port also
    std::string ipAddress = "192.168.0.108";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // connect to router
    int connRes = connect(sock, (sockaddr*)&hint, sizeof(sockaddr_in));
    if(connRes == -1) {
        std::cerr << "error while connecting to router" << std::endl;
        return;
    }

    std::string resMessage = createMessage(message.src_host, "response message, do you hear me?!", message.topic, HOSTNAME);

    // send request
    int resRequestRes = send(sock, resMessage.c_str(), resMessage.size() + 1, 0);
}


int main() {
    Router router;
    Obj firstObj = Obj(router, "first");
    Obj secObj = Obj(router, "second");
    secObj.registerReceiver(router, "basic", secondReceiver);


    //create a socket for client part
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return -1;
    }

    // create a hint structure for the main router
    int port = 4554; // you can change the port, but you will need to change router port also
    std::string ipAddress = "192.168.0.108";
    sockaddr_in shint;
    shint.sin_family = AF_INET;
    shint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &shint.sin_addr);

    // connect to router
    int connRes = connect(sock, (sockaddr*)&shint, sizeof(sockaddr_in));
    if(connRes == -1) {
        std::cerr << "error while connecting to router" << std::endl;
        return -2;
    }

    std::string regMessage = createMessage(HOSTNAME, "basic");

    // register our host if it is not already
    int regRequestRes = send(sock, regMessage.c_str(), regMessage.size() + 1, 0);

    // send broadcast message because I want to test if another full client will respond to me
    std::string resMessage = createMessage("message 4 response", "basic", HOSTNAME);

    // register our host if it is not already
    int resRequestRes = send(sock, resMessage.c_str(), resMessage.size() + 1, 0);


    // -------------------------------------------------------------

    //create a server part socket (for responding)
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
        std::string messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - (message.find("/type ") + 6));
        if(messageType == "direct" || messageType == "broadcast") {
            router.pushMessageTo(message);
        }
        
    }

    // close socket
    close(clientSocket);

    return 0;
}