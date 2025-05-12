#include <gtest/gtest.h>

#include <common/util/strings.hpp>

#include <list>
#include <string>

class ToStringStruct {
public:
    ToStringStruct(int a, int b)
        : a_(a)
        , b_(b) {
    }

    ToStringStruct() = default;

    std::string ToString() const {
        return "[" + std::to_string(a_) + ":" + std::to_string(b_) + "]";
    }

private:
    int a_;
    int b_;
};

TEST(Algorithm, Join) {
    std::vector<int> a = {1, 2, 3, 4, 5};
    ASSERT_EQ(common::util::Join(a), "1,2,3,4,5");

    std::vector<std::string> b = {"1", "2", "3", "4", "5"};
    ASSERT_EQ(common::util::Join(b), "1,2,3,4,5");

    std::list<int> c = {10};
    ASSERT_EQ(common::util::Join(c), "10");

    std::vector<ToStringStruct> d(2);
    d[0] = ToStringStruct(1, 1);
    d[1] = ToStringStruct(2, 3);

    ASSERT_EQ(common::util::Join(d), "[1:1],[2:3]");
}
