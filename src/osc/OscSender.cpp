#include "OscSender.h"

using dap::OscSender;

OscSender::OscSender(const char* host, unsigned int port)
: m_host(host)
, m_port(port)
, m_socket(IpEndpointName(m_host.c_str(), port))
, m_packet(buffer, IP_MTU_SIZE)
{
}
const std::string& OscSender::host() const
{
    return m_host;
}
const unsigned int& OscSender::port() const
{
    return m_port;
}
void OscSender::beginBundle()
{
    if (m_isBundled)
        throw UnbalancedBeginBundle();
    m_isBundled = true;
    m_packet.Clear();
    m_packet << osc::BeginBundleImmediate;
}
void OscSender::endBundle()
{
    if (!m_isBundled)
        throw UnbalancedEndBundle();
    m_packet << osc::EndBundle;
    m_socket.Send(m_packet.Data(), m_packet.Size());
    m_isBundled = false;
}
