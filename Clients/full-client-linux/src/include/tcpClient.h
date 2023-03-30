#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <string>
class Router;
#include "Routing.h"
#include <netinet/in.h>
#include <vector>

class TCPClientHandler {
    public:
        TCPClientHandler();

        int RunClient(std::vector<std::string> topics, Router & router, Mailman & mailman);
    private:
        static int BuildClient(std::vector<std::string> topics, Router & router, Mailman & mailman);
        static void handleMessage(int clientSocket, Router & router);
};

#endif