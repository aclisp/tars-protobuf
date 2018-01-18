#include <iostream>
#include "servant/Application.h"

using namespace std;
using namespace tars;

// Tars服务实现类
class FlightService : public Servant
{
    virtual void initialize()
    {
        TLOGDEBUG("Enter " << __PRETTY_FUNCTION__ << endl);
    }
    virtual void destroy()
    {
        TLOGDEBUG("Enter " << __PRETTY_FUNCTION__ << endl);
    }
};

// Tars框架实现类
class MyProtoBufApp : public Application
{
    virtual void initialize()
    {
        TLOGDEBUG("Enter " << __PRETTY_FUNCTION__ << endl);
        // 添加Tars服务实现类到框架
        addServant<FlightService>(ServerConfig::Application + "." + ServerConfig::ServerName + ".FlightService");
    }
    virtual void destroyApp()
    {
        TLOGDEBUG("Enter " << __PRETTY_FUNCTION__ << endl);
    }
};

// Tars框架实例
MyProtoBufApp g_app;

int main(int argc, char* argv[])
{
    try
    {
        TLOGDEBUG("Enter " << __PRETTY_FUNCTION__ << endl);
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}

