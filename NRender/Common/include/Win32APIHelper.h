/***
 * @Author: Dusk
 * @Date: 2022-01-28 23:05:12
 * @FilePath: \NRender\NRender\Common\include\WinAPIHelper.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <memory>
#include <Windows.h>

namespace dusk
{
    constexpr static DWORD EMPTY_FLAG = 0;

    int EvaluateWideCharToMultiByteSize(UINT code_page, wchar_t* p_wchar_string, DWORD flags = 0, int wchar_string_length = -1);
    auto AllocateWideCharToMultiByteMemory(UINT code_page, wchar_t* p_wchar_string, DWORD flags = 0, int wchar_string_length = -1)
        -> std::unique_ptr<char[]>;

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

        char* ToWriteByByteSize(std::size_t memory_byte_size)
        {
            if (memory_byte_size <= buffer_byte_size_)
            {
                return up_buffer_.get();
            }
            else
            {
                up_buffer_.reset();
                up_buffer_ = std::make_unique<T[]>(memory_byte_size / sizeof(T));
                return up_buffer_.get();
            }
        }
        char* ToWriteByElementSize(std::size_t element_count)
        {
            if (element_count * sizeof(T) <= buffer_byte_size_)
            {
                return up_buffer_.get();
            }
            else
            {
                up_buffer_.reset();
                up_buffer_ = std::make_unique<T[]>(element_count);
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