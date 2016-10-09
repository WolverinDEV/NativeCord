//
// Created by wolverindev on 11.09.16.
//

#include "../../../include/plugin/java/JavaPluginManagerImpl.h"
#include "../../../include/plugin/java/JavaPlugin.h"

JavaPluginManagerImpl* JavaPluginManagerImpl::instance = nullptr;

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
    JavaVMOption options[1];
    options[0].optionString = "-Djava.class.path=/home/wolverindev/ClionProjects/CBungee/java/JavaPluginManager/target/JavaImplementation-1.0-SNAPSHOT.jar"; //TODO add java plugin manager
    vm_args.version = 0x00010002;
    vm_args.options = options;
    vm_args.nOptions = 1;
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
            {"unregisterListener", "(JLjava/lang/Object;)V", (void*) &NATIVE_unregisterListener1}
    };

    debugMessage("Registering "+to_string(sizeof(methods) / sizeof(*methods))+" native methods");
    getEnv()->RegisterNatives(reflectManager->clazz_pluginManagerImpl, methods, sizeof(methods) / sizeof(*methods));
    flushException();
    return 1;
}

bool JavaPluginManagerImpl::stopJavaVM() {
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

const DataStorageImpl* JavaPluginManagerImpl::getStorageImpl() const {
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

long JavaPluginManagerImpl::NATIVE_registerPlugin(JNIEnv *env, jobject manager, jobject plugin) {
    JavaPlugin* nplugin = new JavaPlugin(*(JavaPluginManagerImpl::instance), plugin);
    JavaPluginManagerImpl::instance->plugins.push_back(nplugin);
    nplugin->load();
    return nplugin->getPluginId();
}

void JavaPluginManagerImpl::NATIVE_enablePlugin(JNIEnv *env, jobject manager, long plugin) {}

void JavaPluginManagerImpl::NATIVE_disablePlugin(JNIEnv *env, jobject manager, long plugin) {}

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

    jobject listenerInstance = env->NewGlobalRef(listenerClassInstance);
    plugin->registerEventListener((EventType) eventId, eventName, [eventId, eventName, clazzName, listenerInstance, methodeId](DataStorage* data){
        JavaPluginManagerImpl::instance->getEnv()->CallVoidMethod(listenerInstance, methodeId, EventHelper::createJavaInstance(JavaPluginManagerImpl::instance, (EventType) eventId, data));
        return 0;
    });
}

void JavaPluginManagerImpl::NATIVE_unregisterListener0(JNIEnv *env, jobject manager, jlong plugin) {}

void JavaPluginManagerImpl::NATIVE_unregisterListener1(JNIEnv *env, jobject manager, jlong plugin, jobject listenerClassInstance) {}