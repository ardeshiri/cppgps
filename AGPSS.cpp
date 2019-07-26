#include "AGPSS.h"


AGPSS::AGPSS(const std::string& prt):fd(-1),newline(false),valid(false),strport(prt),GPSActiveF(true),exit(false)
{
    memset(line,'\0',sizeof(line));
}

AGPSS::~AGPSS()
{
    close(fd);
}

int AGPSS::serialConnect(const string prt)
{
    if(!prt.empty())
        strport.assign(prt);
    if(strport.empty())
        throw Error{"1"};

    fd = open(strport.c_str() ,O_RDONLY | O_NOCTTY);  ///O_RDRW
    if(fd == -1)
        throw Error{"2"};
    //tcgetattr(fd, &SerialPortSettings);
    memset(&SerialPortSettings,0,sizeof(struct termios));
    cfsetispeed(&SerialPortSettings,B9600);
    cfsetospeed(&SerialPortSettings,B9600);
    SerialPortSettings.c_cflag &= ~PARENB;
    SerialPortSettings.c_cflag &= ~CSTOPB;
    SerialPortSettings.c_cflag &= ~CSIZE;
    SerialPortSettings.c_cflag |=  CS8;
    SerialPortSettings.c_cflag &= ~CRTSCTS;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    SerialPortSettings.c_oflag &= ~OPOST;
    SerialPortSettings.c_cc[VMIN] = 10;
    SerialPortSettings.c_cc[VTIME] = 0;
    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)
        throw Error{"3"};
    tcflush(fd, TCIFLUSH);
    return fd;
}


int AGPSS::readLine()
{
    char tmpstr[NMEASTRMAXLEN];
    memset(tmpstr,'\0',sizeof(tmpstr));

    char tmpchar;
    unsigned int counter = 0;

    while(read(fd,&tmpchar,1) && counter<(sizeof(tmpstr)-1))
    {
        if(tmpchar == '\n')
        {
            tmpstr[counter] = '\0';
            break;
        }
        tmpstr[counter++] = tmpchar;
    }
    std::unique_lock<std::mutex> ul(mtx);
    memcpy(line,tmpstr,sizeof(line));
    newline = true;  // or newline.store(true,memory_order_seq_cst)
    ul.unlock();
    cv.notify_one();
    return strlen(line);
}

int AGPSS::getRowLine(char* outline,size_t ls)
{
    std::unique_lock<std::mutex> ul(mtx);
    cv.wait(ul,[this]{return lineIsNew();});
    strncpy(outline,line,ls);
    newline = false;
    ul.unlock();
    if(ls >= strlen(line))
        return 0;
    else
        {
            outline[ls-1] = '\0';
            return -1;
        }
}

void AGPSS::GPSSStart()
{
    try
    {
        valid = true;
        while(true)
        {
            if(exit == true)
            {
                GPSActiveF = false;
                return;
            }
            if( readLine() == 0)
                break;
        }
        valid = false;
        throw Error{"4"};
    }
    catch(const Error& er)
    {
        GPSActiveF = false;
        er.what();
        return;
    }
}


bool AGPSS::lineIsNew() const noexcept
{
    return newline;
}

bool AGPSS::lineIsValid() const noexcept
{
    return valid;
}

bool AGPSS::lineIsCorrect() const
{
    if(valid && line[0]=='$' && line[strlen(line)]=='\0' && line[strlen(line)-1]=='\r')
        return true;
    else
        return false;
}

bool AGPSS::GPSActive() const noexcept
{
    return GPSActiveF;
}

void AGPSS::DeactivGPS()
{
    GPSActiveF = false;
}

void AGPSS::turnOff()
{
    exit = true;
}
