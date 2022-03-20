#include "TestMain.h"
#include "../Version.h"
#include <iostream>

NERAL_ENGINE_TEST_NAMESPACE_START

TEST(VersionTest, CheckVersion)
{
    std::cout << "Build Date: " << Neral::BuildDate::Get() << std::endl;
    std::cout << "Build Time: " << Neral::BuildTime::Get() << std::endl;

    std::cout << "Manual Version: " << Neral::ManualVersion::Get() << std::endl;
    std::cout << "Build Version: " << Neral::BuildVersion::Get() << std::endl;
    std::cout << "Build type: " << Neral::BuildType::Get() << std::endl;
}

NERAL_ENGINE_TEST_NAMESPACE_END