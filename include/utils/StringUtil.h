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

        static string replaceAll(std::string str, const std::string& from, const std::string& to) {
            size_t start_pos = 0;
            while((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
            }
            return str;
        }

        static string copy(string old){
            const char* _new = new char(old.size()+1);
            memcpy((char*) _new,old.c_str(),old.size()+1);
            return _new;
        }

        static vector<string> split(const string &s, char delim) {
            vector<string> elems;
            split(s, delim, elems);
            return elems;
        }
};

#endif //CBUNGEE_STRINGUTIL_H
