#ifndef TCP_H
#define TCP_H
class Obj;
#include <string>
#include <vector>
#include "messagesPassing.h"
#include <netinet/in.h>

class TCPHandler {
    public:
        TCPHandler();

        int BuildRouter(unsigned int port);        
    private:
        static void handleClient(int clientSocket, sockaddr_in & client, Router & router);
};

#endif