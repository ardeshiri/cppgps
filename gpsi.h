#ifndef GPSI_H
#define GPSI_H
#include <iostream>
#include "gpscore.h"
#include <chrono>
#include <ctime>
#include <iomanip>


#define DEBGGA  /// for debugging gga
#define DEBGLL  /// for debugging gll
#define DEBRMC  /// for debugging rmc
#define DEBVTG


 struct GPS_time
{
    int h;
    int m;
    float s;
};
typedef struct GPS_time gps_time;


 struct GPS_date
{
    int d;
    int m;
    int y;
};
typedef struct GPS_date gps_date;

struct GPS_speed
{
    float knots;
    char n;
    float kph;
    char k;
    float cog;
    char t;
};
typedef struct GPS_speed gps_speed;

 struct GPS_location
{
    int lat_d;
    int lat_m;
    float lat_s;
    double latDD;
    int lon_d;
    int lon_m;
    float lon_s;
    double lonDD;
};
typedef struct GPS_location gps_location;

enum class mainDirs{none,N,S,E,W};
enum class status{none,A,V};
enum class posMode{none,N,E,D,A,F,R};

struct GPS_loc
{
    std::string sat;
    high_resolution_clock::time_point rcvtime;
    gps_time tme;
    gps_date dte;
    gps_location loc;
    mainDirs ns;
    mainDirs ew;
    status stts;
    gps_speed spd;
    posMode pm;
    float alt;
    float qlt;
    char uAlt;
    int numSV;
    float HDOP;
    GPS_loc():sat{},rcvtime{},tme{},dte{},
    loc{},ns{mainDirs::none},ew{mainDirs::none},
    stts{status::none},spd{},pm{posMode::none},
    alt{0},qlt{0},uAlt{'X'},numSV{0},HDOP{0}
    {}
};
typedef struct GPS_loc loc;


struct GPS_spd
{
    std::string sat;
    high_resolution_clock::time_point rcvtime;
    gps_speed spd;
    posMode pm;
    GPS_spd():sat(),rcvtime(),spd(),pm(posMode::none)
    { }
};
typedef struct GPS_spd spd;




class gpsi
{
    public:
        gpsi(const std::string port="");
        virtual ~gpsi();
        void setPort(const std::string port = "");
        void start();
        std::map<std::string, data> GPSTable() const;
        std::vector<std::pair<std::string,data>> NMEADataVec(const std::string address = "GNGGA");
        std::vector<loc> getGPSLoc();
        std::vector<spd> getGPSSpd();
        bool GPSActive() const noexcept;
        void turnOff();


    protected:

    private:
        std::string getRowNMEA();
        gpscore gps;
};

#endif // GPSI_H
