#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

class union_find {
    std::vector<int> roots;
    std::vector<int> sizes;
    int groups_count;

public:  
    union_find(const int max_count) : roots(max_count), sizes(max_count), groups_count(max_count) {
        std::fill(std::begin(roots), std::end(roots), 1);
        std::iota(std::begin(roots), std::end(roots), 0);
    }
 
    int find(int idx) noexcept {
        //Find root index
        int root_idx = idx;
        while(root_idx != roots[root_idx]) {
            root_idx = roots[root_idx];
        }

        //Reconnect all elements to root
        while (idx != root_idx) {
            const int next_idx = roots[idx];
            roots[idx] = root_idx;
            idx = next_idx;
        }

        return root_idx;
    }
 
    bool merge(const int first, const int second) noexcept {
        int first_idx = find(first);
        int second_idx = find(second);

        if (first_idx == second_idx) {
            return true;
        }

        groups_count--;

        if(sizes[first_idx] > sizes[second_idx]){
            std::swap(first_idx, second_idx);
        }

        roots[first_idx] = second_idx;
        sizes[second_idx] += sizes[first_idx];

        return false;
    }

    int get_groups_count() const noexcept {
        return groups_count;
    }
};
