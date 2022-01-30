/***
 * @Author: Dusk
 * @Date: 2021-12-03 17:59:24
 * @LastEditTime: 2021-12-03 17:59:24
 * @LastEditors: Dusk
 * @FilePath: \NRender\NRender\Common\include\Utils.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <type_traits>
#include <memory>
#include <utility>

#if defined(_MSC_VER)
#define EXPORT_SYMBOL __declspec(dllexport)
#elif defined(__clang__)
#define EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(__INTEL_COMPILER)
static_assert(false, "Unsupport!")
/*Unsupport!*/
#elif defined(__GNUC__)
#define EXPORT_SYMBOL __attribute__((visibility("default")))
#endif

namespace dusk
{
    template <typename T>
    struct function_first_argument_type;
    template <typename C, typename R, typename First, typename... Args>
    struct function_first_argument_type<R (C::*)(First, Args...)>
    {
        using type = First;
    };

    template <typename T, std::size_t Length>
    constexpr inline std::size_t GetLength(const T (&data)[Length])
    {
        return Length;
    }
    template <typename T, typename C, std::size_t Length>
    constexpr inline std::size_t GetLength(const T (C::*(data))[Length])
    {
        return Length;
    }
    template <typename T, typename C, std::size_t Length>
    constexpr inline std::size_t GetLength(T (C::*(data))[Length])
    {
        return Length;
    }

    template <typename MemoryType>
    inline void SetMemoryZero(MemoryType* p_memory_to_set_zero, std::size_t element_length)
    {
        memset(p_memory_to_set_zero, 0, sizeof(MemoryType) * element_length);
    }

    template <typename T>
    class DynamicBuffer
    {
    public:
        DynamicBuffer(std::size_t expected_element_count)
            : buffer_byte_size_{expected_element_count * sizeof(T)}
        {
            up_buffer_ = std::make_unique<T[]>(expected_element_count);
        }
        ~DynamicBuffer() = default;
        DynamicBuffer(const DynamicBuffer&) = delete;
        DynamicBuffer& operator=(const DynamicBuffer&) = delete;
        DynamicBuffer Clone() const
        {
            DynamicBuffer result{buffer_byte_size_ / sizeof(T)};
            memcpy(result.up_buffer_.get(), this->up_buffer_.get(), buffer_byte_size_);
            return result;
        }
        DynamicBuffer(DynamicBuffer&&) = default;
        DynamicBuffer& operator=(DynamicBuffer&&) = default;

    private:
        void ExpandAndRetain(std::size_t element_count, std::size_t new_buffer_byte_size)
        {
            auto up_bigger_buffer = std::make_unique<T[]>(element_count);
            memcpy(up_bigger_buffer.get(), up_buffer_.get(), buffer_byte_size_);
            up_buffer_.reset(std::move(up_bigger_buffer));
            buffer_byte_size_ = new_buffer_byte_size;
        }
        void ExpandAndDiscard(std::size_t element_count, std::size_t new_buffer_byte_size)
        {
            up_buffer_.reset();
            up_buffer_ = std::make_unique<T[]>(element_count);
            buffer_byte_size_ = new_buffer_byte_size;
        }

    public:
        char* ToWriteUnsafe() noexcept
        {
            return up_buffer_.get();
        }
        char* ToWriteByByteSize(std::size_t memory_byte_size)
        {
            if (memory_byte_size <= buffer_byte_size_)
            {
                return up_buffer_.get();
            }
            else
            {
                std::size_t element_count = memory_byte_size / sizeof(T);
                ExpandAndRetain(element_count, memory_byte_size);
                return up_buffer_.get();
            }
        }
        char* ToWriteByElementCount(std::size_t element_count)
        {
            std::size_t memory_byte_size = element_count * sizeof(T);
            if (memory_byte_size <= buffer_byte_size_)
            {
                return up_buffer_.get();
            }
            else
            {
                ExpandAndRetain(element_count, memory_byte_size);
                return up_buffer_.get();
            }
        }
        char* ToWriteAllByByteSize(std::size_t memory_byte_size)
        {
            if (memory_byte_size <= buffer_byte_size_)
            {
                return up_buffer_.get();
            }
            else
            {
                std::size_t element_count = memory_byte_size / sizeof(T);
                ExpandAndDiscard(memory_byte_size, element_count);
                return up_buffer_.get();
            }
        }
        char* ToWriteAllByElementCount(std::size_t element_count)
        {
            std::size_t memory_byte_size = element_count * sizeof(T);
            if (memory_byte_size <= buffer_byte_size_)
            {
                return up_buffer_.get();
            }
            else
            {
                ExpandAndDiscard(memory_byte_size, element_count);
                return up_buffer_.get();
            }
        }

        const char* ToRead() const noexcept
        {
            return up_buffer_.get();
        }
        void ResetMemory() noexcept
        {
            memset(up_buffer_.get(), 0, buffer_byte_size_);
        }

    private:
        std::unique_ptr<T[]> up_buffer_;
        std::size_t buffer_byte_size_;
    };
}