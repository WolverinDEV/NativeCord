//
// Created by wolverindev on 23.08.16.
//

#include "../../include/connection/PlayerConnection.h"
#include "../../include/plugin/java/JavaPluginManagerImpl.h"
#include "../../include/cryption/RSAUtil.h"
#include "../../include/utils/Base64Utils.h"
#include "../../include/utils/VectorUtils.h"

vector<PlayerConnection*> PlayerConnection::connections = vector<PlayerConnection*>(); //activeConnections
vector<PlayerConnection*> PlayerConnection::activeConnections = vector<PlayerConnection*>();


//TODO register / deregister
PlayerConnection::PlayerConnection(sockaddr_in *adress, Socket *socket)  : Connection(socket), adress(adress) {
    PlayerConnection::connections.push_back(this);
    if(JavaPluginManagerImpl::instance != nullptr){
        this->nativeAddress = (uint64_t) this;
        this->javaInstance = JavaPluginManagerImpl::instance->getRefelectManager()->createPlayerInstance(this);
        JavaPluginManagerImpl::instance->getRefelectManager()->registerPlayer(this);
    }
    this->open = true;
}

PlayerConnection::~PlayerConnection(){
    delete (this->handshake);
    delete (this->currentTargetConnection);
    delete (this->tabManager);
    delete (this->scoreManager);
    delete (this->packetHandler);
    delete (this->adress);
    delete (this->profile);
    if(this->lastDisconnectMessage != nullptr)
        delete this->lastDisconnectMessage;
    if(this->javaInstance != nullptr){
        JavaPluginManagerImpl::instance->getRefelectManager()->unregisterPlayer(this);
        JavaPluginManagerImpl::instance->getEnv()->DeleteGlobalRef(this->javaInstance);
    }
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
}

void PlayerConnection::disconnect(ChatMessage* message, bool deleteMessage = true) {
    if(message != NULL) {
        if (getState() == LOGIN) {
            DataBuffer *buffer = new DataBuffer();
            buffer->writeVarInt(0x00);
            buffer->writeString(message->toString());
            writePacket(buffer);
            delete (buffer);
        } else if (getState() == PLAYING) {
            writePacket(getClientVersion(), new PacketPlayDisconnect(message));
        }
        if(deleteMessage)
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

string& PlayerConnection::getName() {
    return name;
}

void PlayerConnection::setName(const string &name) {
    PlayerConnection::name = name;
    if(JavaPluginManagerImpl::instance != nullptr){
        jstring string = JavaPluginManagerImpl::instance->getEnv()->NewStringUTF(name.c_str());
        JavaPluginManagerImpl::instance->getEnv()->SetObjectField(PlayerConnection::getJavaInstance(), JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection_playerName, string);
    }
}

void PlayerConnection::sendMessage(string message) {
    ChatMessage* message1 = new ChatMessage(message);
    sendMessage(message1);
    delete message1;
}
void PlayerConnection::sendMessage(ChatMessage* message) {
    DataBuffer* buffer = new DataBuffer();
    buffer->writeVarInt(getClientVersion() == 47 ? 0x02 : 0x0F);
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
                if(cparms->getPlayerConnection()->getPendingConnection().empty()) {
                    ChatMessage* message = new ChatMessage("§fNative-Proxy:\n§7Cant connect to target server and your last server kicked you.\n§6Message: ");
                    if(cparms->getPlayerConnection()->getLastDisconnectMessage() != nullptr)
                        message->addSibling(cparms->getPlayerConnection()->getLastDisconnectMessage()->clone());
                    else
                        message->addSibling(new ChatMessage("No message"));
                    cparms->getPlayerConnection()->disconnect(message);
                }else
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
    target = EventHelper::callServerConnectEvent(this, target);
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
    debugMessage("Close player connection!");
    if(!this->open)
        return;
    this->open = false;
    Connection::closeChannel();
    VectorUtils::remove(PlayerConnection::connections, this);
    VectorUtils::remove(PlayerConnection::activeConnections, this);
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

    SHA1_Update(&context, (unsigned char*) Cipper::hash.data(), Cipper::hash.size());
    SHA1_Update(&context, (unsigned char*) secret.data(), secret.size());
    SHA1_Update(&context, (unsigned char*) data.data(), data.size());
    SHA1_Final(md, &context);

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

void PlayerConnection::setState(ConnectionState state) {
    Connection::setState(state);
    //TODO set java state!
    if(JavaPluginManagerImpl::instance != nullptr) {
        jobject obj = nullptr;
        switch (state) {
            case HANDSHAKING:
                obj = JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection$state_handshaking;
                break;
            case STATUS:
                obj = JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection$state_status;
                break;
            case LOGIN:
                obj = JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection$state_login;
                break;
            case ENCRIPTING:
                obj = JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection$state_encripting;
                break;
            case PLAYING:
                obj = JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection$state_playing;
                break;
            case CLOSED:
                obj = JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection$state_disconnected;
                break;
        }
        JavaPluginManagerImpl::instance->getEnv()->SetObjectField(this->javaInstance, JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection_connectionState, obj);
    }
}

PlayerConnection* getPlayerConnection(jobject player){
    return JavaPluginManagerImpl::instance->runOperation([player](JNIEnv* env){
        jlong playerId = JavaPluginManagerImpl::instance->getEnv()->GetLongField(player, JavaPluginManagerImpl::instance->getRefelectManager()->f_playerConnection_nativeAdress);
        vector<PlayerConnection*> pc = PlayerConnection::connections;
        for(vector<PlayerConnection*>::iterator current = pc.begin();current != pc.end();current++)
            if((*current)->getJavaNativeAddress() == playerId)
                return *current;
        JavaPluginManagerImpl::instance->getEnv()->ThrowNew(JavaPluginManagerImpl::instance->getRefelectManager()->clazz_illegalArgumentException, "Cant find player connection!");
        return (PlayerConnection*) nullptr;
    });
}

void PlayerConnection::NATIVE_disconnect0(JNIEnv *env, jobject caller, jstring message) {
    PlayerConnection* conn = getPlayerConnection(caller);
    if(conn == nullptr || conn->getState() == ConnectionState::CLOSED)
        return;
    const char* cmassage = JavaPluginManagerImpl::instance->getEnv()->GetStringUTFChars(message, 0);
    conn->disconnect(new ChatMessage(string(cmassage)));
}

jboolean PlayerConnection::NATIVE_sendPacket0(JNIEnv *env, jobject caller, jobject jstorage) {
    PlayerConnection* conn = getPlayerConnection(caller);
    if(conn == nullptr || conn->getState() == ConnectionState::CLOSED)
        return 0;

    DataStorage storage;
    JavaPluginManagerImpl::instance->getStorageImpl()->fromJavaObject(jstorage, &storage);

    DataBuffer buffer (storage.bytes.size()+DataBuffer::getVarIntSize(storage.ints[0]));
    buffer.writeVarInt(storage.ints[0]);
    for(vector<uint8_t>::iterator it = storage.bytes.begin(); it != storage.bytes.end(); it++)
        buffer.writeByte(*it);
    conn->writePacket(&buffer);
    return 1;
}

void PlayerConnection::handleConnectionClosed() {
    debugMessage("Connection closed");
    if(getCurrentTargetConnection() != nullptr)
        getCurrentTargetConnection()->disconnect(new ChatMessage("Player disconnected"));
}

void PlayerConnection::handleException(Exception *data) {
    debugMessage("Having exception: "+string(data->what()));
}

void PlayerConnection::handlePacket(DataBuffer *data) {
    this->packetHandler->handlePacket(data);
}
