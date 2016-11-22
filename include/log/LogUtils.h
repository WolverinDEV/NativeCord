//
// Created by wolverindev on 03.10.16.
//

#ifndef NATIVECORD_LOGUTILS_H
#define NATIVECORD_LOGUTILS_H

#include <CXXTerminal/Terminal.h>
#include <string>
#include <jni.h>

void logError(std::string message);
void logMessage(std::string message);
void debugMessage(std::string message);
void logFatal(std::string message);

void NATIVE_logMessage(JNIEnv*,jclass,jstring);
void NATIVE_debugMessage(JNIEnv*,jclass,jstring);
void NATIVE_logError(JNIEnv*,jclass,jstring);
#endif //NATIVECORD_LOGUTILS_H
