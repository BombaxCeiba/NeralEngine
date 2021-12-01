#pragma once
#include<string>
#include<hash_map>
#include<source_location>
#include<wrl.h>

namespace Dusk
{
    namespace Tools
    {
        class DXException :public std::exception
        {
        private:
            const static std::hash_map<HRESULT, std::string> error_description_map_;
            const std::source_location source_info_;
            const HRESULT error_;
            const static std::string before_error_;
            const static std::string after_error_;
            const static std::string before_description_;
        public:
            DXException(HRESULT hCode, const std::source_location& source_info) :source_info_{ source_info } {};
            const char* what() const noexcept override//C++11Æð
            {
                return;
            }
        };
        inline void ThrowIfFailed(HRESULT hCode, const std::source_location source_info = std::source_location::current())
        {
            if (FAILED(hCode))
            {

            }
        }
    }

    //#if ((defined _DEBUG) || (defined DEBUG))
    //#else
    //
    //#endif // DEBUG
}
