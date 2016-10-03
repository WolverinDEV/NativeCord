//
// Created by wolverindev on 11.09.16.
//

#ifndef NATIVECORD_CXXPLUGIN_H
#define NATIVECORD_CXXPLUGIN_H

#include "../Plugin.h"

class CXXPlugin : public Plugin{
    public:
        virtual std::string getName();
        virtual std::string getVersion();
};

#endif //NATIVECORD_CXXPLUGIN_H
