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
