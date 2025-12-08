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
    float pixel_size;       // Wie groß ist ein Pixel in Welt-Einheiten?
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

        // 1. Koordinatensystem der Kamera berechnen
        // Wir gehen davon aus, dass lookDir senkrecht auf dem Rechteck steht.
        forward = lookDir;
        forward.normalize();

        // Rechtes Vektor (Kreuzprodukt aus Welt-Oben und Blickrichtung)
        right = forward.cross_product(worldUp); 
        right.normalize();

        // Echter Oben-Vektor (Senkrecht zu forward und right)
        up = right.cross_product(forward);
        up.normalize();
    }

    // Erzeugt einen Strahl für einen spezifischen Pixel (x, y)
    Ray3df getRay(int x, int y) const {
        // 1. Mittelpunkt des Bild-Rechtecks im Raum
        Vector3df imageCenter = origin + (focal_length * forward);

        // 2. Offset vom Mittelpunkt berechnen
        // Wir wollen, dass x=width/2 und y=height/2 genau in der Mitte (0,0) sind.
        // x geht nach Rechts (+), y geht nach Oben (+) oder Unten (-).
        
        float x_offset = (x - img_width / 2.0f) * pixel_size;
        
        // ACHTUNG: Bildkoordinaten sind meist y=0 oben. 3D-Welt ist y=0 unten.
        // Wir drehen y hier um, damit das Bild nicht auf dem Kopf steht.
        // (img_height / 2.0f - y) sorgt dafür, dass y=0 oben positiv ist.
        float y_offset = (img_height / 2.0f - y) * pixel_size; 

        // 3. Position des Pixels im 3D-Raum
        Vector3df pixelPos = imageCenter + (x_offset * right) + (y_offset * up);

        // 4. Strahl-Richtung: Vom Auge zum Pixel
        Vector3df rayDir = pixelPos - origin;
        rayDir.normalize();

        return Ray(origin, rayDir);
    }
};