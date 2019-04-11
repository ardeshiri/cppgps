
#include <iostream>
#include <iomanip>
#include <iterator>
#include <thread>
#include <future>
#include "AGPSS.h"
#include "Error.h"
#include "gpscore.h"
#include "gpsi.h"
using namespace std;


int main()
try
{
    gpsi gps{};
    gps.setPort("/dev/ttyUSB0");
    gps.start();

    for(int i = 0 ; i < 10000000 ; i++)
{
    cout<<"<<<"<<i;
    if(i == 400)
        gps.turnOff();

    gps.getGPSLoc();

    gps.getGPSSpd();
   // for(auto o: gps.GPSTable())
   //     cout<<o.first<<"  ";
   // cout<<endl;
   // if(!o.empty())
   // for(auto p : o)
     //   {
     //       cout<<p.loc.lat_d<<"-"<<p.loc.lat_m<<"-"<<p.loc.lat_s<<endl;
     //       cout<<p.loc.lon_d<<"-"<<p.loc.lon_m<<"-"<<p.loc.lon_s<<endl;
     //       cout<<endl;

   //     }
        //for(auto o:gps.getGPSLoc())
        //{
          //  std::time_t t= std::chrono::system_clock::to_time_t(o.rcvtime);
         //   std::cout<< o.sat << std::ctime(&t)<<std::endl;
        //}

       // for(auto p: gps.NMEADataVec(""))
        //{cout<<p.first<<"  ";
            //for(auto q: p.second.strvec)
               // cout<<q<< "-";
            //   }

        this_thread::sleep_for(std::chrono::milliseconds(100));
        if(!gps.GPSActive())
            return -1;
}

    return 0;
}
catch(const Error& err)
{
    err.what();
}


