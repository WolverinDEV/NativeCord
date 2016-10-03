//
// Created by wolverindev on 11.09.16.
//

#ifndef NATIVECORD_JAVAPLUGINMANAGERIMPL_H
#define NATIVECORD_JAVAPLUGINMANAGERIMPL_H

#include "jni.h"

class JavaPluginManagerImpl {
    public:
        void startJavaVM();
        void stopJavaVM();
    private:
        JNIEnv *env;
        JavaVM *jvm;
};


#endif //NATIVECORD_JAVAPLUGINMANAGERIMPL_H
