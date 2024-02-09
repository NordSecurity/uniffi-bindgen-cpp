#include "lib_geometry.hpp"

double geometry::gradient(geometry::Line line) {
    return (line.end.coord_y - line.start.coord_y) / (line.end.coord_x - line.start.coord_x);
}

std::optional<geometry::Point> geometry::intersection(geometry::Line line1, geometry::Line line2) {
    double m1 = gradient(line1);
    double m2 = gradient(line2);
    double c1 = line1.start.coord_y - m1 * line1.start.coord_x;
    double c2 = line2.start.coord_y - m2 * line2.start.coord_x;
    if (m1 == m2) {
        return std::nullopt;
    }
    double x = (c2 - c1) / (m1 - m2);
    double y = m1 * x + c1;
    return geometry::Point{x, y};
}

#include <geometry_cpp_scaffolding.cpp>