//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_ENTITYREWRITE_H
#define CBUNGEE_ENTITYREWRITE_H

#include "../protocoll/Buffers/DataBuffer.h"
class EntityRewrite{
public:
    static void replaceEntityId(int pid,DataBuffer* buffer,int old,int _new){
        if(old == _new)
            return;
        int current = buffer->readVarInt();
        if(current == _new){
            int temp = old;
            old = _new;
            _new = temp;
        }else if(current != old)
            return;
        if(DataBuffer::getVarIntSize(old) != DataBuffer::getVarIntSize(_new)){
            if(DataBuffer::getVarIntSize(old) > DataBuffer::getVarIntSize(_new)){
                cout << "Push -1" << endl;
                buffer->push(-1);
                buffer->setWriterindex(0);
                buffer->writeVarInt(pid);
                buffer->writeVarInt(_new);
                buffer->setWriterindex(buffer->getBufferLength());
                return;
            } else {
                cout << "Push 1" << endl;
                buffer->push(1);
                buffer->setWriterindex(0);
                buffer->writeVarInt(pid);
                buffer->writeVarInt(_new);
                buffer->setWriterindex(buffer->getBufferLength());
                return;
            }
        }
        cout << "Change " << old << " to " << _new << endl;
        buffer->setWriterindex(0);
        buffer->writeVarInt(pid);
        buffer->writeVarInt(_new);
        buffer->setWriterindex(buffer->getBufferLength());
    }

    static void entityRewide210Server(int packetId,DataBuffer* buffer,int currentEntityId,int targetEntitryId){ //TODO 0x12 -> Inv 4 horse | 0x2C 4 combat event | 0x31 Entity effect | 0x3B Attach entity | 0x49 Collect item
        if(packetId >= 0x00 && packetId <= 0x06 || packetId == 0x08 || packetId == 0x1B || packetId >= 0x25 && packetId <= 0x28 || packetId == 0x2F || packetId == 0x31 || packetId == 0x34 || packetId == 0x36
           || packetId == 0x39 || packetId == 0x3B || packetId == 0x3C || packetId == 0x40 || packetId == 0x4A || packetId == 0x4B || packetId == 0x4C){
            replaceEntityId(packetId,buffer,currentEntityId,targetEntitryId);
        }
    }

    static void entityRewide210Client(int packetId,DataBuffer* buffer,int currentEntityId,int targetEntitryId){ //TODO Nothink
        if(packetId == 0x14){
            replaceEntityId(packetId,buffer,currentEntityId,targetEntitryId);
        }
    }

    static void entityRewide110Server(int packetId,DataBuffer* buffer,int currentEntityId,int targetEntitryId){ //TODO 0x13 -> Inv 4 horse | 0x2C 4 combat event | 0x30 Destroy entity | 0x3B Attach entity | 0x48 Collect item
        if(packetId >= 0x00 && packetId <= 0x06 || packetId == 0x08 || packetId == 0x1B || packetId >= 0x25 && packetId <= 0x28 || packetId == 0x2F || packetId == 0x31 || packetId == 0x34 || packetId == 0x36
           || packetId == 0x39 || packetId == 0x3B || packetId == 0x3C || packetId == 0x40 || packetId == 0x49 || packetId == 0x4A || packetId == 0x4B){
            replaceEntityId(packetId,buffer,currentEntityId,targetEntitryId);
        }
    }

    static void entityRewide110Client(int packetId,DataBuffer* buffer,int currentEntityId,int targetEntitryId){ //TODO Nothink
        if(packetId == 0x14){
            replaceEntityId(packetId,buffer,currentEntityId,targetEntitryId);
        }
    }

    static void entityRewide47Server(int packetId,DataBuffer* buffer,int currentEntityId,int targetEntitryId){ //TODO 0x0D | 0x2D | 0x42
        if(packetId == 0x04 || packetId == 0x0A || packetId == 0x0B || packetId == 0x0C || packetId >= 0x0E && packetId <= 0x1E || packetId == 0x20 || packetId == 0x25 || packetId == 0x2C || packetId == 0x49){
            replaceEntityId(packetId,buffer,currentEntityId,targetEntitryId);
        }
    }

    static void entityRewide47Client(int packetId,DataBuffer* buffer,int currentEntityId,int targetEntitryId){ //TODO Nothink
        if(packetId == 0x0B){
            replaceEntityId(packetId,buffer,currentEntityId,targetEntitryId);
        }
    }
};

#endif //CBUNGEE_ENTITYREWRITE_H
