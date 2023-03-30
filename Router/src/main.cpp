#include "include/tcpServer.h"

int main() {
    TCPHandler handler;
    // call BuildRouter method with port to listen as parameter
    handler.BuildRouter(4554);

    return 0;
}