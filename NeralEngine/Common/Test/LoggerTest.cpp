/**
 * @author: Dusk
 * @date: 2022-01-24 11:42:25
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:09:46
 * @copyright Copyright (c) 2022 Dusk.
 */
#include "TestMain.h"
#include "../include/Logger.h"

TEST(Log, TryToLog)
{
    dusk::Outputer::Console::StringType test_str{"Test Error Log"};
    dusk::Log<dusk::Outputer::Console>(&dusk::LogType::Error, test_str);
    dusk::LogError<dusk::Outputer::Console>(test_str);
    dusk::LogError<dusk::Outputer::Console>(test_str, test_str);

    dusk::LogWarning<dusk::Outputer::Console>(test_str);
    dusk::LogDebug<dusk::Outputer::Console>(test_str);
}
