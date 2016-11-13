//
// Created by wolverindev on 18.10.16.
//

#ifndef MCJOINBOT_VECTORUTILS_H
#define MCJOINBOT_VECTORUTILS_H

#include <vector>
#include <algorithm>
using namespace std;
class VectorUtils {
    public:
        template<typename T>
        static void remove(vector<T>& vector, T element){
            /*
            for(vector<T>::iterator it = vector.begin(); it != vector.end(), it++){
                if(*it == element)
                {
                    vector.erase(it);
                }
            }
             */
            vector.erase( std::remove( vector.begin(), vector.end(), element ), vector.end() );
        }
};

#endif //MCJOINBOT_VECTORUTILS_H
