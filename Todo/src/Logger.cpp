//
// Created by ianpo on 31/08/2024.
//

#include "Todo/Core/Logger.hpp"

using namespace std::chrono;
using namespace std::literals;
// Reduce verbosity but let you know what is in what namespace

namespace Todo {

	static LogFuncPtr s_LogFunc{nullptr};

	std::string GetLogTypeStr(LogType logType)
	{
		switch (logType) {

			case LogType::Trace: return "Trace"s;
			case LogType::Info: return "Info"s;
			case LogType::Warning: return "Warning"s;
			case LogType::Error: return "Error"s;
		}
	}

	void Logger::Log(std::source_location source, Clock::time_point timelog, LogType logType, std::string_view message)
	{
		if (s_LogFunc) {
			s_LogFunc(source, timelog, logType, std::string{message});
		} else {
			const std::chrono::zoned_time cur_time{ std::chrono::current_zone(), timelog };
			std::cout << std::format("[{0:%F} {0:%R}:{0:%S} {0:%Z}] ", cur_time) << "[TODO] " << std::format("[{}] ",GetLogTypeStr(logType)) << std::format("[{}:{}] ", source.file_name(), source.line()) << message << std::endl;
		}
	}

	void Logger::SetupLogger(LogFuncPtr logfunction)
	{
		s_LogFunc = logfunction;
	}
}