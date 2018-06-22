#include "dap_gtest.h"
#include <vector>
#include <iostream>
#include <future>

#include "threadsafe/Stack.h"

using namespace testing;
using namespace dap;
using dap::threadsafe::Stack;

class StackTest : public Test
{
};

DAP_TEST_F(StackTest, single_thread)
{
    Stack<int> s;

    auto x = s.pop_front();
    s.push_front(44);
    s.push_front(43);
    s.push_front(42);
    x = s.pop_front();
    DAP_ASSERT_EQ(42, *x);
    x = s.pop_front();
    DAP_ASSERT_EQ(43, *x);
    x = s.pop_front();
    DAP_ASSERT_EQ(44, *x);
    DAP_ASSERT_FALSE(s.pop_front());

    for (size_t i = 0; i < 10; i++)
    {
        s.push_front(i);
    }

    for (size_t i = 0; i < 10; i++)
    {
        x = s.find(i);
        DAP_ASSERT_EQ(int(i), *x);
    }

    DAP_ASSERT_FALSE(s.find(10));

    int count = 10;
    while (auto i = s.pop_front())
    {
        DAP_ASSERT_EQ(--count, *i);
    }
}

DAP_TEST_F(StackTest, multi_consumer_multi_producer)
{
    Stack<int> s;

    const int size = 128;
    std::atomic<int> current{0};
    std::set<int> resultSet;

    std::mutex mtx;

    auto produce = [&s, &current, size]()
    {
        if (current < size - 1)
        {
            s.push_front(current++);
        }
    };
    auto consume = [&s, size, &resultSet, &mtx]()
    {
        while (auto x = s.pop_front())
        {
            auto val = *x;
            DAP_ASSERT_TRUE(val < size);
            std::lock_guard<std::mutex> lk(mtx);
            DAP_ASSERT_FALSE(resultSet.count(val));
            resultSet.emplace(val);
        }
    };

    size_t n = 0;
    while (n++ < 100)
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

        resultSet.clear();
    }
}
