#include "packed_list.h"
#include <iostream>
#include <forward_list>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <random>
#include <ranges>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

void print(const auto &container) {
	for(const auto value : container) {
		std::cout << int(value) << " ";
	}
	std::cout << std::endl;
}

template<class T>
void printrange(T from, const T to) {
	while (from != to) {
		std::cout << int(*from) << " ";
		++from;
	}
	std::cout << std::endl;
}


template<class ForwardIt>
void quicksort(ForwardIt first, ForwardIt last)
{
	std::cout << "before: ";
	printrange(first, last);
    if (first == last)
        return;
 
    auto pivot = *std::next(first, std::distance(first, last) / 2);
    std::cout << "pivot: " << int(pivot) << std::endl;
    auto middle1 = std::partition(first, last, [pivot](const auto& em)
    {
        return em < pivot;
    });
    std::cout << "partition1: ";
	printrange(first, last);
    auto middle2 = std::partition(middle1, last, [pivot](const auto& em)
    {
        return !(pivot < em);
    });
    std::cout << "M1, M2: " << int(*middle1) << ", " << int(*middle2) << std::endl;
	std::cout << "partition: ";
	printrange(first, last);
 
    quicksort(first, middle1);
    quicksort(middle2, last);

	std::cout << "after: ";
	printrange(first, last);
}

template<class ForwardIt>
void xxx(ForwardIt first, ForwardIt last) {
    std::cout << "before: "; printrange(first, last);
    int pivot = *std::next(first, std::distance(first, last) / 2);
    std::cout << "pivot: " << int(pivot) << std::endl;
    auto middle1 = std::partition(first, last, [pivot](const auto& em){ return em < pivot; });
    std::cout <<  int(*middle1) << std::endl;
    std::cout << "partition1: "; printrange(middle1, last);
    auto middle2 = std::partition(middle1, last, [pivot](const auto& em){
      std::cout << int(pivot) << " " << int(em) << " " << (!(pivot < em)) << std::endl;
      return !(pivot < em);
    });
    std::cout << "M1, M2: " << int(*middle1) << ", " << int(*middle2) << std::endl;
    std::cout << "partition: ";
    printrange(first, last);
}


void list_push_back(auto& slist, const auto& value) {
	auto before_end = slist.before_begin();
	for ([[maybe_unused]]auto& _ : slist) {
		++before_end;
	}
	slist.insert_after(before_end, value);
}

template<typename T, size_t value_limit, size_t max_size>
void push_back_both(std::forward_list<T>& slist, compact_forward_list<value_limit, max_size> &clist, const T& value) {
	auto before_end = slist.before_begin();
	for ([[maybe_unused]]auto& _ : slist) {
		++before_end;
	}
	slist.insert_after(before_end, value);

	clist.push_back(value);
}

template<typename T, size_t value_limit, size_t max_size>
void push_front_both(std::forward_list<T>& slist, compact_forward_list<value_limit, max_size> &clist, const T& value) {
	slist.push_front(value);
	clist.push_front(value);
}

template<typename T, size_t value_limit, size_t max_size>
bool are_lists_equal(std::forward_list<T>& slist, compact_forward_list<value_limit, max_size> &clist) {
	return std::equal(slist.begin(), slist.end(), clist.begin(), clist.end());
}

template<typename T, size_t value_limit, size_t max_size>
void pop_front_both(std::forward_list<T>& slist, compact_forward_list<value_limit, max_size> &clist) {
	slist.pop_front();
	clist.pop_front();
}

std::vector<int> shuffled_sequence(size_t length) {
  std::vector<int> numbers(length);
  std::iota(std::begin(numbers), std::end(numbers), 0);
  std::mt19937 gen;
  std::shuffle(numbers.begin(), numbers.end(), gen);
  return numbers;
}

void insert_all(auto& list, const auto& numbers) {
	for(const int num : std::ranges::reverse_view(numbers)) {
		list.push_front(num);
	}
}

// TEST(LinkedListTest, CanHoldValue) {
//   compact_forward_list<42, 6> l;
//   l.push_back(4);
//   ASSERT_EQ(*l.begin(), 4);
// }

// TEST(LinkedList, CanHoldMaximumValues) {
// 	constexpr size_t max_values = 50;
// 	compact_forward_list<max_values, 42> l;
// 	const std::vector<int> numbers = shuffled_sequence(max_values);

// 	for(const int num : std::ranges::reverse_view(numbers)) {
// 		l.push_front(num);
// 	}

// 	ASSERT_THAT(l, ::testing::ElementsAreArray(numbers));
// }


