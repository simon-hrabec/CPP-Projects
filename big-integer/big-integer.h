#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

class big_intiger {
public:
    static constexpr uint32_t max_size = 1'000'000'000;
    std::vector<uint32_t> data;
    
    static void shrink(std::vector<uint32_t>& vec) {
        for(int i = vec.size()-1; i >= 1; i--){
            if (vec[i] == 0) {
                vec.pop_back();
            } else {
                break;
            }
        }
    }
    
public:
    big_intiger() : data({0}) {
    }

    big_intiger(uint64_t num) {
        data.clear();
        while(num){
            uint32_t chunk = num % max_size;
            data.push_back(chunk);
            num /= max_size;
        }
    }
    
    big_intiger(std::string str) {
        data.clear();
        const int len = (int)str.size();
        while(str.size() > 0){
            const int len = (int)str.size();
            std::string sub = str.substr(std::max(len-9, 0), std::max(len, len-9));
            uint32_t num = std::stoi(sub, nullptr);
            str.resize(std::max(0, len-9));
            data.push_back(num);
        }
    }
    
    big_intiger(const std::vector<uint32_t> &vec) : data(vec) {
        shrink(data);
    }
    
    void multiply(const big_intiger &val){
        const int len1 = std::min(data.size(), val.data.size());
        const int len2 = std::max(data.size(), val.data.size());
        const bool switchdata = data.size() > val.data.size();
        const std::vector<uint32_t> &a1 = (switchdata ? val.data : data);
        const std::vector<uint32_t> &a2 = (switchdata ? data : val.data); 

        std::vector<uint32_t> res(2*std::max(len1, len2)+1, uint32_t(0));
        for(int i = 0; i < len1; i++){
            for(int j = 0; j < len2; j++){
                const int pos = i+j;
                uint64_t multres = a1[i] * uint64_t(a2[j]);
                res[pos] += multres % max_size;
                res[pos+1] += multres/max_size + res[pos]/max_size;
                res[pos] %= max_size;
            }
        }
        shrink(res);
        data = std::move(res);
    }
    
    void add(const big_intiger &val){
        const int len1 = std::min(data.size(), val.data.size());
        const int len2 = std::max(data.size(), val.data.size());
        const bool switchdata = data.size() > val.data.size();
        const std::vector<uint32_t> &a1 = (switchdata ? val.data : data);
        const std::vector<uint32_t> &a2 = (switchdata ? data : val.data); 

        std::vector<uint32_t> res(std::max(len1, len2)+1, uint32_t(0));
        for(int i = 0; i < len2; i++){
            res[i] += a2[i] + (i < len1 ? a1[i] : 0);
            res[i+1] = res[i]/max_size;
            res[i] %= max_size;
        }
        shrink(res);
        data = std::move(res);
    }

    void power(uint32_t exp) {
        big_intiger res(1);
        while(exp){
            if (exp & 1){
                res.multiply(*this);
            }
            exp >>= 1;
            multiply(*this);
        }
        data = res.data;
    }
    
    std::string tostr() const {
        std::stringstream ss;
        ss << std::setfill('0');
        ss << data.back();
        for(int i = (int)data.size()-2; i >= 0; i--){
            ss << std::setw(9) << data[i];
        }
        return ss.str();
    }
    
    void print() const {
        std::cout << tostr();
    }
    
    void printl() const {
        std::cout << tostr() << std::endl;
    } 

    friend big_intiger operator+(const big_intiger &val1, const big_intiger &val2) {
        big_intiger copy = val1;
        copy.add(val2);
        return copy;
    }

    friend big_intiger operator*(const big_intiger &val1, const big_intiger &val2) {
        big_intiger copy = val1;
        copy.multiply(val2);
        return copy;
    }

    uint32_t digit_sum() const noexcept {
        uint32_t sum = 0;
        for(uint32_t num : data){
            while (num){
                sum += num % 10;
                num /= 10;
            }
        }
        return sum;
    }
};
