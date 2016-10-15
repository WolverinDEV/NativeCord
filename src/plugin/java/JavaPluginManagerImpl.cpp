//
// Created by wolverindev on 11.09.16.
//

#include "../../../include/plugin/java/JavaPluginManagerImpl.h"
#include "../../../include/plugin/java/JavaPlugin.h"
#include <sys/types.h>

void env_destroy(void * env)
{
    JNIEnv* e = (JNIEnv*) env;
    JavaVM* jvm;
    e->GetJavaVM(&jvm);
    jvm->DetachCurrentThread();
    debugMessage("Destroy JNIEnv attachment!");
}

pthread_key_t createKey(){
    pthread_key_t key;
    pthread_key_create(&key, env_destroy);
    return key;
}

JavaPluginManagerImpl* JavaPluginManagerImpl::instance = nullptr;
pthread_key_t JavaPluginManagerImpl::ENV_KEY = createKey();

JavaPluginManagerImpl::~JavaPluginManagerImpl() {
    this->plugins.clear();
    this->env = nullptr;
    this->jvm = nullptr;
    delete this->storageImpl;
}

JavaPluginManagerImpl::JavaPluginManagerImpl() {
    JavaPluginManagerImpl::instance = this;
    cout << "Create java plugin manager" << endl;
}

bool JavaPluginManagerImpl::startJavaVM() {
    int res;
#ifdef JNI_VERSION_1_2
    JavaVMInitArgs vm_args;
    JavaVMOption options[2];
    options[0].optionString = "-Djava.class.path=/home/wolverindev/ClionProjects/CBungee/java/JavaPluginManager/target/JavaImplementation-1.0-SNAPSHOT.jar"; //TODO add java plugin manager
    options[1].optionString = "-verbose:sizes";
    vm_args.version = 0x00010002;
    vm_args.options = options;
    vm_args.nOptions = 2;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&jvm, (void **) &env, &vm_args);
#else
    JDK1_1InitArgs vm_args;
    char classpath[1024];
    vm_args.version = 0x00010001;
    JNI_GetDefaultJavaVMInitArgs(&vm_args);
    /* Append USER_CLASSPATH to the default system class path */
    sprintf(classpath, "%s%c%s",
            vm_args.classpath, PATH_SEPARATOR, USER_CLASSPATH);
    vm_args.classpath = classpath;
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&jvm, &env, &vm_args);
#endif

    if (res < 0) {
        fprintf(stderr, "Can't create Java VM\n");
        return 0;
    }
    this->reflectManager = new JavaReflectManager(*this);
    if(this->reflectManager->loadFields() == 0){
        logError("An error happend while loading fields!");
        return 0;
    }
    this->storageImpl = new DataStorageImpl(*this);
    return 1;
}

bool JavaPluginManagerImpl::registerNatives() {
    static JNINativeMethod methods[] = {
            {"registerPlugin0", "(Ldev/wolveringer/nativecord/plugin/Plugin;)J", (void *)&NATIVE_registerPlugin},
            {"enablePlugin", "(J)V", (void*) &NATIVE_enablePlugin},
            {"disablePlugin", "(J)V", (void*) &NATIVE_disablePlugin},
            {"registerListener", "(JLjava/lang/Object;ILjava/lang/reflect/Method;)V", (void*) &NATIVE_registerListener},
            {"unregisterListener", "(J)V", (void*) &NATIVE_unregisterListener0},
            {"unregisterListener", "(JLjava/lang/Object;)V", (void*) &NATIVE_unregisterListener1},
            {"sendPacket0","(Ldev/wolveringer/nativecord/impl/DataStorage;)Z", (void*) &PlayerConnection::NATIVE_sendPacket0},
            {"disconnect0","(Ljava/lang/String;)V", (void*) &PlayerConnection::NATIVE_disconnect0}
    };

    debugMessage("Registering "+to_string(sizeof(methods) / sizeof(*methods))+" native methods");
    getEnv()->RegisterNatives(reflectManager->clazz_pluginManagerImpl, methods, sizeof(methods) / sizeof(*methods));
    flushException();
    return 1;
}

void JavaPluginManagerImpl::runOperation(std::function<void(JNIEnv *)> func) {

}

JNIEnv* JavaPluginManagerImpl::getEnv() {
    JNIEnv* ptr = nullptr;

    if(nullptr == (ptr = (JNIEnv*) pthread_getspecific(ENV_KEY))){
        debugMessage("Create JNIEnv attachment!");
        this->jvm->AttachCurrentThread((void**) &ptr, NULL);
        pthread_setspecific(ENV_KEY, ptr);
    }

    return ptr;

   /* //Old
    switch (this->jvm->GetEnv((void**) &ptr, JNI_VERSION_1_6)){
        case JNI_EDETACHED:
            if(this->jvm->AttachCurrentThread((void**)&env,NULL) == 0){
                logError("AttachCurrentThread(void** penv, void* args) was not successful. This may be due to the thread being attached already to another JVM instance");
                return ptr;
            }
        case JNI_OK:
            debugMessage("JVM successfull attached from "+to_string(pthread_self()));
    };
    */
}

bool JavaPluginManagerImpl::stopJavaVM() {
    JNIEnv* env = getEnv();
    jclass clazz_system = env->FindClass("java/lang/System");
    jmethodID m_exit = env->GetStaticMethodID(clazz_system,"exit","(I)V");
    env->CallStaticVoidMethod(clazz_system, m_exit, 0);
    debugMessage("Detroy jvm after System.exit(1)");
    this->jvm->DestroyJavaVM();
    return 1;
}

