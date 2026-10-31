#pragma once
#include <format>
#include <string_view>

enum class LogLvl {Debag, Info, Warn, Error};
class Logger
{
private:

	static void Write(LogLvl lvl, std::string_view message);
public:
	template<typename... Args>
	static void Log(LogLvl level, std::format_string<Args...> fmt, Args&&... args)
	{
		Write(level, std::format(fmt, std::forward<Args>(args)...));
	}
};

