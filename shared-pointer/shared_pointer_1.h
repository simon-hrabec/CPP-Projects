#include <atomic>

template <typename T>
class shared_pointer {
private:
    struct control_block {
        control_block() = default;

        std::atomic<std::size_t> reference_count{1};
    };

public:
    shared_pointer(T* data) : block(new control_block()), data(data) {
    }

    shared_pointer(const shared_pointer& other) noexcept {
        block = other.block;
        block->reference_count.fetch_add(1);
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
    control_block* block;
    T* data;
};
