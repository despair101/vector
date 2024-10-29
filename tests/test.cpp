#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>
#include <algorithm>
#include <random>

#include "../src/vector.h"

template <typename T>
std::vector<T> GetVectorStd(const Vector<T>& v) {
    return std::vector<T>(v.begin(), v.end());
}

TEST_CASE("Constructors") {
    Vector<int> a;
    REQUIRE(a.Size() == 0);
    REQUIRE(a.Capacity() == 0);

    Vector<size_t> b(5, 7);
    REQUIRE(GetVectorStd(b) == std::vector<size_t>(5, 7));

    std::vector<int> arr{1, 2, 3, 4, 5};
    Vector<int> my_arr(arr.begin(), arr.end());
    REQUIRE(arr == GetVectorStd(my_arr));

    Vector<std::string> c{"d", "i", "m", "a"};
    REQUIRE(GetVectorStd(c) == std::vector<std::string>{"d", "i", "m", "a"});

    Vector d = c;
    REQUIRE(c == d);

    Vector e = std::move(d);
    REQUIRE(c == e);
}

TEST_CASE("Empty and Size") {
    Vector<int> a;
    REQUIRE(a.Empty());

    a.PushBack(2);
    a.PushBack(7);
    REQUIRE(a.Size() == 2);

    a.PopBack();
    a.PopBack();
    REQUIRE(a.Empty());

    a.PushBack(7);
    a.Clear();
    REQUIRE(a.Empty());
}

TEST_CASE("Pushback and Popback") {
    const size_t ITERS = 10000;
    Vector<int> a;
    std::vector<int> b;
    for (size_t i = 0; i < ITERS; ++i) {
        a.PushBack(i);
        b.push_back(i);
    }

    REQUIRE(GetVectorStd(a) == b);

    while (!a.Empty()) {
        a.PopBack();
    }

    REQUIRE(a.Empty());
}

TEST_CASE("Capacity") {
    Vector<int> v;
    v.PushBack(1);
    size_t initial_capacity = v.Capacity();

    for (int i = 0; i < 100; ++i) {
        v.PushBack(i);
    }

    REQUIRE(v.Capacity() > initial_capacity);
    REQUIRE(v.Size() == 101);
}

TEST_CASE("Get by index") {
    Vector<int> v;
    v.PushBack(10);
    v.PushBack(20);

    REQUIRE(v[0] == 10);
    REQUIRE(v[1] == 20);
}

TEST_CASE("Resize") {
    Vector<int> v;
    v.PushBack(1);
    v.PushBack(2);
    v.Resize(5);

    REQUIRE(v.Size() == 5);
    REQUIRE(v.Capacity() >= 5);

    v.Resize(1);
    REQUIRE(v.Size() == 1);
    REQUIRE(v[0] == 1);
    REQUIRE(v.Capacity() >= 5);
}

TEST_CASE("Clear") {
    Vector<int> v;
    v.PushBack(1);
    v.PushBack(2);
    v.Clear();
    v.Clear();
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Capacity() >= 2);
}

TEST_CASE("Reserve") {
    Vector<int> a{1, 2, 3};
    a.Reserve(228);
    REQUIRE(a.Size() == 3);
    REQUIRE(a.Capacity() == 228);
    for (size_t i = 0; i < 100; ++i) {
        a.PushBack(i);
    }
    REQUIRE(a.Capacity() == 228);
    a.Reserve(10);
    REQUIRE(a.Capacity() == 228);
}

TEST_CASE("Swap") {
    Vector<int> a{1, 2, 3};
    Vector<int> b{2, 2, 8};
    Vector<int> c;
    a.Swap(c);
    c.Swap(a);
    a.Swap(b);
    REQUIRE(GetVectorStd(a) == std::vector{2, 2, 8});
    REQUIRE(GetVectorStd(b) == std::vector{1, 2, 3});
    Vector<Vector<int>> v{{1, 2}, {2, 1}};
    v.Front().Swap(v.Back());
    REQUIRE(v[0] == Vector{2, 1});
    REQUIRE(v[1] == Vector{1, 2});
}

TEST_CASE("Copy") {
    Vector<int> a{1, 2, 3};
    Vector b = a;
    Vector c = a;
    b = b;
    assert(b[1] == 2);
    REQUIRE(b == c);
}

TEST_CASE("Move") {
    Vector<int> a{2, 2, 8};
    Vector b = a;
    Vector c = std::move(a);
    b = std::move(b);
    REQUIRE(b == c);
}

