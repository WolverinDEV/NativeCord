//
// Created by wolverindev on 05.09.16.
//

#ifndef NATIVECORD_HEXUTILS_H
#define NATIVECORD_HEXUTILS_H

#include <string>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "../../numbers/BigIntegerLibrary.hh"

using namespace std;

typedef unsigned long Block;
class HexUtils {
    public:
        static char hexmap[];

        static std::string hexStr(unsigned char *data, int len) {

            int ulength = ceil(len / (double) 8);
            cout << "Length: " << ulength << " (" << len << ")" << endl;
            Block* udata = (Block *) new Block[ulength];
            memset(udata,0x00,ulength*sizeof(Block));
            cout << "Offset: " << (sizeof(Block) * ulength) - (sizeof(char) * len) << endl;
            memcpy((void *) udata + ((sizeof(Block) * ulength) - (sizeof(char) * len)), data, len);

            /*
           BigInt::Rossi number(0);
           BigInt::Rossi nnumber(-1);

           number.getUnits().clear();
           number.getUnits().resize(ulength);
           cout << "Cap: " <<  number.getUnits().capacity() << endl;
           for (int i = 0; i < ulength; i++) {
               number.getUnits().push_back(udata[i]);
           }
           cout << number.toStr0xHex() << endl;
           cout << nnumber.toStr0xHex() << endl;
           number = number * nnumber;
           cout << number.toStr0xHex() << endl;

           cout << "X: "<< number.getUnits().size() << "/" << number.getUnits().at(1) << "/" << udata[0] << endl;
           return number.toStrHex(std::string(""));
            */
            BigInteger i(udata,ulength);
            stringstream ss;
            ss << i << endl;
            string s = ss.str();
            return s;
        }
};

#endif //NATIVECORD_HEXUTILS_H
