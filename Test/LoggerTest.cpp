/***
 * @Author: Dusk
 * @Date: 2022-01-24 11:42:25
 * @FilePath: \NRender\Test\LoggerTest.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include <string>
#include "TestMain.hpp"
#include "Logger.h"

TEST(Log, TryToLog)
{
    dusk::Outputer::Console::StringType test_str{"Test Error Log"};
    dusk::Log<dusk::Outputer::Console>(&dusk::LogType::Error, test_str);
}
