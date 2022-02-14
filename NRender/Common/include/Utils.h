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
#include <algorithm>
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

#if __cplusplus >= 202002L
#define DUSK_NO_UNIQUE_ADDRESS_ATTRIBUTE [[no_unique_address]]
#else
#define DUSK_NO_UNIQUE_ADDRESS_ATTRIBUTE
#endif //__cplusplus >= 202002L

namespace dusk
{

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

    template <typename T, typename Allocator = std::allocator<T>>
    class DynamicBuffer
    {
    public:
        static_assert(std::is_default_constructible<T>{},
                      "T is not default constructible!");
        using AllocatorTraits = std::allocator_traits<Allocator>;
        using Pointer = AllocatorTraits::pointer;
        using ConstPointer = AllocatorTraits::const_pointer;
        using SizeType = AllocatorTraits::size_type;

        DynamicBuffer(SizeType expected_element_count, Allocator allocator = {})
            : element_count_{expected_element_count}, allocator_{allocator},
              p_buffer_{AllocatorTraits::allocate(allocator, expected_element_count)}
        {
            ConstructElements(allocator_, p_buffer_, element_count_);
        }
        template <typename InputIt>
        DynamicBuffer(InputIt first, InputIt last, Allocator allocator = {})
            : element_count_{std::distance(first, last)}, allocator_{allocator}
        {
            Pointer p_destination = AllocatorTraits::allocate(allocator, element_count_);
            p_buffer_ = p_destination;
            SizeType counter = 0;
            try
            {
                while (first != last)
                {
                    *p_destination = *first;
                    std::advance(p_destination);
                    std::advance(first);
                    counter++;
                }
            }
            catch (...)
            {
                DestoryElements(allocator_, p_buffer_, counter);
                AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);
            }
        }
        ~DynamicBuffer()
        {
            DestoryElements(allocator_, p_buffer_, element_count_);
            AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);
        }
        DynamicBuffer(const DynamicBuffer&) = delete;
        DynamicBuffer& operator=(const DynamicBuffer&) = delete;
        DynamicBuffer Clone() const
        {
            return {p_buffer_, std::next(p_buffer_, element_count_), allocator_};
        }

        DynamicBuffer(DynamicBuffer&&) = default;
        DynamicBuffer& operator=(DynamicBuffer&&) = default;

    private:
        void ResetMemory() noexcept
        {
            memset(p_buffer_, 0, CalculateByteSize(element_count_));
        }
        void ResetMemory(void* start, SizeType size) noexcept
        {
            memset(start, 0, sizeof(T) * size);
        }
        void ExpandAndRetain(SizeType element_count)
        {
            Pointer p_new_buffer = AllocatorTraits::allocate(allocator_, element_count);

            auto first = p_buffer_;
            auto destination = p_new_buffer;
            ::memmove(destination, first, sizeof(T) * element_count_);
            element_count_ = element_count;
            //初始化多余内存
            SizeType expand_count = element_count - element_count_;
            memset(std::next(p_new_buffer + expand_size), 0, sizeof(T) * expand_count);

            DestoryElements(allocator_, p_buffer_, element_count_);
            AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);
        }
        void ExpandAndDiscard(SizeType element_count)
        {
            DestoryElements(allocator_, p_buffer_, element_count_);
            AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);
            p_buffer_ = AllocatorTraits::allocate(allocator_, element_count_);
            element_count_ = element_count;
            ResetMemory();
        }
        inline static SizeType CalculateElementCount(SizeType buffer_byte_size) noexcept
        {
            return buffer_byte_size / sizeof(T);
        }
        inline static SizeType CalculateByteSize(SizeType element_count) noexcept
        {
            return element_count * sizeof(T);
        }
        inline static void ConstructElements(Allocator allocator, Pointer p_buffer, SizeType element_count) noexcept
        {
            SizeType counter = 0;
            try
            {
                for (; counter < element_count; ++counter)
                {
                    AllocatorTraits::construct(allocator, p_buffer);
                    std::advance(p_buffer, 1);
                }
            }
            catch (...)
            {
                DestoryElements(allocator, p_buffer, counter);
                AllocatorTraits::deallocate(allocator, p_buffer, element_count);
            }
        }
        inline static void DestoryElements(Allocator allocator, Pointer p_buffer, SizeType element_count) noexcept(noexcept(~T()))
        {
            Pointer p_buffer_it = p_buffer + element_count;
            for (SizeType i = 0; i < element_count; ++i)
            {
                AllocatorTraits::destroy(allocator, p_buffer_it);
                std::advance(p_buffer_it, -1);
            }
        }

    public:
        void ResetBuffer() noexcept(noexcept(~T()))
        {
            DestoryElements(allocator_, p_buffer_, element_count_);
            ResetMemory();
        }
        Pointer ToWriteUnsafe() noexcept
        {
            return p_buffer_;
        }
        Pointer ToWriteByByteSize(SizeType required_byte_size)
        {
            if (required_byte_size <= CalculateByteSize(element_count_))
            {
                return p_buffer_;
            }
            else
            {
                SizeType required_count = CalculateElementCount(required_byte_size);
                ExpandAndRetain(required_count);
                return p_buffer_;
            }
        }
        Pointer ToWriteByElementCount(SizeType required_element_count)
        {
            SizeType memory_byte_size = CalculateByteSize(required_element_count);
            if (memory_byte_size <= memory_byte_size)
            {
                return p_buffer_;
            }
            else
            {
                ExpandAndRetain(required_element_count);
                return p_buffer_;
            }
        }
        Pointer ToWriteAllByByteSize(SizeType required_byte_size)
        {
            if (required_byte_size <= CalculateByteSize(element_count_))
            {
                return p_buffer_;
            }
            else
            {
                SizeType required_element_count = CalculateElementCount(required_byte_size);
                ExpandAndDiscard(required_element_count);
                return p_buffer_;
            }
        }
        Pointer ToWriteAllByElementCount(SizeType required_element_count)
        {
            SizeType memory_byte_size = CalculateByteSize(required_element_count);
            if (memory_byte_size <= CalculateByteSize(element_count_))
            {
                return p_buffer_;
            }
            else
            {
                ExpandAndDiscard(required_element_count);
                return p_buffer_;
            }
        }
        ConstPointer ToRead() const noexcept
        {
            return p_buffer_;
        }

    private:
        Pointer p_buffer_;
        SizeType element_count_;
        DUSK_NO_UNIQUE_ADDRESS_ATTRIBUTE
        Allocator allocator_;
    };
}