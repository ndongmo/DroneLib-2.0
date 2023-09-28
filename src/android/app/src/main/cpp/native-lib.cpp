#include "AndroidController.h"
#include "EventHandler.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#include <jni.h>
#include <thread>

/** The Android controller */
static AndroidController ctrl;
/** The Android touch event handler */
static EventHandler evHandler;
/** Init process */
static std::thread mainProcess;


extern "C" JNIEXPORT jboolean JNICALL
Java_ns_dronelib_android_MainActivity_startFromJNI(JNIEnv* env,jobject obj)
{
    ctrl.updateEnv(env, obj);

    if (!Config::exists()) {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\n\t\"" << DRONE_ADDRESS << "\":\"" << DRONE_IPV4_ADDRESS_DEFAULT << "\"\n}";
        configFile.close();
    }
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

extern "C" JNIEXPORT void JNICALL
Java_ns_dronelib_android_MainActivity_updateEventFromJNI(JNIEnv* env, jobject /* this */, jint eventID) {
    evHandler.pressEvent((int)eventID);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_ns_dronelib_android_MainActivity_updateVideoFromJNI(JNIEnv* env, jobject /* this */) {
    UINT8 *buffer = nullptr;
    int size = ctrl.getVideoStream(buffer);

    if(size > 0 && buffer != nullptr) {
        jbyteArray bytes = env->NewByteArray(size);
        env->SetByteArrayRegion(bytes, 0, size, reinterpret_cast<jbyte*>(buffer));
        return bytes;
    } 
    else {
        return env->NewByteArray(0);
    }
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_ns_dronelib_android_MainActivity_updateAudioFromJNI(JNIEnv* env, jobject /* this */) {
    UINT8 *buffer = nullptr;
    int size = ctrl.getAudioStream(buffer);

    if(size > 0 && buffer != nullptr) {
        jbyteArray bytes = env->NewByteArray(size);
        env->SetByteArrayRegion(bytes, 0, size, reinterpret_cast<jbyte*>(buffer));
        return bytes;
    } 
    else {
        return env->NewByteArray(0);
    }
}
