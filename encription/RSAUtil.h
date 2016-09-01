//
// Created by wolverindev on 01.09.16.
//

#ifndef NATIVECORD_RSAUTIL_H
#define NATIVECORD_RSAUTIL_H

#include <stdlib.h>
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

struct KeyEncripted {
    string keyStart;
    string keyEnd;
    char* buffer;
    int length;

    string getBase64Buffer(){
        string keyBase46 = string(buffer+keyStart.length(),length-keyEnd.length()-keyStart.length()); //Attantion \n in string
        return StringUtils::replaceAll(keyBase46,"\n","");
    }

    ~KeyEncripted(){
        cout << "Remove" << endl;
        free(buffer);
    }
};
using namespace std;
class RSAUtil {
    public:
        static KeyEncripted* getPublicEncriptedKey(RSA* key){
            BIO *mem = BIO_new(BIO_s_mem());
            if(PEM_write_bio_RSA_PUBKEY(mem,key) != 1)
                return NULL;
            int pub_len = BIO_pending(mem);
            char* pub_key = (char*) malloc(pub_len);
            BIO_read(mem, pub_key, pub_len);
            KeyEncripted* enc =  new KeyEncripted;
            string keyStart = string("-----BEGIN PUBLIC KEY-----\n");
            string keyEnd = string("\n-----END PUBLIC KEY-----\n");
            cout << "Public Key: " << endl << string(pub_key,pub_len) << endl;
            enc->keyStart = keyStart;
            enc->keyEnd = keyEnd;
            enc->buffer = pub_key;
            enc->length = pub_len;
            return enc;
        }
        static KeyEncripted* getPrivateEncriptedKey(RSA* key){
            BIO *mem = BIO_new(BIO_s_mem());
            if(PEM_write_bio_RSAPrivateKey(mem,key, NULL, NULL, 0, NULL, NULL) != 1) {
                cout << "Cant write private key" << endl;
                return NULL;
            }
            int pub_len = BIO_pending(mem);
            char* pub_key = (char*) malloc(pub_len);
            BIO_read(mem, pub_key, pub_len);
            KeyEncripted* enc =  new KeyEncripted;
            string keyStart = string("-----BEGIN RSA PRIVATE KEY-----\n");
            string keyEnd = string("\n-----END RSA PRIVATE KEY-----\n");
            cout << "Private Key: " << endl << string(pub_key,pub_len) << endl;
            enc->keyStart = keyStart;
            enc->keyEnd = keyEnd;
            enc->buffer = pub_key;
            enc->length = pub_len;
            return enc;
        }
};

#endif //NATIVECORD_RSAUTIL_H
