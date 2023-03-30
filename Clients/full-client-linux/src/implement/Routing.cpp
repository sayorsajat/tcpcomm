#include <string>
#include <vector>
#include <iostream>
#include <bitset>
#include <sstream>
#include <algorithm>
#include "../include/messagesPassing.h"
#include "../include/Routing.h"

Obj* Router::getObjectWithID(std::string ID) {
    
    for (auto i: objectIDS) {
        if(i->messageSourceHostID==ID) {
            return i;
        };
    };
}

std::vector<Obj*> Router::getObjectsByAcceptedTopic(std::string topic) {
    std::vector<Obj*> objects;
    for (auto i: objectIDS) {
        for (auto j: i->receivers) {
            if(j.path == topic) {
                objects.push_back(i);
            }
        }
    }
    return objects;
}

Router::Router(Mailman & mail) : mailman(mail) {
    std::vector<std::string> messagesBuf;
    Router::messagesBuff = messagesBuf;

    std::vector<Obj*> IDS;
    Router::objectIDS = IDS;
};

void Router::handlePacket() {
    if (!messagesBuff.empty()) {
        std::string decoPacket = messagesBuff[messagesBuff.size()-1];
        std::string mBody = decoPacket.substr(decoPacket.find("/body ") + 6, (decoPacket.find("/topic") - (decoPacket.find("/body ") + 6)));
        std::string mTopic = decoPacket.substr(decoPacket.find("/topic ") + 7, (decoPacket.find("/hst") - (decoPacket.find("/topic ") + 7)));
        std::string host = decoPacket.substr(decoPacket.find("/hst ") + 5, (decoPacket.find("/nof") - (decoPacket.find("/hst ") + 5)));
        messagesBuff.pop_back();

        std::vector<Obj*> destinationHosts = getObjectsByAcceptedTopic(mTopic);

        Message_T messageSignature;
        messageSignature.body = mBody;
        messageSignature.topic = mTopic;
        messageSignature.src_host = host;

        for (auto i: destinationHosts) {
            i->receiveMessage(messageSignature, mailman);
        }
    }
};

void Router::pushMessageTo(std::string message) {
    messagesBuff.push_back(message);
    handlePacket();
};

void Router::addObjectToList(Obj* object) {
    objectIDS.push_back(object);
}