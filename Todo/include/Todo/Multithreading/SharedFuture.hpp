//
// Created by ianpo on 06/01/2026.
//

#pragma once


namespace TODO {

	template<typename T>
	class SharedFuture {
	public:
		template<typename Type>
		using Allocator = std::allocator<Type>;
	public:
		SharedFuture();
		~SharedFuture();
		SharedFuture(const SharedFuture &o);
		SharedFuture &operator=(const SharedFuture &o);
		SharedFuture(SharedFuture &&o) noexcept;
		SharedFuture &operator=(SharedFuture &&o) noexcept;
		void swap(SharedFuture &other);

	public:
		template<typename... Args>
		void Set(Args &&...args);

	public:
		[[nodiscard]] bool IsReady() const {
			return ready->test(std::memory_order_acquire);
		}

		std::shared_ptr<T> WaitUntilReady() {
			ready->wait(false, std::memory_order_acquire);
			return *sharedData;
		}

		[[nodiscard]] std::shared_ptr<T> Get();

	private:
		void Close();
		void Copy(const SharedFuture &o);

	private:
		T *data = nullptr;
		std::shared_ptr<T>* sharedData = nullptr;
		std::atomic_uint64_t *counter = nullptr;
		std::atomic_flag *ready = nullptr;
	};

	template<typename T>
	SharedFuture<T>::SharedFuture() {
		Allocator<T> allocatorT;
		data = allocatorT.allocate(1);
		sharedData = new std::shared_ptr<T>();
		counter = new std::atomic_uint64_t(1u);
		ready = new std::atomic_flag();
	}

	template<typename T>
	SharedFuture<T>::~SharedFuture() {
		Close();
	}

	template<typename T>
	SharedFuture<T>::SharedFuture(const SharedFuture &o) {
		Copy(o);
	}

	template<typename T>
	SharedFuture<T>& SharedFuture<T>::operator=(const SharedFuture &o) {
		Close();
		Copy(o);
		return *this;
	}

	template<typename T>
	SharedFuture<T>::SharedFuture(SharedFuture &&o) noexcept {
		swap(o);
	}

	template<typename T>
	SharedFuture<T> &SharedFuture<T>::operator=(SharedFuture &&o) noexcept {
		swap(o);
		return *this;
	}

	template<typename T>
	void SharedFuture<T>::swap(SharedFuture &other) {
		std::swap(data, other.data);
		std::swap(sharedData, other.sharedData);
		std::swap(counter, other.counter);
		std::swap(ready, other.ready);
	}

	template<typename T>
	template<typename... Args>
	void SharedFuture<T>::Set(Args &&...args) {
		std::construct_at(data, std::forward<Args>(args)...);
		sharedData->reset(data);
		data = nullptr;
		ready->test_and_set(std::memory_order_release);
		ready->notify_all();
	}

	template<typename T>
	std::shared_ptr<T> SharedFuture<T>::Get() {
		if (IsReady()) {
			return *sharedData;
		}
		return nullptr;
	}

	template<typename T>
	void SharedFuture<T>::Close() {
		const auto prev = counter->fetch_sub(1, std::memory_order_acq_rel);

		if (prev == 1) {
			if (data) {
				if (ready->test(std::memory_order_acquire)) {
					std::destroy_at(data);
				}

				Allocator<T> allocatorT;
				allocatorT.deallocate(data, 1);
			}

			delete sharedData;
			delete counter;
			delete ready;
		}

		data = nullptr;
		sharedData = nullptr;
		counter = nullptr;
		ready = nullptr;
	}

	template<typename T>
	void SharedFuture<T>::Copy(const SharedFuture &o) {
		data = o.data;
		sharedData = o.sharedData;
		counter = o.counter;
		ready = o.ready;
		counter->fetch_add(1, std::memory_order_release);
	}
} // namespace EID
