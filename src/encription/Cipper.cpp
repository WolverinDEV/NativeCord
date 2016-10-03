//
// Created by wolverindev on 30.08.16.
//

#include "../../include/cryption/Cipper.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "../../include/log/LogUtils.h"

RSA* generateRND_RSA_Key(){
    RSA* rsa = RSA_generate_key(1024,17,NULL,NULL);

    char* buffer = (char*) malloc(170);
    memset(buffer,0x00,170);
    BIO *mem = BIO_new(BIO_s_mem());
    debugMessage("Bio PEM_write_bio_RSA_PUBKEY: "+to_string(PEM_write_bio_RSA_PUBKEY(mem,rsa)));
    debugMessage("Bio num:   " + to_string(BIO_pending(mem)));

    int pub_len = BIO_pending(mem);

    char* pub_key = (char*) malloc(pub_len + 1);

    BIO_read(mem, pub_key, pub_len);

    pub_key[pub_len] = '\0';
    cout << pub_key << endl;

    debugMessage("First: " + to_string((int) buffer[0] ) + "/" + to_string((int) buffer[1]));
    return rsa;
}

RSA* Cipper::publicKey = generateRND_RSA_Key();
string Cipper::hash = string("WolverinDEV");