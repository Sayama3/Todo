//
// Created by ianpo on 05/09/2024.
//

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

Profiler::Clock::duration Profiler::GetDuration() const {
	return end - begin;
}

long double Profiler::GetMilli() const {
	return std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(GetDuration()).count();
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
