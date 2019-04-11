#include "gpsi.h"

gpsi::gpsi(const std::string port):gps(port)
{
    //ctor
}

gpsi::~gpsi()
{
    //dtor
}

void gpsi::setPort(std::string port)
{
    gps.setPort(port);
}

void gpsi::start()
{
    gps.startGPS();
    std::thread(&gpscore::updateTableLoop, &gps).detach();
}

std::string gpsi::getRowNMEA()
{
    return gps.getGPSNMEAData();
}

std::map<std::string,data> gpsi::GPSTable() const
{
    return gps.returnTable();
}

std::vector<std::pair<std::string, data>> gpsi::NMEADataVec(const std::string address)
{
    std::vector<std::pair<std::string, data>> retvec{};
    for(auto o: gps.returnTable())
        if(o.first.find(address) != std::string::npos)
        {
            retvec.push_back(o);
        }
    return retvec;
}

std::vector<loc> gpsi::getGPSLoc()
{
    std::vector<loc> retvec{};

    for(auto o: NMEADataVec("GGA"))
    {
        if(!(o.second.strvec.empty()))
        {
            loc retloc{};

            if(!o.first.empty())
                retloc.sat.assign(o.first);
            retloc.rcvtime = o.second.uptime;

            if(!o.second.strvec[0].empty())
            {
                retloc.tme.h = stoi(o.second.strvec[0].substr(0,2));
                retloc.tme.m = stoi(o.second.strvec[0].substr(2,2));
                retloc.tme.s = stof(o.second.strvec[0].substr(4,4));
            }

            if(!o.second.strvec[1].empty())
            {
                retloc.loc.lat_d = stoi(o.second.strvec[1].substr(0,2));
                retloc.loc.lat_m = stoi(o.second.strvec[1].substr(2,2));
                retloc.loc.lat_s = (stof(o.second.strvec[1].substr(4)))*60;

                retloc.loc.latDD = ((stof(o.second.strvec[1].substr(2)))/60) + stoi(o.second.strvec[1].substr(0,2));
            }

            if(!o.second.strvec[3].empty())
            {
                retloc.loc.lon_d = stoi(o.second.strvec[3].substr(0,3));
                retloc.loc.lon_m = stoi(o.second.strvec[3].substr(3,2));
                retloc.loc.lon_s = (stof(o.second.strvec[3].substr(5)))*60;

                retloc.loc.lonDD = ((stof(o.second.strvec[3].substr(3)))/60) + stoi(o.second.strvec[3].substr(0,3));
            }

            if(!o.second.strvec[5].empty())     ///********** 1 means we've got a fix!
            {
                retloc.qlt = stof(o.second.strvec[5]);
            }

            if(!o.second.strvec[6].empty())
            {
                retloc.numSV = stoi(o.second.strvec[6]);
            }

            if(!o.second.strvec[7].empty())
            {
                retloc.HDOP = stof(o.second.strvec[7]);
            }

            if(!o.second.strvec[8].empty())
            {
                retloc.alt = stof(o.second.strvec[8]);
            }

            if(!o.second.strvec[2].empty())
            retloc.ns = ((o.second.strvec[2].compare("N")==0)?mainDirs::N:mainDirs::S);

            if(!o.second.strvec[4].empty())
            retloc.ew = ((o.second.strvec[4].compare("E")==0)?mainDirs::E:mainDirs::W);

#ifdef DEBGGA
            std::cout.precision(10);
            std::cout<<"\n===============================================\n";
            std::cout<<"sat-> "<< retloc.sat <<"**"<<std::endl;
            std::cout<<"rcvtme-> " << std::chrono::system_clock::to_time_t(retloc.rcvtime)<<"**"<<std::endl;
            std::cout<<"time-> "<<"h: "<< retloc.tme.h << " m: " << retloc.tme.m << " s: " << retloc.tme.s <<"**"<<std::endl;
            std::cout<<"lat-> "<<"d: "<<retloc.loc.lat_d<<" m: "<<retloc.loc.lat_m<<" s: "<<retloc.loc.lat_s<<"**"<<std::endl;
            std::cout<<"full degree lat-> "<<retloc.loc.latDD<<"**"<<std::endl;
            std::cout<<"lon-> "<<"d: "<<retloc.loc.lon_d<<" m: "<<retloc.loc.lon_m<<" s: "<<retloc.loc.lon_s<<"**"<<std::endl;
            std::cout<<"full degree lon-> "<<retloc.loc.lonDD<<"**"<<std::endl;
            std::cout<<"qlt-> "<<retloc.qlt<<"**"<<std::endl;
            std::cout<<"numSV-> "<<retloc.numSV<<"**"<<std::endl;
            std::cout<<"HDOP-> "<<retloc.HDOP<<"**"<<std::endl;
            std::cout<<"alt-> "<<retloc.alt<<"**"<<std::endl;
            std::cout<<"NS-> "<<(retloc.ns==mainDirs::N?"N":"S")<<"**"<<std::endl;
            std::cout<<"EW-> "<<(retloc.ew==mainDirs::E?"E":"W")<<"**"<<std::endl;
            std::cout<<"===============================================";
#endif // DEBGGA
            retvec.push_back(retloc);
        }
    }

    for(auto o: NMEADataVec("GLL"))
    {
        if(!(o.second.strvec.empty()))
        {
            loc retloc{};

            if(!o.first.empty())
                retloc.sat.assign(o.first);
            retloc.rcvtime = o.second.uptime;

            if(!o.second.strvec[4].empty())
            {
                retloc.tme.h = stoi(o.second.strvec[4].substr(0,2));
                retloc.tme.m = stoi(o.second.strvec[4].substr(2,2));
                retloc.tme.s = stof(o.second.strvec[4].substr(4,4));
            }

            if(!o.second.strvec[0].empty())
            {
                retloc.loc.lat_d = stoi(o.second.strvec[0].substr(0,2));
                retloc.loc.lat_m = stoi(o.second.strvec[0].substr(2,2));
                retloc.loc.lat_s = (stof(o.second.strvec[0].substr(4)))*60;

                retloc.loc.latDD = ((stof(o.second.strvec[0].substr(2)))/60) + stoi(o.second.strvec[0].substr(0,2));
            }

            if(!o.second.strvec[2].empty())
            {
                retloc.loc.lon_d = stoi(o.second.strvec[2].substr(0,3));
                retloc.loc.lon_m = stoi(o.second.strvec[2].substr(3,2));
                retloc.loc.lon_s = (stof(o.second.strvec[2].substr(5)))*60;

                retloc.loc.lonDD = ((stof(o.second.strvec[2].substr(3)))/60) + stoi(o.second.strvec[2].substr(0,3));
            }

            if(!o.second.strvec[5].empty())
            {
                retloc.stts = ((o.second.strvec[5].compare("A")==0)?status::A:status::V);
            }

            if(!o.second.strvec[1].empty())
            retloc.ns = ((o.second.strvec[1].compare("N")==0)?mainDirs::N:mainDirs::S);

            if(!o.second.strvec[3].empty())
            retloc.ew = ((o.second.strvec[3].compare("E")==0)?mainDirs::E:mainDirs::W);

            if(!o.second.strvec[6].empty())
            {
                if(o.second.strvec[6].at(0)=='N')
                    retloc.pm = posMode::N;
                if(o.second.strvec[6].at(0)=='E')
                    retloc.pm = posMode::E;
                if(o.second.strvec[6].at(0)=='A')
                    retloc.pm = posMode::A;
                if(o.second.strvec[6].at(0)=='D')
                    retloc.pm = posMode::D;
            }

#ifdef DEBGLL
            std::cout<<"\n===============================================\n";
            std::cout<<"sat-> "<< retloc.sat <<"**"<<std::endl;
            std::cout<<"rcvtme-> " << std::chrono::system_clock::to_time_t(retloc.rcvtime)<<"**"<<std::endl;
            std::cout<<"time-> "<<"h: "<< retloc.tme.h << " m: " << retloc.tme.m << " s: " << retloc.tme.s <<"**"<<std::endl;
            std::cout<<"lat-> "<<"d: "<<retloc.loc.lat_d<<" m: "<<retloc.loc.lat_m<<" s: "<<retloc.loc.lat_s<<"**"<<std::endl;
            std::cout<<"full degree lat-> "<<retloc.loc.latDD<<"**"<<std::endl;
            std::cout<<"lon-> "<<"d: "<<retloc.loc.lon_d<<" m: "<<retloc.loc.lon_m<<" s: "<<retloc.loc.lon_s<<"**"<<std::endl;
            std::cout<<"full degree lon-> "<<retloc.loc.lonDD<<"**"<<std::endl;
            std::cout<<"NS-> "<<(retloc.ns==mainDirs::N?"N":"S")<<"**"<<std::endl;
            std::cout<<"EW-> "<<(retloc.ew==mainDirs::E?"E":"W")<<"**"<<std::endl;
            std::cout<<"stts-> "<<(retloc.stts==status::A?"A":"V")<<"**"<<std::endl;
            if(retloc.pm == posMode::A)
                std::cout<<"posMode-> "<<"A"<<"**"<<std::endl;
            if(retloc.pm == posMode::D)
                std::cout<<"posMode-> "<<"D"<<"**"<<std::endl;
            if(retloc.pm == posMode::E)
                std::cout<<"posMode-> "<<"E"<<"**"<<std::endl;
            if(retloc.pm == posMode::N)
                std::cout<<"posMode-> "<<"N"<<"**"<<std::endl;
            if(retloc.pm == posMode::none)
                std::cout<<"posMode-> "<<"none"<<"**"<<std::endl;

            std::cout<<"===============================================";
#endif // DEBGLL
            retvec.push_back(retloc);
        }
    }

        for(auto o: NMEADataVec("RMC"))
    {
        if(!(o.second.strvec.empty()))
        {
            loc retloc{};

            if(!o.first.empty())
                retloc.sat.assign(o.first);
            retloc.rcvtime = o.second.uptime;

            if(!o.second.strvec[0].empty())
            {
                retloc.tme.h = stoi(o.second.strvec[0].substr(0,2));
                retloc.tme.m = stoi(o.second.strvec[0].substr(2,2));
                retloc.tme.s = stof(o.second.strvec[0].substr(4,4));
            }

            if(!o.second.strvec[2].empty())
            {
                retloc.loc.lat_d = stoi(o.second.strvec[2].substr(0,2));
                retloc.loc.lat_m = stoi(o.second.strvec[2].substr(2,2));
                retloc.loc.lat_s = (stof(o.second.strvec[2].substr(4)))*60;

                retloc.loc.latDD = ((stof(o.second.strvec[2].substr(2)))/60) + stoi(o.second.strvec[2].substr(0,2));
            }

            if(!o.second.strvec[4].empty())
            {
                retloc.loc.lon_d = stoi(o.second.strvec[4].substr(0,3));
                retloc.loc.lon_m = stoi(o.second.strvec[4].substr(3,2));
                retloc.loc.lon_s = (stof(o.second.strvec[4].substr(5)))*60;

                retloc.loc.lonDD = ((stof(o.second.strvec[4].substr(3)))/60) + stoi(o.second.strvec[4].substr(0,3));
            }

            if(!o.second.strvec[1].empty())
            {
                retloc.stts = ((o.second.strvec[1].compare("A")==0)?status::A:status::V);
            }

            if(!o.second.strvec[6].empty())
            {
                retloc.spd.knots = stof(o.second.strvec[6]);
                retloc.spd.n = 'N';
            }

            if(!o.second.strvec[7].empty())
            {
                retloc.spd.cog = stof(o.second.strvec[7]);
                retloc.spd.t = '_';
            }

            if(!o.second.strvec[3].empty())
            retloc.ns = ((o.second.strvec[3].compare("N")==0)?mainDirs::N:mainDirs::S);

            if(!o.second.strvec[5].empty())
            retloc.ew = ((o.second.strvec[5].compare("E")==0)?mainDirs::E:mainDirs::W);

            if(!o.second.strvec[8].empty())
            {
                retloc.dte.y = stoi(o.second.strvec[8].substr(4,2));
                retloc.dte.m = stoi(o.second.strvec[8].substr(2,2));
                retloc.dte.d = stoi(o.second.strvec[8].substr(0,2));
            }

            if(!o.second.strvec[11].empty())
            {
                if(o.second.strvec[11].at(0)=='N')
                    retloc.pm = posMode::N;
                if(o.second.strvec[11].at(0)=='E')
                    retloc.pm = posMode::E;
                if(o.second.strvec[11].at(0)=='A')
                    retloc.pm = posMode::A;
                if(o.second.strvec[11].at(0)=='D')
                    retloc.pm = posMode::D;
            }

#ifdef DEBRMC
            std::cout<<"\n===============================================\n";
            std::cout<<"sat-> "<< retloc.sat <<"**"<<std::endl;
            std::cout<<"rcvtme-> " << std::chrono::system_clock::to_time_t(retloc.rcvtime)<<"**"<<std::endl;
            std::cout<<"time-> "<<"h: "<< retloc.tme.h << " m: " << retloc.tme.m << " s: " << retloc.tme.s <<"**"<<std::endl;
            std::cout<<"date-> "<<"y: "<< retloc.dte.y << " m: " << retloc.dte.m << " d: " << retloc.dte.d <<"**"<<std::endl;
            std::cout<<"lat-> "<<"d: "<<retloc.loc.lat_d<<" m: "<<retloc.loc.lat_m<<" s: "<<retloc.loc.lat_s<<"**"<<std::endl;
            std::cout<<"full degree lat-> "<<retloc.loc.latDD<<"**"<<std::endl;
            std::cout<<"lon-> "<<"d: "<<retloc.loc.lon_d<<" m: "<<retloc.loc.lon_m<<" s: "<<retloc.loc.lon_s<<"**"<<std::endl;
            std::cout<<"full degree lon-> "<<retloc.loc.lonDD<<"**"<<std::endl;
            std::cout<<"NS-> "<<(retloc.ns==mainDirs::N?"N":"S")<<"**"<<std::endl;
            std::cout<<"EW-> "<<(retloc.ew==mainDirs::E?"E":"W")<<"**"<<std::endl;
            std::cout<<"speed (knots)-> "<<retloc.spd.knots<<"**"<<std::endl;
            std::cout<<"course over ground-> "<<retloc.spd.cog<<"**"<<std::endl;
            std::cout<<"stts-> "<<(retloc.stts==status::A?"A":"V")<<"**"<<std::endl;
            if(retloc.pm == posMode::A)
                std::cout<<"posMode-> "<<"A"<<"**"<<std::endl;
            if(retloc.pm == posMode::D)
                std::cout<<"posMode-> "<<"D"<<"**"<<std::endl;
            if(retloc.pm == posMode::E)
                std::cout<<"posMode-> "<<"E"<<"**"<<std::endl;
            if(retloc.pm == posMode::N)
                std::cout<<"posMode-> "<<"N"<<"**"<<std::endl;
            if(retloc.pm == posMode::none)
                std::cout<<"posMode-> "<<"none"<<"**"<<std::endl;

            std::cout<<"===============================================\n";
#endif // DEBGLL
            retvec.push_back(retloc);
        }
    }



    return retvec;
}

