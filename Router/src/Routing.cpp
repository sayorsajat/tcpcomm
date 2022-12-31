#include <string>
#include <vector>
#include <iostream>
#include <bitset>
#include <sstream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "include/messagesPassing.h"
#include "include/Routing.h"

Obj* Router::getObjectWithHostname(std::string hostname) {
    
    for (auto i: objectIDS) {
        if(i->hostname==hostname) {
            return i;
        };
    };
    Obj* obj;
    errorDetector = 'a' + std::rand()%26;
    obj->hostname = errorDetector;
    return obj;
}

Router::Router() {
    std::vector<std::string> messagesBuf;
    Router::messagesBuff = messagesBuf;

    std::vector<Obj*> IDS;
    Router::objectIDS = IDS;
    Router::errorDetector;
};

void Router::handlePacket() {
    if (!messagesBuff.empty()) {
        std::string decoPacket = messagesBuff[messagesBuff.size()-1];
        std::string destinationHostID = decoPacket.substr(decoPacket.find("/dst ") + 5, (decoPacket.find("/body") - (decoPacket.find("/dst ") + 5)));

        Obj* destinationHost = getObjectWithHostname(destinationHostID);

        // Create socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "can't create socket" << std::endl;
            return;
        }

        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(destinationHost->port);
        inet_pton(AF_INET, destinationHost->ipAddress.c_str(), &hint.sin_addr);

        int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
        if (connectRes == -1) {
            std::cerr << "can't connect to socket" << std::endl;
            return;
        }

        int sendRes = send(sock, decoPacket.c_str(), decoPacket.size() + 1, 0);
        if(sendRes == -1) {
            std::cout << "Could not send to client!" << std::endl;
        }
    
        close(sock);
    }
};

void Router::pushMessageTo(std::string message) {
    messagesBuff.push_back(message);
    handlePacket();
};

void Router::addObjectToList(Obj* object) {
    if(getObjectWithHostname(object->hostname)->hostname == (std::string(1, errorDetector))) {
        objectIDS.push_back(object);
    }
}