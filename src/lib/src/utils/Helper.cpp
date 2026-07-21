#include "utils/Helper.h"
#include "utils/Config.h"
#include "utils/Logger.h"
#include "utils/Constants.h"

#include <iostream>
#include <random>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
    #include <sched.h> 
#endif

using namespace utils;

std::string Helper::generateRandomString(int length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijk"
        "lmnopqrstuvwxyz0123456789";

    std::string random_string;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
    
    for (int i = 0; i < length; ++i) {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

std::string Helper::generateStreamUrl() {
    return 
        Config::getString(STREAM_PROTOCOL) + "://" +
        Config::getString(DRONE_ADDRESS) + ":" + 
        std::to_string(Config::getInt(DRONE_PORT_STREAM)) + "/" +
        generateRandomString(URL_KEY_LENGTH);
}

void Helper::configureProcess(std::thread& process, unsigned int priority, unsigned int cpuCore) {
    std::thread::native_handle_type handle = process.native_handle();
#ifdef _WIN32
    if (priority > 0) {
        if (priority > THREAD_PRIORITY_TIME_CRITICAL) {
            priority = THREAD_PRIORITY_TIME_CRITICAL; // Cap at max allowed priority
        } 
        if (!SetThreadPriority(handle, priority)) {
            logW << "Failed to set process priority. Did you run with admin rights?" << std::endl;
        }
    }
    if (cpuCore > 0) {
        DWORD_PTR mask = ((DWORD_PTR)1) << cpuCore;
        if (!SetThreadAffinityMask(handle, mask)) {
            logW << "Failed to set thread affinity for core " << cpuCore << std::endl;
        }
    }
#else
    if (priority > 0) {
        if (priority > 99) {
            priority = 99; // Cap at max allowed real-time priority
        } 
        struct sched_param param;
        param.sched_priority = priority; 
        
        if (pthread_setschedparam(handle, SCHED_FIFO, &param) != 0) {
            logW << "Failed to set process priority. Did you run with sudo?" << std::endl;
        }
    }
    if (cpuCore > 0) {
        unsigned int totalCores = std::thread::hardware_concurrency();
        if (totalCores == 0) totalCores = 1; 

        if (cpuCore >= totalCores) {
            cpuCore = totalCores - 1;
        }

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cpuCore, &cpuset);

        if (pthread_setaffinity_np(handle, sizeof(cpu_set_t), &cpuset) != 0) {
            logW << "Failed to set thread affinity for core " << cpuCore << std::endl;
        }
    }
#endif
}