//
// Created by wolverindev on 09.10.16.
//

#ifndef NATIVECORD_EVENT_H
#define NATIVECORD_EVENT_H

#include "../../protocoll/DataBuffer.h"

class Event {
    public:
        DataBuffer* createDataBuffer() const ;
};

#endif //NATIVECORD_EVENT_H
