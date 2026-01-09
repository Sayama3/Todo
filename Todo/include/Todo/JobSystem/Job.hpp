//
// Created by Sayama on 31/08/2024.
//

#pragma once

namespace Todo {

	using JobId = uint64_t;

	template<typename T>
	concept IJob = requires(const T& v) {
		{v.id()} -> std::convertible_to<JobId>;
		{v.dependencies()} -> std::convertible_to<const JobId*>;
		{v.dependencies_count()} -> std::convertible_to<uint64_t>;
	};

	class Job {

	};

} // Todo
