//
// Created by Sayama on 05/09/2024.
//

#pragma once

#include <chrono>

class Profiler {
public:
	using ClockType = long double;
	using SecDuration = std::chrono::duration<ClockType, std::ratio<1>>;
	using MilliDuration = std::chrono::duration<ClockType, std::milli>;
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock, SecDuration>;
public:
	Profiler() = default;
	~Profiler() = default;
public:
	void Start();
	void Stop();
	[[nodiscard]] SecDuration GetDuration() const;
	[[nodiscard]] ClockType GetMilli() const;
private:
	TimePoint begin;
	TimePoint end;
};

class ScopeProfiler
{
public:
	ScopeProfiler();
	ScopeProfiler(std::string name);
	~ScopeProfiler();
private:
	std::string m_Name;
	Profiler m_Profiler;
};


#define CONCAT(x, y) x ## y
#define COMBINE(x, y) CONCAT(x, y)

#define SCOPED_PROFILING(name) ::ScopeProfiler COMBINE(scoped_profile_, __LINE__)(name)
#define FUNCTION_PROFILING(name) ::ScopeProfiler COMBINE(func_profile_, __LINE__)(__FUNCTION__)
