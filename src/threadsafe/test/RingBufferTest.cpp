#include "dap_gtest.h"
#include <vector>
#include <iostream>
#include <future>

#include "threadsafe/RingBuffer.h"

using namespace testing;
using namespace dap;
using dap::threadsafe::RingBuffer;

class RingBufferTest : public Test
{
};

template <typename T, size_t Size>
size_t getMaxReadSize(const RingBuffer<T, Size>& buffer)
{
    return buffer.getTotalWrittenSize() - buffer.getTotalReadSize();
};
template <typename T, size_t Size>
size_t getMaxWriteSize(const RingBuffer<T, Size>& buffer)
{
    return buffer.getBufferSize() - getMaxReadSize(buffer);
};

DAP_TEST_F(RingBufferTest, single_thread)
{
    constexpr int kMaxSize = 64;
    RingBuffer<int, kMaxSize> buffer;
    constexpr int inputSize  = kMaxSize;
    constexpr int outputSize = 8;


    std::vector<int> output(outputSize, 0);

    constexpr auto range = dap::irange<int, 0, inputSize>();

    DAP_ASSERT_EQ(0u, buffer.read(output.data(), outputSize));

    DAP_ASSERT_EQ(buffer.getBufferSize(), getMaxWriteSize(buffer));


    int n               = 0;
    size_t totalRead    = 0;
    size_t totalWritten = 0;

    while (n++ < 1000)
    {
        DAP_ASSERT_EQ(size_t(inputSize), buffer.write(range.data(), range.size()));
        totalWritten += inputSize;
        DAP_ASSERT_EQ(totalWritten, buffer.getTotalWrittenSize());
        DAP_ASSERT_EQ(0u, getMaxWriteSize(buffer));

        DAP_ASSERT_EQ(totalRead, buffer.getTotalReadSize());
        DAP_ASSERT_EQ(size_t(inputSize), getMaxReadSize(buffer));

        int count = 0;
        while (count < inputSize)
        {
            DAP_ASSERT_EQ(size_t(outputSize), buffer.read(output.data(), outputSize));

            for (int i = 0; i < outputSize; i++)
            {
                DAP_ASSERT_EQ(i + count, output.at(i));
            }
            count += outputSize;
            totalRead += outputSize;

            DAP_ASSERT_EQ(totalRead, buffer.getTotalReadSize());
            DAP_ASSERT_EQ(buffer.getBufferSize() - count, getMaxReadSize(buffer));
        }
        DAP_ASSERT_EQ(0u, getMaxReadSize(buffer));
    }
}

DAP_TEST_F(RingBufferTest, multi_consumer_multi_producer)
{
    constexpr int kMaxSize = 512;
    RingBuffer<int, kMaxSize> buffer;
    constexpr int size = 16;

    constexpr auto range = dap::irange<int, 0, size>();

    auto produce = [&buffer, &range]()
    {
        const auto wsize = buffer.exclusiveWrite(range.data(), range.size());
        if (wsize != 0u)
        {
            // if we were able to write, it should have written the hold block
            DAP_ASSERT_EQ(range.size(), wsize);
        }
    };
    auto consume = [&buffer, &range]()
    {
        std::vector<int> output(range.size(), 0);
        const auto rsize = buffer.exclusiveRead(output.data(), range.size());
        if (rsize != 0u)
        {
            DAP_ASSERT_EQ(range.size(), rsize);
            for (size_t i = 0; i < rsize; i++)
            {
                DAP_ASSERT_EQ(range.at(i), output.at(i));
            }
        }
    };

    size_t n = 0;
    while (n++ < 1000)
    {
        size_t i = 0;
        std::vector<std::future<void>> consumers;
        std::vector<std::future<void>> producers;
        while (i++ < size)
        {
            consumers.emplace_back(std::async(std::launch::async, consume));
            producers.emplace_back(std::async(std::launch::async, produce));
            producers.emplace_back(std::async(std::launch::async, produce));
            producers.emplace_back(std::async(std::launch::async, produce));
            consumers.emplace_back(std::async(std::launch::async, consume));
            consumers.emplace_back(std::async(std::launch::async, consume));
        }
        for (auto& f : consumers)
        {
            f.get();
        }
        for (auto& f : producers)
        {
            f.get();
        }
        buffer.exclusiveClear();
    }
}