vector<Plugin *> &JavaPluginManagerImpl::getLoadedPlugins() {
    return plugins;
}

vector<Plugin *> &JavaPluginManagerImpl::getEnabledPlugins() {
    return plugins;
}

bool JavaPluginManagerImpl::enablePlugin(Plugin* hplugin) {
    JavaPlugin* plugin = (JavaPlugin*) hplugin;
    plugin->enable();
    return 1;
}

bool JavaPluginManagerImpl::disablePlugin(Plugin *plugin) {
    return false;
}

bool JavaPluginManagerImpl::enable() {
    if(!startJavaVM()) {
        logError("Cant start jvm!");
        return 0;
    }
    if (!registerNatives()){
        logError("Cant register natives");
        return 0;
    }
    pluginManagerImpl = getEnv()->AllocObject(getRefelectManager()->clazz_pluginManagerImpl);
    pluginManager = getEnv()->NewObject(getRefelectManager()->clazz_pluginManager, getRefelectManager()->c_pluginManager, pluginManagerImpl);

    getEnv()->CallVoidMethod(pluginManager, getRefelectManager()->m_pluginManager_loadPlugins);
    return 1;
}

bool JavaPluginManagerImpl::disable() {
    return stopJavaVM();
}

DataStorageImpl* JavaPluginManagerImpl::getStorageImpl() {
    return storageImpl;
}

bool JavaPluginManagerImpl::flushException() {
    if(getEnv()->ExceptionCheck()){
        getEnv()->ExceptionDescribe();
        getEnv()->ExceptionClear();
        return 1;
    }
    return 0;
}

void JavaPluginManagerImpl::callEvent(EventType type, DataStorage *storage) {
    jobject obj = getEnv()->NewGlobalRef(EventHelper::createJavaInstance(this,type,storage));

    for(vector<Plugin*>::iterator it = this->plugins.begin(); it != this->plugins.end();it++){
        if(dynamic_cast<JavaPlugin*>(*it) == nullptr)
            continue;
        JavaPlugin* plugin = (JavaPlugin*) *it;
        plugin->callEvent(type,obj);
    }
    getEnv()->DeleteGlobalRef(obj);
}

long JavaPluginManagerImpl::NATIVE_registerPlugin(JNIEnv *env, jobject manager, jobject plugin) {
    JavaPlugin* nplugin = new JavaPlugin(*(JavaPluginManagerImpl::instance), plugin);
    JavaPluginManagerImpl::instance->plugins.push_back(nplugin);
    nplugin->load();
    return nplugin->getPluginId();
}

void JavaPluginManagerImpl::NATIVE_enablePlugin(JNIEnv *env, jobject manager, long plugin) {}

void JavaPluginManagerImpl::NATIVE_disablePlugin(JNIEnv *env, jobject manager, long plugin) {}

struct EventProps {
    jmethodID id;
    jobject instanceObject;
    jobject object;
};

void JavaPluginManagerImpl::NATIVE_registerListener(JNIEnv *env, jobject jmanager, jlong jplugin, jobject listenerClassInstance, jint eventId, jobject ref_methode) {
    JavaPluginManagerImpl* manager = JavaPluginManagerImpl::instance;
    Plugin* hplugin = manager->getPlugin(jplugin);
    if(hplugin == nullptr) {
        env->ThrowNew(manager->getRefelectManager()->clazz_illegalArgumentException,"Cant find plugin!");
        debugMessage("Cant find plugin "+to_string(jplugin));
        return;
    }
    if(dynamic_cast<JavaPlugin*>(hplugin) == nullptr){ //TODO test
        debugMessage("Plugin isnt a java plugin!");
        env->ThrowNew(manager->getRefelectManager()->clazz_illegalArgumentException,"Plugin isnt an JavaPlugin!");
        return;
    }
    JavaPlugin* plugin = (JavaPlugin*) hplugin;
    jmethodID methodeId = manager->getEnv()->FromReflectedMethod(ref_methode);
    string clazzName = manager->getRefelectManager()->classToString(listenerClassInstance);
    string eventName = clazzName+"#"+manager->getRefelectManager()->methodeToString(ref_methode)+"(...)";
    if(plugin->isEventListenerRegistered((EventType) eventId, eventName)){
        env->ThrowNew(manager->getRefelectManager()->clazz_illegalArgumentException, ("Event handler "+eventName+" alredyregistered!").c_str());
        debugMessage("Alredy registered!");
        return;
    }
    cout << "Methode id: " << methodeId << endl;

    jobject listenerInstance = env->NewGlobalRef(listenerClassInstance);
    plugin->registerEventListener((EventType) eventId, eventName, [eventId, ref_methode, manager, eventName, clazzName, listenerInstance, methodeId](void* data){ //TODO void* ptr check!
        cout << "Methode id: " << methodeId << endl;
        JavaPluginManagerImpl::instance->getEnv()->CallObjectMethod(listenerInstance, methodeId, data); //*((jobject*) data)
        return 0;
    });
}

void JavaPluginManagerImpl::NATIVE_unregisterListener0(JNIEnv *env, jobject manager, jlong plugin) {}

void JavaPluginManagerImpl::NATIVE_unregisterListener1(JNIEnv *env, jobject manager, jlong plugin, jobject listenerClassInstance) {}