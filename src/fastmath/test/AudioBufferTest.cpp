#include <gtest/gtest.h>
#include "fastmath/AudioBuffer.h"
#include <cmath>
#include <limits>
#include <vector>

using namespace testing;
using namespace dap;
using dap::fastmath::Array;
using dap::fastmath::AudioBuffer;

template <typename T>
using aligned_vector = std::vector<T, fastmath::AlignedAllocator<T>>;

template <typename T, typename Alloc>
void print(const Array<T, Alloc>& v)
{
    for (const auto& x : v)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T, typename Alloc>
void assert_near(const Array<T, Alloc>& a, const Array<T, Alloc>& b, float eps)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ASSERT_NEAR(a[i], b[i], eps);
    }
}

template <typename T, typename Alloc>
void assert_eq(const Array<T, Alloc>& a, const Array<T, Alloc>& b)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ASSERT_EQ(a[i], b[i]);
    }
}
template <typename T, typename Alloc>
void assert_near(const std::vector<std::complex<T>, Alloc>& a,
                 const std::vector<std::complex<T>, Alloc>& b,
                 float eps)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ASSERT_NEAR(a[i].real(), b[i].real(), eps);
        ASSERT_NEAR(a[i].imag(), b[i].imag(), eps);
    }
}
TEST(AudioBufferTest, ctor)
{
    size_t channels   = 2;
    size_t bufferSize = 10;
    AudioBuffer<float> buf(channels, bufferSize, 1);
    ASSERT_EQ(2u, buf.channelCount());
    ASSERT_EQ(10u, buf.channelSize());
    ASSERT_EQ(10u, buf.channel(0).size());
    ASSERT_EQ(10u, buf.channel(1).size());

    buf.channel(1) = buf.channel(0).map() * 5;

    AudioBuffer<float>::Channel expected_0(bufferSize, 1);
    AudioBuffer<float>::Channel expected_1(bufferSize, 5);

    assert_eq(expected_0, buf.channel(0));
    assert_eq(expected_1, buf.channel(1));
}
TEST(AudioBufferTest, float_ptr)
{
    size_t channels   = 2;
    size_t bufferSize = 5;
    AudioBuffer<float> buf(channels, bufferSize, 1);

    buf.channel(0) = {1, 2, 3, 4, 5};
    buf.channel(1) = {6, 7, 8, 9, 10};

    float** data = buf.data();

    for (size_t i = 0; i < bufferSize; ++i)
    {
        data[1][i] *= 5; // NOLINT
    }

    Array<float> expected_0({1, 2, 3, 4, 5});
    Array<float> expected_1({30, 35, 40, 45, 50});

    assert_eq(expected_0, buf.channel(0));
    assert_eq(expected_1, buf.channel(1));
}
TEST(AudioBufferTest, copy_ctr)
{
    size_t channels   = 2;
    size_t bufferSize = 5;
    AudioBuffer<float> buf(channels, bufferSize, 1);

    buf.channel(0) = {1, 2, 3, 4, 5};
    buf.channel(1) = {6, 7, 8, 9, 10};

    Array<float> expected_0({1, 2, 3, 4, 5});
    Array<float> expected_1({6, 7, 8, 9, 10});

    AudioBuffer<float> buf2(buf);

    assert_eq(expected_0, buf2.channel(0));
    assert_eq(expected_1, buf2.channel(1));
}
TEST(AudioBufferTest, operator_equal)
{
    size_t channels   = 2;
    size_t bufferSize = 5;
    AudioBuffer<float> buf(channels, bufferSize, 1);

    buf.channel(0) = {1, 2, 3, 4, 5};
    buf.channel(1) = {6, 7, 8, 9, 10};

    Array<float> expected_0({1, 2, 3, 4, 5});
    Array<float> expected_1({6, 7, 8, 9, 10});

    AudioBuffer<float> buf2;
    buf2 = buf;

    assert_eq(expected_0, buf2.channel(0));
    assert_eq(expected_1, buf2.channel(1));
}
TEST(AudioBufferTest, initializer_list)
{
    using Channel = AudioBuffer<int>::Channel;
    AudioBuffer<int> buf({Channel({1, 2, 3, 4, 5}), Channel({6, 7, 8, 9, 10})});

    Array<int> expected_0({1, 2, 3, 4, 5});
    Array<int> expected_1({6, 7, 8, 9, 10});

    assert_eq(expected_0, buf.channel(0));
    assert_eq(expected_1, buf.channel(1));
}
TEST(AudioBufferTest, resize)
{
    AudioBuffer<int> buf;
    size_t channelCount = 15;
    size_t channelSize  = 512;
    int value = 42;
    buf.resize(channelCount, channelSize, value);
    ASSERT_EQ(channelSize, buf.channelSize());
    ASSERT_EQ(channelCount, buf.channelCount());
    std::vector<int> expected(channelSize, value);
    for (size_t ch = 0; ch < channelCount; ++ch)
    {
        const AudioBuffer<int>::Channel& channel = buf.channel(ch);
        for (size_t samp = 0; samp < channelSize; ++samp)
        {
            ASSERT_EQ(value, channel[samp]);
        }
    }
}
TEST(AudioBufferTest, move)
{
    AudioBuffer<int> buf;
    size_t channelCount = 15;
    size_t channelSize  = 512;
    int value = 42;
    buf.resize(channelCount, channelSize, value);
    ASSERT_EQ(channelSize, buf.channelSize());
    ASSERT_EQ(channelCount, buf.channelCount());
    std::vector<int> expected(channelSize, value);
    for (size_t ch = 0; ch < channelCount; ++ch)
    {
        const AudioBuffer<int>::Channel& channel = buf.channel(ch);
        for (size_t samp = 0; samp < channelSize; ++samp)
        {
            ASSERT_EQ(value, channel[samp]);
        }
    }
    AudioBuffer<int> buf2 = std::move(buf);
    for (size_t ch = 0; ch < channelCount; ++ch)
    {
        const AudioBuffer<int>::Channel& channel = buf2.channel(ch);
        for (size_t samp = 0; samp < channelSize; ++samp)
        {
            ASSERT_EQ(value, channel[samp]);
        }
    }
}