TEST_CASE("Equal") {
    Vector<int> a{1, 2, 3};
    Vector<int> b{1, 2, 3};
    REQUIRE(a == b);
    a.PushBack(4);
    b.Resize(4);
    b[3] = 4;
    REQUIRE(a == b);
    a.Reserve(228);
    REQUIRE(a == b);
}

struct MyInt {
    int x_;
    MyInt() {
        throw std::exception{};
    }
    MyInt(int x) : x_(x) {
    }
    MyInt(const MyInt& other) {
        throw std::exception{};
    }
};

TEST_CASE("Throw in copy and default constructor") {
    REQUIRE_THROWS(Vector<MyInt>(10));
    REQUIRE_THROWS(Vector<MyInt>{1, 2, 3});
}

TEST_CASE("Assign") {
    Vector<int> a;
    a.Assign(5, 228);
    REQUIRE(a == Vector<int>(5, 228));
    a.Assign(0, 3);
    REQUIRE(a.Empty());
}

TEST_CASE("Iterators") {
    Vector<int> a{3, 2, 1, 4, 5};
    Vector<int> b;
    for (auto el : a) {
        b.PushBack(el);
    }
    REQUIRE(a == b);
    std::sort(a.begin(), a.end());
    for (size_t i = 0; i < 5; ++i) {
        REQUIRE(a[i] == i + 1);
    }
}

struct Kek {
    std::string data_;
    Kek() = delete;
    Kek(const std::string& data) : data_(data) {
    }
};

TEST_CASE("Deleted default constructor") {
    std::vector<Kek> bebras(3, std::string{"bebra"});
    std::string result;
    for (auto el : bebras) {
        result += el.data_;
    }
    REQUIRE(result == "bebrabebrabebra");
}

TEST_CASE("Random operations") {
    const size_t bignum = 100'000;
    const size_t op_cnt = 3;

    Vector<int> a(bignum, 1337);
    std::vector<int> b(bignum, 1337);

    std::mt19937 rnd(101);
    std::uniform_int_distribution<size_t> rd_ind(0, bignum - 1);
    std::uniform_int_distribution<size_t> rd_op(1, op_cnt);

    for (size_t i = 0; i < bignum; ++i) {
        size_t op = rd_op(rnd);
        if (op == 1) {
            a.PushBack(i);
            b.push_back(i);
        } else if (op == 2) {
            REQUIRE(a.Empty() == b.empty());
            if (!a.Empty()) {
                REQUIRE(a.Back() == b.back());
                a.PopBack();
                b.pop_back();
            }
        } else if (op == 3) {
            if (!a.Empty()) {
                size_t ind = std::uniform_int_distribution<size_t>(0, a.Size() - 1)(rnd);
                REQUIRE(a[ind] == b[ind]);
                a[ind] = b[ind] = 228;
            }
        }
        REQUIRE(a.Size() == b.size());
    }

    REQUIRE(GetVectorStd(a) == b);
}

TEST_CASE("Vector of vectors") {
    Vector<Vector<int>> v{{1, 2}, {3, 4}};
    REQUIRE(v[0][0] == 1);
    REQUIRE(v[0][1] == 2);
    REQUIRE(v[1][0] == 3);
    REQUIRE(v[1][1] == 4);
    v.PushBack({1, 3, 3, 7});
    REQUIRE(v.Size() == 3);
    REQUIRE(v[2] == Vector{1, 3, 3, 7});
}

struct EvilInt {
    static size_t counter;
    EvilInt(int val) : val_(val) {
        if (val == 228) {
            throw 1941;
        }
        if (counter == 200) {
            throw 1945;
        }
        ++counter;
    }
    EvilInt(const EvilInt& other) : val_(other.val_) {
        if (counter == 200) {
            throw 1945;
        }
        ++counter;
    }
    ~EvilInt() {
        --counter;
    }
    int val_;
};

size_t EvilInt::counter = 0;

TEST_CASE("Strong exception safety") {
    Vector<EvilInt> a;
    a.PushBack(EvilInt{0});
    a.PushBack(EvilInt{1});
    REQUIRE_THROWS(a.PushBack(EvilInt{228}));
    REQUIRE(a[0].val_ == 0);
    REQUIRE(a[1].val_ == 1);
    REQUIRE(a.Size() == 2);
    a.PushBack(EvilInt{2});
    REQUIRE(a[2].val_ == 2);
    size_t old_cap = a.Capacity();
    REQUIRE_THROWS(a.Resize(228, 1337));
    REQUIRE(a[2].val_ == 2);
    REQUIRE(a.Size() == 3);
    REQUIRE(old_cap == a.Capacity());
}