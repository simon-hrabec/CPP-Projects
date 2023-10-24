#include <atomic>

template <typename T>
class shared_pointer {
private:
    struct control_block {
    protected:
        control_block() = default;
    public:
        virtual ~control_block() = default;

        std::atomic<std::size_t> reference_count{1};
    };

    struct control_block_standalone : public control_block {
        control_block_standalone(T* data) noexcept : data(data) {
        }

        virtual ~control_block_standalone(){
            delete data;
        };

        T* data;
    };

    struct control_block_with_object : public control_block {
        template <typename... Args>
        control_block_with_object(Args&&... args) : data(std::forward<Args>(args)...) {
        }

        virtual ~control_block_with_object() = default;

        T data;
    };

    shared_pointer(control_block_with_object* block) noexcept : block(block), data(&block->data) {
    }

public:
    shared_pointer() noexcept : block(nullptr), data(nullptr) {
    }

    shared_pointer(T* data) : block(new control_block_standalone(data)), data(data) {
    }

    shared_pointer(const shared_pointer& other) noexcept {
        block = other.block;
        data = other.data;
        block->reference_count.fetch_add(1);
    }

    shared_pointer(shared_pointer&& other) noexcept {
        block = other.block;
        data = other.data;
        other.block = nullptr;
        other.data = nullptr;
    }

    ~shared_pointer() {
        check_and_release_ownership();
    }

    shared_pointer& operator=(const shared_pointer& other) noexcept {
        if (block != other.block) {
            check_and_release_ownership();
            block = other.block;
            data = other.data;
            block->reference_count.fetch_add(1);
        }

        return *this;
    }

    shared_pointer& operator=(shared_pointer&& other) noexcept {
        if (block != other.block) {
            check_and_release_ownership();
        }

        block = other.block;
        data = other.data;
        other.block = nullptr;
        other.data = nullptr;

        return *this;
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

    template <typename U, typename... Args>
    friend shared_pointer<U> make_shared_pointer(Args&&... args);

private:
    bool has_value() const noexcept {
        return block != nullptr;
    }

    void check_and_release_ownership() {
        if (has_value() && block->reference_count.fetch_sub(1) == 1) {
            delete block;
        }
    }

    control_block* block;
    T* data;
};

template <typename T, typename... Args>
inline shared_pointer<T> make_shared_pointer(Args&&... args) {
    return shared_pointer<T>(new typename shared_pointer<T>::control_block_with_object(std::forward<Args>(args)...));
}
