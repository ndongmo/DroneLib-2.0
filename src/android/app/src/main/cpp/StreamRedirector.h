//
// Created by ndongmo on 25.05.24.
//

#ifndef ANDROID_STREAMREDIRECTOR_H
#define ANDROID_STREAMREDIRECTOR_H

#include <android/log.h>
#include <iostream>

class StreamBuf2Log : public std::streambuf {
public:
    enum Type { OUT, ERR };
    StreamBuf2Log(Type t) {
        this->setp(buffer, buffer + bufsize - 2); // extra space for \0 char at the end
        switch (t) {
            case OUT:
                PRIORITY = ANDROID_LOG_INFO;
                strcpy(TAG,"std::cout");
                break;
            case ERR:
                PRIORITY = ANDROID_LOG_ERROR;
                strcpy(TAG, "std::cerr");
                break;
        }
    }

private:
    int overflow(int c)
    {
        *this->pptr() = traits_type::to_char_type(c); // there is just enough space for c and \0 on buffer
        pbump(1);
        sync();
        return 0;
    }

    int sync()
    {
        int n = pptr () - pbase ();
        if (!n || (n == 1 && buffer[0] == '\n')) return 0; // spurious flushes removed
        buffer[n] = '\0';
        __android_log_write(PRIORITY, TAG, buffer);
        pbump (-n);  // Reset pptr().
        return 0;
    }

    static constexpr int bufsize = 2048;
    char TAG[10];
    android_LogPriority PRIORITY;
    char buffer[bufsize];
};

class StdStreamRedirector {
private:
    StreamBuf2Log outbuf;
    StreamBuf2Log errbuf;
public:
    StdStreamRedirector() : outbuf(StreamBuf2Log::OUT), errbuf(StreamBuf2Log::ERR) {
        std::cout.rdbuf(&outbuf);
        std::cerr.rdbuf(&errbuf);
    }
    ~StdStreamRedirector() {
        std::cout << std::flush;
        std::cerr << std::flush;
    }
};

#endif //ANDROID_STREAMREDIRECTOR_H
