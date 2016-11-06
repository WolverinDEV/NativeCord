//
// Created by wolverindev on 03.10.16.
//

#include "../../include/log/LogUtils.h"
#include "../../include/utils/StringUtil.h"
#include "../../include/log/Terminal.h"

using namespace std;
pthread_mutex_t writeMutex = PTHREAD_MUTEX_INITIALIZER;

void printAnsi(string message,string defaultBefore = ""){
    stringstream stream;
    pthread_mutex_lock(&writeMutex);
    vector<string> parts = StringUtils::split(message, "§");

    bool first = true;
    for(vector<string>::iterator it = parts.begin(); it != parts.end();it++) {
        int index = 0;
        if(!first){
            index = 1;
            string first = (*it).substr(0,1);
            //string second = (*it).substr(1,2);
            switch (tolower(first[0])){
                case '0':
                    stream << ANSI_BLACK << defaultBefore;
                    break;
                case '1':
                    stream << ANSI_BLUE << defaultBefore;
                    break;
                case '2':
                    stream << ANSI_GREEN << defaultBefore;
                    break;
                case '3':
                    stream << ANSI_CYAN << defaultBefore;
                    break;
                case '4':
                    stream << ANSI_RED << defaultBefore; break;
                case '5':
                    stream << ANSI_PURPLE << defaultBefore; break;
                case '6':
                    stream << ANSI_BROWN << defaultBefore; break;
                case '7':
                    stream << ANSI_GRAY << defaultBefore; break;
                case '8':
                    stream << ANSI_DARK_GREY << defaultBefore; break;
                case '9':
                    stream << ANSI_LIGHT_BLUE << defaultBefore; break;
                case 'a':
                    stream << ANSI_LIGHT_GREEN << defaultBefore; break;
                case 'b':
                    stream << ANSI_LIGHT_CYAN << defaultBefore; break;
                case 'c':
                    stream << ANSI_LIGHT_RED << defaultBefore; break;
                case 'd':
                    stream << ANSI_LIGHT_PURPLE << defaultBefore; break;
                case 'e':
                    stream << ANSI_YELLOW << defaultBefore; break;
                case 'f':
                    stream << ANSI_WHITE << defaultBefore; break;
                case 'n':
                    stream << ANSI_UNDERLINE << defaultBefore; break;
                case 'm':
                    stream << ANSI_UNDERLINE << ANSI_REVERSE << defaultBefore; break;
                case 'o':
                    break;
                case 'l':
                    stream << ANSI_BOLD << defaultBefore;
                    break;
                case 'r':
                    stream << ANSI_RESET << defaultBefore;
                    break;
                default:
                    stream << "§";
                    index = 0;
            }
        }
        else
            first = false;
        stream << (*it).substr(index);
    }
    stream << ANSI_RESET;
    Terminal::instance->printMessage(stream.str());
    pthread_mutex_unlock(&writeMutex);
}

void printMessage(string prefix, string& message,string def = ""){
    vector<string> strings = StringUtils::split(message, "\n");
    for(vector<string>::iterator it = strings.begin(); it != strings.end();it++) {
        printAnsi(prefix + *it, def);
    }
}

void logMessage(std::string message){
    printMessage("§7[§6§lLOG§7] §r", message);
}
void logError(std::string message){
    printMessage("§7[§cERROR§7] §r", message);
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
    logError(string("§7[§5JAVA§7] §r")+env->GetStringUTFChars(message, 0));
}