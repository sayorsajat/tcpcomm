#include "./include/Routing.h"
#include "./include/messagesPassing.h"
#include "./include/tcpClient.h"
#include "./include/mailman.h"
#include "./include/messages.h"
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
// define name for this host
#define HOSTNAME "firstObj"

void secondReceiver(Message_T message, Mailman & mailman) {
    std::cout << "receiver works!" << std::endl;

    int res = mailman.sendDirectMessage("response message, do you hear me?!", message.topic, message.src_host);
}


int main() {
    
    TCPClientHandler handler;
    
    Mailman mailman(4554, "192.168.0.108", HOSTNAME);
    Router router(mailman);

    Obj secObj = Obj(router, "second");
    secObj.registerReceiver(router, "basic", secondReceiver);

    std::vector<std::string> topicsToJoin;
    topicsToJoin = {"basic", "anothertopic"};

    handler.RunClient(topicsToJoin, router, mailman);
    std::cout << "it works!" << std::endl;

    return 0;
}