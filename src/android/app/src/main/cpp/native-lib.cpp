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

std::string getConfigPath(JNIEnv *env, jstring fileDir) {
    const char *path = env->GetStringUTFChars(fileDir, 0);
    std::string completePath(path);
    completePath.append("/").append(CONFIG_FILE);
    env->ReleaseStringUTFChars(fileDir, path);
    return completePath;
}

extern "C"
JNIEXPORT void JNICALL
Java_ns_dronelib_android_MainActivity_initFromJNI(JNIEnv *env, jobject obj, jstring fileDir) {
    const std::string& path = getConfigPath(env, fileDir);

    if (!Config::existsFilePath(path)) {
        std::ofstream configFile(path);
        configFile << "{\n\t\"" << DRONE_ADDRESS << "\":\"" << DRONE_IPV4_ADDRESS_DEFAULT << "\","
            "\n\t\"" << STREAM_MODE << "\":" << STREAM_MODE_FILE << ","
            "\n\t\"" << STREAM_PROTOCOL << "\":\"rtmp\","
            "\n\t\"" << STREAM_OUTPUT_FORMAT << "\":\"flv\","
            "\n\t\"" << VIDEO_ENCODER << "\":\"libx264\""
            "}";
        configFile.close();
    }

    Config::initWithFilePath(path);
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
Java_ns_dronelib_android_JNIHelper_saveConfig(JNIEnv *env, jclass clazz, jstring fileDir) {
    std::string json = Config::encodeJson({
        DRONE_ADDRESS, DRONE_PORT_DISCOVERY, STREAM_MODE, STREAM_PROTOCOL, STREAM_OUTPUT_FORMAT,
        VIDEO_FPS, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_ENCODER,
        AUDIO_CHANNELS, AUDIO_SAMPLE, AUDIO_NB_SAMPLES,
        CAMERA_ACTIVE, MICRO_ACTIVE, SERVOS_ACTIVE, MOTORS_ACTIVE,
        LEDS_ACTIVE, BATTERY_ACTIVE, BUZZER_ACTIVE
    });
    std::ofstream configFile(getConfigPath(env, fileDir));
    configFile << json;
    configFile.close();
}