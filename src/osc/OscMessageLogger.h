#ifndef DAP_OSC_OSC_MESSAGE_LOGGER_H
#define DAP_OSC_OSC_MESSAGE_LOGGER_H

#include <oscpack/osc/OscReceivedElements.h>
#include <ostream>
#include <sstream>

namespace osc
{
    std::ostream& operator<<(std::ostream& out, const osc::ReceivedMessage::const_iterator& arg);
}

namespace dap
{
    class OscMessageLogger;
    class DummyOscLogger;
    class CoutOscLogger;
} // namespace dap

class dap::OscMessageLogger
{
public:
    OscMessageLogger()                                            = default;
    virtual ~OscMessageLogger()                                   = default;
    virtual void append(osc::ReceivedMessage::const_iterator arg) = 0;
    virtual void start(const std::string& msg)                    = 0;
    virtual void flush()                                          = 0;
};

class dap::DummyOscLogger : public dap::OscMessageLogger
{
public:
    DummyOscLogger()          = default;
    virtual ~DummyOscLogger() = default;
    void append(osc::ReceivedMessage::const_iterator) override
    {
    }
    void start(const std::string&) override{};
    void flush() override{};
};
class dap::CoutOscLogger : public OscMessageLogger
{
    std::stringstream ss;
    int elemCount;

public:
    CoutOscLogger();
    ~CoutOscLogger() = default;
    void flush() override;
    void start(const std::string& msg) override;
    void append(osc::ReceivedMessage::const_iterator arg) override;
};

#endif // DAP_OSC_OSC_MESSAGE_LOGGER_H
