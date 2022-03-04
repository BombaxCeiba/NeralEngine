/***
 * @Author: dusk
 * @Date: 2021-11-30 20:42:26
 * @FilePath: \NRender\NRender\Common\include\DXUtils.h
 * @(c) 2021 dusk.
 */
#pragma once
#include<string>
#include<source_location>
#include<wrl/client.h>

namespace dusk
{
    namespace tools
    {
        class DXException :public std::exception
        {
        private:
            const HRESULT error_;
            const static std::string before_error_;
            const static std::string after_error_;
            const static std::string after_line_;
            const static std::string after_column_;
            const static std::string after_file_name_;

            const std::string full_text_;
        public:
            DXException(HRESULT hCode, const std::source_location& source_info);
            const char* what() const noexcept override//C++11?
            {
                return full_text_.c_str();
            }
        };
        inline void ThrowIfFailed(HRESULT hCode, std::source_location source_info = std::source_location::current())
        {
            if (FAILED(hCode))
            {
                throw DXException(hCode, source_info);
            }
        }
    }
}
