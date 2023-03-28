#include "include/tcpServer.h"
#include <iostream>
<<<<<<< Updated upstream
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <thread>

void handleClient(int clientSocket, sockaddr_in client, Router & router) {
    std::cout << "`handleClient` successfully called" << std::endl;
    // handle messages
    const int MAX_BUF_SIZE = 4096;
    std::vector<char> buf(MAX_BUF_SIZE);
    std::vector<char> receivedData;
    while(true) {
        // clear the buffer
        std::memset(buf.data(), 0, MAX_BUF_SIZE);
        // wait for a message
        int bytesRecv = recv(clientSocket, buf.data(), MAX_BUF_SIZE, 0);
        if (bytesRecv == -1) {
            std::cerr << "Connection issue" << std::endl;
            break;
        }
        if(bytesRecv == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        // accumulate received data in a buffer
        receivedData.insert(receivedData.end(), buf.begin(), buf.begin() + bytesRecv);

        // check if we have received a complete message
        auto pos = std::search(receivedData.begin(), receivedData.end(), std::begin("/nof"), std::end("/nof"));
        std::string message;
        if (pos != receivedData.end()) {
            // extract the message and remove it from the buffer
            message = std::string(receivedData.begin(), pos);
            receivedData.erase(receivedData.begin(), pos + 4); // remove "/nof" from the buffer

            // display the message
            std::cout << "Received: " << message << std::endl;
        } else if (receivedData.size() > MAX_BUF_SIZE) {
            // if the buffer is full and we haven't received a complete message,
            // assume that the data is a partial message and display it
            message = std::string(receivedData.begin(), receivedData.end());
            receivedData.clear();

            // display the partial message
            std::cout << "Received partial message: " << message << std::endl;
        }

        //std::string message = std::string(buf.data(), bytesRecv);
        std::string messageType = message.substr(message.find("/type ") + 6, message.find("/nof") - (message.find("/type ") + 6));
        if(messageType == "register") {
            std::cout << "Recognized `register` type" << std::endl;
            Obj* obj = new Obj;
            obj->hostname = message.substr(message.find("/hst ") + 5, message.find("/topic ") -( message.find("/hst ") + 5));
            std::string topic = message.substr(message.find("/topic ") + 7, message.find("/type ") - (message.find("/topic ") + 7));
            obj->topics.push_back(topic);
            char ipAddr[NI_MAXHOST];
            inet_ntop(AF_INET, &client.sin_addr, ipAddr, NI_MAXHOST);
            obj->ipAddress = ipAddr;
            obj->port = ntohs(client.sin_port);
            std::cout << "Registered object:" << std::endl 
                      << "hostname: " << obj->hostname << std::endl
                      << "IPv4" << obj->ipAddress << std::endl
                      << "port: " << obj->port << std::endl
                      << "topics: " << std::endl;
            for(auto i: obj->topics) {
                std::cout << i << ", ";
            }
            std::cout << std::endl;
            router.addObjectToList(obj);
        } else if(messageType == "direct" || messageType == "broadcast") {
            std::cout << "Recognized `direct` or `broadcast` type" << std::endl;
            router.pushMessageTo(message);
        } else {
            std::cout << "invalid message:" << std::endl
                      << message << std::endl;
        }
        
    }

    // close socket
    close(clientSocket);
}
=======
>>>>>>> Stashed changes

int main() {
    TCPHandler handler;
    handler.BuildRouter(4554);

    return 0;
}