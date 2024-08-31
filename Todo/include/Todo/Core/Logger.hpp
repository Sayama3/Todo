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
		static void ResetLogger() { SetupLogger(nullptr); }
		static bool HasLogger();
		static bool HasCustomLogger();
		static void Log	(LogType logType, std::string_view message, std::source_location source = std::source_location::current(), Clock::time_point timelog = Clock::now());

		static void Trace	(std::string_view message, 	std::source_location source = std::source_location::current(), Clock::time_point timelog = Clock::now()) { Log(LogType::Trace,		message, source, timelog); }
		static void Info	(std::string_view message, 	std::source_location source = std::source_location::current(), Clock::time_point timelog = Clock::now()) { Log(LogType::Info,		message, source, timelog); }
		static void Warning	(std::string_view message, 	std::source_location source = std::source_location::current(), Clock::time_point timelog = Clock::now()) { Log(LogType::Warning,	message, source, timelog); }
		static void Error	(std::string_view message, 	std::source_location source = std::source_location::current(), Clock::time_point timelog = Clock::now()) { Log(LogType::Error,		message, source, timelog); }
	};
}

#ifndef TODO_TRACE
	#define TODO_TRACE(message)		Logger::Trace((message))
#endif

#ifndef TODO_INFO
	#define TODO_INFO(message)		Logger::Info((message))
#endif

#ifndef TODO_WARNING
	#define TODO_WARNING(message)	Logger::Warning((message))
#endif

#ifndef TODO_ERROR
	#define TODO_ERROR(message)		Logger::Error((message))
#endif

#ifndef TODO_ERR
	#define TODO_ERR(message)		TODO_ERROR(message)
#endif

#ifndef TODO_ASSERT
	#include <cassert>
	#define TODO_ASSERT(condition,message) if(!(condition)) {TODO_ERROR(message); assert(condition);}
#endif