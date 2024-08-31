//
// Created by ianpo on 31/08/2024.
//

#include <Todo/Todo.hpp>
#include <Todo/Core/Logger.hpp>

#include <iostream>
#include <chrono>

#ifndef TODO_LOG
void Log(std::source_location source, Todo::Clock::time_point timelog, Todo::LogType logType, std::string message)
{
	std::cout << "[" << timelog << " UTC] [" << "CUSTOM TODO" << "] [" << std::filesystem::path(source.file_name()).filename().string() << ":" << source.line() << "] - " << message << std::endl;
}
#endif

int main(int argc, char* argv[])
{
#ifndef TODO_LOG
	Todo::Logger::SetupLogger(Log);
#endif
	Todo::Initialize(Todo::GetOptimalThreadCount());
	Todo::Destroy();
	return 0;
}