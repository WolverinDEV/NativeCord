//
// Created by wolverindev on 04.10.16.
//

#include "../../../../include/plugin/java/jni/DataStorageImpl.h"
#include "../../../../include/plugin/java/JavaReflectManager.h"
#include "../../../../include/plugin/java/JavaPluginManagerImpl.h"

DataStorage* DataStorageImpl::fromJavaObject(jobject object){
    DataStorage* storage = new DataStorage;
    fromJavaObject(object, storage);
    return  storage;
}

void DataStorageImpl::fromJavaObject(jobject object, DataStorage *storage) {
    jboolean _true = true;
    JavaReflectManager* ref = handle.getRefelectManager();
    
    int length = handle.getEnv()->GetIntField(object, ref->f_datastorage_stringLength);

    if(length > 0){
        jobjectArray strings = (jobjectArray) handle.getEnv()->GetObjectField(object, ref->f_datastorage_strings);
        for(int i = 0;i<length;i++)
            storage->strings.push_back(string(handle.getEnv()->GetStringUTFChars((jstring) handle.getEnv()->GetObjectArrayElement(strings, i), &_true)));
    }

    length = handle.getEnv()->GetIntField(object, ref->f_datastorage_longLength);
    if(length > 0){
        jlong * longs = handle.getEnv()->GetLongArrayElements((jlongArray) handle.getEnv()->GetObjectField(object, ref->f_datastorage_longs),&_true);
        storage->longs = vector<uint64_t>(longs, &longs[length]);
    }

    length = handle.getEnv()->GetIntField(object, ref->f_datastorage_intLength);
    if(length > 0){
        jint * ints = handle.getEnv()->GetIntArrayElements((jintArray) handle.getEnv()->GetObjectField(object, ref->f_datastorage_ints), &_true);
        storage->ints = vector<uint32_t>(ints, &ints[length]);
    }

    length = handle.getEnv()->GetByteField(object, ref->f_datastorage_byteLength);
    if(length > 0){
        jbyte* ints = handle.getEnv()->GetByteArrayElements((jbyteArray) handle.getEnv()->GetObjectField(object, ref->f_datastorage_bytes), &_true);
        storage->bytes = vector<uint8_t>(ints, &ints[length]);
    }

    length = handle.getEnv()->GetByteField(object, ref->f_datastorage_doubleLength);
    if(length > 0){
        jdouble * ints = handle.getEnv()->GetDoubleArrayElements((jdoubleArray) handle.getEnv()->GetObjectField(object, ref->f_datastorage_doubles), &_true);
        storage->doubles = vector<double>(ints, &ints[length]);
    }

    length = handle.getEnv()->GetByteField(object, ref->f_datastorage_floatLength);
    if(length > 0){
        jfloat * ints = handle.getEnv()->GetFloatArrayElements((jfloatArray) handle.getEnv()->GetObjectField(object, ref->f_datastorage_floats), &_true);
        storage->floats = vector<float >(ints, &ints[length]);
    }
}

jobject DataStorageImpl::toJavaObject(DataStorage& storage){
    JavaReflectManager* ref = handle.getRefelectManager();
    jobject clazzInstance = handle.getEnv()->AllocObject(ref->clazz_datastorage);

    if(storage.strings.size() > 0){
        jobjectArray array = handle.getEnv()->NewObjectArray(storage.strings.size(), handle.getEnv()->FindClass("java/lang/String"), handle.getEnv()->NewStringUTF(""));
        for(int i = 0;i<storage.strings.size();i++){
            handle.getEnv()->SetObjectArrayElement(array, i, handle.getEnv()->NewStringUTF(storage.strings[i].c_str()));
        }
        handle.getEnv()->SetObjectField(clazzInstance, ref->f_datastorage_strings, array);
        handle.getEnv()->SetIntField(clazzInstance, ref->f_datastorage_stringLength, storage.strings.size());
    }

    if(storage.longs.size() > 0) {
        jlongArray array = handle.getEnv()->NewLongArray(storage.longs.size());
        handle.getEnv()->SetLongArrayRegion(array, 0, storage.longs.size(), (const jlong *) &(storage.longs[0]));
        handle.getEnv()->SetObjectField(clazzInstance, ref->f_datastorage_longs, array);
        handle.getEnv()->SetIntField(clazzInstance, ref->f_datastorage_longLength, storage.longs.size());
    }

    if(storage.ints.size() > 0) {
        jintArray array = handle.getEnv()->NewIntArray(storage.ints.size());
        handle.getEnv()->SetIntArrayRegion(array, 0, storage.ints.size(), (const jint *) &(storage.ints[0]));
        handle.getEnv()->SetObjectField(clazzInstance, ref->f_datastorage_ints, array);
        handle.getEnv()->SetIntField(clazzInstance, ref->f_datastorage_intLength, storage.ints.size());
    }

    if(storage.bytes.size() > 0) {
        jbyteArray array = handle.getEnv()->NewByteArray(storage.bytes.size());
        handle.getEnv()->SetByteArrayRegion(array, 0, storage.bytes.size(), (const jbyte *) &(storage.bytes[0]));
        handle.getEnv()->SetObjectField(clazzInstance, ref->f_datastorage_bytes, array);
        handle.getEnv()->SetIntField(clazzInstance, ref->f_datastorage_byteLength, storage.bytes.size());
    }

    if(storage.doubles.size() > 0) {
        jdoubleArray array = handle.getEnv()->NewDoubleArray(storage.doubles.size());
        handle.getEnv()->SetDoubleArrayRegion(array, 0, storage.doubles.size(), (const jdouble *) &(storage.doubles[0]));
        handle.getEnv()->SetObjectField(clazzInstance, ref->f_datastorage_doubles, array);
        handle.getEnv()->SetIntField(clazzInstance, ref->f_datastorage_doubleLength, storage.doubles.size());
    }

    if(storage.floats.size() > 0) {
        jfloatArray array = handle.getEnv()->NewFloatArray(storage.floats.size());
        handle.getEnv()->SetFloatArrayRegion(array, 0, storage.floats.size(), (const jfloat *) &(storage.floats[0]));
        handle.getEnv()->SetObjectField(clazzInstance, ref->f_datastorage_floats, array);
        handle.getEnv()->SetIntField(clazzInstance, ref->f_datastorage_floatLength, storage.floats.size());
    }

    return clazzInstance;
}