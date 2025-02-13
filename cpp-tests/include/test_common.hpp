#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_NE(a, b) assert((a) != (b))
#define ASSERT_TRUE(a) assert((a))
#define ASSERT_FALSE(a) assert(!(a))

#define EXPECT_EXCEPTION(expr, exception) \
    try { \
        expr; \
        assert(false && "Didn't throw an exception"); \
    } catch (exception const &) { \
    } catch (...) { \
        assert(false && "Exception didn't match" #exception); \
    }
