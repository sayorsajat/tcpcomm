#ifndef ROUTING_H
#define ROUTING_H
class Obj;
#include <string>
#include <vector>
#include "messagesPassing.h"
#include  "mailman.h"

struct ObjToTopic {
    Obj* obj;
    std::vector<std::string> topics;
};

class Router {
    public:
        Router(Mailman & mail);

        void pushMessageTo(std::string message);
        void addObjectToList(Obj* object);
    private:
        std::vector<Obj*> objectIDS;
        std::vector<std::string> messagesBuff;
        Mailman & mailman;

        Obj* getObjectWithID(std::string ID);
        std::vector<Obj*> getObjectsByAcceptedTopic(std::string topic);

        void handlePacket();
};

#endif