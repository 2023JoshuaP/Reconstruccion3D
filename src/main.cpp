#include "../headers/Extraction_Points_Tiff.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <tiff_file>" << std::endl;
    }

    std::string filename = argv[1];
    
    std::string base_name = fs::path(filename).stem().string();
    std::string file_xyz = "coordenadas/" + base_name + "_extraction_points.xyz";

    std::vector<Point3D> points = extraction_points_edge(filename);

    system("mkdir coordenadas >nul 2>nul");

    if (!save_points(file_xyz, points)) {
        std::cerr << "Error saving points to XYZ file." << std::endl;
        return 1;
    }
    else {
        std::cout << "Extraction points saved to: " << file_xyz << std::endl;
    }
    return 0;
}