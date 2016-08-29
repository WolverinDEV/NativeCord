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
#include "ActionEvent.h"
#include <algorithm>
#include "../json/json.hpp"

using json = nlohmann::json;

class ChatMessage {
    public:
        static string DEFAULT;
        static int count;

        ChatMessage() {}

        ChatMessage(std::string message) {
            setMessage(message);
            count++;
        }

        ChatMessage(const char *message) : ChatMessage(string(message)) {

        }

        ChatMessage(json raw);

        ~ChatMessage() {
            if(hover != NULL)
            delete hover;
            if(click != NULL)
            delete click;
            if(message != NULL)
            free(message);
            for (std::vector<ChatMessage *>::iterator it = this->cildren.begin(); it != this->cildren.end(); ++it) {
                delete *it;
            }
            count--;
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

        ActionEvent *getHoverAction() const {
            return hover;
        }

        void setHoverAction(ActionEvent *hover) {
            ChatMessage::hover = hover;
        }

        ActionEvent *getClickAction() const {
            return click;
        }

        void setClickAction(ActionEvent *click) {
            ChatMessage::click = click;
        }
    private:
        char* message = NULL;
        std::vector<ChatMessage *> cildren;
        ChatColor color = ChatColor::WHITE;
        ActionEvent *hover = NULL;
        ActionEvent *click = NULL;
        bool random = 0;
        bool bold = 0;
        bool strikethrough = 0;
        bool italic = 0;
        bool underlined = 0;
};


#endif //CBUNGEE_CHATMESSAGE_H
