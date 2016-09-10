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
#include "../numbers/bigInt.h"

using namespace std;

class HexUtils {
    public:
        static char hexmap[];

        static std::string hexStr(unsigned char *data, int len) {
            int ulength = ceil(len / (double) 8);
            cout << "Length: " << ulength << " (" << len << ")" << endl;
            BigInt::Unit* udata = (BigInt::Unit *) new BigInt::Unit[ulength];
            cout << "Offset: " << (sizeof(BigInt::Unit) * ulength) - (sizeof(char) * len) << endl;
            memcpy((void *) udata + ((sizeof(BigInt::Unit) * ulength) - (sizeof(char) * len)), data, len);
            BigInt::Rossi number(0);
            BigInt::Rossi nnumber(-1);
            number.getUnits().clear();
            number.getUnits().resize(ulength);
            cout << "Cap: " <<  number.getUnits().capacity() << endl;
            for (int i = 0; i < ulength; i++) {
                number.getUnits().push_back(udata[i]);
            }
            cout << "X: "<< number.getUnits().size() << "/" << number.getUnits().at(1) << "/" << udata[0] << endl;
            return number.toStrHex(std::string(""));
        }
};

#endif //NATIVECORD_HEXUTILS_H
