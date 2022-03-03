/***
 * @Author: Dusk
 * @Date: 2021-12-01 19:00:12
 * @FilePath: \NRender\NRender\Common\source\DXUtils.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include "../include/DXUtils.h"

using namespace dusk::tools;

const std::string DXException::before_error_{ "HRESULT: " };
const std::string DXException::after_error_{ "\nline: " };
const std::string DXException::after_line_{ "  column: " };
const std::string DXException::after_column_{ "\nfile name: " };
const std::string DXException::after_file_name_{ "\nfunction name: " };


DXException::DXException(HRESULT hCode, const std::source_location& source_info) :error_{ hCode },
full_text_{
    before_error_ + std::to_string(hCode) +
    after_error_ + std::to_string(source_info.line()) +
    after_line_ + std::to_string(source_info.column()) +
    after_column_ + source_info.file_name() +
    after_file_name_ + source_info.function_name()
    } {}