#include <string>
#include <vector>
#include <iostream>
#include <bitset>
#include <sstream>
#include <algorithm>
#include "include/messagesPassing.h"
#include "include/Routing.h"

Obj* Router::getObjectWithID(std::string ID) {
    
    for (auto i: objectIDS) {
        if(i->messageSourceHostID==ID) {
            return i;
        };
    };
}

std::vector<Obj*> Router::getObjectsByAcceptedTopic(std::string topic) {
    std::vector<Obj*> objects;
    for (auto i: objectRelations) {
        if (std::find(i.topics.begin(), i.topics.end(), topic) != i.topics.end()) {
            objects.push_back(i.obj);
        }
    }
    return objects;
}

Router::Router() {
    std::vector<std::string> messagesBuf;
    Router::messagesBuff = messagesBuf;

    std::vector<Obj*> IDS;
    Router::objectIDS = IDS;
    std::vector<ObjToTopic> relations;
    Router::objectRelations = relations;
};

void Router::linkObjectToTopic(Obj* object, std::string topic) {
    ObjToTopic relation;
    relation.obj = object;
    for (auto i: objectRelations) {
        if (std::find(i.topics.begin(), i.topics.end(), topic) != i.topics.end()) {
            std::cout << "topic already assigned";
        }
        if(i.obj == object) {
            i.topics.push_back(topic);
        } else {
            relation.topics.push_back(topic);
            objectRelations.push_back(relation);
        }
    }
};

void Router::handlePacket() {
    if (!messagesBuff.empty()) {
        std::string decoPacket = messagesBuff[messagesBuff.size()-1];
        std::string destinationHostID = decoPacket.substr(decoPacket.find("/dst ") + 5, (decoPacket.find("/body") - (decoPacket.find("/dst ") + 5)));
        std::string mBody = decoPacket.substr(decoPacket.find("/body ") + 6, (decoPacket.find("/topic") - (decoPacket.find("/body ") + 6)));
        std::string mTopic = decoPacket.substr(decoPacket.find("/topic ") + 7, (decoPacket.find("/hst") - (decoPacket.find("/topic ") + 7)));
        std::string host = decoPacket.substr(decoPacket.find("/hst ") + 5, (decoPacket.find("/nof") - (decoPacket.find("/hst ") + 5)));
        messagesBuff.pop_back();

        Obj* destinationHost = getObjectWithID(destinationHostID);

        Message_T messageSignature;
        messageSignature.destination = destinationHostID;
        messageSignature.body = mBody;
        messageSignature.topic = mTopic;
        messageSignature.src_host = host;

        destinationHost->receiveMessage(messageSignature);
    }
};

void Router::pushMessageTo(std::string message) {
    messagesBuff.push_back(message);
    handlePacket();
};

void Router::addObjectToList(Obj* object) {
    objectIDS.push_back(object);
}