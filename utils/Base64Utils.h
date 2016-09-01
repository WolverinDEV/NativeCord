//
// Created by wolverindev on 01.09.16.
//

#ifndef NATIVECORD_BASE64UTILS_H
#define NATIVECORD_BASE64UTILS_H
#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);


#endif //NATIVECORD_BASE64UTILS_H
