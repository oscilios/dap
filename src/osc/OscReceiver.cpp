#include "OscReceiver.h"
#include <iostream>
#include <oscpack/ip/UdpSocket.h>
#include <oscpack/osc/OscPacketListener.h>
#include <queue>

using dap::OscReceiver;

namespace
{
    struct Unpack
    {
        static std::string address(const osc::ReceivedMessage& msg)
        {
            return msg.AddressPattern();
        }
        static std::vector<dap::Any> values(const osc::ReceivedMessage& msg)
        {
            auto arg = msg.ArgumentsBegin();
            std::vector<dap::Any> result;
            while ((arg != msg.ArgumentsEnd()))
            {
                result.emplace_back(Unpack::asAny(arg++));
            }
            return result;
        }
        static dap::Any asAny(const osc::ReceivedMessage::const_iterator& arg)
        {
            using dap::Any;
            switch (arg->TypeTag())
            {
                case osc::TRUE_TYPE_TAG:
                    return Any{true};
                case osc::FALSE_TYPE_TAG:
                    return Any{false};
                case osc::INT32_TYPE_TAG:
                    return Any{arg->AsInt32Unchecked()};
                case osc::FLOAT_TYPE_TAG:
                    return Any{arg->AsFloatUnchecked()};
                case osc::CHAR_TYPE_TAG:
                    return Any{arg->AsCharUnchecked()};
                case osc::INT64_TYPE_TAG:
                    return Any{arg->AsInt64Unchecked()};
                case osc::DOUBLE_TYPE_TAG:
                    return Any{arg->AsDoubleUnchecked()};
                case osc::STRING_TYPE_TAG:
                    return Any{arg->AsStringUnchecked()};
                case osc::BLOB_TYPE_TAG:
                case osc::NIL_TYPE_TAG:
                case osc::INFINITUM_TYPE_TAG:
                case osc::RGBA_COLOR_TYPE_TAG:
                case osc::MIDI_MESSAGE_TYPE_TAG:
                case osc::TIME_TAG_TYPE_TAG:
                case osc::SYMBOL_TYPE_TAG:
                case osc::ARRAY_BEGIN_TYPE_TAG:
                case osc::ARRAY_END_TYPE_TAG:
                default:
                    break;
            }
            return Any{};
        }
    };

} // namespace

class dap::OscListenerQueue : public osc::OscPacketListener
{
    std::queue<osc::ReceivedMessage> m_queue;
    std::mutex m_mutex;
    OscReceiver* const m_oscReceiver;

    void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName&) override
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_queue.push(m);
        m_oscReceiver->signal();
    }

public:
    OscListenerQueue(OscReceiver* parent)
    : m_oscReceiver(parent)
    {
    }
    std::unique_ptr<osc::ReceivedMessage> pop()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (m_queue.empty())
            return nullptr;
        auto result = std::make_unique<osc::ReceivedMessage>(m_queue.front());
        m_queue.pop();
        return result;
    }
};

OscReceiver::OscReceiver()
: m_listener(std::make_shared<OscListenerQueue>(this))
, m_socket(nullptr)
, m_port(defaultPort)
{
    openPort(m_port);
}
OscReceiver::~OscReceiver()
{
    stop();
}
bool OscReceiver::run()
{
    if (m_running.load(std::memory_order_acquire))
        return true;

    if (!m_socket)
    {
        std::cerr << "You need to open a port before start running" << std::endl;
        return false;
    }
    m_socketThread = std::async(std::launch::async, &UdpListeningReceiveSocket::Run, m_socket);
    m_running.store(true, std::memory_order_release);
    m_dequeueMessageThread = std::async(std::launch::async, &OscReceiver::dequeueMessages, this);
    return true;
}
void OscReceiver::signal()
{
    m_sem.signal();
}
void OscReceiver::stop()
{
    if (!m_running || !m_socket)
        return;
    auto future =
        std::async(std::launch::async, &UdpListeningReceiveSocket::AsynchronousBreak, m_socket);
    future.get();
    m_running.store(false, std::memory_order_release);
    signal();
    m_dequeueMessageThread.get();
}

bool OscReceiver::openPort(uint32_t port)
{
    try
    {
        stop();
        m_socket = std::make_shared<UdpListeningReceiveSocket>(
            IpEndpointName(IpEndpointName::ANY_ADDRESS, port), m_listener.get());
        m_port = port;
    }
    catch (const osc::Exception& e)
    {
        std::cerr << "Exception thrown while opening port " << port << ":" << e.what() << std::endl;
        m_socket.reset();
        return false;
    }
    catch (...)
    {
        std::cerr << "Unknown exception thrown while opening port " << port << std::endl;
        m_socket.reset();
        return false;
    }
    return true;
}
uint32_t OscReceiver::port() const
{
    return m_port;
}

void OscReceiver::dequeueMessages()
{
    while (m_running.load(std::memory_order_acquire))
    {
        m_sem.wait();
        while (auto msg = m_listener->pop())
        {
            process(*msg);
        }
    }
}
void OscReceiver::process(const osc::ReceivedMessage& msg)
{
    try
    {
        auto address = Unpack::address(msg);
        auto values  = Unpack::values(msg);
        auto iter    = eventCallbacks.find(address);
        if (iter != eventCallbacks.end())
        {
            iter->second(std::move(values));
        }
        else
        {
            std::cerr << "no callback registered for address: " << address << "." << std::endl;
        }
    }
    catch (const osc::Exception& e)
    {
        std::cerr << "Error processing osc message, " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing osc message, " << e.what() << std::endl;
    }
}

bool OscReceiver::addCallback(const std::string& event, Callback&& callback)
{
    return eventCallbacks.insert({event, callback}).second;
}
