//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_CHATCOLOR_H
#define CBUNGEE_CHATCOLOR_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <iostream>

enum ChatColor : char {
    BLACK = '0',
    DARK_BLUE = '1',
    DARK_GREEN = '2',
    DARK_CYAN = '3',
    DARK_RED = '4',
    PURPLE = '5',
    GOLD = '6',
    GRAY = '7',
    DARK_GRAY = '8',
    BLUE = '9',
    BRIGHT_GREEN = 'a',
    CYAN = 'b',
    RED = 'c',
    PINK = 'd',
    YELLOW = 'e',
    WHITE = 'f'
};

/*
     // * black
     // * dark_blue
     // * dark_green
     // * dark_aqua
     // * dark_red
     // * dark_purple
     // * gold
     // * gray
     // * dark_gray
     // * blue
     // * green
     // * aqua
     // * red
     // * light_purple
     // * yellow
     // * white
     // * obfuscated
     // * bold
     // * strikethrough
     // * underline
     // * italic
     // * reset
 */
//enumeration types (both scoped and unscoped) can have overloaded operators

inline std::ostream &operator<<(std::ostream &os, ChatColor c) {
    os << "§" << static_cast<char>(c);
    return os;
}

inline std::string &operator+(std::string &os, ChatColor c) {
    char message = static_cast<char>(c);
    os.append("§").append(&message, 1);
    return os;
}

inline std::string &operator+(ChatColor &c, std::string os) {
    char message = static_cast<char>(c);
    os.append("§").append(&message, 1);
    return os;
}

#endif //CBUNGEE_CHATCOLOR_H
