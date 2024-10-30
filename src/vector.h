#pragma once

#include <new>
#include <type_traits>
#include <memory>
#include <algorithm>

template <typename T>
class Vector {
public:
    Vector() : size_{0}, cap_{0}, data_{nullptr} {
    }

    explicit Vector(size_t size, const T& val = T{}) : Vector() {
        try {
            Reserve(size);
            for (size_t i = 0; i < size; ++i) {
                PushBack(val);
            }
        } catch (...) {
            Clear();
            delete[] reinterpret_cast<char*>(data_);
            data_ = nullptr;
            throw;
        }
    }

    template <typename Iterator>
        requires(!std::is_integral_v<Iterator>)
    Vector(Iterator begin, Iterator end) : Vector() {
        try {
            Reserve(std::distance(begin, end));
            for (auto it = begin; it != end; ++it) {
                PushBack(*it);
            }
        } catch (...) {
            Clear();
            delete[] reinterpret_cast<char*>(data_);
            data_ = nullptr;
            throw;
        }
    }

    Vector(const std::initializer_list<T>& data) : Vector(data.begin(), data.end()) {
    }

    Vector(const Vector& other) : Vector(other.begin(), other.end()) {
    }

    Vector(Vector&& other) noexcept : size_(other.size_), cap_(other.cap_), data_(other.data_) {
        other.size_ = other.cap_ = 0;
        other.data_ = nullptr;
    }

    ~Vector() {
        Clear();
        delete[] reinterpret_cast<char*>(data_);
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        Vector tmp = other;
        Swap(tmp);
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        Vector tmp = std::move(other);
        Swap(tmp);
        return *this;
    }

    void Assign(size_t n, const T& val) {
        Vector tmp(n, val);
        Swap(tmp);
    }

    T& operator[](size_t i) {
        assert(i < size_ && "Index is out of range");
        return data_[i];
    }

    const T& operator[](size_t i) const {
        assert(i < size_ && "Index is out of range");
        return data_[i];
    }

    T& At(size_t i) {
        if (i >= size_) {
            throw std::out_of_range{std::to_string(i)};
        }
        return data_[i];
    }

    const T& At(size_t i) const {
        if (i >= size_) {
            throw std::out_of_range{std::to_string(i)};
        }
        return data_[i];
    }

    T* Data() const {
        return data_;
    }

    T* begin() {
        return data_;
    }

    const T* begin() const {
        return data_;
    }

    T* end() {
        return data_ + size_;
    }

    const T* end() const {
        return data_ + size_;
    }

    T& Front() {
        return *data_;
    }

    const T& Front() const {
        return *data_;
    }

    T& Back() {
        return *(data_ + size_ - 1);
    }

    const T& Back() const {
        return *(data_ + size_ - 1);
    }

    bool Empty() const {
        return !size_;
    }

    size_t Size() const {
        return size_;
    }

    void Reserve(size_t ncap) {
        if (ncap <= cap_) {
            return;
        }
        ncap = std::max(ncap, cap_ * 2);
        T* ndata = reinterpret_cast<T*>(new char[ncap * sizeof(T)]);
        size_t j = 0;
        try {
            for (; j < size_; ++j) {
                new (ndata + j) T(data_[j]);
            }
        } catch (...) {
            for (size_t i = 0; i < j; ++i) {
                (ndata + i)->~T();
            }
            delete[] reinterpret_cast<char*>(ndata);
            throw;
        }
        for (size_t i = 0; i < size_; ++i) {
            (data_ + i)->~T();
        }
        delete[] reinterpret_cast<char*>(data_);
        data_ = ndata;
        cap_ = ncap;
    }

    size_t Capacity() const {
        return cap_;
    }

    void Clear() {
        for (size_t i = 0; i < size_; ++i) {
            (data_ + i)->~T();
        }
        size_ = 0;
    }

    void PushBack(const T& x) {
        if (size_ == cap_) {
            size_t ncap = cap_ ? cap_ * 2 : 1;
            T* ndata = reinterpret_cast<T*>(new char[ncap * sizeof(T)]);
            size_t j = 0;
            try {
                for (; j < size_; ++j) {
                    new (ndata + j) T(data_[j]);
                }
                new (ndata + j) T(x);
                ++j;
            } catch (...) {
                for (size_t i = 0; i < j; ++i) {
                    (ndata + i)->~T();
                }
                delete[] reinterpret_cast<char*>(ndata);
                throw;
            }
            for (size_t i = 0; i < size_; ++i) {
                (data_ + i)->~T();
            }
            delete[] reinterpret_cast<char*>(data_);
            data_ = ndata;
            cap_ = ncap;
        } else {
            new (data_ + size_) T(x);
        }
        ++size_;
    }

    void PopBack() {
        (data_ + size_ - 1)->~T();
        --size_;
    }

    void Resize(size_t nsize, const T& val = T{}) {
        while (size_ > nsize) {
            PopBack();
        }
        if (size_ == nsize) {
            return;
        }
        if (nsize <= cap_) {
            size_t j = size_;
            try {
                for (; j < nsize; ++j) {
                    new (data_ + j) T(val);
                }
            } catch (...) {
                for (size_t i = size_; i < j; ++i) {
                    (data_ + i)->~T();
                }
                throw;
            }
        } else {
            size_t ncap = std::max(nsize, cap_ * 2);
            T* ndata = reinterpret_cast<T*>(new char[ncap * sizeof(T)]);
            size_t j = 0;
            try {
                for (; j < size_; ++j) {
                    new (ndata + j) T(data_[j]);
                }
                for (; j < nsize; ++j) {
                    new (ndata + j) T(val);
                }
            } catch (...) {
                for (size_t i = 0; i < j; ++i) {
                    (ndata + i)->~T();
                }
                delete[] reinterpret_cast<char*>(ndata);
                throw;
            }
            for (size_t i = 0; i < size_; ++i) {
                (data_ + i)->~T();
            }
            delete[] reinterpret_cast<char*>(data_);
            data_ = ndata;
            cap_ = ncap;
        }
        size_ = nsize;
    }

    void Swap(Vector& other) {
        std::swap(size_, other.size_);
        std::swap(cap_, other.cap_);
        std::swap(data_, other.data_);
    }

    bool operator==(const Vector& other) const {
        if (size_ != other.size_) {
            return false;
        }
        for (size_t i = 0; i < size_; ++i) {
            if (data_[i] != other.data_[i]) {
                return false;
            }
        }
        return true;
    }

private:
    size_t size_;
    size_t cap_;
    T* data_;
};
