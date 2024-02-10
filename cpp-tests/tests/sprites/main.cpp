#include <test_common.hpp>

#include <sprites.hpp>

int main() {
    auto sprite = sprites::Sprite::init(std::nullopt);
    ASSERT_EQ(sprite->get_position().x, 0.0);
    ASSERT_EQ(sprite->get_position().y, 0.0);

    auto s = sprites::Sprite::init(sprites::Point { 1.0, 2.0 });
    ASSERT_EQ(s->get_position().x, 1.0);
    ASSERT_EQ(s->get_position().y, 2.0);

    s->move_to(sprites::Point { 3.0, 4.0 });
    ASSERT_EQ(s->get_position().x, 3.0);
    ASSERT_EQ(s->get_position().y, 4.0);

    s->move_by(sprites::Vector { -4.0, 2.0 });
    ASSERT_EQ(s->get_position().x, -1.0);
    ASSERT_EQ(s->get_position().y, 6.0);

    auto rel = sprites::Sprite::new_relative_to(sprites::Point {0.0, 1.0}, sprites::Vector { 1.0, 1.5});
    ASSERT_EQ(rel->get_position().x, 1.0);
    ASSERT_EQ(rel->get_position().y, 2.5);

    auto p = sprites::Point { 1.0, 2.0 };
    auto t_p = sprites::translate(p, sprites::Vector { 1.0, 1.5 });
    ASSERT_EQ(t_p.x, 2.0);
    ASSERT_EQ(t_p.y, 3.5);

    return 0;
}
