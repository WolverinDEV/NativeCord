//
// Created by wolverindev on 30.08.16.
//

#include "../../include/cryption/Cipper.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

RSA* generateRND_RSA_Key(){
    RSA* rsa = RSA_generate_key(1024,17,NULL,NULL);

    char* buffer = (char*) malloc(170);
    memset(buffer,0x00,170);
    BIO *mem = BIO_new(BIO_s_mem());
    cout << "Bio PEM_write_bio_RSA_PUBKEY: " << PEM_write_bio_RSA_PUBKEY(mem,rsa) << endl;
    cout << "Bio num:   " << BIO_pending(mem) << endl;

    int pub_len = BIO_pending(mem);

    char* pub_key = (char*) malloc(pub_len + 1);

    BIO_read(mem, pub_key, pub_len);

    pub_key[pub_len] = '\0';
    cout << pub_key << endl;

    cout << "First: " << (int) buffer[0] << "/" << (int) buffer[1] << endl;
    return rsa;
}

RSA* Cipper::publicKey = generateRND_RSA_Key();
string Cipper::hash = string("WolverinDEV");