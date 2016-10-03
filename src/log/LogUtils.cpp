//
// Created by wolverindev on 03.10.16.
//

#include "../../include/log/LogUtils.h"
#include "../../include/utils/StringUtil.h"

using namespace std;
void logMessage(std::string message){
    cout << "[LOG] " << message << endl;
}
void logError(std::string message){
    cout << "[ERROR] " << message << endl;
}
void debugMessage(std::string message){
    cout << "[DEBUG] " << message << endl;
}