#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

#include "Camera.cc"
#include "Screen.cc"


struct Material {
    Vector3df color;        // RGB Farbe (0.0 bis 1.0)
    float k_ambient;        // Umgebungslicht
    float k_diffuse;        // Diffuser Anteil
    float k_reflective;     // Spiegelnder Anteil
    float shininess;        // Glanzpunkt-Härte (Phong)
};

// Neue Struktur für Ebenen (Wände/Boden) - Löst das Artefakt-Problem!
struct Plane {
    Vector3df position; // Ein Punkt auf der Ebene
    Vector3df normal;   // Senkrechte Richtung der Ebene

    bool intersects(const Ray3df& ray, Intersection_Context<float, 3u>& ctx) const {
        float denom = ray.direction * normal; 

        // Nur Schnittpunkte von "vorne" betrachten 
        // und Division durch Null vermeiden.
        if (denom < -1e-6f) { 
            Vector3df p0l0 = position - ray.origin;
            float t = (p0l0 * normal) / denom;

            if (t >= 0.001f) { 
                ctx.t = t;
                ctx.normal = normal; 
                ctx.intersection = ray.origin + (t * ray.direction);
                return true;
            }
        }
        return false;
    }
};

enum ObjectType { SPHERE_OBJ, PLANE_OBJ };


struct SceneObject {
    ObjectType type;
    Sphere3df sphere;
    Plane plane;      
    Material material;

    // Konstruktor für Kugel
    SceneObject(Vector3df c, float r, Material m) 
        : type(SPHERE_OBJ), 
          sphere(c, r), 
          plane({ Vector3df{0.0f, 0.0f, 0.0f}, Vector3df{0.0f, 1.0f, 0.0f} }), 
          material(m) {}

    // Konstruktor für Wand
    SceneObject(Vector3df pos, Vector3df norm, Material m) 
        : type(PLANE_OBJ), 
          sphere(Vector3df{0.0f, 0.0f, 0.0f}, 0.0f), 
          plane({pos, norm}), 
          material(m) {}


    bool intersects(const Ray3df& ray, Intersection_Context<float, 3u>& ctx) const {
        if (type == SPHERE_OBJ) {
            return sphere.intersects(ray, ctx);
        } else {
            return plane.intersects(ray, ctx);
        }
    }
};

struct PointLight {
    Vector3df position;
    Vector3df color; 
};

