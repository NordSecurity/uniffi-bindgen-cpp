#include "lib_geometry.hpp"

double lib_geometry::gradient(lib_geometry::Line line) {
    return (line.end.coord_y - line.start.coord_y) / (line.end.coord_x - line.start.coord_x);
}

std::optional<lib_geometry::Point> lib_geometry::intersection(lib_geometry::Line line1, lib_geometry::Line line2) {
    double m1 = gradient(line1);
    double m2 = gradient(line2);
    double c1 = line1.start.coord_y - m1 * line1.start.coord_x;
    double c2 = line2.start.coord_y - m2 * line2.start.coord_x;
    if (m1 == m2) {
        return std::nullopt;
    }
    double x = (c2 - c1) / (m1 - m2);
    double y = m1 * x + c1;
    return lib_geometry::Point{x, y};
}