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

#include "Profiler.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string>

using namespace std::string_literals;

void Profiler::Start() {
	begin = Clock::now();
}

void Profiler::Stop() {
	end = Clock::now();
}

Profiler::SecDuration Profiler::GetDuration() const {
	return end - begin;
}

long double Profiler::GetMilli() const {
	return std::chrono::duration_cast<MilliDuration>(GetDuration()).count();
}

ScopeProfiler::ScopeProfiler() : m_Name("Unknown"s)
{
	m_Profiler.Start();
}

ScopeProfiler::ScopeProfiler(std::string name) : m_Name(std::move(name))
{
	m_Profiler.Start();
}

ScopeProfiler::~ScopeProfiler() {
	m_Profiler.Stop();

	LOG("[" << m_Name << "] - " << m_Profiler.GetMilli() << "ms.");
}
