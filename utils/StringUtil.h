//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_STRINGUTIL_H
#define CBUNGEE_STRINGUTIL_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

class StringUtils {
public:
    static void split(const string &s, char delim, vector<string> &elems) {
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }


    static vector<string> split(const string &s, char delim) {
        vector<string> elems;
        split(s, delim, elems);
        return elems;
    }
};

#endif //CBUNGEE_STRINGUTIL_H
