//
// Created by wolverindev on 30.08.16.
//

#ifndef NATIVECORD_CIPPER_H
#define NATIVECORD_CIPPER_H

#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <string.h>
#include <cstdlib>

#include "../protocoll/Buffers/DataBuffer.h"
#include "../protocoll/packet/Packets.h"

class Cipper {
    public:
        static RSA* publicKey;
        //static RSA* privateKey;
        static string hash;

        //See https://github.com/SpigotMC/BungeeCord/blob/master/proxy/src/main/java/net/md_5/bungee/EncryptionUtil.java#L81-L84  ?Unused?
        static EVP_PKEY* createPublicKey(char *key, int keyLength){
            X509 * cert = X509_new();
            EVP_PKEY * pubkey;
            d2i_X509(&cert,(const unsigned char**) &key,keyLength);
            pubkey = X509_get_pubkey (cert);
            cout << "Pub size: " << sizeof(pubkey->pkey) << endl;
            return pubkey;
        }

        static string decodeMessagePrivateKey(char* buffer,int length,RSA* key){
            char* out = (char*) malloc(length);
            cout << "Decript length: " << length << endl;
            int state = 0;
            if((state = RSA_private_decrypt(length, (unsigned char*)buffer, (unsigned char*)out,  key, RSA_PKCS1_PADDING)) == -1) {
                char   *err;               // Buffer for any error messages
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error decrypting message: %s\n", err);
                return NULL;
            }
            char* outRegion = (char*) malloc(state);
            memcpy(outRegion,out,state);
            delete out;
            return string(outRegion,state);
        }
        /*
        static EVP_PKEY* createPublicKey(PacketLoginEncryption* packet){
            X509 * cert;
            EVP_PKEY * pubkey;
            //d2i_X509(&cert,(const unsigned char**) &key, packet->);
            pubkey = X509_get_pubkey (cert);
            return pubkey;
        }
         */


        Cipper(unsigned char* key, bool decript){
            this->key = key;
            this->decript = decript;
        }
        Cipper(){

            //AES_KEY key;
            //AES_set_decrypt_key();
            //AES_set_encrypt_key(keyBytes, 256, &key);
        }

        ~Cipper(){
            freeCipher();
            //free(key);
        }
        void init(){
            ctx = EVP_CIPHER_CTX_new();
            EVP_CipherInit(ctx,EVP_aes_128_cfb8(), key, key, decript); //True for decript
        }
        void freeCipher(){
            if(ctx != nullptr){
                EVP_CIPHER_CTX_free(ctx);
                ctx = nullptr;
            }
        }
        DataBuffer* cipher(DataBuffer* in, bool deleteIn = true){
            DataBuffer* out = new DataBuffer(in->getBufferLength());
            uint32_t length = out->getBufferLength();
            EVP_CipherUpdate(ctx, (unsigned char*) out->getBuffer(), (int*) &length, (unsigned char*) in->getBuffer(), (int) in->getBufferLength());
            if(deleteIn)
                delete in;
            return out;
        }

        char* cipher(char* buffer,int length, bool freeIn = true){
            char* out = (char*) malloc(length);
            cipher(buffer,length,out,freeIn);
            return out;
        }

        void cipher(char* buffer,int length, char* out, bool freeIn = true){
            EVP_CipherUpdate(ctx, (unsigned char*) out, (int*) &length, (unsigned char*) buffer, (int) length);
            if(freeIn)
                free(buffer);
        }
    private:
        EVP_CIPHER_CTX *ctx = nullptr;
        bool decript;
        unsigned char* key;
};


#endif //NATIVECORD_CIPPER_H
