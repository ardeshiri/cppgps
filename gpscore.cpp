#include "gpscore.h"

gpscore::gpscore(const std::string port):gpss(port),GPSTable{},exit(false)
{
}

gpscore::~gpscore()
{
}

void gpscore::setPort(std::string port)
{
    gpss.serialConnect(port);
}

void gpscore::startGPS()
{
    std::thread(&AGPSS::GPSSStart, &gpss).detach();
}

string gpscore::getGPSNMEAData()
{
    char tmpstr[NMEASTRMAXLEN];
    memset(tmpstr,0,sizeof(tmpstr));
    gpss.getRowLine(tmpstr,sizeof(tmpstr));
    return std::string{tmpstr};///
}

bool gpscore::GPSNewData()
{
    return gpss.lineIsNew();
}

bool gpscore::GPSValidData()
{
    return (gpss.lineIsValid()&&gpss.lineIsCorrect()) ;
}

int gpscore::updateTable()
{
    if(GPSValidData())
    {
        char tmpstr[NMEASTRMAXLEN];
        memset(tmpstr,'\0',sizeof(tmpstr));
        gpss.getRowLine(tmpstr,sizeof(tmpstr));
        char* ch = NULL;
        ch = tmpstr;
        char tmp[NMEA_SUBSEC_SIZE];
        memset(tmp,'\0',sizeof(tmp));
        int i = 0;
        std::pair<std::string, data> tmpair{};
        while(1)
        {
            if(*ch == ','  )
            {
                if(tmp[0] == '$')
                    tmpair.first = std::string{tmp+1};
                else
                    tmpair.second.strvec.push_back(std::string{tmp});

                memset(tmp,'\0',NMEA_SUBSEC_SIZE);
                i = 0;
                ch++;
                continue;
            }
            tmp[i++] = *ch;
            ch++;
            if(*ch == '\0')
            {
                tmpair.second.strvec.push_back(std::string{tmp});
                break;
            }
        }
        GPSTable[tmpair.first] = tmpair.second;
        return 0;  /// mgk number
    }
    return -1;  /// mgk number
}


std::map<std::string,data> gpscore::returnTable() const
{
    return GPSTable;
}


int gpscore::updateTableLoop()
{
    try
    {
        while(updateTable() == -1)
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_BEFORE_CONNECT));
        while(1)
        {
            if(exit == true)
            {
                gpss.DeactivGPS();
                return 0;
            }
            if(updateTable() == -1)  /// mgk number
                {
                    break;
                }
        }
        throw Error{"5"}; /// to do: should be organize
    }
    catch(const Error& er)
    {
        gpss.DeactivGPS();
        er.what();
        return -1; /// to do: magic numbers should be defined
    }
}

bool gpscore::GPSActive() const noexcept
{
    return gpss.GPSActive();
}

void gpscore::turnOff()
{
    exit = true;
    gpss.turnOff();
}
