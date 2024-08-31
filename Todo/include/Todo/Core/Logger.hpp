//
// Created by ianpo on 30/08/2024.
//

#pragma once

//TODO: Handle a way to pass message

namespace Todo
{
	enum class LogType
	{
		Trace,
		Info,
		Warning,
		Error,
	};

	using Clock = std::chrono::system_clock;
	using LogFuncPtr = void (*/*variable_name*/)(std::source_location, Clock::time_point, LogType, std::string);

	class Logger
	{
	public:
		static void SetupLogger(LogFuncPtr logfunction);
	public:
		static void Log(std::source_location source, Clock::time_point timelog, LogType logType, std::string_view message);
	};
}

#ifndef TODO_TRACE
	#define TODO_TRACE(...)		Logger::Log(std::source_location::current(), Clock::now(), ::Todo::LogType::Trace, std::format(__VA_ARGS__))
#endif

#ifndef TODO_INFO
	#define TODO_INFO(...)		Logger::Log(std::source_location::current(), Clock::now(), ::Todo::LogType::Info, std::format(__VA_ARGS__))
#endif

#ifndef TODO_WARNING
	#define TODO_WARNING(...)	Logger::Log(std::source_location::current(), Clock::now(), ::Todo::LogType::Warning, std::format(__VA_ARGS__))
#endif

#ifndef TODO_ERROR
	#define TODO_ERROR(...)		Logger::Log(std::source_location::current(), Clock::now(), ::Todo::LogType::Error, std::format(__VA_ARGS__))
#endif

#ifndef TODO_ERR
	#define TODO_ERR(...) TODO_ERROR(__VA_ARGS__)
#endif

#ifndef TODO_ASSERT
	#include <cassert>
	#define TODO_ASSERT(condition,...) if(!(condition)) {TODO_ERROR(__VA_ARGS__); assert(condition);}
#endif