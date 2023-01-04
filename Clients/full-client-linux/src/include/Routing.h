#ifndef ROUTING_H
#define ROUTING_H
class Obj;
#include <string>
#include <vector>
#include "messagesPassing.h"

struct ObjToTopic {
    Obj* obj;
    std::vector<std::string> topics;
};

class Router {
    public:
        Router();

        void pushMessageTo(std::string message);
        void addObjectToList(Obj* object);
        void linkObjectToTopic(Obj* object, std::string topic);
    private:
        std::vector<Obj*> objectIDS;
        std::vector<ObjToTopic> objectRelations;
        std::vector<std::string> messagesBuff;

        Obj* getObjectWithID(std::string ID);
        std::vector<Obj*> getObjectsByAcceptedTopic(std::string topic);

        void handlePacket();
};

#endif