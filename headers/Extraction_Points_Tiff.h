#pragma once

#include <tiffio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct Point3D {
    int coordinate_x, coordinate_y, coordinate_z;
};

bool is_edge_pixel(uint8_t** binary_image, int x, int y, int width, int height);
std::vector<Point3D> extraction_points_edge(const std::string& path);
bool save_points(const std::string& filename, const std::vector<Point3D>& points);