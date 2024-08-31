//
// Created by ianpo on 31/08/2024.
//

#include "Todo/Core/Logger.hpp"


#ifdef TODO_LOG
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/std.h>
#include <iostream>
#include <string>
using namespace std::literals;
#endif

// Reduce verbosity but let you know what is in what namespace

namespace Todo {

	static LogFuncPtr s_LogFunc{nullptr};

#ifdef TODO_LOG
	static std::string GetLogTypeStr(LogType logType)
	{
		switch (logType) {
			case LogType::Trace: return "Trace"s;
			case LogType::Info: return "Info"s;
			case LogType::Warning: return "Warning"s;
			case LogType::Error: return "Error"s;
		}
		return "Unknown"s;
	}
#endif

	void Logger::Log(LogType logType, std::string_view message, std::source_location source, Clock::time_point timelog)
	{
		if (s_LogFunc) {
			s_LogFunc(source, timelog, logType, std::string{message});
		}
#ifdef TODO_LOG
		else {
			std::cout << fmt::format("[{:%F %R:%S} UTC] ", timelog) << "[TODO] " << fmt::format("[{}] ",GetLogTypeStr(logType)) << fmt::format("[{}:{}] ", source.file_name(), source.line()) << message << std::endl;
		}
#endif
	}

	void Logger::SetupLogger(LogFuncPtr logfunction)
	{
		s_LogFunc = logfunction;
	}

	bool Logger::HasLogger()
	{
#ifdef TODO_LOG
		return true;
#else
		return HasCustomLogger();
#endif
	}

	bool Logger::HasCustomLogger()
	{
		return s_LogFunc;
	}
}