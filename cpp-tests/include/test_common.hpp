#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_NE(a, b) assert((a) != (b))
#define ASSERT_TRUE(a) assert((a))
#define ASSERT_FALSE(a) assert(!(a))

#define ASSERT_VECTOR_EQ(a, b) assert(vectors_equal((a), (b)))
#define ASSERT_VECTOR_NE(a, b) assert(!vectors_equal((a), (b)))

#define EXPECT_EXCEPTION(expr, exception) \
    try { \
        expr; \
        assert(false && "Didn't throw an exception"); \
    } catch (exception const &) { \
    } catch (...) { \
        assert(false && "Exception didn't match" #exception); \
    }

template <typename T>
bool vectors_equal(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size()) return false;

    return std::equal(a.begin(), a.end(), b.begin());
}
