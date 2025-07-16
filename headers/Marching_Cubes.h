#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

struct Point3DMC {
    float x, y, z;
    Point3DMC(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    
    Point3DMC operator+(const Point3DMC& other) const {
        return Point3DMC(x + other.x, y + other.y, z + other.z);
    }

    Point3DMC operator*(float scalar) const {
        return Point3DMC(x * scalar, y * scalar, z * scalar);
    }

    Point3DMC operator-(const Point3DMC& other) const {
        return Point3DMC(x - other.x, y - other.y, z - other.z);
    }

    float length() const {
        return sqrt(x * x + y * y + z * z);
    }

    Point3DMC normalize() const {
        float len = length();
        if (len > 0) {
            return Point3DMC(x / len, y / len, z / len);
        }
        return Point3DMC(0, 0, 0);
    }
};

struct Triangle {
    Point3DMC vertexes[3];
    Point3DMC normalize;
};

struct GridCell {
    Point3DMC point[8];
    float value[8];
};

class MarchingCubes {
    private:
        std::vector<Point3DMC> point_cloud;
        std::vector<Triangle> triangles;

        int grid_size_x, grid_size_y, grid_size_z;
        float cell_size;
        Point3DMC min_bounds, max_bounds;

        std::vector<std::vector<std::vector<float>>> scalar_field;
    
    public:
        MarchingCubes(float cell_size = 1.0f);

        bool load_points_cloud(const std::string& file_cloud);
        void calculate_bounds();
        void initialize_grid();
        void calculate_scalar_field();
        Point3DMC vertex_interpolation(const Point3DMC& point_1, const Point3DMC& point2, float value_1, float value_2, float iso_level);
        Point3DMC get_grid_point(int x, int y, int z);
        float get_scalar_value(int x, int y, int z);
        void process_cube(int x, int y, int z, float iso_level);
        void generate_mesh(float iso_level = 0.5f);
        void process_point_cloud(const std::string& filename, float iso_level = 0.5f);
        const std::vector<Triangle>& get_triangles() const;
        void export_file_obj(const std::string& fileobj);
};

class SpatialHash {
    private:
        std::unordered_map<int, std::vector<Point3DMC*>> hash_grid;
        float cell_size;
        
        int hash_function(int x, int y, int z) {
            return x * 73856093 + y * 19349663 + z * 83492791;
        }
        
    public:
        SpatialHash(float cs) : cell_size(cs) {}
        
        void insert(Point3DMC* point) {
            int x = (int)(point->x / cell_size);
            int y = (int)(point->y / cell_size);
            int z = (int)(point->z / cell_size);
            
            hash_grid[hash_function(x, y, z)].push_back(point);
        }
        
        std::vector<Point3DMC*> get_nearby_points(const Point3DMC& query, float radius) {
            std::vector<Point3DMC*> nearby;
            int cells_to_check = (int)ceil(radius / cell_size);
            
            int qx = (int)(query.x / cell_size);
            int qy = (int)(query.y / cell_size);
            int qz = (int)(query.z / cell_size);
            
            for (int dx = -cells_to_check; dx <= cells_to_check; dx++) {
                for (int dy = -cells_to_check; dy <= cells_to_check; dy++) {
                    for (int dz = -cells_to_check; dz <= cells_to_check; dz++) {
                        int hash = hash_function(qx + dx, qy + dy, qz + dz);
                        if (hash_grid.find(hash) != hash_grid.end()) {
                            for (auto* point : hash_grid[hash]) {
                                nearby.push_back(point);
                            }
                        }
                    }
                }
            }
            
            return nearby;
        }
};

extern const int edge_table[256];
extern const int tri_table[256][16];
extern const Point3DMC cube_vertex_offset[8];
extern const int edge_index_pairs[12][2];