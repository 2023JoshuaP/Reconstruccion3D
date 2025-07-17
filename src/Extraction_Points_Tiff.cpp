#include "../headers/Extraction_Points_Tiff.h"

std::vector<Point3D> extraction_all_white_points(const std::string& filename) {
    std::vector<Point3D> points;

    TIFF* tif = TIFFOpen(filename.c_str(), "r");
    if (!tif) {
        std::cerr << "Don't open the TIFF file." << std::endl;
        return points;
    }

    int index_page = 0;
    do {
        uint32_t width, height;
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

        uint32_t* raster = (uint32_t*)_TIFFmalloc(width * height * sizeof(uint32_t));
        if (!raster) {
            std::cerr << "Memory error." << std::endl;
            break;
        }

        if (!TIFFReadRGBAImageOriented(tif, width, height, raster, ORIENTATION_TOPLEFT, 0)) {
            std::cerr << "Error reading image." << std::endl;
            _TIFFfree(raster);
            break;
        }

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                uint32_t pixel = raster[y * width + x];
                uint8_t r = TIFFGetR(pixel);
                uint8_t g = TIFFGetG(pixel);
                uint8_t b = TIFFGetB(pixel);

                if (r > 0 || g > 0 || b > 0) {
                    points.push_back({ (int)x, (int)(height - y), index_page });
                }
            }
        }

        _TIFFfree(raster);
        index_page++;
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return points;
}

bool save_points(const std::string& filename, const std::vector<Point3D>& points) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& point : points) {
        file << point.coordinate_x << " " << point.coordinate_y << " " << point.coordinate_z << "\n";
    }

    file.close();
    return true;
}