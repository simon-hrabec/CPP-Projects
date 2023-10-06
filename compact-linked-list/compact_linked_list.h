#include <cstddef>
#include <cmath>
#include <bit>
#include <type_traits>
#include <cstdint>

template <size_t bit_count>
using smallest_usigned_type_with_bits = std::conditional_t<bit_count <= 8, uint8_t, std::conditional_t<bit_count <= 16, uint16_t, std::conditional_t<bit_count <= 32, uint32_t, uint64_t>>>;

template<size_t value_limit, size_t max_size>
class compact_forward_list {
private:
    constexpr static size_t value_bits = ceil(log2(value_limit));
    constexpr static size_t count_bits = ceil(log2(max_size+1));
    constexpr static size_t bit_size = max_size*(value_bits+count_bits)+2*count_bits;
    constexpr static size_t byte_size = (bit_size+7)/8;
    std::byte *data;

public:
    using index_type = smallest_usigned_type_with_bits<count_bits>;
    using value_type = smallest_usigned_type_with_bits<value_bits>;

    template <typename T>
    class iterator_base;

    using iterator = iterator_base<compact_forward_list>;
    using const_iterator = iterator_base<const compact_forward_list>;

    class proxy;
    class const_proxy;

    template <typename T>
    class iterator_base {
        friend class compact_forward_list;
        index_type index;
        T& list;

    public:
        using difference_type = compact_forward_list<value_limit, max_size>::value_type;
        using value_type = compact_forward_list<value_limit, max_size>::value_type;
        using pointer = const compact_forward_list<value_limit, max_size>::value_type*;
        using reference = const compact_forward_list<value_limit, max_size>::value_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator_base(index_type index, T& list) : index(index), list(list) {
        }

        auto operator*(){
            if constexpr (std::is_const_v<T>) {
                return const_proxy(list, index);    
            } else {
                return proxy(list, index);
            }
            
        }

        iterator_base& operator++() {
            index_type next_index = list.load_index(index);
            index = next_index;
            return *this;
        }

        bool operator==(const iterator_base& other) {
            return index == other.index;
        }
    };

    // using value_type = value_type_private;

    class proxy {
        template<typename U>
        friend class iterator_base;
        
        compact_forward_list& list;
        index_type position;
        proxy(compact_forward_list& list, index_type position) : list(list), position(position) {}
    public:
        operator value_type () const {
            return list.load_value(position);
        }

        proxy& operator= (const value_type value) {
            list.store_value(position, value);
            return *this;
        }
        proxy& operator= (const proxy& other) {
            list.store_value(position, list.load_value(other.position));
            return *this;
        }

        friend void swap(compact_forward_list<value_limit, max_size>::proxy first, compact_forward_list<value_limit, max_size>::proxy second) {
            const value_type first_value = first;
            first = second;
            second = first_value;
        }
    };

    class const_proxy {
        template<typename U>
        friend class iterator_base;

        const compact_forward_list& list;
        index_type position;
        const_proxy(const compact_forward_list& list, index_type position) : list(list), position(position) {}
    public:
        operator value_type () const {
            return list.load_value(position);
        }
    };

    compact_forward_list() {
        data = new std::byte[byte_size];
        clear();
    }

    ~compact_forward_list() {
        delete [] data;
    }

    void clear(){
        store_free_index(1);
        store_first_index(0);
        store_index(1, 0);
    }

    bool operator==(const compact_forward_list& other) const {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    iterator insert_after(const iterator position, const value_type value) {
        const index_type new_element = store_in_free_node(value);
        const index_type next_element = load_index(position.index);
        store_index(position.index, new_element);
        store_index(new_element, next_element);
        return iterator(new_element, *this);
    }

    void push_front(const value_type value) {
        const index_type new_element = store_in_free_node(value);
        store_index(new_element, load_first_index());
        store_first_index(new_element); 
    }

    iterator erase_after(const iterator position) {
        const index_type next_element = load_index(position.index);
        const index_type next_next_element = load_index(next_element);
        const index_type first_free = load_free_index();

        store_index(next_element, first_free);
        store_free_index(next_element);
        store_index(position.index, next_next_element);

        return iterator(next_next_element, position.list);
    }

    void pop_front() {
        const index_type first_free = load_free_index();
        const index_type first_element = load_first_index();
        const index_type second_element = load_index(first_element);

        store_first_index(second_element);
        store_index(first_element, first_free);
        store_free_index(first_element);
    }

    // // Could have only one begin function with C++23 deducing this (P0847R7), but GCC does not support that yet - https://gcc.gnu.org/projects/cxx-status.html
    // template <typename Self>
    // auto&& begin(this Self&& self) {
    //  return iterator_base<std::conditional_t<std::is_const_v<Self>, const value_type, value_type>(load_first_index(), *this);
    // }

    compact_forward_list::iterator begin() {
        return iterator(load_first_index(), *this);
    }

    compact_forward_list::const_iterator begin() const {
        return const_iterator(load_first_index(), *this);
    }

    compact_forward_list::iterator end() {
        return iterator(0, *this);
    }

    compact_forward_list::const_iterator end() const {
        return const_iterator(0, *this);
    }

    void push_back(const value_type value) {
        index_type new_element = store_in_free_node(value);
        store_index(new_element, 0);

        index_type iter = load_first_index();
        if (iter == 0) {
            store_first_index(new_element);
        } else {
            index_type next = load_index(iter);
            while(next != 0) {
                iter = next;
                next = load_index(iter);
            }
            store_index(iter, new_element);
        }
    }

private:
    index_type store_in_free_node(const value_type value) {
        const index_type first_free = load_free_index();
        index_type next_free = load_index(first_free);
        if (next_free == 0) {
            next_free = first_free+1;
            store_index(next_free, 0);
        }
        store_free_index(next_free);

        store_value(first_free, value);
        return first_free;
    }

    void store_index(const index_type index, const index_type value) {
        store<count_bits, index_type>(count_bits+index*(value_bits+count_bits), value);
    }

    void store_free_index(const index_type value) {
        store<count_bits, index_type>(0, value);
    }

    void store_first_index(const index_type value) {
        store<count_bits, index_type>(count_bits, value);
    }

    void store_value(const index_type position, const value_type value) {
        store<value_bits, value_type>(2*count_bits + (position-1)*(value_bits+count_bits), value);
    }

    template<size_t bit_count, class T>
    void store(const size_t bit_position, const T value) {
        for(size_t i = 0; i < bit_count; i++) {
            const int byte_position = (bit_position+i)/8;
            const std::byte bit = std::byte{1}<<((bit_position+i)%8);
            std::byte& target = data[byte_position];
            if (value & (1<<i)) {
                target |= bit;
            } else {
                target &= ~bit;
            }
        }
    }

    index_type load_index(const index_type index) const {
        return load<count_bits, index_type>(count_bits+index*(value_bits+count_bits));
    }

    index_type load_free_index() const {
        return load<count_bits, index_type>(0);
    }

    index_type load_first_index() const {
        return load<count_bits, index_type>(count_bits);
    }

    value_type load_value(const index_type index) const {
        return load<value_bits, value_type>(2*count_bits + (index-1)*(value_bits+count_bits));
    }

    template<size_t bit_count, class T>
    T load(const size_t bit_position) const {
        T result = 0;
        for(size_t i = 0; i < bit_count; i++) {
            const int byte_position = (bit_position+i)/8;
            const std::byte bit = std::byte{1}<<(bit_position+i)%8;
            std::byte& target = data[byte_position];
            if ((target & bit) != std::byte{0}) {
                result |= T{1}<<i;
            }
        }

        return result;
    }
};
