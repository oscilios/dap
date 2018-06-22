#include "osc/OscSender.h"
#include <iostream>
#include <thread>


int usage(const char* programName)
{
    std::cout << programName << " <address> <min_value> [<max_value>]" << std::endl;
    return 0;
}

int main(int argc, const char** argv)
{
    unsigned int port = 7000;
    const char* hostName = "localhost";
    float fromValue = 0.0f;
    float toValue = 0.0f;
    if (argc < 3)
        return usage(argv[0]);

    dap::OscSender sender(hostName, port);

    std::string msg(argv[1]);
    fromValue = std::atof(argv[2]);
    float step = 0;
    if (argc > 3)
    {
        toValue   = std::atof(argv[3]);
        step = (toValue-fromValue)/500.0f;
    }
    while (true)
    {
        sender.send(msg.c_str(), fromValue);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        fromValue += step;
        if ((step > 0) && (fromValue >= toValue))
            break;
        else if ((step < 0) && (fromValue <= toValue))
            break;
        else if (step == 0)
            break;
    }
    return 0;
}
