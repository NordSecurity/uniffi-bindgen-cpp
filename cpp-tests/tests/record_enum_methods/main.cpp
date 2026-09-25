#include "test_common.hpp"

#include <cmath>
#include <record_enum_methods.hpp>

void test_record_methods()
{
    auto point = record_enum_methods::Point{.x = 3, .y = 4};

    ASSERT_EQ(25, point.magnitude_squared());

    auto moved = point.translated(2, -1);
    ASSERT_EQ(5, moved.x);
    ASSERT_EQ(3, moved.y);

    ASSERT_EQ(3, point.x);
    ASSERT_EQ(4, point.y);

    ASSERT_EQ("(3, 4)", point.label());
}

void test_enum_methods()
{
    auto circle = record_enum_methods::Shape(record_enum_methods::Shape::kCircle{.radius = 2.0});
    auto rectangle = record_enum_methods::Shape(record_enum_methods::Shape::kRectangle{.width = 3.0, .height = 4.0});

    ASSERT_TRUE(std::abs(circle.area() - (M_PI * 4.0)) < 1e-9);
    ASSERT_TRUE(std::abs(rectangle.area() - 12.0) < 1e-9);

    ASSERT_EQ("circle r=2", circle.describe());
    ASSERT_EQ("rectangle 3x4", rectangle.describe());

    auto bigger = rectangle.scaled(2.0);
    ASSERT_TRUE(std::abs(bigger.area() - 48.0) < 1e-9);

    EXPECT_EXCEPTION(circle.scaled(-1.0), record_enum_methods::shape_error::NegativeScale);
}

void test_flat_enum_methods()
{
    auto on = record_enum_methods::Flag::kOn;

    ASSERT_EQ(record_enum_methods::Flag::kOff, inverted(on));
    ASSERT_EQ(record_enum_methods::Flag::kOn, inverted(inverted(on)));
}

void test_trait_impl_order()
{
    auto alpha = record_enum_methods::Alpha::init();
    ASSERT_EQ(42, alpha->zeta());

    std::shared_ptr<record_enum_methods::Zeta> as_trait = alpha;
    ASSERT_EQ(42, as_trait->zeta());

    auto trait_from_rust = record_enum_methods::make_zeta();
    ASSERT_EQ(42, trait_from_rust->zeta());
}

int main()
{
    test_record_methods();
    test_enum_methods();
    test_flat_enum_methods();
    test_trait_impl_order();

    return 0;
}
