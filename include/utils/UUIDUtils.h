//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_UUIDUTILS_H
#define CBUNGEE_UUIDUTILS_H

#include <iostream>
#include <string>
#include <uuid/uuid.h>

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/md5.h>

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

        /*
         *
         */
        static uuid_t& getOfflineUUID(string& name){

            MD5_CTX ctx;
            MD5_Init(&ctx);
            MD5_Update(&ctx, (unsigned char*) name.c_str(), name.size());

            unsigned char* uuid = new unsigned char [16];
            bzero(uuid, 16);

            MD5_Final((unsigned char*) uuid, &ctx);

            uuid[6]  &= 0x0f;  /* clear version        */
            uuid[6]  |= 0x30;  /* set to version 3     */
            uuid[8]  = uuid[8] & 0x3f;  /* clear variant        */
            uuid[8]  = uuid[8] | 0x80;  /* set to IETF variant  */


            //uuid_t* tyoe = (uuid_t*) new uuid_t;
            //memcpy(tyoe, out, 16);
            //*tyoe = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
            //uuid_t& uout = (uuid_t&) out;
            return *((uuid_t*) uuid);
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
