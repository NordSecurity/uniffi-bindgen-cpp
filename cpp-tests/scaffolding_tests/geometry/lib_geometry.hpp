#include <optional>

namespace {
    namespace geometry {
        struct Point {
            double coord_x;
            double coord_y;
        };

        struct Line {
            Point start;
            Point end;
        };

        double gradient(Line line);
        std::optional<Point> intersection(Line line1, Line line2);
    }
}