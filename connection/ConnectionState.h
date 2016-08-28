//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_CONNECTIONSTATE_H
#define CBUNGEE_CONNECTIONSTATE_H

enum ConnectionState {
    HANDSHAKING,
    STATUS,
    LOGIN,
    PLAYING,
    CLOSED
};

#endif //CBUNGEE_CONNECTIONSTATE_H
