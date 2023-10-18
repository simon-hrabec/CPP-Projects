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
	shared_pointer(T* data_pointer) : block(new controll_block()), data(data_pointer) {
	}

	shared_pointer(const shared_pointer& other) noexcept {
		block = other.block;
		block->reference_count++;
		data = other.data;
	}

	~shared_pointer() {
		if (block->reference_count.fetch_sub(1) == 1) {
			delete block;
			delete data;
		}
	}

	T& operator*() const noexcept {
		return *data;
	}

	T* operator->() const noexcept {
		return data;
	}

private:
	controll_block *block;
	T *data;
};
