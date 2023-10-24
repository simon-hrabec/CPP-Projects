# Shared Pointer
If I need to have shared ownership in my C++ code, I can use a shared pointer. C++11 introduced `std::shared_ptr<T>` smart pointer, which is now part of the standard library. Therefore, there is no need to reimplement it (reinvent the wheel). Unless... you want to have some fun or see it as a learning experience and entertaining exercise. On this page I will show several iterations of shared pointer implementation from the most basic one to (ideally/eventually) a full version that supports all the features like the standard one. I intend to have this page also as a learning resource. 

## Versions
Building complexity one layer at a time.

### Version 1 - Barebone shared pointer
This version is the absolute minimal one. It has a constructor that allocates the control block with a reference counter. Since the shared pointer can be used by multiple threads at the same time, to avoid race conditions, the counter needs to be an atomic variable. Contrary to the constructor, the destructor decrements the counter and if it is the last one, it deallocates the owned object. The shared pointer can be copied via the copy constructor.

### Version 2 - Additional methods and `make_shared_pointer`
This version adds the option to have an empty shared pointer (using the `reset` method). It adds several other methods (`swap`, `operator bool`, `use_count`, empty constructor) and adds the function `make_shared_pointer` (mirroring `make_shared`), which can accept parameters for a constructor of `T` and handle the object construction (via parameter forwarding). Note that this function should normally allocate the object and control block together (to save 1 allocation and improve space locality), but this version does not do it - it will be addressed in future versions. However, the interface of the method will not change.

### Version 3 - Assignment operator, move assignment, move constructor
The assignment operator is always a bit more complicated than a copy constructor because we need to deal with the already existing object and the possibility of self-assignment. Moreover, a shared pointer can be empty. C++11 also introduced move semantics, hence we should cover them as well. All of those are just a bunch of ifs, checking the state properly, reassigning the data/block pointer, possibly decreasing the counter, and deallocating the block if necessary.

### Version 4 - `make_shared_pointer` with a single allocation
When creating the shared pointer with the `make_shared_pointer` helper function, instead of having two separate allocations - one for the control block and one for the object - we want to have only 1 such call and allocate both simultaneously. We therefore need to create another version of the control block that, besides the reference counter, will also contain the object itself. However, both shared pointers (one created with an object pointer and the second created with the `make_shared_pointer` function) are of the same type and are interoperable. To allow for such flexibility, we create 2 specializations of the block class and let them do the deallocations in their (virtual) destructor. We then have the `make_shared_pointer` to forward the arguments to the constructor of the `control_block_with_object`, which then creates the object.

### Version 5 - Aliasing constructor, comparisons and `get`
The standard shared pointer also has an "aliasing constructor". It allows to make a distinction between what the pointer is pointing to and the managed data. You can have a managed `struct` and the pointer pointing only to its member variable. This is fairly easy to achieve with another constructor. However, a problem occurs because now we need to mix together different instantiations of `shared_poointer`. So far the control block could be a nested class (to hide it as an implementation detail), but now we have an issue, because `shared_pointer<T1>::control_block` and `shared_pointer<T2>::control_block` are two different (and hence incompatible types). Therefore it is must be declared as a standalone class (see [SO](https://stackoverflow.com/questions/77342783/nested-class-of-a-template-class-interoperability-and-visibility/77343089#77343089)).
Implementation of comparison is fairly simple since C++20 and the introduction of the spaceship operator `<=>`. We could also simply support a comparison between different pointer types with the following code:

    template<std::three_way_comparable_with<T> U>
    std::compare_three_way_result_t<T, U> operator<=>(const shared_pointer<U>& other) const noexcept {
        if (has_value() && other.has_value()) {
            return *data <=> *other.data;
        }
        return (has_value()) <=> (other.has_value());
    }

However, the `std::shared_ptr` does not allow for mixing pointer types in comparisons, therefore I decided to implement a more restricted version where only pointers of the same type can be compared. Finally, the `get()` method is the same as `operator->`.
