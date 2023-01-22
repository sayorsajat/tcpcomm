#include <string>
#include <sstream>
#include <bitset>
#include <iostream>
#include "include/messagesPassing.h"
#include "include/Routing.h"
///dst destination-object-name/body 5 moles ethyl detected/topic getting ready/hst hostname/nof

Obj::Obj(Router & router, std::string ID) {
    Obj::messageSourceHostID = ID;
    router.addObjectToList(this);

    std::vector<Receiver> receiversBuf;
    Obj::receivers = receiversBuf;

};

std::function<void(Message_T message)> Obj::getFunctionById(std::string topic) {
    for (auto i: receivers) {
        if(i.path == topic) {
            return i.receiver;
        }
    }
}

void Obj::receiveMessage(Message_T message) {
    std::function<void(Message_T message)> receiver = getFunctionById(message.topic);
    receiver(message);
};

Message::Message(std::string destination_sys, std::string mBody, std::string topic, std::string src_host) : message("/dst " + destination_sys + "/body " + mBody + "/topic " + topic + "/hst " + src_host + "/nof") {
}
