//
// Created by wolverindev on 03.10.16.
//

#include <CXXTerminal/Terminal.h>
#include "../../include/log/LogUtils.h"
#include "../../include/utils/StringUtil.h"

using namespace std;

void printAnsi(string message){
    Terminal::getInstance()->writeMessage(message);
}

void printMessage(string prefix, string& message,string def = ""){
    vector<string> strings = StringUtils::split(message, "\n");
    for(vector<string>::iterator it = strings.begin(); it != strings.end();it++) {
        printAnsi(prefix + *it);
    }
}

void logMessage(std::string message){
    printMessage("§7[§6§lLOG§7] §r", message);
}
void logError(std::string message){
    printMessage("§7[§cERROR§7] §r"ANSI_RED, message);
}
void logFatal(std::string message){
    printMessage("§7[§4"+string(ANSI_BOLD)+ANSI_REVERSE+"FATAL§7] §r", message);
}
void debugMessage(std::string message){
    printMessage("§7[§3DEBUG§7] §r", message);
}


void NATIVE_logMessage(JNIEnv* env,jclass,jstring message){
    logMessage(string("§7[§5JAVA§7] §f")+env->GetStringUTFChars(message, 0));
}
void NATIVE_debugMessage(JNIEnv* env,jclass,jstring message){
    debugMessage(string("§7[§5JAVA§7] §r")+env->GetStringUTFChars(message, 0));
}
void NATIVE_logError(JNIEnv* env,jclass,jstring message){
    logError(string("§7[§5JAVA§7] §r"ANSI_RED)+env->GetStringUTFChars(message, 0));
}