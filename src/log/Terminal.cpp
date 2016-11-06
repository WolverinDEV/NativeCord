//
// Created by wolverindev on 06.11.16.
//

#include "../../include/log/Terminal.h"
#include "../../NativeCord.h"

termios Terminal::orig_termios;
Terminal* Terminal::instance = nullptr;

void Terminal::handleLine(string message) {
    printMessage("Having line: "+message, false);
    if(message.compare("stop") == 0 || message.compare("end") == 0 || message.compare("exit") == 0){
        printMessage("§cStopping nativecord");
        NativeCord::exitNativeCoord();
        return;
    }
}