std::vector<spd> gpsi::getGPSSpd()
{
    std::vector<spd> spdvec{};

    for(auto o: NMEADataVec("VTG"))
    {
        if(!(o.second.strvec.empty()))
        {
            spd retspd;
            if(!(o.first.empty()))
                retspd.sat.assign(o.first);
            retspd.rcvtime = o.second.uptime;

            if(!(o.second.strvec[4].empty()))
            {
                retspd.spd.knots = stof(o.second.strvec[4]);
                retspd.spd.n = 'T';
            }
            if(!(o.second.strvec[6].empty()))
            {
                retspd.spd.kph = stof(o.second.strvec[6]);
                retspd.spd.k = 'K';
            }
            if(!(o.second.strvec[0].empty()))
            {
                retspd.spd.cog = stof(o.second.strvec[0]);
                retspd.spd.t = 'T';
            }

            if(!o.second.strvec[8].empty())
            {
                if(o.second.strvec[8].at(0)=='N')
                    retspd.pm = posMode::N;
                if(o.second.strvec[8].at(0)=='E')
                    retspd.pm = posMode::E;
                if(o.second.strvec[8].at(0)=='A')
                    retspd.pm = posMode::A;
                if(o.second.strvec[8].at(0)=='D')
                    retspd.pm = posMode::D;
            }

#ifdef DEBVTG
            std::cout<<"\n===============================================\n";
            std::cout<<"sat-> "<< retspd.sat <<"**"<<std::endl;
            std::cout<<"rcvtme-> " << std::chrono::system_clock::to_time_t(retspd.rcvtime)<<"**"<<std::endl;
            std::cout<<"speed knots-> "<<retspd.spd.knots<<"**"<<std::endl;
            std::cout<<"N-> "<<retspd.spd.n<<"**"<<std::endl;
            std::cout<<"speed kph-> "<<retspd.spd.kph<<"**"<<std::endl;
            std::cout<<"K-> "<<retspd.spd.k<<"**"<<std::endl;
            std::cout<<"course over ground-> "<<retspd.spd.cog<<"**"<<std::endl;
            std::cout<<"T-> "<<retspd.spd.t<<"**"<<std::endl;

            if(retspd.pm == posMode::A)
                std::cout<<"posMode-> "<<"A"<<"**"<<std::endl;
            if(retspd.pm == posMode::D)
                std::cout<<"posMode-> "<<"D"<<"**"<<std::endl;
            if(retspd.pm == posMode::E)
                std::cout<<"posMode-> "<<"E"<<"**"<<std::endl;
            if(retspd.pm == posMode::N)
                std::cout<<"posMode-> "<<"N"<<"**"<<std::endl;
            if(retspd.pm == posMode::none)
                std::cout<<"posMode-> "<<"none"<<"**"<<std::endl;

            std::cout<<"===============================================\n";
#endif // DEBGLL
            spdvec.push_back(retspd);
        }

    }
    return spdvec;
}

bool gpsi::GPSActive() const noexcept
{
    return gps.GPSActive();
}

void gpsi::turnOff()
{
    gps.turnOff();
}
