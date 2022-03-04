/**
 * @author: Dusk
 * @date: 2022-01-24 11:42:25
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:09:46
 * @copyright Copyright (c) 2022 Dusk.
 */
#include "TestMain.h"
#include "../include/Logger.h"

NERAL_ENGINE_TEST_NAMESPACE_START
TEST(Log, TryToLog)
{
    Neral::Outputer::Console::StringType test_str{"Test Error Log"};
    Neral::Log<Neral::Outputer::Console>(&Neral::LogType::Error, test_str);
    Neral::LogError<Neral::Outputer::Console>(test_str);
    Neral::LogError<Neral::Outputer::Console>(test_str, test_str);

    Neral::LogWarning<Neral::Outputer::Console>(test_str);
    Neral::LogDebug<Neral::Outputer::Console>(test_str);
}
NERAL_ENGINE_TEST_NAMESPACE_END