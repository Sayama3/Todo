//
// Created by Sayama on 07/08/2025.
//

#pragma once

#include "../Core/Concepts.hpp"
#include "Mutex.hpp"
#include "ThreadsafeList.hpp"

namespace Todo {
	/// A map that can safely be accessed across multiple threads using locks.
	/// @tparam Key The Trivial Key Type
	/// @tparam Value The Movable Value
	template<Trivial Key, std::copyable Value, typename Hash = std::hash<Key>, typename EqualTo = std::equal_to<Key>, CSharedMutex SharedMutex = std::shared_mutex>
	class ThreadsafeMap {
	private:
		class Bucket
		{
		public:
			template<typename Type>
			using Allocator = std::allocator<Type>;
		private:
			using bucker_value = std::pair<const Key, Value>;
			using bucker_data = ThreadsafeList<bucker_value, SharedMutex>;
			using iterator = bucker_data::iterator;
		public:
			Value value_for(const Key& key, Value default_value = {});
			void add_or_update_mapping(const Key& key, const Value& value);
			void remove_mapping(const Key& key);
		public:
			bucker_data data;
			// mutable TSharedMutex<SharedMutex> mutex;
		};
	public:
		ThreadsafeMap(uint32_t num_buckets = 19, const Hash& hasher = Hash{});
		~ThreadsafeMap();
		ThreadsafeMap(const ThreadsafeMap&) = delete;
		ThreadsafeMap& operator=(const ThreadsafeMap&) = delete;
	public:
		Value value_for(const Key& key, const Value& default_value = {});
		void add_or_update_mapping(const Key& key, const Value& value);
		void remove_mapping(const Key& key);


		std::unordered_map<Key, Value, Hash, EqualTo> get_map();
	private:
		Bucket& GetBucket(const Key& key) const;

	private:
		std::vector<std::unique_ptr<Bucket>> buckets;
		Hash hasher;
	};

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	Value ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::Bucket::value_for(const Key& key, Value default_value)
	{
		auto result = data.find_first_if([&key](const bucker_value& pair){return EqualTo(pair.first, key);});
		return result ? default_value : result->second;
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::Bucket::add_or_update_mapping( const Key& key, const Value& value)
	{
		bool found = false;
		data.for_each([&key, &value, &found](bucker_value& pair)
		{
			if (EqualTo(pair.first, key))
			{
				found = true;
				pair.second = value;
			}
		});

		if (!found)
		{
			data.push_front(bucker_value{key, value});
		}
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::Bucket::remove_mapping(const Key& key)
	{
		data.remove_if([&key](const bucker_value& pair) {return EqualTo(pair.first, key);});
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::ThreadsafeMap(const uint32_t num_buckets, const Hash& hasher) : buckets(num_buckets), hasher(hasher)
	{
		for (uint32_t i = 0u; i < num_buckets; ++i)
		{
			buckets[i].reset(new Bucket);
		}
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::~ThreadsafeMap() = default;

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	Value ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::value_for(const Key& key, const Value& default_value)
	{
		return GetBucket(key).value_for(key, default_value);
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::add_or_update_mapping(const Key& key, const Value& value)
	{
		return GetBucket(key).add_or_update_mapping(key, value);
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::remove_mapping(const Key& key)
	{
		return GetBucket(key).remove_mapping(key);
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	std::unordered_map<Key, Value, Hash, EqualTo> ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::get_map()
	{
		std::vector<std::unique_lock<std::shared_mutex>> locks;
		for (uint32_t i = 0u; buckets.size(); ++i)
		{
			locks.push_back(std::unique_lock(buckets[i]->mutex));
		}

		std::unordered_map<Key, Value, Hash, EqualTo> map;
		for (uint32_t i = 0u; buckets.size(); ++i)
		{
			for (uint64_t j = 0u; j < buckets[i]->data.size(); ++j)
			{
				map.insert(buckets[i]->data[j]);
			}
		}

		return map;
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CSharedMutex SharedMutex>
	typename ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex>::Bucket& ThreadsafeMap<Key, Value,
	Hash, EqualTo, SharedMutex>::GetBucket(const Key& key) const
	{
		const auto bucket_index = hasher(key) % buckets.size();
		return buckets.at(bucket_index);
	}
}
