#include "../headers/Extraction_Points_Tiff.h"
#include "../headers/Marching_Cubes.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <tiff_file | xy<_file> <opcion>" << std::endl;
        std::cerr << "Options: 1 = Extract points only, 2 = Marching Cubes" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int option = std::stoi(argv[2]);
    
    std::string extension = fs::path(filename).extension().string();
    std::string base_name = fs::path(filename).stem().string();

    system("mkdir coordenadas >nul 2>nul");
    system("mkdir mallas >nul 2>nul");

    switch(option) {
        case 1: {
            std::cout << "Extraction points" << std::endl;
            if (extension != ".tif" && extension != ".tiff") {
                std::cerr << "Error: opción 1 requiere un archivo TIFF (.tif)" << std::endl;
                return 1;
            }

            std::vector<Point3D> points = extraction_points_edge(filename);
            std::string file_xyz = "coordenadas/" + base_name + "_extraction_points.xyz";

            if (!save_points(file_xyz, points)) {
                std::cerr << "Error saving points to XYZ file." << std::endl;
                return 1;
            }
            else {
                std::cout << "Extraction points saved to: " << file_xyz << std::endl;
            }
            break;
        }
        
        case 2: {
            if (extension != ".xyz") {
                std::cerr << "Error: opción 2 requiere un archivo XYZ como entrada." << std::endl;
                return 1;
            }
            
            MarchingCubes marching(0.5f);
            marching.process_point_cloud(filename, 0.5f);

            std::string file_obj = "mallas/" + base_name + "_mesh.obj";
            marching.export_file_obj(file_obj);

            std::cout << "Marching Cubes completado. Malla exportada a: " << file_obj << std::endl;
            break;
        }
        
        default:
            std::cerr << "Invalid option. Use 1 for extraction points or 2 for Marching Cubes." << std::endl;
            return 1;
    }
    return 0;
}