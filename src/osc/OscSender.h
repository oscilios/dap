#ifndef DAP_OSC_OSC_SENDER_H
#define DAP_OSC_OSC_SENDER_H

#include "oscpack/ip/IpEndpointName.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/osc/OscOutboundPacketStream.h"
#include <string>

namespace dap
{
    class OscSender;
}

class dap::OscSender
{
    static constexpr unsigned int IP_MTU_SIZE = 1536;
    char buffer[IP_MTU_SIZE];
    std::string m_host;
    unsigned int m_port;
    UdpTransmitSocket m_socket;
    osc::OutboundPacketStream m_packet;
    bool m_isBundled{false};

public:
    struct UnbalancedEndBundle : std::exception
    {
        virtual const char* what() const noexcept
        {
            return "Unbalanced call to dap::OscSender::endBundle";
        }
    };
    struct UnbalancedBeginBundle : std::exception
    {
        virtual const char* what() const noexcept
        {
            return "Unbalanced call to dap::OscSender::beginBundle";
        }
    };

    OscSender(const char* host, unsigned int port);
    template <typename... Values>
    void send(const std::string& msg, const Values&... values)
    {
        if (!m_isBundled)
            m_packet.Clear();
        m_packet << osc::BeginMessage(msg.c_str());
        (m_packet << ... << values);
        m_packet << osc::EndMessage;
        if (!m_isBundled)
            m_socket.Send(m_packet.Data(), m_packet.Size());
    }
    const std::string& host() const;
    const unsigned int& port() const;
    void beginBundle();
    void endBundle();

    static constexpr const char* defaultHostName = "localhost";
    static constexpr unsigned int defaultPort = 7000;
};

#endif // DAP_OSC_OSC_SENDER_H
