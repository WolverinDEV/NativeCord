//
// Created by wolverindev on 23.08.16.
//

#include "ChatMessage.h"
#include "../json/json.hpp"

using namespace std;
using json = nlohmann::json;
std::string ChatMessage::toString() {
    json out;
    out["text"] = this->message.c_str();
    if(this->bold)
        out["bold"] = 1;
    if(this->italic)
        out["italic"] = 1;
    if(this->random)
        out["obfuscated"] = 1;
    if(this->strikethrough)
        out["strikethrough"] = 1;
    if(this->underlined)
        out["underlined"] = 1;
    switch (this->color){
        case ChatColor::BLACK:
            out["color"] = "black";
            break;
        case ChatColor::DARK_BLUE:
            out["color"] = "dark_blue";
            break;
        case ChatColor::DARK_GREEN:
            out["color"] = "dark_green";
            break;
        case ChatColor::DARK_CYAN:
            out["color"] = "dark_aqua";
            break;
        case ChatColor::DARK_RED:
            out["color"] = "dark_red";
            break;
        case ChatColor::PURPLE:
            out["color"] = "dark_purple";
            break;
        case ChatColor::GOLD:
            out["color"] = "gold";
            break;
        case ChatColor::GRAY:
            out["color"] = "gray";
            break;
        case ChatColor::DARK_GRAY:
            out["color"] = "dark_gray";
            break;
        case ChatColor::BLUE:
            out["color"] = "blue";
            break;
        case ChatColor::BRIGHT_GREEN:
            out["color"] = "green";
            break;
        case ChatColor::CYAN:
            out["color"] = "aqua";
            break;
        case ChatColor::RED:
            out["color"] = "red";
            break;
        case ChatColor::PINK:
            out["color"] = "light_purple";
            break;
        case ChatColor::YELLOW:
            out["color"] = "yellow";
            break;
    }
    int index = 0;
    for(std::vector<ChatMessage*>::iterator it = this->cildren.begin(); it != this->cildren.end(); ++it) {
        /* std::cout << *it; ... */
        out["extra"][index++] = json::parse((*it)->toString().c_str());
    }
    if(this->hover != NULL){
        if(strcmp("show_achievement",this->hover->getAction().c_str()) != 0 && this->hover->getValue().c_str()[0] == '{')
            out["hoverEvent"] = {{"action",this->hover->getAction()},{"value",json::parse(this->hover->getValue())}};
        else
           out["hoverEvent"] = {{"action",this->hover->getAction()},{"value",this->hover->getValue()}};
    }
    return out.dump();
}
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

/*
 *    "bold":true,
     //Controls whether the text is italic or not. Default false
     "italic":false,
     //Controls whether the text is underlined or not. Default false
     "underlined": false,
     //Controls whether the text is striked out or not. Default false
     "strikethrough": false,
     //Controls whether the text is randomised constantly or not. Default false
     "obfuscated": false,
 */