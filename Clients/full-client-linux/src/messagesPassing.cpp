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

void Obj::passMessageTo(Router router, std::string destObjId, std::string messageBody, std::string messageTopic, bool provideSrcHost) {
    if(!provideSrcHost) {
        Message messageObject(destObjId, messageBody, messageTopic);
        std::string strMessage = messageObject.message;
        router.pushMessageTo(strMessage);
    } else {
        Message messageObject(destObjId, messageBody, messageTopic, messageSourceHostID);
        std::string strMessage = messageObject.message;
        router.pushMessageTo(strMessage);
    }
};

Message::Message(std::string destination_sys, std::string mBody, std::string topic, std::string src_host) : message("/dst " + destination_sys + "/body " + mBody + "/topic " + topic + "/hst " + src_host + "/nof") {
}
