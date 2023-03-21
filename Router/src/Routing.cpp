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
#include <thread>
#include "include/messagesPassing.h"
#include "include/Routing.h"

Obj* Router::getObjectWithHostname(std::string hostname) {
    
    for (auto i: objectIDS) {
        if(i->hostname==hostname) {
            return i;
        };
    };
    Obj* obj = new Obj;
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

void Router::sendMessageToClientTask(Obj* destinationHost, std::string message) {

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "can't create socket" << std::endl;
        return;
    }

    // hint structure for destination end-system
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(destinationHost->port);
    inet_pton(AF_INET, destinationHost->ipAddress.c_str(), &hint.sin_addr);

    // connect to
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1) {
        std::cerr << "can't connect to socket" << std::endl;
        return;
    }

    // sending
    int sendRes = send(sock, message.c_str(), message.size() + 1, 0);
    if(sendRes == -1) {
        std::cout << "Could not send to client!" << std::endl;
    }

    close(sock);
    // delete &sock;
    // delete &hint;
    // delete &connectRes;
    // delete &sendRes;
};

void Router::handlePacket(std::string messageType) {
    if (!messagesBuff.empty()) {
        std::string decoPacket = messagesBuff[messagesBuff.size()-1];

        if(messageType == "direct") {
            std::string destinationHostID = decoPacket.substr(decoPacket.find("/dst ") + 5, (decoPacket.find("/body") - (decoPacket.find("/dst ") + 5)));
            Obj* destinationHost = getObjectWithHostname(destinationHostID);

            sendMessageToClientTask(destinationHost, decoPacket);
        } else if(messageType == "broadcast") {
            std::string topic = decoPacket.substr(decoPacket.find("/topic ") + 7, decoPacket.find("/hst ") - decoPacket.find("/topic ") + 7);
            std::vector<Obj*> objects = getObjectsWithSameTopic(topic);

            // define number of tasks to complete
            std::vector<int> tasks;
            for(int i = 0; i < objects.size(); i++) {
                tasks.push_back(i);
            }

            // create a thread pool
            std::vector<std::thread> threads;
            for(int taskID : tasks) {
                threads.emplace_back(sendMessageToClientTask, objects[taskID], decoPacket);
            }

            // wait for all threads to finish
            for(std::thread& t : threads) {
                t.join();
            }
        }
    }
};

void Router::pushMessageTo(std::string message) {
    messagesBuff.push_back(message);
    std::string messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - message.find("/type ") + 6);
    handlePacket(messageType);
};

void Router::addObjectToList(Obj* object) {
    Obj* registeredObject = getObjectWithHostname(object->hostname);

    // if object is not registered
    if(registeredObject->hostname == (std::string(1, errorDetector))) {
        objectIDS.push_back(object);
        errorDetector = ' ';

        // if client wants to subscribe to new topic
    } else if(std::find(registeredObject->topics.begin(), registeredObject->topics.end(), object->topics[0]) != registeredObject->topics.end()) {
        registeredObject->topics.push_back(object->topics[0]);
    }
}