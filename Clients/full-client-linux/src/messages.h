#ifndef MESSAGE_T
#define MESSAGE_T
#include <string>
using std::string;
// /type direct/dst destination-object-name/body 5 moles ethyl detected/topic getting ready/hst hostname/nof
// /type broadcost/body 5 moles ethyl detected/topic getting ready/hst hostname/nof
// register me/hst hostname/nof

string createMessage(string type, string destHost, string body, string topic, string srcHost) {
    return "/type " + type + "/dst " + destHost + "/body " + body + "/topic " + topic + "/hst " + srcHost + "/nof"; 
};
string createMessage(string type, string body, string topic, string srcHost) {
    return "/type " + type + "/body " + body + "/topic " + topic + "/hst " + srcHost + "/nof"; 
};
string createMessage(string srcHost, string topic) {
    return "/hst " + srcHost + "/topic " + topic + "/type register/nof";
};

#endif