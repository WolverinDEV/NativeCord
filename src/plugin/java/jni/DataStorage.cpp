//
// Created by wolverindev on 04.10.16.
//

#include "../../../../include/plugin/java/jni/DataStorage.h"

string DataStorage::_toString() {
    stringstream ss;
    ss << "[";

    ss << "Strings: ";
    for(vector<string>::iterator it = strings.begin(); it != strings.end(); it++){
        ss << '\'' << *it << '\'' << ",";
    }

    ss << "Longs: ";
    for(vector<uint64_t>::iterator it = longs.begin(); it != longs.end(); it++){
        ss << '\'' << *it << '\'' << ",";
    }

    ss << "Ints: ";
    for(vector<uint32_t>::iterator it = ints.begin(); it != ints.end(); it++){
        ss << '\'' << *it << '\'' << ",";
    }

    ss << "Bytes: ";
    for(vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end(); it++){
        ss << '\'' << *it << '\'' << ",";
    }

    ss << "Floats: ";
    for(vector<float >::iterator it = floats.begin(); it != floats.end(); it++){
        ss << '\'' << *it << '\'' << ",";
    }

    ss << "Doubles: ";
    for(vector<double >::iterator it = doubles.begin(); it != doubles.end(); it++){
        ss << '\'' << *it << '\'' << ",";
    }

    ss << "]";
    return ss.str();
}