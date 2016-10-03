//
// Created by wolverindev on 23.08.16.
//

#include "../../include/connection/PlayerConnection.h"
#include "../../include/cryption/RSAUtil.h"
#include "../../include/utils/Base64Utils.h"

vector<PlayerConnection*> PlayerConnection::connections = vector<PlayerConnection*>(); //activeConnections
vector<PlayerConnection*> PlayerConnection::activeConnections = vector<PlayerConnection*>();

PlayerConnection::~PlayerConnection(){
    delete (this->handshake);
    delete (this->currentTargetConnection);
    delete (this->tabManager);
    delete (this->scoreManager);
    delete (this->packetHandler);
    delete (this->adress);
    delete (this->profile);
    for(std::vector<ServerConnection*>::iterator it = this->pendingConnections.begin(); it != this->pendingConnections.end(); ++it) {
        if ((*it)->getState() == ConnectionState::CLOSED) {
            //delete *it;
            continue;
        }
        if(*it != NULL)
            (*it)->disconnect(nullptr);
        delete *it;
    }
    pendingConnections.clear();
    cout << "~PlayerConnection" << endl;
}

void PlayerConnection::disconnect(ChatMessage* message) {
    if(message != NULL) {
        if (getState() == LOGIN) {
            cout << "Login disconnect" << endl;
            DataBuffer *buffer = new DataBuffer();
            buffer->writeVarInt(0x00);
            buffer->writeString(message->toString());
            writePacket(buffer);
            delete (buffer);
        } else if (getState() == PLAYING) {
            writePacket(getClientVersion(), new PacketPlayDisconnect(message));
        }
        delete message;
    }
    closeChannel();
}

ServerConnection *PlayerConnection::getCurrentTargetConnection() const {
    return currentTargetConnection;
}

void PlayerConnection::setCurrentTargetConnection(ServerConnection *currentTargetConnection) {
    PlayerConnection::currentTargetConnection = currentTargetConnection;
}

const string& PlayerConnection::getName() const {
    return name;
}

void PlayerConnection::setName(const string &name) {
    PlayerConnection::name = name;
}

void PlayerConnection::sendMessage(string message) {
    ChatMessage* message1 = new ChatMessage(message);
    sendMessage(message1);
    delete message1;
}
void PlayerConnection::sendMessage(ChatMessage* message) {
    DataBuffer* buffer = new DataBuffer();
    buffer->writeVarInt(getClientVersion() == 46 ? 0x02 : 0x0F);
    buffer->writeString(message->toString());
    buffer->write(0);
    writePacket(buffer);
    delete buffer;
}

void* connectMethode(void* parm){
    ServerConnection* cparms = (ServerConnection*) parm;
    cparms->setSocket(cparms->getServerInfo()->createSocket()); //May by take some seconds to connect
    if(!cparms->isSocketValid()){
        if(cparms->getPlayerConnection()->getState() == ConnectionState::LOGIN){
            cparms->getPlayerConnection()->connectToNextFallback();
        }
        else {
            if(cparms->getPlayerConnection()->getCurrentTargetConnection() == NULL || cparms->getPlayerConnection()->getCurrentTargetConnection()->getState() == ConnectionState::CLOSED){
                if(cparms->getPlayerConnection()->getPendingConnection().empty())
                    cparms->getPlayerConnection()->disconnect(new ChatMessage("§fNative-Proxy:\n§7Cant connect to target server and your last server kicked you."));
                else
                    cparms->getPlayerConnection()->sendDimswitch();
                return nullptr;
            }
            else
                cparms->getPlayerConnection()->sendMessage("§c§l» §7Cant connect to target server.");
        }
        cparms->getPacketHandler()->removeFromPending();
        return nullptr;
    }
    cparms->startConnect();
    return nullptr;
}

void PlayerConnection::connect(ServerInfo *target, bool sync) {
    for(vector<ServerConnection*>::iterator it = pendingConnections.begin();it != pendingConnections.end();it++){
        if((*it)->getServerInfo()->getName().compare(target->getName()) == 0){
            sendMessage("§c§l» §7You alredy connecting to this server.");
            return;
        }
    }
    if(getCurrentTargetConnection() != NULL && getCurrentTargetConnection()->getState() != ConnectionState::CLOSED)
        if(target->getName().compare(getCurrentTargetConnection()->getServerInfo()->getName()) == 0){
            sendMessage("§c§l» §7You are alredy connected to the server.");
            return;
        }
    ServerConnection* c = new ServerConnection(this, target, false);
    getPendingConnection().push_back(c);
    pthread_t threadHandle;
    pthread_create(&threadHandle,NULL,connectMethode,(void*) c);
    if(sync)
        pthread_join(threadHandle, NULL); //Make sync
}

void* runlater(void* conn){
    usleep(100*1000);
    PlayerConnection* pconn = (PlayerConnection*) conn;
    delete pconn;
}

void PlayerConnection::closeChannel() {
    if(!this->open)
        return;
    this->open = false;
    Connection::closeChannel();
    PlayerConnection::connections.erase(std::find(PlayerConnection::connections.begin(),PlayerConnection::connections.end(), this));
    vector<PlayerConnection*>::iterator rit = std::find(PlayerConnection::activeConnections.begin(),PlayerConnection::activeConnections.end(), this);
    if(rit != PlayerConnection::activeConnections.end())
        PlayerConnection::activeConnections.erase(rit);
    pthread_t  thread;
    pthread_create(&thread,NULL,&runlater,this);
}

const vector<ServerInfo *> PlayerConnection::getFallbackServers() const {
    return fallbackServers;
}

string PlayerConnection::generateServerHash() {
    int mdLength = 20;
    SHA_CTX context;
    unsigned char md[mdLength];
    std::string out;
    int i;
    const char hex[] = "0123456789abcdef";

    SHA1_Init(&context);
    string unencripted = RSAUtil::getPublicEncriptedKey(Cipper::publicKey)->getBase64Buffer();
    string data = base64_decode(unencripted);

    //cout << "Pubkey: " << data.size() << endl;
    //cout << "Key: " << unencripted << endl;
    //cout << "Secret: " << secret.size() << "(" << base64_encode((const unsigned char*) secret.data(),secret.size()) << ")" << endl;
    //cout << "ServerID: " << Cipper::hash << endl;

    SHA1_Update(&context, (unsigned char*) Cipper::hash.data(), Cipper::hash.size());
    SHA1_Update(&context, (unsigned char*) secret.data(), secret.size());
    SHA1_Update(&context, (unsigned char*) data.data(), data.size());
    SHA1_Final(md, &context);

    //cout << "SHA1 base: " << base64_encode(md,28) << endl;

    if(md[0] & 0x80)
    {
        unsigned char carry = 1;
        out = '-';
        for (i = mdLength-1; i >= 0; i--)
        {
            unsigned short twocomp = (unsigned char)~md[i];
            twocomp+=carry;
            if(twocomp & 0xff00)
            {
                twocomp = twocomp&0xff;
            }
            else
            {
                carry = 0;
            }
            md[i] = (uint8_t)twocomp;
        }
    }

    for (i = 0; i < mdLength; i++)
    {
        if(i || md[i]>>4)
        {
            out += hex[(md[i]>>4)];
        }
        if(i || md[i]>>4 || md[i]&0xf)
        {
            out += hex[(md[i]&0xf)];
        }
    }

    return out;
}

sockaddr_in *PlayerConnection::getAdress() const {
    return adress;
}
