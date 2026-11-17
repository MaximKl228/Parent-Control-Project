#pragma once
#include <format>
#include <string_view>
#include "Logger.cpp"

enum class LogLvl {Debag = 0, Info = 1, Warn = 2, Error = 3};

// Minimum level that actually gets logged, picked from the build mode.
// It's a plain number (not LogLvl::...) so the preprocessor can compare it below.
// To override it, add LOG_MIN_LEVEL=<0..3> to Project Properties ->
// C/C++ -> Preprocessor -> Preprocessor Definitions.
#ifndef LOG_MIN_LEVEL
	#ifdef _DEBUG
		#define LOG_MIN_LEVEL 0   // Debug: everything
	#else
		#define LOG_MIN_LEVEL 1   // Release: Info and up
	#endif
#endif

class Logger
{
private:

	static void Write(LogLvl lvl, std::string_view message);
public:
	static constexpr LogLvl MinLevel = static_cast<LogLvl>(LOG_MIN_LEVEL);

	template<typename... Args>
	static void Log(LogLvl level, std::format_string<Args...> fmt, Args&&... args)
	{
		if (level < MinLevel) return; // skipped before formatting, so it costs almost nothing
		Write(level, std::format(fmt, std::forward<Args>(args)...));
	}
};

// Shortcut macros. Any level below LOG_MIN_LEVEL compiles to nothing,
// so its arguments aren't even evaluated in that build.
#if LOG_MIN_LEVEL <= 0
	#define LOG_DEBUG(...) Logger::Log(LogLvl::Debag, __VA_ARGS__)
#else
	#define LOG_DEBUG(...) ((void)0)
#endif

#if LOG_MIN_LEVEL <= 1
	#define LOG_INFO(...) Logger::Log(LogLvl::Info, __VA_ARGS__)
#else
	#define LOG_INFO(...) ((void)0)
#endif

#if LOG_MIN_LEVEL <= 2
	#define LOG_WARN(...) Logger::Log(LogLvl::Warn, __VA_ARGS__)
#else
	#define LOG_WARN(...) ((void)0)
#endif

#define LOG_ERROR(...) Logger::Log(LogLvl::Error, __VA_ARGS__)
