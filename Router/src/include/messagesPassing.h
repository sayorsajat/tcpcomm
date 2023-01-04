#ifndef OBJ_RELATED
#define OBJ_RELATED
class Router;
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <bits/stdc++.h> 
#include "Routing.h"


struct Obj {
    std::string hostname;
    std::vector<std::string> topics;
    int port;
    std::string ipAddress;
};

#endif
