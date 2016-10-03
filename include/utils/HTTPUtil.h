//
// Created by wolverindev on 03.09.16.
//

#ifndef NATIVECORD_HTTPUTIL_H
#define NATIVECORD_HTTPUTIL_H

#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>

using namespace std;
vector<string> HTTP_buildDefaultHeaders(string url){
    vector<string> v;
    return v;
};

class HTTPResponse;
using Callback = std::function<void(HTTPResponse*)>;
struct HTTPRequest {
    string host;
    int port = 80;
    string path = "/";
    string type = "GET";
    vector<string> headers;
    vector<string> parms;
    Callback callback = nullptr;
};
struct HTTPResponse {
    HTTPRequest* request;
    int state;
    string body;
};

void* httpProgress(void* ptrRequest){
    HTTPRequest* request = (HTTPRequest*) ptrRequest;
    //Socket* socket  = SocketUtil::createTCPSocket()
}

HTTPResponse* HTTP_requestHTTP(HTTPRequest* request){
    pthread_t handle;
    bool sync = request->callback == nullptr;

    HTTPResponse** responsePtr;
    if(sync)
        request->callback = [responsePtr](HTTPResponse* response){
            *responsePtr = nullptr;
        };
    pthread_create(&handle,NULL,httpProgress,request);
    if(sync)
        pthread_join(handle, NULL);
    return responsePtr == nullptr ? nullptr : *responsePtr;
};

#endif //NATIVECORD_HTTPUTIL_H
