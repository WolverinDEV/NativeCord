//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_STRINGUTIL_H
#define CBUNGEE_STRINGUTIL_H

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>

using namespace std;

class StringUtils {
    public:
        static void split(const string &s, string delim, vector<string> &elems) {
            auto start = 0U;
            auto end = s.find(delim);
            while (end != std::string::npos)
            {
                elems.push_back(s.substr(start, end - start));
                start = end + delim.length();
                end = s.find(delim, start);
            }
            elems.push_back(s.substr(start, end - start));
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

        static vector<string> split(const string &s, string delim) {
            vector<string> elems;
            split(s, delim, elems);
            return elems;
        }

        static bool contains(string& a, string b){
            return a.find(b) != string::npos;
        }
};

#endif //CBUNGEE_STRINGUTIL_H
