# Space efficient, bit-packed singly linked list
The `compact_linked_list` (CLL) data structure is a variation of a singly linked list. It resembles `std::forward_list` with its basic API (insertions, deletions, iteration), but differs in a fay ways:
 - CLL can only store unsigned integral values between 0 and a specified (by a template parameter) value (up to `2^64-1`.
 - CLL has a limit on the number of elements, which is supplied as a template parameter.
 - Knowing the value and size cap, it allocates all storage upfront, therefore no memory allocations/deallocations are performed during insertions and deletions.
 - All data are stored using only the necessary number of bits, packed tightly so as not to waste any bits.

## Current features:
 - Data can be accessed via iterators, same as `std::forward_list`.
 - Dereferencing an iterator does not return a (const) reference to the value, but rather a proxy object that can be used to modify/get the value. It ss easy to work with due to implicit conversions. Conceptually the same things as in `std::vector<bool>`.
 - Pointers are implemented as indices, that can be easily converted into bit offset.
 - Requires less memory

## Memory use
One of the main features of this implementation is the efficient use of memory. For a regular `std::forward_list`, allocating a new node on the heap has a cost, that consists of several things:
 1. The value itself + the pointer to the next element
 2. Alignment of those types
 3. Alignment of heap-allocated object (determined by `std::max_align_t` and `__STDCPP_DEFAULT_NEW_ALIGNMENT__`)
 4. Bookkeeping data of heap-allocated objects

On my system, any heap allocated object will take at least 16 bytes and the bookkeeping data takes 16 bytes as well, making it at least 32 bytes per node. Whereas CLL can take even a few bites, when the constraints are small enough, and 16 bytes at most.

## Future work
Currently, data is extracted from the bit field bit by bit. This is not efficient as it could be done byte by byte. However, this requires properly checking all edge cases and has more complex logic, especially when treating the first and last byte (of which only a part of the bits might be used).

Also, the current API is limited. Some of the member functions are not necessary since CLL supports only primitive types (the unsigned value is determined by the number of bits), but some are missing and will be added later.

## Remarks
As rarely as you resort even to a regular linked list, CLL does not have much practical use in real scenarios. Its use case is extremely limited. However, if you manage to get into a situation, in which you have 1. a hard cap on the number of elements, 2. small values, and 3. a preference for memory footprint over the speed of access, then this might be a tool for you. Also, this data structure might be of use in embedded or real-time systems, where doing memory allocations after the startup of a program is not allowed.
