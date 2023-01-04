#ifndef OBJ_RELATED
#define OBJ_RELATED
class Router;
#include <string>
#include <memory>
#include <iostream>
#include <bits/stdc++.h> 
#include "Routing.h"

class Message {
    public:
        std::string message;
        Message(std::string destination_sys, std::string mBody, std::string topic = "basic", std::string src_host = "");
        std::string stringToBinary(const std::string & input);
};

struct Message_T {
    std::string destination;
    std::string body;
    std::string topic;
    std::string src_host;
};

struct Receiver {
    std::string path;
    std::function<void(Message_T message)> receiver;
};

class Obj {
	public:
        std::string messageSourceHostID;

        Obj(Router & router, std::string ID);

        void registerReceiver(Router & router, std::string path, std::function<void(Message_T message)> function) {
            //unique validation
            for (auto i: receivers) {
                if(i.path == path) {
                    std::cout << "receiver for this topic already exists" << std::endl;
                    return;
                }
            }

            Receiver recv = Receiver();
            recv.path = path;
            recv.receiver = function;
            receivers.push_back(recv);
        };

        void passMessageTo(Router router, std::string destObjId, std::string messageBody, std::string messageTopic = "basic", bool provideSrcHost = false);
        void receiveMessage(Message_T message);
    private:
        std::vector<Receiver> receivers;
        std::function<void(Message_T message)> getFunctionById(std::string topic);
};

#endif
