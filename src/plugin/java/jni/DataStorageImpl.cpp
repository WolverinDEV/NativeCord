//
// Created by wolverindev on 04.10.16.
//

#include "../../../../include/plugin/java/jni/DataStorageImpl.h"
#include "../../../../include/plugin/java/JavaReflectManager.h"
#include "../../../../include/plugin/java/JavaPluginManagerImpl.h"

DataStorage DataStorageImpl::fromJavaObject(jobject object){
    struct DataStorage storage;
    fromJavaObject(object, &storage);
    return storage;
}

void DataStorageImpl::fromJavaObject(jobject object, DataStorage* storage) {
    JNIEnv* env = JavaPluginManagerImpl::instance->getEnv();
    jboolean _true = true;
    JavaReflectManager* ref = handle.getRefelectManager();

    int length = env->GetIntField(object, ref->f_datastorage_stringLength);
    if(length > 0){
        jobjectArray strings = (jobjectArray) env->GetObjectField(object, ref->f_datastorage_strings);
        for(int i = 0;i<length;i++)
            storage->strings.push_back(string(env->GetStringUTFChars((jstring) env->GetObjectArrayElement(strings, i), &_true)));
    }

    length = env->GetIntField(object, ref->f_datastorage_longLength);
    if(length > 0){
        jlong * longs = env->GetLongArrayElements((jlongArray) env->GetObjectField(object, ref->f_datastorage_longs),&_true);
        storage->longs = vector<uint64_t>(longs, &longs[length]);
    }

    length = env->GetIntField(object, ref->f_datastorage_intLength);
    if(length > 0){
        jint * ints = env->GetIntArrayElements((jintArray) env->GetObjectField(object, ref->f_datastorage_ints), &_true);
        storage->ints = vector<uint32_t>(ints, &ints[length]);
    }

    length = env->GetByteField(object, ref->f_datastorage_byteLength);
    if(length > 0){
        jbyte* ints = env->GetByteArrayElements((jbyteArray) env->GetObjectField(object, ref->f_datastorage_bytes), &_true);
        storage->bytes = vector<uint8_t>(ints, &ints[length]);
    }

    length = env->GetByteField(object, ref->f_datastorage_doubleLength);
    if(length > 0){
        jdouble * ints = env->GetDoubleArrayElements((jdoubleArray) env->GetObjectField(object, ref->f_datastorage_doubles), &_true);
        storage->doubles = vector<double>(ints, &ints[length]);
    }

    length = env->GetByteField(object, ref->f_datastorage_floatLength);
    if(length > 0){
        jfloat * ints = env->GetFloatArrayElements((jfloatArray) env->GetObjectField(object, ref->f_datastorage_floats), &_true);
        storage->floats = vector<float >(ints, &ints[length]);
    }
}

jobject DataStorageImpl::toJavaObject(DataStorage& storage){
    return (jobject) handle.runOperation([&, storage](JNIEnv* env){
        JavaReflectManager* ref = handle.getRefelectManager();
        jobject clazzInstance = env->AllocObject(ref->clazz_datastorage);

        if(storage.strings.size() > 0){
            jobjectArray array = env->NewObjectArray(storage.strings.size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));
            for(int i = 0;i<storage.strings.size();i++){
                env->SetObjectArrayElement(array, i, env->NewStringUTF(storage.strings[i].c_str()));
            }
            env->SetObjectField(clazzInstance, ref->f_datastorage_strings, array);
            env->SetIntField(clazzInstance, ref->f_datastorage_stringLength, storage.strings.size());
        }

        if(storage.longs.size() > 0) {
            jlongArray array = env->NewLongArray(storage.longs.size());
            env->SetLongArrayRegion(array, 0, storage.longs.size(), (const jlong *) &(storage.longs[0]));
            env->SetObjectField(clazzInstance, ref->f_datastorage_longs, array);
            env->SetIntField(clazzInstance, ref->f_datastorage_longLength, storage.longs.size());
        }

        if(storage.ints.size() > 0) {
            jintArray array = env->NewIntArray(storage.ints.size());
            env->SetIntArrayRegion(array, 0, storage.ints.size(), (const jint *) &(storage.ints[0]));
            env->SetObjectField(clazzInstance, ref->f_datastorage_ints, array);
            env->SetIntField(clazzInstance, ref->f_datastorage_intLength, storage.ints.size());
        }

        if(storage.bytes.size() > 0) {
            jbyteArray array = env->NewByteArray(storage.bytes.size());
            env->SetByteArrayRegion(array, 0, storage.bytes.size(), (const jbyte *) &(storage.bytes[0]));
            env->SetObjectField(clazzInstance, ref->f_datastorage_bytes, array);
            env->SetIntField(clazzInstance, ref->f_datastorage_byteLength, storage.bytes.size());
        }

        if(storage.doubles.size() > 0) {
            jdoubleArray array = env->NewDoubleArray(storage.doubles.size());
            env->SetDoubleArrayRegion(array, 0, storage.doubles.size(), (const jdouble *) &(storage.doubles[0]));
            env->SetObjectField(clazzInstance, ref->f_datastorage_doubles, array);
            env->SetIntField(clazzInstance, ref->f_datastorage_doubleLength, storage.doubles.size());
        }

        if(storage.floats.size() > 0) {
            jfloatArray array = env->NewFloatArray(storage.floats.size());
            env->SetFloatArrayRegion(array, 0, storage.floats.size(), (const jfloat *) &(storage.floats[0]));
            env->SetObjectField(clazzInstance, ref->f_datastorage_floats, array);
            env->SetIntField(clazzInstance, ref->f_datastorage_floatLength, storage.floats.size());
        }

        return clazzInstance;
    });
}