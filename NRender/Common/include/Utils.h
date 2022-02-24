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

    template <typename Allocator, typename It>
    inline void DestroyRange(Allocator allocator, It first, It last) noexcept
    {
        while (first != last)
        {
            std::allocator_traits<Allocator>::destroy(allocator, first);
            std::advance(first, 1);
        }
    }

    template <typename T, typename Allocator = std::allocator<T>>
    class DynamicBuffer
    {
    public:
        static_assert(std::is_default_constructible<T>{},
                      "T is not default constructible!");
        static_assert(std::is_nothrow_move_constructible<T>{},
                      "T must be nothrow move constructible!");
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
            Pointer p_destination = AllocatorTraits::allocate(allocator_, element_count_);
            InputIt current = first;
            try
            {
                while (current != last)
                {
                    *p_destination = *current;
                    std::advance(p_destination, 1);
                    std::advance(current, 1);
                }
            }
            catch (...)
            {
                dusk::DestroyRange(allocator_, first, current);
                AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);
                throw;
            }
            p_buffer_ = std::move(p_destination);
        }
        ~DynamicBuffer()
        {
            DestroyElements(allocator_, p_buffer_, element_count_);
            AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);
        }
        DynamicBuffer(const DynamicBuffer&) = delete;
        DynamicBuffer operator=(const DynamicBuffer&) = delete;
        DynamicBuffer Clone() const
        {
            return {p_buffer_, std::next(p_buffer_, element_count_), allocator_};
        }

        DynamicBuffer(DynamicBuffer&&) = default;
        DynamicBuffer operator=(DynamicBuffer&& other)
        {
            return {std::forward<DynamicBuffer>(other)};
        }

    private:
        void ExpandAndRetain(SizeType new_element_count)
        {
            Pointer p_new_buffer = AllocatorTraits::allocate(allocator_, new_element_count);

            auto first = p_buffer_;
            auto last = std::next(p_buffer_, element_count_);
            Pointer p_current = p_new_buffer;
            try
            {
                while (first != last)
                {
                    AllocatorTraits::construct(allocator_, p_current, std::move(*first));
                    std::advance(first, 1);
                    std::advance(p_current, 1);
                }
            }
            catch (...)
            {
                dusk::DestroyRange(allocator_, p_new_buffer, first);
                AllocatorTraits::deallocate(allocator_, p_new_buffer, new_element_count);
                throw;
            }

            DestroyElements(allocator_, p_buffer_, element_count_);
            AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);

            //初始化多余内存
            SizeType expand_count = new_element_count - element_count_;
            Pointer construct_start_from = std::next(p_new_buffer, element_count_);
            element_count_ = new_element_count;
            ConstructElements(allocator_, construct_start_from, expand_count);

            p_buffer_ = std::move(p_new_buffer);
        }
        void ExpandAndDiscard(SizeType new_element_count)
        {
            Pointer p_new_buffer = AllocatorTraits::allocate(allocator_, new_element_count);

            DestroyElements(allocator_, p_buffer_, element_count_);
            AllocatorTraits::deallocate(allocator_, p_buffer_, element_count_);

            element_count_ = new_element_count;
            p_buffer_ = std::move(p_new_buffer);

            ConstructElements(allocator_, p_buffer_, element_count_);
        }
        static SizeType CalculateElementCount(SizeType buffer_byte_size) noexcept
        {
            return buffer_byte_size / sizeof(T);
        }
        static SizeType CalculateByteSize(SizeType element_count) noexcept
        {
            return element_count * sizeof(T);
        }
        static void ConstructElements(Allocator allocator, Pointer p_buffer, SizeType element_count)
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
                DestroyElements(allocator, p_buffer, counter);
                AllocatorTraits::deallocate(allocator, p_buffer, element_count);
                throw;
            }
        }

        static void DestroyElements(Allocator allocator, Pointer p_buffer, SizeType element_count)
        {
            for (SizeType i = 0; i < element_count; ++i)
            {
                AllocatorTraits::destroy(allocator, p_buffer);
                std::advance(p_buffer, 1);
            }
        }

    public:
        void ResetBuffer()
        {
            DestroyElements(allocator_, p_buffer_, element_count_);
            ConstructElements(allocator_, p_buffer_, element_count_);
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
            if (required_element_count <= element_count_)
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
            if (required_element_count <= element_count_)
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