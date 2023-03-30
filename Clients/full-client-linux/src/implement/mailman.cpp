#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include "../include/mailman.h"
#include "../include/messages.h"

Mailman::Mailman(unsigned int routerPort, std::string routerIPAddress, std::string hostname) {
    Mailman::HOSTNAME = hostname;
    // create a hint structure for the main router
    int port = routerPort;
    std::string ipAddress = routerIPAddress;
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    Mailman::shint = hint;
}

int Mailman::subscribeToTopic(std::string topic) {
    //create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return -1;
    }
    // connect to router
    int connRes = connect(sock, (sockaddr*)&shint, sizeof(sockaddr_in));
    if(connRes == -1) {
        std::cerr << "error while connecting to router" << std::endl;
        return -2;
    }

    std::string regMessage = createRegMessage(HOSTNAME, topic);

    // register our host if it is not already
    int regRequestRes = send(sock, regMessage.c_str(), regMessage.size(), 0);

    close(sock);
    return regRequestRes;
};

int Mailman::sendBroadcastMessage(std::string message, std::string topic) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return -1;
    };
    // connect to router
    int connRes = connect(sock, (sockaddr*)&shint, sizeof(sockaddr_in));
    if(connRes == -1) {
        std::cerr << "error while connecting to router" << std::endl;
        return -2;
    }

    // send broadcast message because I want to test if another full client will respond to me
    std::string resMessage = createBroadcastMessage(message, topic, HOSTNAME);

    int resRequestRes = send(sock, resMessage.c_str(), resMessage.size(), 0);
    return resRequestRes;
};

int Mailman::sendDirectMessage(std::string message, std::string topic, std::string destinationHostname) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return -1;
    };
    // connect to router
    int connRes = connect(sock, (sockaddr*)&shint, sizeof(sockaddr_in));
    if(connRes == -1) {
        std::cerr << "error while connecting to router" << std::endl;
        return -2;
    }

    // send broadcast message because I want to test if another full client will respond to me
    std::string resMessage = createDirectMessage(destinationHostname, message, topic, HOSTNAME);

    int resRequestRes = send(sock, resMessage.c_str(), resMessage.size(), 0);
    return resRequestRes;
};