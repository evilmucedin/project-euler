#pragma once

#include <lib/header.h>

class BigInteger {
   public:
    BigInteger() { data_.emplace_back(0); }

    BigInteger(u32 init) {
        data_.emplace_back(init);
        norm();
    }

    u32 toU32() const {
        u32 result = 0;
        for (int i = data_.size() - 1; i >= 0; --i) {
            result = result * MOD + data_[i];
        }
        return result;
    }

    operator int() const { return toU32(); }

    BigInteger& operator*=(u32 value) {
        for (auto& d: data_) {
            d *= value;
        }
        norm();
        return *this;
    }

    BigInteger& operator*=(int value) {
        for (auto& d: data_) {
            d *= value;
        }
        norm();
        return *this;
    }

    BigInteger operator*(u32 value) const {
        BigInteger result = *this;
        result *= value;
        return result;
    }

    BigInteger operator*(int value) const {
        BigInteger result = *this;
        result *= value;
        return result;
    }

    BigInteger& operator+=(u32 value) {
        data_[0] += value;
        norm();
        return *this;
    }

    BigInteger& operator+=(int value) {
        data_[0] += value;
        norm();
        return *this;
    }

    BigInteger operator+(u32 value) const {
        BigInteger result = *this;
        result += value;
        return result;
    }

    BigInteger operator+(int value) const {
        BigInteger result = *this;
        result += value;
        return result;
    }

    BigInteger& operator-=(const BigInteger& rhs) {
        while (data_.size() < rhs.data_.size()) {
            data_.emplace_back(0);
        }
        for (int i = 0; i < rhs.data_.size(); ++i) {
            data_[i] -= rhs.data_[i];
        }
        norm();
        return *this;
    }

    BigInteger operator-(const BigInteger& value) const {
        BigInteger result = *this;
        result -= value;
        return result;
    }

    bool operator<(const BigInteger& rhs) const {
        if (data_.size() == rhs.data_.size()) {
            int i = data_.size() - 1;
            while (i >= 0 && data_[i] == rhs.data_[i]) {
                --i;
            }
            if (i >= 0) {
                return data_[i] < rhs.data_[i];
            }
            return false;
        }
        return data_.size() < rhs.data_.size();
    }

   private:
    void norm() {
        u32 carry = 0;
        for (size_t i = 0; i < data_.size(); ++i) {
            u32 value = data_[i] + carry;
            data_[i] = value % MOD;
            carry = value / MOD;
        }
        if (carry) {
            data_.emplace_back(carry);
        }
        while ((data_.size() > 1) && (data_.back() == 0)) {
            data_.pop_back();
        }
    }

    static constexpr u32 MOD = 10000;

    vector<u32> data_;
};
