//
// Created by ianpo on 31/08/2024.
//

#include "Todo/Core/Logger.hpp"


#ifdef TODO_LOG
#include <iostream>
#include <string>
using namespace std::literals;
#endif

// Reduce verbosity but let you know what is in what namespace

namespace Todo {
#ifndef TODO_DISABLE_LOG
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
#endif

	void Logger::Log(LogType logType, std::string_view message, std::source_location source, Clock::time_point timelog)
	{

#ifndef TODO_DISABLE_LOG
		if (s_LogFunc) {
			s_LogFunc(source, timelog, logType, std::string{message});
		}
#ifdef TODO_LOG
		else {
			std::cout << "[" << timelog << " UTC] [" << "TODO" << "] [" << std::filesystem::path(source.file_name()).filename().string() << ":" << source.line() << "] - " << message << std::endl;
		}
#endif
#endif
	}

	void Logger::SetupLogger(LogFuncPtr logfunction)
	{

#ifndef TODO_DISABLE_LOG
		s_LogFunc = logfunction;
#endif
	}

	bool Logger::HasLogger()
	{
#ifndef TODO_DISABLE_LOG
#ifdef TODO_LOG
		return true;
#else
		return HasCustomLogger();
#endif
#else
		return false;
#endif
	}

	bool Logger::HasCustomLogger()
	{
#ifndef TODO_DISABLE_LOG
		return s_LogFunc;
#else
		return false;
#endif
	}
}