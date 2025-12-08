#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "math.h"
#include "geometry.h"


struct Material {
    Vector<float, 3> color; // RGB Farbe (0.0 bis 1.0)
    float k_ambient;        // Umgebungslicht-Anteil (0.0 bis 1.0)
    float k_diffuse;        // Diffuser Anteil (Lambert)
    float k_reflective;     // Spiegelnder Anteil (Reflektion)
    float shininess;        // Für Specular Highlights (Phong), optional
};

struct SceneObject {
    Sphere3df sphere;
    Material material;

    SceneObject(Vector3df c, float r, Material m) : sphere(c, r), material(m) {}
};

// =========================================================
// 3. SZENE ERSTELLEN (Cornel Box + 3 Kugeln)
// =========================================================

std::vector<SceneObject> createScene() {
    std::vector<SceneObject> scene;
    
    // --- 1. Die Cornell Box (Simuliert durch 4 riesige Kugeln) ---
    // Radius sehr groß (100000), damit die Oberfläche flach wirkt.
    float wallR = 1e5f; 
    float roomDist = 1e5f + 10.0f; // Raum ist ca 20 breit (-10 bis 10)

    Material matLeft   = { {0.8f, 0.2f, 0.2f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Rot
    Material matRight  = { {0.2f, 0.8f, 0.2f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Grün
    Material matWhite  = { {0.9f, 0.9f, 0.9f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Weiß

    // Linke Wand (Rot)
    scene.emplace_back(Vector3df{-roomDist, 0, 0}, wallR, matLeft);
    // Rechte Wand (Grün)
    scene.emplace_back(Vector3df{ roomDist, 0, 0}, wallR, matRight);
    // Rückwand (Weiß)
    scene.emplace_back(Vector3df{0, 0, roomDist + 10.0f}, wallR, matWhite); // +10 Tiefe
    // Boden (Weiß)
    scene.emplace_back(Vector3df{0, -roomDist, 0}, wallR, matWhite);
    // Decke (Weiß)
    scene.emplace_back(Vector3df{0, roomDist + 20.0f, 0}, wallR, matWhite);

    // --- 2. Die 3 speziellen Kugeln ---
    
    // A. AMBIENT Kugel (Links)
    // Hoher Ambient-Wert, kein Diffus, keine Reflexion.
    // Sieht aus wie ein flacher Farbkreis, "leuchtet" selbst.
    Material matAmbient = { {0.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0.0f, 0.0f }; // Cyan
    scene.emplace_back(Vector3df{-5.0f, 3.0f, 0.0f}, 3.0f, matAmbient);

    // B. DIFFUSE Kugel (Mitte)
    // Klassisches Mattes Material (Lambert). Reagiert auf Lichtwinkel.
    Material matDiffuse = { {1.0f, 1.0f, 0.0f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Gelb
    scene.emplace_back(Vector3df{0.0f, 3.0f, 5.0f}, 3.0f, matDiffuse);

    // C. REFLEKTIVE Kugel (Rechts)
    // Spiegel. Diffus niedrig, Reflektion hoch.
    // Benutzt math.h's `get_reflective`.
    Material matReflect = { {0.9f, 0.9f, 0.9f}, 0.0f, 0.1f, 0.9f, 0.0f }; // Weiß/Spiegel
    scene.emplace_back(Vector3df{5.0f, 3.0f, 0.0f}, 3.0f, matReflect);

    return scene;
}

Vector3df trace(const Ray3df& ray, const std::vector<SceneObject>& scene, int depth) {
    if (depth > 5) return {0,0,0}; // Abbruch bei zu vielen Spiegelungen

    const SceneObject* hitObject = nullptr;
    Intersection_Context<float, 3u> hitContext;
    float closestT = std::numeric_limits<float>::max();

    // 1. Nächsten Schnittpunkt finden
    for (const auto& obj : scene) {
        Intersection_Context<float, 3u> ctx;
        if (obj.sphere.intersects(ray, ctx)) {
            if (ctx.t < closestT) {
                closestT = ctx.t;
                hitObject = &obj;
                hitContext = ctx;
            }
        }
    }

    if (!hitObject) return {0,0,0}; // Nichts getroffen -> Schwarz

    // 2. Beleuchtung berechnen
    Material mat = hitObject->material;
    Vector3df hitPoint = hitContext.intersection;
    Vector3df normal = hitContext.normal;
    Vector3df color = {0,0,0};

    // Lichtquelle (Punktlicht oben vorne)
    Vector3df lightPos = {0.0f, 15.0f, -10.0f};
    Vector3df lightDir = lightPos - hitPoint;
    lightDir.normalize();

    // A. Ambient Anteil (Grundhelligkeit)
    // Farbe = MaterialFarbe * k_ambient
    color += (mat.k_ambient * mat.color);

    // B. Diffuser Anteil (Lambert)
    // Farbe += MaterialFarbe * k_diffuse * (Normal . LightDir)
    float NdotL = normal * lightDir; // Skalarprodukt aus math.h
    if (NdotL > 0) {
        color += NdotL * (mat.k_diffuse * mat.color);
    }

    // C. Reflektiver Anteil (Spiegelung)
    if (mat.k_reflective > 0) {
        // Hier nutzen wir die Funktion aus deiner math.h:
        Vector3df reflectDir = ray.direction.get_reflective(normal);
        reflectDir.normalize();

        Ray3df reflectRay = { hitPoint + (0.01f * reflectDir), reflectDir };
        
        // Rekursiver Aufruf
        Vector3df reflectColor = trace(reflectRay, scene, depth + 1);
        
        // Farbe += ReflektierteFarbe * k_reflective
        color += (mat.k_reflective * reflectColor);
    }

    // Clipping auf 1.0 (damit es nicht weiß ausbrennt)
    if (color[0] > 1.0f) color[0] = 1.0f;
    if (color[1] > 1.0f) color[1] = 1.0f;
    if (color[2] > 1.0f) color[2] = 1.0f;

    return color;
}