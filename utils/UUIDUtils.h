//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_UUIDUTILS_H
#define CBUNGEE_UUIDUTILS_H

#include <iostream>
#include <string>
#include <uuid/uuid.h>

using namespace std;
class UUIDUtils {
public:
    static string uuidToString(uuid_t uuid) {
        char uuid_str[37];//Length 36+String sep
        uuid_unparse(uuid, uuid_str);
        return string(uuid_str);
    }

        static uuid_t& getUUIDFromString(string suuid, bool braces = true){
            if(braces){
                uuid_t target;
                uuid_parse(suuid.c_str(),target);
                cout << "" << uuidToString(target) << "- " << suuid.c_str() << endl;
                return target;
            }else {
                //8-4-4-4-12
                return getUUIDFromString(getUUIDString36(suuid));
            }
        }

        static string getUUIDString36(string str32){
            stringstream out;
            out << str32.substr(0,8) << "-" << str32.substr(8,4) << "-" << str32.substr(12, 4) << "-" << str32.substr(16, 4) << "-" << str32.substr(20, 12);
            return out.str();
        }

        static uuid_t& randomUUID(){
            uuid_t  out;
            uuid_generate(out);
            return out;
        }
};

#endif //CBUNGEE_UUIDUTILS_H
