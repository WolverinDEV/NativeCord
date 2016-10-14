//
// Created by wolverindev on 23.08.16.
//

#include "../../include/chat/ChatMessage.h"
#include "../../include/json/json.hpp"
using namespace std;

string ChatMessage::DEFAULT = string("");
int ChatMessage::count = 0;

std::string ChatMessage::toString() {
    json out;
    if(!this->message.empty())
        out["text"] = this->message;
    else
        out["text"] = "";
    if(this->bold)
        out["bold"] = true;
    if(this->italic)
        out["italic"] = true;
    if(this->random)
        out["obfuscated"] = true;
    if(this->strikethrough)
        out["strikethrough"] = true;
    if(this->underlined)
        out["underlined"] = true;
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
    if(this->click != NULL){
        out["clickEvent"] = {{"action",this->click->getAction()},{"value",this->click->getValue()}};
    }
    return out.dump();
}

ChatMessage::ChatMessage(json raw) : ChatMessage(){
    if(raw.count("text") == 1) {
        string m = raw["text"];
        setMessage(m);
    }

    bold = raw.count("bold") == 1 && raw["bold"];
    italic = raw.count("italic") == 1 && raw["italic"];
    random = raw.count("obfuscated") == 1 && raw["obfuscated"];
    underlined = raw.count("underlined") == 1 && raw["underlined"];
    strikethrough = raw.count("strikethrough") == 1 && raw["strikethrough"];

    if(raw.count("color") == 1){
        string scolor = raw["color"];
        const char* ccolor = scolor.c_str();
        if(strcmp("black",ccolor) == 0)
            color = ChatColor::BLACK;
        else if(strcmp("dark_blue",ccolor) == 0)
            color = ChatColor::DARK_BLUE;
        else if(strcmp("dark_green",ccolor) == 0)
            color = ChatColor::DARK_GREEN;
        else if(strcmp("dark_aqua",ccolor) == 0)
            color = ChatColor::DARK_CYAN;
        else if(strcmp("dark_red",ccolor) == 0)
            color = ChatColor::DARK_RED;
        else if(strcmp("dark_purple",ccolor) == 0)
            color = ChatColor::PURPLE;
        else if(strcmp("gold",ccolor) == 0)
            color = ChatColor::GOLD;
        else if(strcmp("gray",ccolor) == 0)
            color = ChatColor::GRAY;
        else if(strcmp("dark_gray",ccolor) == 0)
            color = ChatColor::DARK_GRAY;
        else if(strcmp("blue",ccolor) == 0)
            color = ChatColor::BLUE;
        else if(strcmp("green",ccolor) == 0)
            color = ChatColor::BRIGHT_GREEN;
        else if(strcmp("aqua",ccolor) == 0)
            color = ChatColor::CYAN;
        else if(strcmp("red",ccolor) == 0)
            color = ChatColor::RED;
        else if(strcmp("light_purple",ccolor) == 0)
            color = ChatColor::PINK;
        else
            color = ChatColor::WHITE;
    }

    if(raw.count("extra") == 1) {
        vector<json> extras = raw["extra"];
        for(std::vector<json>::iterator it = extras.begin(); it != extras.end(); ++it) {
            this->cildren.push_back(new ChatMessage(*it));
        }
    }

    if(raw.count("hoverEvent") == 1){
        json hover = raw["hoverEvent"];
        json j = hover["value"];
        if(j.is_string())
            this->hover = new ActionEvent(hover["action"],hover["value"]);
        else
            this->hover = new ActionEvent(hover["action"],j.dump());
    }

    if(raw.count("clickEvent") == 1){
        json click = raw["clickEvent"];
        json j = click["value"];
        if(j.is_string())
            this->click = new ActionEvent(click["action"],click["value"]);
        else
            this->click = new ActionEvent(click["action"],j.dump());
    }
}