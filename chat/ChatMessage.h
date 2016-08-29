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
        static string DEFAULT;

        ChatMessage() {}

        ChatMessage(std::string message) {
            setMessage(message);
        }

        ChatMessage(const char *message) : ChatMessage(string(message)) {

        }

        ChatMessage(json raw);

        ~ChatMessage() {
            delete hover;
            free(message);
            for (std::vector<ChatMessage *>::iterator it = this->cildren.begin(); it != this->cildren.end(); ++it) {
                delete *it;
            }
        }

        void addSibling(ChatMessage *message) {
            this->cildren.push_back(message);
        }

        void removeSibling(ChatMessage *message) {
            auto it = std::find(cildren.begin(), cildren.end(), message);
            if (it != cildren.end())
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
            return italic; //https://www.computersnyou.com/4945/re-install-xorg-xserver-completely-ubuntu/
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

        const string getMessage() const {
            return string(message);
        }

        void setMessage(const string &m) {
            if(message != NULL)
                delete message;
            message = (char*) malloc(m.length()+1);
            memcpy(message,m.c_str(),m.length()+1);
        }

        HoverEvent *getHover() const {
            return hover;
        }

        void setHover(HoverEvent *hover) {
            ChatMessage::hover = hover;
        }

    private:
        char* message = NULL;
        std::vector<ChatMessage *> cildren;
        ChatColor color = ChatColor::WHITE;
        HoverEvent *hover = NULL;
        bool random = 0;
        bool bold = 0;
        bool strikethrough = 0;
        bool italic = 0;
        bool underlined = 0;
};


#endif //CBUNGEE_CHATMESSAGE_H
