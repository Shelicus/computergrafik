#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "math.h"
#include "geometry.h"



class Camera {
public:
    Vector3df origin;       // Augenpunkt
    Vector3df forward;      // Blickrichtung (Z)
    Vector3df right;        // Rechts-Vektor (X)
    Vector3df up;           // Hoch-Vektor (Y)
    
    float focal_length;     // Abstand Auge -> Bildmitte
    float pixel_size;       // größe Pixel in Welt-Einheiten
    int img_width;          // Auflösung Breite
    int img_height;         // Auflösung Höhe

    // Konstruktor
    Camera(Vector3df eye, Vector3df lookDir, Vector3df worldUp, 
           float distToPlane, float pixelSizeInWorld, 
           int width, int height)
           :origin(eye),     
      forward(lookDir),     
      right({0,0,0}),       
      up({0,0,0})
    {
        origin = eye;
        img_width = width;
        img_height = height;
        focal_length = distToPlane;
        pixel_size = pixelSizeInWorld;

        forward = lookDir;
        forward.normalize();

        right = forward.cross_product(worldUp); 
        right.normalize();
 
        up = right.cross_product(forward);
        up.normalize();
    }

    // Erzeugt einen Strahl für einen spezifischen Pixel 
    Ray3df getRay(int x, int y) const {
        Vector3df imageCenter = origin + (focal_length * forward);

        // x=width/2 und y=height/2 -> Mitte (0,0) 
        // x geht nach Rechts (+), y geht nach Oben (+) 
        float x_offset = (x - img_width / 2.0f) * pixel_size;
      
        // (img_height / 2.0f - y) -> y=0 oben positiv ist
        float y_offset = (img_height / 2.0f - y) * pixel_size; 

        Vector3df pixelPos = imageCenter + (x_offset * right) + (y_offset * up);

        Vector3df rayDir = pixelPos - origin;
        rayDir.normalize();

        return Ray(origin, rayDir);
    }
};