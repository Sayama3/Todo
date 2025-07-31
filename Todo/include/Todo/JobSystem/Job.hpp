//
// Created by ianpo on 31/08/2024.
//

#pragma once

namespace Todo {

	using JobId = uint64_t;

	template<typename T>
	concept IJob = requires(const T& v) {
		{v.id()} -> JobId;
		{v.dependencies()} -> const JobId*;
		{v.dependencies_count()} -> uint64_t;
	};

	class Job {

	};

} // Todo
