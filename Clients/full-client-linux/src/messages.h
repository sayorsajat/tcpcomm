#ifndef MESSAGE_T
#define MESSAGE_T
#include <string>
using std::string;
// /dst destination-object-name/body 5 moles ethyl detected/topic getting ready/hst hostname/type direct/nof
// /body 5 moles ethyl detected/topic getting ready/hst hostname/type broadcast/nof
// /hst hostname/topic getting ready/type register/nof

string createDirectMessage(string destHost, string body, string topic, string srcHost) {
    return "/dst " + destHost + "/body " + body + "/topic " + topic + "/hst " + srcHost + "/type direct/nof";
};
string createBroadcastMessage(string body, string topic, string srcHost) {
    return "/body " + body + "/topic " + topic + "/hst " + srcHost + "/type broadcast/nof"; 
};
string createMessage(string srcHost, string topic) {
    return "/hst " + srcHost + "/topic " + topic + "/type register/nof";
};

// /hst firstObj/topic basic/type register/nof

#endif