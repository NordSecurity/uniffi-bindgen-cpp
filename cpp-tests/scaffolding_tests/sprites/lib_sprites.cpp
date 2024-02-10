#include "lib_sprites.hpp"

sprites::Point sprites::translate(const Point &position, Vector direction) {
    return sprites::Point { position.x + direction.dx, position.y + direction.dy };
}

sprites::Sprite::Sprite(std::optional<Point> initial_position) : position { initial_position.value_or(Point { 0.0, 0.0 }) } {}

sprites::Sprite::Sprite(Point reference, Vector direction) : position { translate(reference, direction) } {}

sprites::Point sprites::Sprite::get_position() {
    return position;
}

void sprites::Sprite::move_to(Point point) {
    position = point;
}

void sprites::Sprite::move_by(Vector vector) {
    position = translate(position, vector);
}

#include <sprites_cpp_scaffolding.cpp>
