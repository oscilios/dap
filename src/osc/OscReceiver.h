#ifndef DAP_OSC_OSC_RECEIVER_H
#define DAP_OSC_OSC_RECEIVER_H

#include "base/Any.h"
#include "base/Semaphore.h"
#include <atomic>
#include <future>
#include <memory>
#include <oscpack/osc/OscReceivedElements.h>
#include <unordered_map>
#include <vector>

class UdpListeningReceiveSocket;
namespace osc
{
    class ReceivedMessage;
}
namespace dap
{
    class OscListenerQueue;
    class OscReceiver;
}

class dap::OscReceiver final
{
    friend OscListenerQueue;

    using Callback = std::function<void(std::vector<Any>&&)>;
    std::unordered_map<std::string, Callback> eventCallbacks;
    std::shared_ptr<OscListenerQueue> m_listener;
    std::shared_ptr<UdpListeningReceiveSocket> m_socket;
    uint32_t m_port;
    std::future<void> m_socketThread;
    std::future<void> m_dequeueMessageThread;
    std::atomic<bool> m_running{false};
    Semaphore m_sem;

    void signal();
    void dequeueMessages();
    void process(const osc::ReceivedMessage& msg);

public:
    OscReceiver();
    ~OscReceiver();
    bool openPort(uint32_t);
    uint32_t port() const;
    void stop();
    bool run();
    bool addCallback(const std::string& event, Callback&& callback);
    static constexpr unsigned int defaultPort = 7000u;
};
#endif // DAP_OSC_OSC_RECEIVER_H
