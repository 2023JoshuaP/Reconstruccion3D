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
        std::cerr << "cell_size: Optional parameter for Marching Cubes (default: 0.5)" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int option = std::stoi(argv[2]);
    float cell_size = 0.5f;
    if (argc >= 4) {
        cell_size = std::stof(argv[3]);
    }
    
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

            std::vector<Point3D> points = extraction_all_white_points(filename);
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
            std::cout << "=== Marching Cubes ===" << std::endl;
            if (extension != ".xyz") {
                std::cerr << "Error: Option 2 requires an XYZ file as input." << std::endl;
                return 1;
            }
            
            std::cout << "Processing point cloud: " << filename << std::endl;
            std::cout << "Cell size: " << cell_size << std::endl;
            
            // Crear instancia de MarchingCubes con el tamaño de celda especificado
            MarchingCubes marching(cell_size);
            
            // Procesar la nube de puntos
            marching.process_point_cloud(filename, cell_size);
            
            // Exportar la malla
            std::string file_obj = "mallas/" + base_name + "_mesh.obj";
            marching.export_file_obj(file_obj);

            std::cout << "Marching Cubes completed. Mesh exported to: " << file_obj << std::endl;
            break;
        }
        
        default:
            std::cerr << "Invalid option. Use 1 for extraction points or 2 for Marching Cubes." << std::endl;
            return 1;
    }
    return 0;
}