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
    static string uuidToStrring(uuid_t uuid){
        char uuid_str[37];//Length 36+String sep
        uuid_unparse(uuid, uuid_str);
        return string(uuid_str);
    }
};

#endif //CBUNGEE_UUIDUTILS_H
