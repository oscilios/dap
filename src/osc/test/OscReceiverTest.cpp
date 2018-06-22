#include "osc/OscReceiver.h"
#include <oscpack/ip/IpEndpointName.h>
#include <oscpack/ip/UdpSocket.h>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include "dap_gtest.h"

using namespace testing;
using namespace dap;

#define IP_MTU_SIZE 1536
class DummyOscSender
{
    char buffer[IP_MTU_SIZE];
    UdpTransmitSocket socket;
    osc::OutboundPacketStream p;

    template <typename T>
    void append(const T& x)
    {
        p << x;
    }

    template <typename T, typename... Values>
    void append(const T& x, Values... values)
    {
        append(x);
        const size_t size = sizeof...(values);
        if (size > 0)
            return append(values...);
    }

public:
    DummyOscSender(const IpEndpointName& host)
    : socket(host)
    , p(buffer, IP_MTU_SIZE)
    {
    }
    template <typename... Values>
    void send(const std::string& msg, Values... values)
    {
        p.Clear();
        p << osc::BeginMessage(msg.c_str());
        append(values...);
        p << osc::EndMessage;
        socket.Send(p.Data(), p.Size());
    }
};

TEST(OscReceiverTest, simple)
{
    const char* hostName = "localhost";
    int port             = 7000;
    IpEndpointName host(hostName, port);
    DummyOscSender sender(host);

    OscReceiver oscReceiver;
    int n = 0;
    std::cout << "waiting for 10s ..." << std::endl;
    int step = 63;
    const bool registered = oscReceiver.addCallback("/test1", [](std::vector<dap::Any>&& v) {
        std::cout << "callback for test1 called with values: " << v[0].to<bool>() << " "
                  << v[1].to<int>() << " " << v[2].to<float>() << " " << v[3].to<const char*>()
                  << std::endl;
    });
    DAP_ASSERT_TRUE(registered);
    oscReceiver.run();
    while (n++ < 1000)
    {
        if (!(n & step))
            sender.send("/test1", true, n, 3.1415f*n, "hello");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    oscReceiver.stop();
}
