#ifndef GPSCORE_H
#define GPSCORE_H

#include "AGPSS.h"
#include <iostream>
#include <future>
#include <thread>
#include <string.h>
#include <map>
#include <vector>
#include <chrono>
#include <algorithm>
#include "Error.h"

#define NMEA_SUBSEC_SIZE 50
#define SLEEP_BEFORE_CONNECT 5

using namespace std::chrono;

struct NMEAData
{
    std::vector<std::string> strvec{};
    high_resolution_clock::time_point uptime;
    NMEAData():uptime(high_resolution_clock::now())
    {}
};
typedef struct NMEAData data;


class gpscore
{
    public:
        gpscore(const std::string port = "");
        virtual ~gpscore();
        void setPort(const std::string port = "");
        void startGPS();
        string getGPSNMEAData();
        int updateTableLoop();
        std::map<std::string, data> returnTable() const;
        bool GPSActive() const noexcept;
        void turnOff();
    protected:

    private:
        int updateTable();
        AGPSS gpss;
        std::map<std::string, data> GPSTable;
        bool GPSValidData();
        bool GPSNewData();
        bool exit;


};

#endif // GPSCORE_H
