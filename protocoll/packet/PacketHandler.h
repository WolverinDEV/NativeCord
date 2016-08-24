//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_PACKETHANDLER_H
#define CBUNGEE_PACKETHANDLER_H

#include <pthread.h>
#include "../../connection/Socket.h"
#include "../Buffers/StreamedDataBuffer.h"
#include "zlib.h"
#include "../../connection/Connection.h"

class PacketHandler {
public:
    static void readerTask(void* handlerPtr){
        PacketHandler *handler = (PacketHandler *) handlerPtr;
        while (1) {
            try {
                int packetLength = handler->connection->getStream()->readVarInt();
                DataBuffer *buffer = handler->connection->getStream()->readBuffer(packetLength);
                if(handler->connection->getThreadshold() != -1 && handler->connection->getThreadshold() < packetLength){
                    cout << "Decompile packet!" << endl;
                    ulong outlength = buffer->readVarInt();
                    if(outlength > 0) {
                        DataBuffer *out = new DataBuffer(outlength);
                        int state = uncompress((Bytef *) out,
                                               (ulong *) outlength,
                                               (Bytef *) buffer->getBuffer()+buffer->getReaderindex(),
                                               (ulong  ) buffer->getWriterindex());
                        switch (state) {
                            case Z_OK:
                                cout << "Decompressed okey" << endl;
                                break;
                            case Z_BUF_ERROR:
                                cout << "Buffer error" << endl;
                                break;
                            case Z_DATA_ERROR:
                                cout << "Invalid data!" << endl;
                                break;
                            default:
                                cout << "Cant find state " << state << endl;
                                break;
                        }
                        delete buffer;
                        buffer = out;
                    }
                }
                cout << "Having packet: Packet length: " << packetLength << " PacketID: ";
                cout << buffer->readVarInt() << endl;
                buffer->setReaderindex(0);
                handler->handlePacket(buffer);
                delete  buffer; //Memory cleanup
            } catch (Exception *ex) {
                if (dynamic_cast<StreamClosedException *>(ex) != NULL) {
                    cout << "Connection closed!" << endl;
                }
                cout << "Client reader exception: " << ex->what() << endl;
                break;
            }
        }
    }

    PacketHandler(Connection* connection) : connection(connection){
    }

    virtual ~PacketHandler(){
    }

    void startReader(){
        pthread_create(&threadHandle,NULL,(void* (*)(void*))&readerTask,(void*) this);
    }

    pthread_t getThreadHandle(){
        return threadHandle;
    }

protected:
    virtual void handlePacket(DataBuffer* buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketHandschake(int packetId,DataBuffer* buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketStatus(int packetId,DataBuffer* buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketLogin(int packetId,DataBuffer *buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketPlay(int packetId,DataBuffer *buffer){
        throw new Exception("Not implemented methode!");
    }

    pthread_t threadHandle;
    Connection* connection;
};

#endif //CBUNGEE_PACKETHANDLER_H
