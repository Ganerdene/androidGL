#include <jni.h>
#include <android/log.h>

#include "Game.h"


#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

AAssetManager *assetManager;

extern "C" {
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_init(JNIEnv *env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_example_myfirstnative_MainActivity_step(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_touch(JNIEnv *env, jclass clazz, jfloat x, jfloat y);
JNIEXPORT jint JNICALL
Java_com_example_myfirstnative_MainActivity_loadTexture(JNIEnv *env, jobject thiz,
                                                        jobject asset_manager, jstring file_name);
};

/* [Native functions] */
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_init(JNIEnv *env, jobject obj, jint width,
                                                 jint height) {
    on_surface_created();

}

JNIEXPORT void JNICALL Java_com_example_myfirstnative_MainActivity_step(JNIEnv *env, jobject obj) {
    on_update();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_touch(JNIEnv *env, jclass clazz, jfloat x, jfloat y) {

}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_myfirstnative_MainActivity_loadTexture(JNIEnv *env, jobject thiz,
                                                        jobject asset_manager, jstring file_name) {
    // TODO: implement loadTexture()

    assetManager = AAssetManager_fromJava(env, asset_manager);

    set_asset_manager(assetManager);
    return 1;
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_onSurfaceChanged(JNIEnv *env, jclass clazz, jint width,
                                                             jint height) {
    on_surface_changed(width,height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_on_1drag(JNIEnv *env, jobject thiz, jfloat normalized_x,
                                                     jfloat normalized_y, jint idx) {
    // TODO: implement on_drag()
    on_drag(normalized_x, normalized_y, idx);
}