// TEST(LinkedListTest, SameAsForwardList) {
//   compact_forward_list<42, 42> l;
//   std::forward_list<int> fl;

//   std::vector<int> numbers(6) ; // vector with 100 ints.
//   std::iota(std::begin(numbers), std::end(numbers), 0); // Fill with 0, 1, ..., 99.
//   std::mt19937 gen;
//   std::shuffle(numbers.begin(), numbers.end(), gen);
//   for(const int num : numbers) {
//   	l.push_front(num);
//   	fl.push_front(num);
//   }
//   ASSERT_THAT(fl, ::testing::ElementsAreArray(l));
//   print(l);
//   print(fl);
//   quicksort(l.begin(), l.end());
//   std::cout << "SORT SECOND" << std::endl;
//   quicksort(fl.begin(), fl.end());
//   // fl.sort();
//   print(l);
//   print(fl);

//   ASSERT_THAT(fl, ::testing::ElementsAreArray(l));
// }

TEST(Partition, failing) {
  std::vector<int> arr({4, 5, 1, 3, 2});
  std::reverse(arr.begin(), arr.end());

  std::forward_list<int> fl;
  compact_forward_list<42, 42> l;

  for(const int num : arr) {
    fl.push_front(num);
    l.push_front(num);
  }
  print(fl);
  print(l);

  std::cout << std::endl << "NORMAL LIST:" << std::endl;
  xxx(fl.begin(), fl.end());
  std::cout << std::endl << "MY LIST:" << std::endl;
  xxx(l.begin(), l.end());

  std::vector<bool> arrbool(10);
  arrbool[0] = 1;
  auto it = arrbool.begin();
  auto elem = *it;
  // elem = std::string{"sadfs"};
  std::cout << elem << std::endl;
  arrbool[0] = 0;
  std::cout << elem << std::endl;
}


// template <typename T>
// class LinkedListCapacity : public testing::Test {};

// template<size_t val>
// struct Wrapper {
// 	static constexpr size_t value = val;
// };

// using TestingSizes = ::testing::Types<Wrapper<8>, Wrapper<16>, Wrapper<32>, Wrapper<64>, Wrapper<128>, Wrapper<256>, Wrapper<512>, Wrapper<1024>, Wrapper<2048>, Wrapper<65536>>;
// TYPED_TEST_SUITE(LinkedListCapacity, TestingSizes);

// TYPED_TEST(LinkedListCapacity, CanHoldUpToPowerOf2) {
// 	const size_t size = TypeParam::value;
// 	compact_forward_list<size, size> list;
// 	const std::vector<int> numbers = shuffled_sequence(size-1);
// 	// print(numbers);
// 	for(const int num : std::ranges::reverse_view(numbers)) {
// 		list.push_front(num);
// 	}

// 	ASSERT_THAT(list, ::testing::ElementsAreArray(numbers));
// }

// TYPED_TEST(LinkedListCapacity, CanHoldPowerOf2) {
// 	const size_t size = TypeParam::value+1;
// 	compact_forward_list<size, size> list;
// 	const std::vector<int> numbers = shuffled_sequence(size-1);
// 	// print(numbers);
// 	for(const int num : std::ranges::reverse_view(numbers)) {
// 		list.push_front(num);
// 	}

// 	ASSERT_THAT(list, ::testing::ElementsAreArray(numbers));
// }


// int main(int argc, char const *argv[]) {
// 	compact_forward_list<42, 6> l;
// 	std::forward_list<int> fl;

// 	push_back_both(fl, l, 7);
// 	push_back_both(fl, l, 11);
// 	push_back_both(fl, l, 22);
// 	push_back_both(fl, l, 3);
	
// 	print(l);
// 	print(fl);
// 	assert(are_lists_equal(fl, l));
// 	pop_front_both(fl, l);
// 	pop_front_both(fl, l);
// 	pop_front_both(fl, l);
// 	pop_front_both(fl, l);

// 	push_back_both(fl, l, 5);
// 	push_back_both(fl, l, 6);
// 	push_back_both(fl, l, 7);
// 	print(l);
// 	print(fl);
// 	assert(are_lists_equal(fl, l));


// 	l.printbegnum();
// 	push_back_both(fl, l, 20);
// 	push_back_both(fl, l, 40);
// 	push_back_both(fl, l, 1);
// 	print(l);
// 	print(fl);
// 	l.printbegnum();
// 	assert(are_lists_equal(fl, l));

// 	quicksort(l.begin(), l.end());
// 	fl.sort();
// 	print(l);
// 	print(fl);
// 	assert(are_lists_equal(fl, l));

// 	return 0;
// }
