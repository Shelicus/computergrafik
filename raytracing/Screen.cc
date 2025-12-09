#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "math.h"

class Screen {
public:
    int width;
    int height;
    std::vector<Vector3df> pixels; // Speichert RGB Werte (0.0 bis 1.0)

    Screen(int w, int h) : width(w), height(h) {
        pixels.resize(w * h, Vector3df({0.0f, 0.0f, 0.0f}));
    }

    // Setzt ein Pixel (kapselt den Zugriff auf das Array)
    void setPixel(int x, int y, const Vector3df& color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] = color;
        }
    }

    void saveToPPM(const std::string& filename) const {
        std::ofstream file(filename);
        file << "P3\n" << width << " " << height << "\n255\n";

        for (int y = height - 1; y >= 0; --y) { 
            for (int x = 0; x < width; ++x) {
                Vector3df col = pixels[y * width + x];
                // Farbwerte von 0.0-1.0 auf 0-255 skalieren
                float r_raw = col[0];
                float g_raw = col[1];
                float b_raw = col[2];

                //255.999 -> damit es wirklich weiß ist (aufrunden)
                int r = static_cast<int>(255.999 * r_raw);
                int g = static_cast<int>(255.999 * g_raw);
                int b = static_cast<int>(255.999 * b_raw);
                file << r << " " << g << " " << b << "\n";
            }
        }
        std::cout << "Bild gespeichert: " << filename << std::endl;
    }
};