std::vector<SceneObject> createScene() {
    std::vector<SceneObject> scene;

    /*//Kugel -> Artefakte
    Material matLeft   = { {0.8f, 0.1f, 0.1f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Rot
    Material matRight  = { {0.1f, 0.8f, 0.1f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Grün
    Material matWhite  = { {0.9f, 0.9f, 0.9f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Weiß
    
   
    // 1e5 = 100.000
    float wallRadius = 1e5f; 
    
    float roomWidth = 10.0f;  // x geht von -10 bis 10
    float roomHeight = 20.0f; // y geht von 0 bis 20
    float roomDepth = 20.0f;  // z geht bis -20

    // Linke Wand -> Mittelpunkt weit links bei x = -100010
    // Berührt den Raum bei x = -10
    scene.emplace_back(Vector3df{-roomWidth - wallRadius, 10.0f, 0.0f}, wallRadius, matLeft);

    // Rechte Wand -> Mittelpunkt weit rechts bei x = +100010
    // Berührt den Raum bei x = +10
    scene.emplace_back(Vector3df{ roomWidth + wallRadius, 10.0f, 0.0f}, wallRadius, matRight);

    // Rückwand -> Mittelpunkt weit hinten bei z = -100020
    // Berührt den Raum bei z = -20
    scene.emplace_back(Vector3df{0.0f, 10.0f, -roomDepth - wallRadius}, wallRadius, matWhite);

    // Boden -> Mittelpunkt weit unten bei y = -100000
    // Berührt den Raum bei y = 0
    scene.emplace_back(Vector3df{0.0f, -wallRadius, 0.0f}, wallRadius, matWhite);

    //Decke  -> Mittelpunkt weit oben bei y = +100020
    // Berührt den Raum bei y = 20
    scene.emplace_back(Vector3df{0.0f, roomHeight + wallRadius, 0.0f}, wallRadius, matWhite);*/
    
    // Wände
    Material matLeft   = { {0.8f, 0.1f, 0.1f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Rot
    Material matRight  = { {0.1f, 0.8f, 0.1f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Grün
    Material matWhite  = { {0.9f, 0.9f, 0.9f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Weiß
    
    float roomWidth = 10.0f;
    float roomDepth = 20.0f;
    float roomHeight = 20.0f;

    // Linke Wand  -> Position x=-10, Normale zeigt nach rechts (1,0,0)
    scene.emplace_back(Vector3df{-roomWidth, 0, 0}, Vector3df{1, 0, 0}, matLeft);

    // Rechte Wand  -> Position x=10, Normale zeigt nach links (-1,0,0)
    scene.emplace_back(Vector3df{ roomWidth, 0, 0}, Vector3df{-1, 0, 0}, matRight);

    // Rückwand  -> Position z=-20, Normale zeigt nach vorne (0,0,1)
    scene.emplace_back(Vector3df{0, 0, -roomDepth}, Vector3df{0, 0, 1}, matWhite);

    // Boden -> Position y=0, Normale zeigt nach oben (0,1,0)
    scene.emplace_back(Vector3df{0, 0.0f, 0}, Vector3df{0, 1, 0}, matWhite);

    // Decke -> Position y=20, Normale zeigt nach unten (0,-1,0)
    scene.emplace_back(Vector3df{0, roomHeight, 0}, Vector3df{0, -1, 0}, matWhite);



    
    float sphereRadius = 2.0f;     
    float sphereY = sphereRadius;   // => Kugel y=0

    // AMBIENT Kugel Links
    Material matAmbient = { {0.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0.0f, 0.0f}; 

    scene.emplace_back(Vector3df{-4.5f, sphereY, -2.0f}, sphereRadius, matAmbient);

    // DIFFUSE Kugel Mitte
    Material matDiffuse = { {1.0f, 1.0f, 0.0f}, 0.1f, 0.9f, 0.0f, 0.0f };
 
    scene.emplace_back(Vector3df{0.0f, sphereY, 3.0f}, sphereRadius, matDiffuse);

    // REFLEKTIVE Kugel Rechts - Spiegel
    Material matReflect = { {0.9f, 0.9f, 0.9f}, 0.0f, 0.1f, 0.9f, 0.0f }; 
   
    scene.emplace_back(Vector3df{4.5f, sphereY, -2.0f}, sphereRadius, matReflect);

    return scene;
}

Vector3df trace(const Ray3df& ray, const std::vector<SceneObject>& scene, const std::vector<PointLight>& lights, int depth) {
    if (depth > 5) return {0,0,0}; 

    const SceneObject* hitObject = nullptr;
    Intersection_Context<float, 3u> hitContext;
    float closestT = std::numeric_limits<float>::max();

    // Schnittpunkt finden
    for (const auto& obj : scene) {
        Intersection_Context<float, 3u> ctx;
    
        if (obj.intersects(ray, ctx)) {
            //check ob Schnittpunkt kleiner als  bisher Gefunden
            if (ctx.t > 0.001f && ctx.t < closestT) {
                closestT = ctx.t;
                hitObject = &obj;
                hitContext = ctx;
            }
        }
    }

    if (!hitObject) return {0,0,0}; // Nichts getroffen 

   
    Material mat = hitObject->material;
    Vector3df hitPoint = hitContext.intersection;
    Vector3df normal = hitContext.normal;
    normal.normalize();
    Vector3df viewDir = -1.0f * ray.direction; 
    viewDir.normalize();

    Vector3df finalColor = (mat.k_ambient * mat.color);

    // LICHTQUELLEN 
    for (const auto& light : lights) {
        
        Vector3df lightDir = light.position - hitPoint;
        float distToLight = lightDir.length();
        lightDir.normalize();

        // SCHATTEN 
        const float shadowBias = 0.001f; 
        
        bool inShadow = false;
        // Bias entlang der Normalen
        Vector3df shadowOrigin = hitPoint + (shadowBias * normal);
        Ray3df shadowRay = { shadowOrigin, lightDir};
        
        //Shatten berechnen
        for (const auto& obj : scene) {
            Intersection_Context<float, 3u> shadowCtx;
            if (obj.intersects(shadowRay, shadowCtx)) { 
                if (shadowCtx.t > 0.0f && shadowCtx.t < distToLight) {
                    inShadow = true;
                    break; 
                }
            }
        }

        if (!inShadow) {
            float NdotL = normal * lightDir;  //Lambert
            if (NdotL > 0) {
                // Diffus
                Vector3df diffuseComponent = { 
                    light.color[0] * mat.color[0], 
                    light.color[1] * mat.color[1], 
                    light.color[2] * mat.color[2] 
                };
                float factor = 1.0f / static_cast<float>(lights.size()); //Beleuchtung teilen durch Lights
                finalColor += factor * (NdotL * (mat.k_diffuse * diffuseComponent)); // adding to color


                // Specular
                if (mat.shininess > 0.0f) {
                    Vector3df reflectDirLight = (-1.0f * lightDir).get_reflective(normal);
                    reflectDirLight.normalize();
                    float specAngle = viewDir * reflectDirLight;
                    
                    if (specAngle > 0.0f) {
                        float specularFactor = std::pow(specAngle, mat.shininess);
                        finalColor += (specularFactor * 0.5f) * light.color; 
                    }
                }
            }
        }
    }

    // Refelektion
    if (mat.k_reflective > 0.0f) {
        Vector3df reflectOrigin = hitPoint + (0.001f * normal); 
        Vector3df reflectDir = ray.direction.get_reflective(normal);
        reflectDir.normalize();

        Ray3df reflectRay = { reflectOrigin, reflectDir };
        Vector3df reflectedColor = trace(reflectRay, scene, lights, depth + 1);
        
        finalColor += (mat.k_reflective * reflectedColor);
    }

    return finalColor;
}


int main() {
    const int image_width = 1024;
    const int image_height = 1024;
    Screen screen(image_width, image_height);
    
    // Kamera angepasst auf die neuen Proportionen
    Vector3df eye({0.0f, 10.0f, 35.0f});   
    Vector3df lookDir({0.0f, 0.0f, -1.0f}); 
    Vector3df worldUp({0.0f, 1.0f, 0.0f});

    // Parameter für die Kamera
    float abstand = 10.0f;    
    float pixelGroesse = 0.01f; 

    Camera cam(eye, lookDir, worldUp, abstand, pixelGroesse, image_width, image_height);

    std::vector<SceneObject> scene = createScene();

    std::vector<PointLight> myLights;
 
    myLights.push_back({ Vector3df{-5.0f, 18.0f, 5.0f}, 
        Vector3df{0.5f, 0.5f, 0.5f} });
    myLights.push_back({ Vector3df{5.0f, 18.0f, 5.0f}, 
        Vector3df{0.5f, 0.5f, 0.5f} });


    for (int j = 0; j < image_height; ++j) {
        if (j % 50 == 0) std::cout << "Rendering Line " << j << " / " << image_height << "\n";
        for (int i = 0; i < image_width; ++i) {
            Ray ray = cam.getRay(i, j);
            
            Vector3df pixelColor = trace(ray, scene, myLights, 0);

            pixelColor[0] = std::min(pixelColor[0], 1.0f);
            pixelColor[1] = std::min(pixelColor[1], 1.0f);
            pixelColor[2] = std::min(pixelColor[2], 1.0f);

            screen.setPixel(i, j, pixelColor);
        }
    }

    screen.saveToPPM("output.ppm");
    return 0;
}