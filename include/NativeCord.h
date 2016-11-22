//
// Created by wolverindev on 06.11.16.
//

#ifndef NATIVECORD_NATIVECORD_H
#define NATIVECORD_NATIVECORD_H

#include <pthread.h>

#ifndef NATIVECORD_VERSION
    #define NATIVECORD_VERSION "unknown"
#else
    //Defined over cmake
#endif

namespace NativeCord {
    extern void exitNativeCoord();
    extern pthread_t clientAcceptThread;
}

#endif //NATIVECORD_NATIVECORD_H
