//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_CHATMESSAGE_H
#define CBUNGEE_CHATMESSAGE_H

#include <string>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>
#include "ChatColor.h"
#include "HoverEvent.h"
#include <algorithm>
#include "../json/json.hpp"

using json = nlohmann::json;
class ChatMessage {
public:
    ChatMessage() : ChatMessage(std::string("")){
    }
    ChatMessage(std::string message) : message(message){
    }
    ChatMessage(const char* message) : message(string(message)){
    }
    ChatMessage(json raw);

    ~ChatMessage(){
        delete hover;
        for(std::vector<ChatMessage*>::iterator it = this->cildren.begin(); it != this->cildren.end(); ++it) {
            delete *it;
        }
    }

    void addSibling(ChatMessage* message){
        this->cildren.push_back(message);
    }
    void removeSibling(ChatMessage* message){
        auto it = std::find(cildren.begin(), cildren.end(), message);
        if(it != cildren.end())
            cildren.erase(it);
    }

    std::string toString();

    ChatColor getColor() const {
        return color;
    }

    void setColor(ChatColor color) {
        ChatMessage::color = color;
    }

    bool isRandom() const {
        return random;
    }

    void setRandom(bool random) {
        ChatMessage::random = random;
    }

    bool isBold() const {
        return bold;
    }

    void setBold(bool bold) {
        ChatMessage::bold = bold;
    }

    bool isStrikethrough() const {
        return strikethrough;
    }

    void setStrikethrough(bool strikethrough) {
        ChatMessage::strikethrough = strikethrough;
    }

    bool isItalic() const {
        return italic;
    }

    void setItalic(bool italic) {
        ChatMessage::italic = italic;
    }

    bool isUnderlined() const {
        return underlined;
    }

    void setUnderlined(bool underlined) {
        ChatMessage::underlined = underlined;
    }

    const string &getMessage() const {
        return message;
    }

    void setMessage(const string &message) {
        ChatMessage::message = message;
    }

    HoverEvent *getHover() const {
        return hover;
    }

    void setHover(HoverEvent *hover) {
        ChatMessage::hover = hover;
    }

private:
    std::string message;
    std::vector<ChatMessage*> cildren;
    ChatColor color = ChatColor::WHITE;
    HoverEvent* hover = NULL;
    bool random;
    bool bold;
    bool strikethrough;
    bool italic;
    bool underlined;
};


#endif //CBUNGEE_CHATMESSAGE_H
