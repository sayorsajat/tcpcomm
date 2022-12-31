#include <string>
#include <vector>
#include <algorithm>
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

std::vector<Obj*> Router::getObjectsWithSameTopic(std::string topic) {
    std::vector<Obj*> objects;

    for (auto i: objectIDS) {
        if(std::find(i->topics.begin(), i->topics.end(), topic) != i->topics.end()) {
            objects.push_back(i);
        };
    };

    return objects;
};

Router::Router() {
    std::vector<std::string> messagesBuf;
    Router::messagesBuff = messagesBuf;

    std::vector<Obj*> IDS;
    Router::objectIDS = IDS;
    Router::errorDetector;
};

void Router::handlePacket(std::string messageType) {
    if (!messagesBuff.empty()) {
        std::string decoPacket = messagesBuff[messagesBuff.size()-1];

        if(messageType == "direct") {
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
        } else if(messageType == "broadcast") {
            std::string topic = decoPacket.substr(decoPacket.find("/topic ") + 7, decoPacket.find("/hst ") - decoPacket.find("/topic ") + 7);
            std::vector<Obj*> objects = getObjectsWithSameTopic(topic); 
        }
    }
};

void Router::pushMessageTo(std::string message) {
    messagesBuff.push_back(message);
    std::string messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - message.find("/type ") + 6);
    handlePacket(messageType);
};

void Router::addObjectToList(Obj* object) {
    if(getObjectWithHostname(object->hostname)->hostname == (std::string(1, errorDetector))) {
        objectIDS.push_back(object);
    }
}