#include <iostream>
#include "servant/Communicator.h"
#include "flight.pb.h"

using namespace std;
using namespace tars;
using namespace google;

// Tars代理实现类
class ProtoBufProxy : public ServantProxy
{
public:
    void proto_invoke(const string& sFuncName, const protobuf::Message& request, protobuf::Message& response)
    {
        bool ok;
        string sBuffer;
        ok = request.SerializeToString(&sBuffer);
        assert(ok);
        vector<char> cBuffer(sBuffer.begin(), sBuffer.end());
        map<string, string> context;
        map<string, string> status;
        ResponsePacket rep;

        tars_invoke(tars::TARSNORMAL, sFuncName, cBuffer, context, status, rep);

        sBuffer.assign(rep.sBuffer.begin(), rep.sBuffer.end());
        ok = response.ParseFromString(sBuffer);
        assert(ok);
    }
};

typedef TC_AutoPtr<ProtoBufProxy> ProtoBufProxyPtr;

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    Communicator comm;
    try
    {
        ProtoBufProxyPtr proxy = comm.stringToProxy<ProtoBufProxyPtr>(
            "ProtoBufDemo.MyProtoBufApp.FlightService@tcp -h 172.26.66.43 -p 20001");
        com::yy::tars::demo::flight::QueryFlightRequest request;
        request.set_name("Leo");
        request.set_password("123");
        request.set_departurecity("Guangzhou");
        request.set_destinationcity("Beijing");
        com::yy::tars::demo::flight::QueryFlightResponse response;
        proxy->proto_invoke("queryFlight", request, response);
        cout << "queryFlight:" << " errcode=" << response.errcode()
                               << " errmsg=" << response.errmsg()
                               << " flightsInfo=" << response.flightsinfo()
                               << endl;
    }
    catch(exception& e)
    {
        cerr << "exception:" << e.what() << endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << endl;
    }
    return 0;
}
