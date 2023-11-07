#include <test_common.hpp>

#include <arithmetic.hpp>

int main() {
    ASSERT_EQ(arithmetic::add(2, 4), 6ul);
    ASSERT_EQ(arithmetic::add(4, 8), 12ul);

    ASSERT_EQ(2ul, arithmetic::sub(4, 2));
    ASSERT_EQ(4ul, arithmetic::sub(8, 4));

    EXPECT_EXCEPTION(arithmetic::sub(2, 4), arithmetic::IntegerOverflow);

    ASSERT_EQ(4ul, arithmetic::div(8, 2));

    EXPECT_EXCEPTION(arithmetic::div(8, 0), std::runtime_error);

    ASSERT_TRUE(arithmetic::equal(2, 2));
    ASSERT_TRUE(arithmetic::equal(4, 4));

    ASSERT_FALSE(arithmetic::equal(2, 4));
    ASSERT_FALSE(arithmetic::equal(4, 8));
    

    return 0;
}