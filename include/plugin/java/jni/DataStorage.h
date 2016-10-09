//
// Created by wolverindev on 04.10.16.
//

#ifndef NATIVECORD_DATASTORAGE_H
#define NATIVECORD_DATASTORAGE_H

#include <vector>
#include <string>
#include <sstream>
#include <jni.h>

using namespace std;

struct DataStorage {
    vector<string> strings;
    vector<uint64_t> longs;
    vector<uint32_t> ints;
    vector<uint8_t> bytes;
    vector<float> floats;
    vector<double> doubles;

    string _toString();
    void reset(){
        strings.clear();
        longs.clear();
        ints.clear();
        bytes.clear();
        floats.clear();
        doubles.clear();
    }
};


#endif //NATIVECORD_DATASTORAGE_H
