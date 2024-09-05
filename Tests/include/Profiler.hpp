//
// Created by ianpo on 05/09/2024.
//

#pragma once

#include <chrono>

class Profiler {
public:
	using Clock = std::chrono::high_resolution_clock;
public:
	Profiler() = default;
	~Profiler() = default;
public:
	void Start();
	void Stop();
	Clock::duration GetDuration() const;
	long double GetMilli() const;
private:
	Clock::time_point begin;
	Clock::time_point end;
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
