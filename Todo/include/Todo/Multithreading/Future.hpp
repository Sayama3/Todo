//
// Created by ianpo on 07/01/2026.
//

#pragma once


namespace TODO {

	template<typename T>
	class Future {
		template<typename Type>
		using Allocator = std::allocator<Type>;

	public:
		Future();
		~Future();
		Future(const Future &) = delete;
		Future &operator=(const Future &) = delete;
		Future(Future &&o) noexcept;
		Future &operator=(Future &&o) noexcept;
		void swap(Future &o) noexcept;

	public:
		bool IsReady() const;

		std::unique_ptr<T> WaitAndGet();

		std::unique_ptr<T> Get();

	public:
		template<typename... Args>
		void Set(Args &&...args);
	private:
		std::unique_ptr<T> PrivateGet();
	private:
		T *data = nullptr;
		std::atomic_flag *ready = nullptr;
		std::atomic_uint8_t *counter = nullptr;
	};

	template<typename T>
	Future<T>::Future() {
		Allocator<T> alloc;
		data = alloc.allocate(1);
		counter = new std::atomic_uint8_t(1u);
		ready = new std::atomic_flag();
	}

	template<typename T>
	Future<T>::~Future() {
		if (counter) {
			const auto count = counter->fetch_sub(1, std::memory_order_acq_rel);
			if (count == 1) {
				if (data) {
					if (ready->test(std::memory_order_acquire)) {
						std::destroy_at(data);
					}

					Allocator<T> alloc;
					alloc.deallocate(data, 1);
				}

				delete ready;
				delete counter;
			}
		}

		data = nullptr;
		ready = nullptr;
		counter = nullptr;
	}
	template<typename T>
	Future<T>::Future(Future &&o) noexcept {
		swap(o);
	}
	template<typename T>
	Future<T> &Future<T>::operator=(Future &&o) noexcept {
		swap(o);
		return *this;
	}
	template<typename T>
	void Future<T>::swap(Future &o) noexcept {
		std::swap(data, o.data);
		std::swap(counter, o.counter);
		std::swap(ready, o.ready);
	}
	template<typename T>
	bool Future<T>::IsReady() const {
		return ready->test(std::memory_order_acquire);
	}
	template<typename T>
	std::unique_ptr<T> Future<T>::WaitAndGet() {
		ready->wait(false, std::memory_order_acquire);
		return PrivateGet();
	}

	template<typename T>
	std::unique_ptr<T> Future<T>::Get() {
		if (IsReady()) {
			return PrivateGet();
		}
		return nullptr;
	}

	template<typename T>
	template<typename... Args>
	void Future<T>::Set(Args &&...args) {
		std::construct_at(data, std::forward<Args>(args)...);
		ready->test_and_set(std::memory_order_release);
		ready->notify_all();
	}

	template<typename T>
	std::unique_ptr<T> Future<T>::PrivateGet() {
		std::unique_ptr<T> data_{};
		data_.reset(data);
		data = nullptr;
		return data_;
	}
} // namespace EID
