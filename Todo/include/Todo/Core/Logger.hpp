//
// Created by Sayama on 30/08/2024.
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

	using StdSystemClock = std::chrono::system_clock;
	using LogFuncPtr = void (*/*variable_name*/)(std::source_location, StdSystemClock::time_point, LogType, std::string);

	class Logger
	{
	public:
		static void SetupLogger(LogFuncPtr logfunction);
		static void ResetLogger() { SetupLogger(nullptr); }
		static bool HasLogger();
		static bool HasCustomLogger();
		static void Log	(LogType logType, std::string_view message, std::source_location source = std::source_location::current(), StdSystemClock::time_point timelog = StdSystemClock::now());

		static void Trace	(std::string_view message, 	std::source_location source = std::source_location::current(), StdSystemClock::time_point timelog = StdSystemClock::now()) { Log(LogType::Trace,	message, source, timelog); }
		static void Info	(std::string_view message, 	std::source_location source = std::source_location::current(), StdSystemClock::time_point timelog = StdSystemClock::now()) { Log(LogType::Info,		message, source, timelog); }
		static void Warning	(std::string_view message, 	std::source_location source = std::source_location::current(), StdSystemClock::time_point timelog = StdSystemClock::now()) { Log(LogType::Warning,	message, source, timelog); }
		static void Error	(std::string_view message, 	std::source_location source = std::source_location::current(), StdSystemClock::time_point timelog = StdSystemClock::now()) { Log(LogType::Error,	message, source, timelog); }
	};
}

#ifndef TODO_DISABLE_LOG
	#ifndef TODO_TRACE
		#define TODO_TRACE(message)		::Todo::Logger::Trace((message))
	#endif

	#ifndef TODO_INFO
		#define TODO_INFO(message)		::Todo::Logger::Info((message))
	#endif

	#ifndef TODO_WARNING
		#define TODO_WARNING(message)	::Todo::Logger::Warning((message))
	#endif

	#ifndef TODO_ERROR
		#define TODO_ERROR(message)		::Todo::Logger::Error((message))
	#endif

	#ifndef TODO_ERR
		#define TODO_ERR(message)		TODO_ERROR(message)
	#endif
#else
	#ifndef TODO_TRACE
		#define TODO_TRACE(message)
	#endif

	#ifndef TODO_INFO
		#define TODO_INFO(message)
	#endif

	#ifndef TODO_WARNING
		#define TODO_WARNING(message)
	#endif

	#ifndef TODO_ERROR
		#define TODO_ERROR(message)
	#endif

	#ifndef TODO_ERR
		#define TODO_ERR(message)
	#endif
#endif

#ifndef TODO_ASSERT
	#include <cassert>
	#define TODO_ASSERT(condition,message) if(!(condition)) {TODO_ERROR(message); assert(condition);}
	#define TODO_SASSERT(condition) TODO_ASSERT(condition, #condition)
#else
	// #define TODO_ASSERT(...)
	#define TODO_SASSERT(condition) TODO_ASSERT(condition, #condition)
#endif