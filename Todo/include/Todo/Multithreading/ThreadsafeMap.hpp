//
// Created by ianpo on 07/08/2025.
//

#pragma once

#include "Concepts.hpp"
#include "Mutex.hpp"

namespace Todo {
	/// A map that can safely be accessed across multiple threads using locks.
	/// @tparam Key The Trivial Key Type
	/// @tparam Value The Movable Value
	template<Trivial Key, std::copyable Value, typename Hash = std::hash<Key>, typename EqualTo = std::equal_to<Key>, CMutex SharedMutex = std::shared_mutex, typename PairAllocator = std::allocator<std::pair<const Key, Value>>>
	class ThreadsafeMap {
	private:
		class Bucket
		{
		public:
			template<typename Type>
			using Allocator = std::allocator<Type>;
		private:
			using bucker_value = std::pair<const Key, Value>;
			using bucker_data = std::list<bucker_value, PairAllocator>;
			using iterator = bucker_data::iterator;
		public:
			Value value_for(const Key& key, Value default_value = {});
			void add_or_update_mapping(const Key& key, const Value& value);
			void remove_mapping(const Key& key);
		private:
			iterator find_entry_for(const Key& key) const;
		public:
			bucker_data data;
			mutable SharedMutex mutex;
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


		std::unordered_map<Key, Value, Hash, EqualTo, PairAllocator> get_map();
	private:
		Bucket& GetBucket(const Key& key) const;

	private:
		std::vector<std::unique_ptr<Bucket>> buckets;
		Hash hasher;
	};

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename
		PairAllocator>
	Value ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::Bucket::value_for(const Key& key, Value default_value)
	{
		std::shared_lock<std::shared_mutex> lock(mutex);
		const auto found_entry = find_entry_for(key);
		return found_entry == data.end() ? default_value : found_entry->second;
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename
		PairAllocator>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::Bucket::add_or_update_mapping( const Key& key, const Value& value)
	{
		std::unique_lock<std::shared_mutex> lock(mutex);
		const auto found_entry = find_entry_for(key);
		if (found_entry == data.end())
		{
			data.push_back({key, value});
		}
		else
		{
			found_entry->second = value;
		}
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename PairAllocator>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::Bucket::remove_mapping(const Key& key)
	{
		std::unique_lock<std::shared_mutex> lock(mutex);
		const auto found_entry = find_entry_for(key);
		if (found_entry != data.end())
		{
			data.erase(found_entry);
		}
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename PairAllocator>
	typename ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::Bucket::iterator ThreadsafeMap<Key,
	Value, Hash, EqualTo, SharedMutex, PairAllocator>::Bucket::find_entry_for(const Key& key) const
	{
		return std::find_if(data.begin(), data.end(), [&key](const bucker_value& value){return EqualTo(value.first, key);});
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename PairAllocator>
	ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::ThreadsafeMap(const uint32_t num_buckets, const Hash& hasher) : buckets(num_buckets), hasher(hasher)
	{
		for (uint32_t i = 0u; i < num_buckets; ++i)
		{
			buckets[i].reset(new Bucket);
		}
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename
		PairAllocator>
	ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::~ThreadsafeMap() = default;

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename
		PairAllocator>
	Value ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::value_for(const Key& key, const Value& default_value)
	{
		return GetBucket(key).value_for(key, default_value);
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename
		PairAllocator>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::add_or_update_mapping(const Key& key, const Value& value)
	{
		return GetBucket(key).add_or_update_mapping(key, value);
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename
		PairAllocator>
	void ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::remove_mapping(const Key& key)
	{
		return GetBucket(key).remove_mapping(key);
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename PairAllocator>
	std::unordered_map<Key, Value, Hash, EqualTo, PairAllocator> ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::get_map()
	{
		std::vector<std::unique_lock<std::shared_mutex>> locks;
		for (uint32_t i = 0u; buckets.size(); ++i)
		{
			locks.push_back(std::unique_lock(buckets[i]->mutex));
		}

		std::unordered_map<Key, Value, Hash, EqualTo, PairAllocator> map;
		for (uint32_t i = 0u; buckets.size(); ++i)
		{
			for (uint64_t j = 0u; j < buckets[i]->data.size(); ++j)
			{
				map.insert(buckets[i]->data[j]);
			}
		}

		return map;
	}

	template <Trivial Key, std::copyable Value, typename Hash, typename EqualTo, CMutex SharedMutex, typename
	                                            PairAllocator>
	typename ThreadsafeMap<Key, Value, Hash, EqualTo, SharedMutex, PairAllocator>::Bucket& ThreadsafeMap<Key, Value,
	Hash, EqualTo, SharedMutex, PairAllocator>::GetBucket(const Key& key) const
	{
		const auto bucket_index = hasher(key) % buckets.size();
		return buckets.at(bucket_index);
	}
}
