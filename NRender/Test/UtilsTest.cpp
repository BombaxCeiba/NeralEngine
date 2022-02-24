/***
 * @Author: Dusk
 * @Date: 2022-02-22 11:24:48
 * @FilePath: \NRender\Test\UtilsTest.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include <unordered_map>
#include <string>
#include <algorithm>
#include <type_traits>
#include "TestMain.h"
#include "Utils.h"

namespace
{
    class TestHeapObject
    {
    private:
        static std::int8_t index_maker_;
        using ObjectPtrMap = std::unordered_map<decltype(index_maker_), TestHeapObject*>;
        static ObjectPtrMap all_objects_;

    public:
        TestHeapObject(std::int8_t int_data, const std::string& string_data)
            : data_int8_{int_data}, data_s_{string_data}, index_{index_maker_}
        {
            all_objects_[index_maker_] = this;
            ++index_maker_;
        }
        ~TestHeapObject()
        {
            auto it = all_objects_.find(index_);
            if (it != all_objects_.end())
            {
                all_objects_.erase(it);
            }
        }
        TestHeapObject(const TestHeapObject& other) = delete;
        TestHeapObject operator=(const TestHeapObject& other) = delete;
        TestHeapObject(TestHeapObject&& other)
            : data_int8_{std::move(other.data_int8_)}, data_s_{std::move(other.data_s_)},
              index_(std::move(other.index_))
        {
            all_objects_[index_] = this;
        }
        TestHeapObject operator=(TestHeapObject&& other)
        {
            return {std::forward<TestHeapObject>(other)};
        }
        static void CheckAndClear()
        {
            bool is_empty = all_objects_.empty();
            EXPECT_TRUE(is_empty);
            if (!is_empty)
            {
                for (auto it = all_objects_.begin(); it != all_objects_.end();)
                {
                    auto current_it = it;
                    ++it;
                    if (current_it->second != nullptr)
                    {
                        delete current_it->second;
                    }
                }
            }
        }
        std::int8_t data_int8_;
        std::string data_s_;

    private:
        decltype(index_maker_) index_;
    };
    std::int8_t TestHeapObject::index_maker_ = 0;
    typename TestHeapObject::ObjectPtrMap TestHeapObject::all_objects_{};

    constexpr std::int8_t default_int_value = 1;
    const std::string default_string_value = "\n";
    constexpr TestHeapObject* default_heapobject_ptr_ = nullptr;
    struct TestStruct
    {
        std::int8_t data_int8_ = default_int_value;
        std::string data_s_ = default_string_value;
        TestHeapObject* p_heap_object_ = default_heapobject_ptr_;
    };
    void ExpectDefaultTestStruct(const TestStruct test_struct)
    {
        EXPECT_EQ(test_struct.data_int8_, default_int_value);
        EXPECT_EQ(test_struct.data_s_, default_string_value);
        EXPECT_EQ(test_struct.p_heap_object_, default_heapobject_ptr_);
    }
}

TEST(UtilsTest, DynamicBufferTest)
{
    dusk::DynamicBuffer<TestStruct> buffer{1};
    ExpectDefaultTestStruct(*buffer.ToRead());
    auto& ref_test_struct = *buffer.ToWriteUnsafe();
    ref_test_struct.data_int8_ = 2;
    ref_test_struct.data_s_ = "2";
    ref_test_struct.p_heap_object_ = new TestHeapObject(2, "2");
    auto backup_ptr = ref_test_struct.p_heap_object_;

    buffer.ToWriteByElementCount(2);
    EXPECT_EQ(buffer.ToRead()->data_int8_, 2);
    EXPECT_EQ(buffer.ToRead()->data_s_, "2");
    EXPECT_EQ(buffer.ToRead()->p_heap_object_, backup_ptr);
    ExpectDefaultTestStruct(buffer.ToRead()[1]);

    buffer.ToWriteAllByByteSize(3 * sizeof(TestStruct));
    for (size_t i = 0; i < 3; ++i)
    {
        ExpectDefaultTestStruct(buffer.ToRead()[i]);
    }

    delete backup_ptr;
    TestHeapObject::CheckAndClear();
}