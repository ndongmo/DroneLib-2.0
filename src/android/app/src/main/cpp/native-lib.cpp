#include "AndroidController.h"
#include "EventHandler.h"
#include "StreamRedirector.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#include <jni.h>
#include <thread>
#include <android/log.h>

/** The Android controller */
static AndroidController ctrl;
/** The Android touch event handler */
static EventHandler evHandler;
/** Init process */
static std::thread mainProcess;
/** Stream redirector */
static StdStreamRedirector streamRedirector;

extern "C"
JNIEXPORT void JNICALL
Java_ns_dronelib_android_MainActivity_initFromJNI(JNIEnv *env, jobject obj) {
    if (!Config::exists()) {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\n\t\"" << DRONE_ADDRESS << "\":\"" << DRONE_IPV4_ADDRESS_DEFAULT << "\"\n}";
        configFile.close();
    }
    ctrl.initConfig(&evHandler);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_ns_dronelib_android_MainActivity_startFromJNI(JNIEnv* env,jobject obj)
{
    if(ctrl.begin() != -1) {
        mainProcess = std::thread([] {
            ctrl.start();
            ctrl.end();
        });
        return true;
    } else {
        logE << "The application cannot be started!" << std::endl;
        return false;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_ns_dronelib_android_MainActivity_stopFromJNI(JNIEnv* env,jobject obj)
{
    ctrl.stop();
    if(mainProcess.joinable()) {
        mainProcess.join();
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_ns_dronelib_android_MainActivity_getBatteryFromJNI(JNIEnv *env, jobject thiz) {
    return ctrl.getBatteryLevel();
}

extern "C"
JNIEXPORT jstring JNICALL
Java_ns_dronelib_android_MainActivity_getMessageFromJNI(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(ctrl.getDebugMessage().c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_ns_dronelib_android_MainActivity_getStateFromJNI(JNIEnv *env, jobject thiz) {
    return ctrl.isRunning();
}

extern "C"
JNIEXPORT jint JNICALL
Java_ns_dronelib_android_MainActivity_getServerStateFromJNI(JNIEnv *env, jobject thiz) {
    return (jint)ctrl.getState();
}

extern "C"
JNIEXPORT void JNICALL
Java_ns_dronelib_android_MainActivity_pressEventFromJNI(JNIEnv* env, jobject /* this */, jint eventID) {
    evHandler.pressEvent((int)eventID);
}

extern "C"
JNIEXPORT void JNICALL
Java_ns_dronelib_android_MainActivity_releaseEventFromJNI(JNIEnv* env, jobject /* this */, jint eventID) {
    evHandler.releaseEvent((int)eventID);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_ns_dronelib_android_VideoDataSource_getVideoFromJNI(JNIEnv *env, jobject /* this */) {
    const UINT8 *buffer;
    int size = ctrl.getVideoStream(&buffer);

    if(size > 0 && buffer != nullptr) {
        jbyteArray bytes = env->NewByteArray(size);
        env->SetByteArrayRegion(bytes, 0, size, reinterpret_cast<const jbyte*>(buffer));

        /*std::stringstream ss;
        ss << std::hex;
        for( int i = 0; i < size; ++i )
            ss << std::setw(2) << std::setfill('0') << (int)buffer[i];
        logI << ss.str()<< std::endl << std::endl;*/

        return bytes;
    }
    else {
        return nullptr;
    }
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_ns_dronelib_android_AudioDataSource_getAudioFromJNI(JNIEnv *env, jobject /* this */) {
    const UINT8 *buffer;
    int size = ctrl.getAudioStream(&buffer);

    if(size > 0 && buffer != nullptr) {
        jbyteArray bytes = env->NewByteArray(size);
        env->SetByteArrayRegion(bytes, 0, size, reinterpret_cast<const jbyte*>(buffer));
        return bytes;
    }
    else {
        return nullptr;
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_ns_dronelib_android_JNIHelper_getInt(JNIEnv *env, jclass clazz, jstring key) {
    const char *nativeKey = env->GetStringUTFChars(key, 0);
    int result = Config::getInt(nativeKey);
    env->ReleaseStringUTFChars(key, nativeKey);
    return result;
}
extern "C"
JNIEXPORT jstring JNICALL
Java_ns_dronelib_android_JNIHelper_getString(JNIEnv *env, jclass clazz, jstring key) {
    const char *nativeKey = env->GetStringUTFChars(key, 0);
    const std::string result = Config::getString(nativeKey);
    env->ReleaseStringUTFChars(key, nativeKey);
    return env->NewStringUTF(result.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_ns_dronelib_android_JNIHelper_setInt(JNIEnv *env, jclass clazz, jstring key, jint value) {
    const char *nativeKey = env->GetStringUTFChars(key, 0);
    Config::setInt(nativeKey, (int)value);
    env->ReleaseStringUTFChars(key, nativeKey);
}
extern "C"
JNIEXPORT void JNICALL
Java_ns_dronelib_android_JNIHelper_setString(JNIEnv *env, jclass clazz, jstring key,
                                             jstring value) {
    const char *nativeKey = env->GetStringUTFChars(key, 0);
    const char *nativeValue = env->GetStringUTFChars(value, 0);
    Config::setString(nativeKey, std::string(nativeValue));
    env->ReleaseStringUTFChars(key, nativeKey);
    env->ReleaseStringUTFChars(value, nativeValue);
}
extern "C"
JNIEXPORT void JNICALL
Java_ns_dronelib_android_JNIHelper_saveConfig(JNIEnv *env, jclass clazz) {
    std::string json = Config::encodeJson({
        DRONE_ADDRESS, DRONE_PORT_DISCOVERY,
        VIDEO_FPS, VIDEO_WIDTH, VIDEO_HEIGHT,
        AUDIO_CHANNELS, AUDIO_SAMPLE, AUDIO_NB_SAMPLES,
        CAMERA_ACTIVE, MICRO_ACTIVE, SERVOS_ACTIVE, MOTORS_ACTIVE,
        LEDS_ACTIVE, BATTERY_ACTIVE, BUZZER_ACTIVE
    });

    std::ofstream configFile(CONFIG_FILE);
    configFile << json;
    configFile.close();
}