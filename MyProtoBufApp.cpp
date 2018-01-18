#include <iostream>
#include "servant/Application.h"
#include "flight.pb.h"

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
    virtual int onDispatch(TarsCurrentPtr current, vector<char> &buffer)
    {
        const vector<char> & req = current->getRequestBuffer();
        TLOGDEBUG(__PRETTY_FUNCTION__ << " request.FuncName=" << current->getFuncName()
                                      << " request.DataSize=" << req.size()
                                      << endl);
        bool ok;
        string sBuffer(req.begin(), req.end());
        com::yy::tars::demo::flight::QueryFlightRequest request;
        ok = request.ParseFromString(sBuffer);
        assert(ok);
        TLOGDEBUG(__PRETTY_FUNCTION__ << " request.Data.name=" << request.name()
                                      << " request.Data.password=" << request.password()
                                      << " request.Data.departureCity=" << request.departurecity()
                                      << " request.Data.destinationCity=" << request.destinationcity()
                                      << endl);
        com::yy::tars::demo::flight::QueryFlightResponse response;
        response.set_errcode(0);
        response.set_errmsg("Success.");
        response.set_flightsinfo("XT3697");
        response.SerializeToString(&sBuffer);
        buffer.assign(sBuffer.begin(), sBuffer.end());
        return TARSSERVERSUCCESS;
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

