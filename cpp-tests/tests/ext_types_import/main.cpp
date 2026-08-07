#include <test_common.hpp>
#include <ext_types_import.hpp>

struct Salutator : public ext_types_export::Greeter
{
    std::string greet() override
    {
        return "Hello, World!";
    }
};

int main()
{
    auto greeter = std::make_shared<Salutator>();
    ASSERT_EQ(ext_types_import::call_greeter(greeter), "Hello, World!");

    ext_types_export::Coordinates coord{3, 4};
    auto rc = ext_types_import::roundtrip_coord(coord);
    ASSERT_EQ(rc.x, 3);
    ASSERT_EQ(rc.y, 4);

    std::vector<ext_types_export::Coordinates> coords{{1, 2}, {5, 6}};
    auto rcs = ext_types_import::roundtrip_coords(coords);
    ASSERT_EQ(rcs.size(), 2);
    ASSERT_EQ(rcs[0].x, 1);
    ASSERT_EQ(rcs[1].y, 6);

    auto rd = ext_types_import::roundtrip_direction(ext_types_export::Direction::kNorth);
    ASSERT_TRUE(rd == ext_types_export::Direction::kNorth);

    ext_types_export::Handle handle = 42;
    auto rh = ext_types_import::roundtrip_handle(handle);
    ASSERT_EQ(rh, 42);

    auto counter = ext_types_export::Counter::init();
    counter->increment();
    counter->increment();
    auto r_counter = ext_types_import::roundtrip_counter(counter);
    ASSERT_EQ(r_counter->get(), 2);
    r_counter->increment();
    ASSERT_EQ(r_counter->get(), 3);

    auto held = ext_types_export::Counter::init();
    held->increment();
    ext_types_import::Counters holder{held, 7};
    auto r_holder = ext_types_import::roundtrip_counters(holder);
    ASSERT_EQ(r_holder.label, 7);
    ASSERT_EQ(r_holder.counter->get(), 1);

    auto opt_coord = ext_types_import::roundtrip_opt_coord(ext_types_export::Coordinates{7, 8});
    ASSERT_TRUE(opt_coord.has_value());
    ASSERT_EQ(opt_coord->x, 7);
    ASSERT_FALSE(ext_types_import::roundtrip_opt_coord(std::nullopt).has_value());

    auto opt_counter = ext_types_export::Counter::init();
    opt_counter->increment();
    auto r_opt_counter = ext_types_import::roundtrip_opt_counter(opt_counter);
    ASSERT_TRUE(r_opt_counter != nullptr);
    ASSERT_EQ(r_opt_counter->get(), 1);
    ASSERT_TRUE(ext_types_import::roundtrip_opt_counter(nullptr) == nullptr);

    auto circle_shape = ext_types_export::Shape(ext_types_export::Shape::kCircle{3.14});
    auto circle = ext_types_import::roundtrip_shape(circle_shape);
    ASSERT_EQ(std::get<ext_types_export::Shape::kCircle>(circle.get_variant()).radius, 3.14);

    auto inner_shape = ext_types_import::InnerShape(ext_types_import::InnerShape::kHold{ext_types_export::Shape::kRectangle{4.0, 8.0}});
    auto rectangle = ext_types_import::roundtrip_inner(inner_shape);
    auto hold_rectangle = std::get<ext_types_import::InnerShape::kHold>(rectangle.get_variant()).shape;
    ASSERT_EQ(std::get<ext_types_export::Shape::kRectangle>(hold_rectangle.get_variant()).width, 4.0);
    ASSERT_EQ(std::get<ext_types_export::Shape::kRectangle>(hold_rectangle.get_variant()).height, 8.0);

    std::unordered_map<std::string, ext_types_export::Coordinates> locations;
    locations["blood falls"] = ext_types_export::Coordinates{77, 162};
    locations["uluru"] = ext_types_export::Coordinates{15, 131};
    auto r_locations = ext_types_import::roundtrip_coord_map(locations);
    ASSERT_EQ(r_locations.size(), 2);
    ASSERT_EQ(r_locations["blood falls"].x, 77);
    ASSERT_EQ(r_locations["uluru"].y, 131);

    std::vector<std::shared_ptr<ext_types_export::Counter>> counters;
    auto counter_a = ext_types_export::Counter::init();
    counter_a->increment();
    auto counter_b = ext_types_export::Counter::init();
    counter_b->increment();
    counter_b->increment();
    counters.push_back(counter_a);
    counters.push_back(counter_b);
    auto r_counters = ext_types_import::roundtrip_counter_list(counters);
    ASSERT_EQ(r_counters.size(), 2);
    ASSERT_EQ(r_counters[0]->get(), 1);
    ASSERT_EQ(r_counters[1]->get(), 2);

    auto car = ext_types_export::to_car();
    ASSERT_EQ(car->year(), 1984);
    ASSERT_EQ(ext_types_import::vehicle_year(car), 1984);

    return 0;
}