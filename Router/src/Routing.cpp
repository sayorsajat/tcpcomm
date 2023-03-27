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
    std::vector<Obj*> IDS;
    Router::objectIDS = IDS;
    Router::errorDetector;
};

void Router::sendMessageToClientTask(Obj* destinationHost, std::string message) {
    std::cout << "creating socket" << std::endl;
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "can't create socket" << std::endl;
        return;
    }

    // hint structure for destination end-system
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(7111);
    inet_pton(AF_INET, destinationHost->ipAddress.c_str(), &hint.sin_addr);
    std::cout << "created destination hint" << std::endl;

    // connect to
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1) {
        std::cerr << "can't connect to socket" << std::endl;
        return;
    }
    std::cout << "connected to destination host" << std::endl;

    // sending
    int sendRes = send(sock, message.c_str(), message.size() + 1, 0);
    if(sendRes == -1) {
        std::cout << "Could not send to client!" << std::endl;
    }
    std::cout << "sent " << sendRes << "bytes" << std::endl;

    close(sock);
    // delete &sock;
    // delete &hint;
    // delete &connectRes;
    // delete &sendRes;
};

void Router::handlePacket(std::string messageType, std::string & decoPacket) {
    if (!decoPacket.empty()) {
        std::cout << "buffer was not empty, all right" << std::endl
                  << "message type: " << messageType << std::endl;
        std::cout << "message: " << decoPacket << std::endl
                  << "size of message: " << decoPacket.size() << std::endl;
        if(messageType == "direct") {
            std::cout << "message type was recognized as `direct`" << std::endl;
            std::string destinationHostID = decoPacket.substr(decoPacket.find("/dst ") + 5, (decoPacket.find("/body") - (decoPacket.find("/dst ") + 5)));
            Obj* destinationHost = getObjectWithHostname(destinationHostID);

            std::cout << "destination host extracted" << std::endl;

            sendMessageToClientTask(destinationHost, decoPacket);
        } else if(messageType == "broadcast") {
            std::cout << "message type was recognized as `broadcast`" << std::endl;
            std::string topic = decoPacket.substr(decoPacket.find("/topic ") + 7, decoPacket.find("/hst ") - (decoPacket.find("/topic ") + 7));
            std::vector<Obj*> objects = getObjectsWithSameTopic(topic);
            std::cout << "objects with same topic extracted" << std::endl;
            std::cout << "object hostnames: ";
            for(auto j: objects) {
                std::cout << j->hostname << ", ";
            }
            std::cout << std::endl;

            // define number of tasks to complete
            if(objects.size() <= 0) {
                return;
            }
            std::vector<int> tasks;
            for(int i = 0; i < objects.size(); i++) {
                tasks.push_back(i);
            }

            std::cout << "creating thread pool" << std::endl;
            // create a thread pool
            std::vector<std::thread> threads;
            for(int taskID : tasks) {
                try {
                    threads.emplace_back(sendMessageToClientTask, objects[taskID], decoPacket);
                } catch(const std::system_error& e) {
                    std::cerr << "Error creating thread: " << e.what() << std::endl;
                }
            }
            std::cout << "created thread pool" << std::endl;

            // wait for all threads to finish
            for(std::thread& t : threads) {
                t.join();
            }
        }
    }
};

void Router::pushMessageTo(std::string message) {
    size_t posType = message.find("/type ");
    size_t posNof = message.find("/nof ");
    
    if (posType == std::string::npos || posNof == std::string::npos) {
        std::cerr << "Invalid message format: " << message << std::endl;
        return;
    }

    std::string messageType = message.substr(posType + 6, posNof - (posType + 6));
    handlePacket(messageType, message);
    std::cout << "handlePacket called successfully" << std::endl;
};

void Router::addObjectToList(Obj* object) {
    Obj* registeredObject = getObjectWithHostname(object->hostname);

    // if object is not registered
    if(registeredObject->hostname == (std::string(1, errorDetector))) {
        std::cout << "object was not registered" << std::endl;
        objectIDS.push_back(object);
        errorDetector = ' ';
        std::cout << "object was successfully added to objectIDS" << std::endl;

        // if client wants to subscribe to new topic
    } else if(std::find(registeredObject->topics.begin(), registeredObject->topics.end(), object->topics[0]) != registeredObject->topics.end()) {
        std::cout << "object requested to join new topic" << std::endl;
        registeredObject->topics.push_back(object->topics[0]);
        std::cout << "successfully added topic" << std::endl;
    }
}