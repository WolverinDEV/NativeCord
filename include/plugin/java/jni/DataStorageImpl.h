//
// Created by wolverindev on 04.10.16.
//

#ifndef NATIVECORD_DATASTORAGEIMPL_H
#define NATIVECORD_DATASTORAGEIMPL_H


#include <jni.h>
#include "DataStorage.h"
#include <iostream>
#include "../../../log/LogUtils.h"
using namespace std;

class JavaPluginManagerImpl;
class DataStorageImpl {
        friend class JavaPluginManagerImpl;
    public:
        DataStorageImpl(JavaPluginManagerImpl& handle) : handle(handle) {

        }
        DataStorage* fromJavaObject(jobject object);
        void fromJavaObject(jobject object,DataStorage* buffer);

        jobject toJavaObject(DataStorage& storage);

    private:
        JavaPluginManagerImpl& handle;
};


#endif //NATIVECORD_DATASTORAGEIMPL_H
