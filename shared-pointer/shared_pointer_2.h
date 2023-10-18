#include <atomic>

template <typename T>
class shared_pointer {
private:
	struct controll_block {
		controll_block() : reference_count(1) {
		}

		std::atomic<std::size_t> reference_count;
	};

public:
	shared_pointer() noexcept : block(nullptr), data(nullptr) {
	}

	shared_pointer(T* data) : block(new controll_block()), data(data) {
	}

	shared_pointer(const shared_pointer& other) noexcept {
		block = other.block;
		block->reference_count++;
		data = other.data;
	}

	~shared_pointer() {
		release_ownership();
	}

	T& operator*() const noexcept {
		return *data;
	}

	T* operator->() const noexcept {
		return data;
	}

	long use_count() const noexcept {
		if (block == nullptr) {
			return 0;
		} else {
			return block->reference_count;	
		}
	}

	explicit operator bool() const noexcept {
		return data != nullptr;
	}

	void reset() noexcept {
		block = nullptr;
		data = nullptr;
	}

	void reset(T *new_data) {
		release_ownership();
		block = new controll_block();
		data = new_data;
	}

	void swap(shared_pointer &other) noexcept {
		std::swap(block, other.block);
		std::swap(data, other.data);
	}

private:
	void release_ownership() {
		if (block->reference_count.fetch_sub(1) == 1) {
			delete block;
			delete data;
		}
	}

	controll_block *block;
	T *data;
};

template <typename T, typename... Args>
shared_pointer<T> make_shared_pointer(Args&&... args) {
	return shared_pointer<T>(new T(std::forward<Args>(args)...));
}
