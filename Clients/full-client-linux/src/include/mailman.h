#ifndef MAILMAN_H
#define MAILMAN_H

#include <netinet/in.h>
#include <string>


class Mailman {
    public:
        Mailman(unsigned int routerPort, std::string routerIPAddress, std::string hostname);
        
        int subscribeToTopic(std::string topic);
        int sendBroadcastMessage(std::string message, std::string topic);
        int sendDirectMessage(std::string message, std::string topic, std::string destinationHostname);
    private:
        std::string HOSTNAME;
        sockaddr_in shint;
};

#endif