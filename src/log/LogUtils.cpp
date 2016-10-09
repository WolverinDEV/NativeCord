//
// Created by wolverindev on 03.10.16.
//

#include "../../include/log/LogUtils.h"
#include "../../include/utils/StringUtil.h"

using namespace std;
void printMessage(ostream& stream, string prefix, string& message){
    vector<string> strings = StringUtils::split(message, "\n");
    for(vector<string>::iterator it = strings.begin(); it != strings.end();it++) {
        stream << prefix << *it << endl;
    }
}

void logMessage(std::string message){
    printMessage(cout, "[LOG] ", message);
}
void logError(std::string message){
    printMessage(cerr, "[ERROR] ", message);
}
void debugMessage(std::string message){
    printMessage(cout, "[DEBUG] ", message);
}