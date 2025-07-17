#pragma once

#include <tiffio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct Point3D {
    int coordinate_x, coordinate_y, coordinate_z;
};

std::vector<Point3D> extraction_all_white_points(const std::string& filename);
bool save_points(const std::string& filename, const std::vector<Point3D>& points);