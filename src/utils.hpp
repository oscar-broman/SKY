#pragma once

#include <string>
#include <sstream>
#include "main.hpp"

inline void ShowPluginInfo()
{
    SkyComponent::Log(LogLevel::Message, "");
    SkyComponent::Log(LogLevel::Message, " ===============================");
    SkyComponent::Log(LogLevel::Message, "   ");
    SkyComponent::Log(LogLevel::Message, "   < " PROJECT_NAME " - %d.%d.%d >", PROJECT_MAJOR, PROJECT_MINOR, PROJECT_PATCH);
    SkyComponent::Log(LogLevel::Message, "   ");
    SkyComponent::Log(LogLevel::Message, "   (c) 2008 - Present | YSF & SKY Maintainers");
    SkyComponent::Log(LogLevel::Message, "   (c) 2015 | Oscar \"Slice\" Broman");
    SkyComponent::Log(LogLevel::Message, "   ");
    SkyComponent::Log(LogLevel::Message, "   Server Version: open.mp");
    SkyComponent::Log(LogLevel::Message, "   Operating System: " OS_NAME);
    SkyComponent::Log(LogLevel::Message, "   Built on: " __DATE__ " at " __TIME__);
    SkyComponent::Log(LogLevel::Message, "   ");
    SkyComponent::Log(LogLevel::Message, " ===============================");
    SkyComponent::Log(LogLevel::Message, "");
}