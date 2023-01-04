#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "messages.h"
// define name for this host
#define HOSTNAME "basic-data-provider"

int main() {
    //create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cerr << "error while creating socket" << std::endl;
        return -1;
    }

    // create a hint structure for the server
    int port = 4554; // you can change the port, but you will need to change router port also
    std::string ipAddress = "YOUR ROUTER IP ADDRESS";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // connect to router
    int connRes = connect(sock, (sockaddr*)&hint, sizeof(sockaddr_in));
    if(connRes == -1) {
        std::cerr << "error while connecting to router" << std::endl;
        return -2;
    }

    std::string regMessage = createMessage(HOSTNAME);

    // register host if it is not already
    int regRequestRes = send(sock, regMessage.c_str(), regMessage.size() + 1, 0);

    // send data
    while(true) {
        // specification of message
        // if you wish, you could change it to any string
        // you can even make it dynamically change on some internal data
        std::string broadMessage = createMessage("broadcast", "data-provider message", "global", HOSTNAME);
        
        // send
        int sendRes = send(sock, broadMessage.c_str(), broadMessage.size() + 1, 0);
        if(sendRes == -1) {
            std::cerr << "error occurred while sending packet!" << std::endl;
            continue;
        }
    }

    // close socket
    close(sock);

    return 0;
}