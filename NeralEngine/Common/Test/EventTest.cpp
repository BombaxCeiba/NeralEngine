/**
 * @author: Dusk
 * @date: 2022-02-02 14:31:57
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:10:01
 * @copyright Copyright (c) 2022 Dusk.
 */
#include <functional>
#include <array>
#include <list>
#include <memory_resource>
#include "TestMain.h"
#include "../include/Event.hpp"

constexpr static std::size_t TEST_TIMES = 2;

struct TestEventArgs
{
    bool isReversed;
    int first;
    long second;
    std::array<std::size_t, TEST_TIMES>& ref_results;
};

////////////////////////////
//index                0 1//
//test_results         0 1//
//test_reverse_results 1 0//
////////////////////////////

void TestEventFunction1Content(const TestEventArgs& event_args)
{
    if (event_args.isReversed)
    {
        event_args.ref_results[0] = 1;
    }
    else
    {
        event_args.ref_results[0] = 0;
    }
}

void TestEventFunction2Content(const TestEventArgs& event_args)
{
    if (event_args.isReversed)
    {
        event_args.ref_results[1] = 0;
    }
    else
    {
        event_args.ref_results[1] = 1;
    }
}

ceiba::EventState TestEventFunction1(const TestEventArgs& event_args)
{
    TestEventFunction1Content(event_args);
    return ceiba::EventState::Continue;
}

ceiba::EventState TestEventFunction2(const TestEventArgs& event_args)
{
    TestEventFunction2Content(event_args);
    return ceiba::EventState::Continue;
}

static std::array<std::size_t, TEST_TIMES> test_results;
static std::array<std::size_t, TEST_TIMES> test_reverse_results;

void RestTestResult()
{
    //重置测试数据
    memset(test_results.data(), 0, sizeof(test_results));
    memset(test_reverse_results.data(), 0, sizeof(test_reverse_results));
}

template <class Event>
void TestEventAfterAddFunction(Event& event)
{
    event.TriggerEvent(TestEventArgs{false, 1, 2, test_results});
    event.TriggerEventReversely(TestEventArgs{true, 1, 2, test_reverse_results});
    EXPECT_EQ(test_results[0], test_reverse_results[1]);
    EXPECT_EQ(test_results[1], test_reverse_results[0]);
    RestTestResult();
}

TEST(EventTest, DefaultEvent)
{
    {
        ceiba::DefaultEvent<const TestEventArgs&> test_event1{};
        test_event1.AddFunction([](const TestEventArgs& event_args) -> ceiba::EventState
                                {
                                    EXPECT_EQ(event_args.first, 1);
                                    EXPECT_EQ(event_args.second, 2);
                                    TestEventFunction1Content(event_args);
                                    return ceiba::EventState::Continue;
                                });
        test_event1.AddFunction(std::bind(&TestEventFunction2, std::placeholders::_1));

        TestEventAfterAddFunction(test_event1);
    }
}
TEST(EventTest, FunctionPointerDefaultEvent)
{
    ceiba::DefaultFunctionPointerEvent<const TestEventArgs&> test_event2{};
    test_event2.AddFunction(&TestEventFunction1);
    test_event2.AddFunction(&TestEventFunction2);

    TestEventAfterAddFunction(test_event2);
}
TEST(EventTest, CustomEvent)
{
    using ListEventFunctionStorageType = ceiba::EventFunctionStorageType<std::function<ceiba::EventState(const TestEventArgs&)>, std::uint32_t>;
    ceiba::Event<
        std::function<ceiba::EventState(const TestEventArgs&)>,
        std::mutex,
        std::uint32_t,
        std::list<ListEventFunctionStorageType, std::pmr::polymorphic_allocator<ListEventFunctionStorageType>>>
        test_event3{[](const TestEventArgs& event_args) -> ceiba::EventState
                    {
                        EXPECT_EQ(event_args.first, 1);
                        EXPECT_EQ(event_args.second, 2);
                        TestEventFunction1Content(event_args);
                        return ceiba::EventState::Continue;
                    },
                    std::pmr::polymorphic_allocator<ListEventFunctionStorageType>{}};
    test_event3.AddFunction(std::bind(&TestEventFunction2, std::placeholders::_1));

    TestEventAfterAddFunction(test_event3);
}