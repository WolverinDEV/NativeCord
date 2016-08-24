//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_HOVEREVENT_H
#define CBUNGEE_HOVEREVENT_H

#include <iostream>
#include <string>

using namespace std;

class HoverEvent {
public:
    HoverEvent(const string &action, const string &value) : action(action), value(value) {
    }

    virtual string getAction(){
        return action;
    }
    virtual string getValue(){
        return value;
    }
private:
    string action;
    string value;
};
#endif //CBUNGEE_HOVEREVENT_H
