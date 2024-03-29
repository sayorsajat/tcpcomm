#ifndef ROUTING_H
#define ROUTING_H
class Obj;
#include <string>
#include <vector>
#include "messagesPassing.h"

class Router {
    public:
        Router();

        void pushMessageTo(std::string message);
        void addObjectToList(Obj* object);
    private:
        std::vector<Obj*> objectIDS;
        std::vector<std::string> messagesBuff;
        char errorDetector;

        Obj* getObjectWithHostname(std::string hostname);
        std::vector<Obj*> getObjectsWithSameTopic(std::string topic);

        void sendPacket(int clientSocket, std::string packet);
        static void sendMessageToClientTask(Obj* destinationHost, std::string message);

        void handlePacket(std::string messageType, std::string & decoPacket);
};

#endif