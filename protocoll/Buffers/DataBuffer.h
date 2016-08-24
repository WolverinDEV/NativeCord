/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DataBuffer.h
 * Author: wolverindev
 *
 * Created on August 11, 2016, 7:47 PM
 */

#ifndef DATABUFFER_H
#define DATABUFFER_H

#include <iostream>
#include <string>
#include <string.h>
#include <uuid/uuid.h>

using namespace std;

class Exception : public exception {
    public:
        Exception(const char *message) : message(message) {
        }

        const char *what() {
            return message;
        };
    private:
        const char *message;
};

class DataBuffer {
    public:
        DataBuffer();

        DataBuffer(int size);

        DataBuffer(const char *buffer, int bufferLength) : DataBuffer(buffer,bufferLength,true){

        }

        DataBuffer(const char *buffer, int bufferLength,bool copy);

        ~DataBuffer();

        virtual void read(const char *buffer, int length);

        virtual char read();

        uint8_t readByte() {
            return read();
        }

        uint16_t readShort() {
            char *buffer = new char[2];
            read(buffer, 2);
            unsigned short out1 = (buffer[0] & 0xFF) << 8;
            unsigned char out2 = buffer[1] & 0xFF;
            delete buffer;
            return out1 + out2;
        }

        uint32_t readInt() {
            char *buffer = new char[4];
            read(buffer, 4);
            uint32_t out1 = (buffer[0] & 0xFF) << 24;
            uint32_t out2 = (buffer[1] & 0xFF) << 16;
            uint16_t out3 = (buffer[2] & 0xFF) << 8;
            uint8_t out4 = buffer[3] & 0xFF;
            delete buffer;
            return out1 + out2 + out3 + out4;
        }

        uint64_t readLong() {
            char *buffer = new char[8];
            read(buffer, 8);
            uint64_t out1 = (uint64_t) (buffer[0] & 0xFF) << 56;
            uint64_t out2 = (uint64_t) (buffer[1] & 0xFF) << 48;
            uint64_t out3 = (uint64_t) (buffer[2] & 0xFF) << 40;
            uint64_t out4 = (uint64_t) (buffer[3] & 0xFF) << 32;
            uint32_t out5 = (buffer[4] & 0xFF) << 24;
            uint32_t out6 = (buffer[5] & 0xFF) << 16;
            uint16_t out7 = (buffer[6] & 0xFF) << 8;
            uint8_t out8 = buffer[7] & 0xFF;
            delete buffer;
            return out1 + out2 + out3 + out4 + out5 + out6 + out7 + out8;
        }

        double readDouble() {
            long long rlong = readLong();
            return (double) *reinterpret_cast<double *> (&rlong);
        }

        float readFloat() {
            int rint = readInt();
            return (float) *reinterpret_cast<float *> (&rint);
        }

        uint32_t readVarInt() {
            int output = 0;
            int byteIndex = 0;
            char currentByte;
            do {
                currentByte = readByte();
                output |= (currentByte & 0x7F) << byteIndex++ * 7;
                if (byteIndex > 5) {
                    throw new Exception("VarInt too big");
                }
            } while ((currentByte & 0x80) == 128);
            return output;
        }

        string readString() {
            int length = readVarInt();
            if (length > 35000)
                throw new Exception("Try to read a too long string!");
            char* buffer = new char[length];
            read(buffer, length);
            return string(buffer);
        }

        uuid_t *readUUID() {
            uuid_t * out = (uuid_t * )
            new char[16];
            read((const char *) out, 16);
            return out;
        }

        DataBuffer* readBuffer(int length);

        virtual  void write(const char *buffer, int length);

        virtual  void write(char byte);

        void writeByte(uint8_t byte) {
            write(byte);
        }

        void writeShort(uint16_t n) {
            writeByte((n >> 8) & 0xFF);
            writeByte(n & 0xFF);
        }

        void writeInt(uint32_t n) {
            writeByte((n >> 24) & 0xFF);
            writeByte((n >> 16) & 0xFF);
            writeByte((n >> 8) & 0xFF);
            writeByte(n & 0xFF);
        }

        void writeLong(uint64_t n) {
            writeByte((n >> 56) & 0xFF);
            writeByte((n >> 48) & 0xFF);
            writeByte((n >> 40) & 0xFF);
            writeByte((n >> 32) & 0xFF);
            writeByte((n >> 24) & 0xFF);
            writeByte((n >> 16) & 0xFF);
            writeByte((n >> 8) & 0xFF);
            writeByte(n & 0xFF);
        }

        void writeVarInt(int val) {
            while ((val & 0xFFFFFF80) != 0) {
                writeByte(val & 0x7F | 0x80);
                val >>= 7;
            }
            writeByte(val);
        }

        void writeString(string message) {
            writeVarInt(message.size());
            write(message.c_str(), message.size());
        }

        void writeString(const char *message, int length) {
            writeVarInt(length);
            write(message, length);
        }

        void writeUUID(uuid_t uuid) {
            write((const char *) uuid, 16);
        }

        virtual int readableBytes();

        virtual int writeableBytes();

        void clear() {
            readerindex = 0;
            writerindex = 0;
            bufferLength = 0;
            delete buffer;
            this->buffer = NULL;
        }

        const char *getBuffer() const {
            return buffer;
        }

        int getBufferLength() const {
            return bufferLength;
        }

        bool isFixedLength() const {
            return fixedLength;
        }

        int getReaderindex() const {
            return readerindex;
        }

        int getWriterindex() const {
            return writerindex;
        }

        void setReaderindex(int readerindex) {
            DataBuffer::readerindex = readerindex;
        }

        void setWriterindex(int writerindex) {
            DataBuffer::writerindex = writerindex;
        }

private:

        virtual void checkReadableBytes(int bytes) {
            if (readableBytes() < bytes)
                throw new Exception("Cant read n bytes!");
        }

        virtual void checkWriteableBytes(int bytes) {
            int n = writeableBytes();
            if (n != -1 && n < bytes) {
                cout << "N: " << n << " B: " << bytes << " F: " << (bool) fixedLength << endl;
                throw new Exception("Cant write n bytes!");
            }
        }

        void allocateBufferSize(int length) {
            if (writerindex + length >= bufferLength) {
                const char *oldBuffer = buffer;
                buffer = (const char *) malloc(writerindex + length);
                if (buffer != NULL)
                    memcpy((void *) buffer, (const void *) oldBuffer, writerindex);
                bufferLength = writerindex + length;
                if (oldBuffer != NULL);
                delete oldBuffer;
            }
        }

        const char *buffer = NULL;
        int bufferLength = -1;
        bool fixedLength = false;
        int readerindex = 0;
        int writerindex = 0;
};

#endif /* DATABUFFER_H */

