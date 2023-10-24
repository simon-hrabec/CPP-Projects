#include <atomic>

template <typename T>
class shared_pointer {
private:
    struct control_block {
        control_block() = default;

        std::atomic<std::size_t> reference_count{1};
    };

public:
    shared_pointer() noexcept : block(nullptr), data(nullptr) {
    }

    shared_pointer(T *data) : block(new control_block()), data(data) {
    }

    shared_pointer(const shared_pointer& other) noexcept {
        block = other.block;
        block->reference_count.fetch_add(1);
        data = other.data;
    }

    ~shared_pointer() {
        check_and_release_ownership();
    }

    T& operator*() const noexcept {
        return *data;
    }

    T* operator->() const noexcept {
        return data;
    }

    long use_count() const noexcept {
        if (has_value()) {
            return block->reference_count;
        } else {
            return 0;
        }
    }

    explicit operator bool() const noexcept {
        return has_value();
    }

    void reset() noexcept {
        check_and_release_ownership();
        block = nullptr;
        data = nullptr;
    }

    void reset(T* new_data) {
        check_and_release_ownership();
        block = new control_block();
        data = new_data;
    }

    void swap(shared_pointer& other) noexcept {
        std::swap(block, other.block);
        std::swap(data, other.data);
    }

private:
    bool has_value() const noexcept {
        return block != nullptr;
    }

    void check_and_release_ownership() noexcept {
        if (has_value() && block->reference_count.fetch_sub(1) == 1) {
            delete block;
            delete data;
        }
    }

    control_block* block;
    T* data;
};

template <typename T, typename... Args>
inline shared_pointer<T> make_shared_pointer(Args&&... args) {
    return shared_pointer<T>(new T(std::forward<Args>(args)...));
}
