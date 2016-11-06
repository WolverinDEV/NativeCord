//
// Created by wolverindev on 06.11.16.
//

#include "../../include/log/Terminal.h"

termios Terminal::orig_termios;
Terminal* Terminal::instance = nullptr;