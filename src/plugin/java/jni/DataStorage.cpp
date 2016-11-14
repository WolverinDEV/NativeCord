//
// Created by wolverindev on 04.10.16.
//

#include "../../../../include/plugin/java/jni/DataStorage.h"

string DataStorage::_toString() {
    stringstream ss;
    ss << "[";

    ss << "Strings[" << strings.size() << "]{";
    for(vector<string>::iterator it = strings.begin(); it != strings.end();){
        ss << '\'' << *it << '\'';
        if(it++ != strings.end())
            ss << ", ";
    }

    ss << "} Longs[" << longs.size() << "]{";
    for(vector<uint64_t>::iterator it = longs.begin(); it != longs.end();){
        ss << '\'' << *it << '\'';
        it++;
        if(it != longs.end())
            ss << ", ";
    }

    ss << "} Ints[" << ints.size() << "]{";
    for(vector<uint32_t>::iterator it = ints.begin(); it != ints.end();){
        ss << '\'' << *it << '\'';
        it++;
        if(it != ints.end())
            ss << ", ";
    }

    ss << "} Bytes[" << bytes.size() << "]{";
    for(vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end();){
        ss << '\'' << to_string(*it) << '\'';
        it++;
        if(it != bytes.end())
            ss << ", ";
    }

    ss << "} Floats[" << floats.size() << "]{";
    for(vector<float >::iterator it = floats.begin(); it != floats.end();){
        ss << '\'' << *it << '\'';
        it++;
        if(it != floats.end())
            ss << ", ";
    }

    ss << "} Doubles[" << doubles.size() << "]{";
    for(vector<double >::iterator it = doubles.begin(); it != doubles.end();){
        ss << '\'' << *it << '\'';
        it++;
        if(it != doubles.end())
            ss << ", ";
    }

    ss << "}";
    return ss.str();
}