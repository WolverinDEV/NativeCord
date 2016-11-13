//
// Created by wolverindev on 11.09.16.
//

#ifndef NATIVECORD_JAVAPLUGIN_H
#define NATIVECORD_JAVAPLUGIN_H

#include <jni.h>
#include "JavaPluginManagerImpl.h"

class JavaPlugin : public Plugin {
    public:
        JavaPlugin(JavaPluginManagerImpl& manager, jobject instance);

        ~JavaPlugin();

        void load();
        void enable();
        void disable();

        std::string name;

        virtual string getName() override;

        virtual string getVersion() override;

        virtual vector<string> getAuthors() override;

    private:
        int checkSumA(){
            return -1;
        }
        int checkSumB(){
            return 123456789;
        }
        JavaPluginManagerImpl& manager;
        jobject pluginInstance;
};

#endif //NATIVECORD_JAVAPLUGIN_H
