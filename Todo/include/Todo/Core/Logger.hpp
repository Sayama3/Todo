//    Copyright 2026 https://github.com/Sayama3
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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