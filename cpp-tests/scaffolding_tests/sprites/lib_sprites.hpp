#include <optional>

namespace {
    namespace sprites {
        struct Point {
            double x;
            double y;
        };

        struct Vector {
            double dx;
            double dy;
        };

        struct Sprite {
            Point position;

            Sprite(std::optional<Point> initial_position);
            Sprite(Point reference, Vector direction);

            Point get_position();
            void move_to(Point point);
            void move_by(Vector vector);
        };

        Point translate(Point point, Vector vector);
    }
}
