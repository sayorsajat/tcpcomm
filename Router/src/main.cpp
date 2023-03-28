#include "include/tcpServer.h"
#include <iostream>

int main() {
    TCPHandler handler;
    handler.BuildRouter(4554);

    return 0;
}