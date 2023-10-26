#include <test_common.hpp>

#include <geometry.hpp>

int main() {
    auto line1 = geometry::Line {
        geometry::Point { 0.0, 0.0 },
        geometry::Point { 1.0, 2.0 }
    };

    auto line2 = geometry::Line {
        geometry::Point { 1.0, 1.0 },
        geometry::Point { 2.0, 2.0 }
    };

    ASSERT_EQ(geometry::gradient(line1), 2.0);
    ASSERT_EQ(geometry::gradient(line2), 1.0);

    auto point = geometry::Point { 0.0, 0.0 };
    auto intersection = geometry::intersection(line1, line2);
    ASSERT_TRUE(intersection.has_value());
    auto intersection_value = intersection.value();
    ASSERT_EQ(intersection_value.coord_x, 0);
    ASSERT_EQ(intersection_value.coord_y, 0);

    // ASSERT_EQ(point, geometry::intersection(line1, line2).value());
    ASSERT_EQ(std::nullopt, geometry::intersection(line1, line1));

    return 0;
}