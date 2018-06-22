#include "OscMessageLogger.h"
#include <iostream>

using dap::CoutOscLogger;

std::ostream& osc::operator<<(std::ostream& out, const osc::ReceivedMessage::const_iterator& arg)
{
    switch (arg->TypeTag())
    {
        case osc::TRUE_TYPE_TAG:
            out << true;
            break;
        case osc::FALSE_TYPE_TAG:
            out << false;
            break;
        case osc::INT32_TYPE_TAG:
            out << arg->AsInt32Unchecked();
            break;
        case osc::FLOAT_TYPE_TAG:
            out << arg->AsFloatUnchecked();
            break;
        case osc::CHAR_TYPE_TAG:
            out << arg->AsCharUnchecked();
            break;
        case osc::INT64_TYPE_TAG:
            out << arg->AsInt64Unchecked();
            break;
        case osc::DOUBLE_TYPE_TAG:
            out << arg->AsDoubleUnchecked();
            break;
        case osc::STRING_TYPE_TAG:
            out << arg->AsStringUnchecked();
            break;
        case osc::BLOB_TYPE_TAG:
            out << "(blob)";
            break;
        // currently unsupported types
        case osc::NIL_TYPE_TAG:
            out << "(nil)";
            break;
        case osc::INFINITUM_TYPE_TAG:
            out << "(infinitum)";
            break;
        case osc::RGBA_COLOR_TYPE_TAG:
            out << "(rgba)";
            break;
        case osc::MIDI_MESSAGE_TYPE_TAG:
            out << "(midi msg)";
            break;
        case osc::TIME_TAG_TYPE_TAG:
            out << "(time)";
            break;
        case osc::SYMBOL_TYPE_TAG:
            out << "(symbol)";
            break;
        case osc::ARRAY_BEGIN_TYPE_TAG:
            out << "[";
            break;
        case osc::ARRAY_END_TYPE_TAG:
            out << "]";
            break;
        default:
            out << "(unknown arg type)";
            break;
    }
    return out;
}

CoutOscLogger::CoutOscLogger()
: elemCount(0)
{
}
void CoutOscLogger::flush()
{
    std::cout << ss.str() << " : " << elemCount << " values" << std::endl;
    elemCount = 0;
};
void CoutOscLogger::start(const std::string& msg)
{
    ss.str("");
    ss << msg;
}
void CoutOscLogger::append(osc::ReceivedMessage::const_iterator arg)
{
    ss << " " << arg;
    ++elemCount;
}
