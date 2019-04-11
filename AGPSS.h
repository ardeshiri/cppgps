#ifndef AGPSS_H
#define AGPSS_H

#include <atomic>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <mutex>
#include <condition_variable>

#include "Error.h"
#define NMEASTRMAXLEN 200
using std::string;

class AGPSS
{
    public:
        AGPSS(const std::string& prt="");
        virtual ~AGPSS();
        AGPSS(const AGPSS&) = delete;
        AGPSS(AGPSS&&) = delete;
        AGPSS& operator=(const AGPSS&) = delete;
        AGPSS& operator=(AGPSS&&) = delete;

        int serialConnect(const string prt="");
        void GPSSStart();
        int getRowLine(char* outline,size_t ls);
        bool lineIsNew() const noexcept;
        bool lineIsValid() const noexcept;
        bool lineIsCorrect() const;
        bool GPSActive() const noexcept;
        void DeactivGPS();
        void turnOff();

    protected:

    private:
        int fd;
        struct termios SerialPortSettings;
        std::atomic<bool> newline;
        int readLine();
        char line[NMEASTRMAXLEN];
        bool valid;
        std::string strport;
        bool GPSActiveF;
        std::mutex mtx;
        std::condition_variable cv{};
        bool exit;
};

#endif // AGPS_H
