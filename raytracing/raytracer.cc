#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "Camera.cc"
#include "Screen.cc"





// Die folgenden Kommentare beschreiben Datenstrukturen und Funktionen                    #
// Die Datenstrukturen und Funktionen die weiter hinten im Text beschrieben sind,         #
// hängen höchstens von den vorhergehenden Datenstrukturen ab, aber nicht umgekehrt.      #<- nur bezug auf den Text zuvor, nicht danach



// Ein "Bildschirm", der das Setzen eines Pixels kapselt   #
// Der Bildschirm hat eine Auflösung (Breite x Höhe)       #
// Kann zur Ausgabe einer PPM-Datei verwendet werden oder #
// mit SDL2 implementiert werden. #



// Eine "Kamera", die von einem Augenpunkt aus in eine Richtung senkrecht auf ein Rechteck (das Bild) zeigt.#
// Für das Rechteck muss die Auflösung oder alternativ die Pixelbreite und -höhe bekannt sein.#
// Für ein Pixel mit Bildkoordinate kann ein Sehstrahl erzeugt werden.#



// Für die "Farbe" benötigt man nicht unbedingt eine eigene Datenstruktur.
// Sie kann als Vector3df implementiert werden mit Farbanteil von 0 bis 1.
// Vor Setzen eines Pixels auf eine bestimmte Farbe (z.B. 8-Bit-Farbtiefe),
// kann der Farbanteil mit 255 multipliziert  und der Nachkommaanteil verworfen werden.


// Das "Material" der Objektoberfläche mit ambienten, diffusem und reflektiven Farbanteil.



// Ein "Objekt", z.B. eine Kugel oder ein Dreieck, und dem zugehörigen Material der Oberfläche.
// Im Prinzip ein Wrapper-Objekt, das mindestens Material und geometrisches Objekt zusammenfasst.
// Kugel und Dreieck finden Sie in geometry.h/tcc


// verschiedene Materialdefinition, z.B. Mattes Schwarz, Mattes Rot, Reflektierendes Weiss, ...
// im wesentlichen Variablen, die mit Konstruktoraufrufen initialisiert werden.


// Die folgenden Werte zur konkreten Objekten, Lichtquellen und Funktionen, wie Lambertian-Shading
// oder die Suche nach einem Sehstrahl für das dem Augenpunkt am nächsten liegenden Objekte,
// können auch zusammen in eine Datenstruktur für die gesammte zu
// rendernde "Szene" zusammengefasst werden.

// Die Cornelbox aufgebaut aus den Objekten
// Am besten verwendet man hier einen std::vector< ... > von Objekten.

// Punktförmige "Lichtquellen" können einfach als Vector3df implementiert werden mit weisser Farbe,
// bei farbigen Lichtquellen müssen die entsprechenden Daten in Objekt zusammengefaßt werden
// Bei mehreren Lichtquellen können diese in einen std::vector gespeichert werden.

// Sie benötigen eine Implementierung von Lambertian-Shading, z.B. als Funktion
// Benötigte Werte können als Parameter übergeben werden, oder wenn diese Funktion eine Objektmethode eines
// Szene-Objekts ist, dann kann auf die Werte teilweise direkt zugegriffen werden.
// Bei mehreren Lichtquellen muss der resultierende diffuse Farbanteil durch die Anzahl Lichtquellen geteilt werden.

// Für einen Sehstrahl aus allen Objekte, dasjenige finden, das dem Augenpunkt am nächsten liegt.
// Am besten einen Zeiger auf das Objekt zurückgeben. Wenn dieser nullptr ist, dann gibt es kein sichtbares Objekt.

// Die rekursive raytracing-Methode. Am besten ab einer bestimmten Rekursionstiefe (z.B. als Parameter übergeben) abbrechen.

struct Material {
    Vector<float, 3> color; // RGB Farbe (0.0 bis 1.0)
    float k_ambient;        // Umgebungslicht-Anteil (0.0 bis 1.0)
    float k_diffuse;        // Diffuser Anteil (Lambert)
    float k_reflective;     // Spiegelnder Anteil (Reflektion)
    float shininess;
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
    float roomDist = 1e5f; // Raum ist ca 20 breit (-10 bis 10)

    Material matLeft   = { {0.8f, 0.1f, 0.1f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Rot
    Material matRight  = { {0.1f, 0.8f, 0.1f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Grün
    Material matWhite  = { {0.9f, 0.9f, 0.9f}, 0.1f, 0.9f, 0.0f, 0.0f }; // Weiß
    Material matGrey   = { {0.7f, 0.7f, 0.7f}, 0.1f, 0.9f, 0.2f, 0.0f }; // Grau

    // Linke Wand (Rot)
    scene.emplace_back(Vector3df{-roomDist- 10.0f, 0.0f, 0.0f}, wallR, matLeft);
    // Rechte Wand (Grün)
    scene.emplace_back(Vector3df{ roomDist+ 10.0f, 0.0f, 0.0f}, wallR, matRight);
    // Rückwand (Weiß)
    scene.emplace_back(Vector3df{0, 0.0f, -roomDist - 20.0f}, wallR, matWhite); // +10 Tiefe
    // Boden (weiß)
    scene.emplace_back(Vector3df{0, -roomDist, 0}, wallR, matWhite);
    // Decke (grau)
    scene.emplace_back(Vector3df{0, roomDist + 20.0f, 0}, wallR, matWhite);

    // --- 2. Die 3 speziellen Kugeln ---
    
    // A. AMBIENT Kugel (Links)
    // Hoher Ambient-Wert, kein Diffus, keine Reflexion.
    // Sieht aus wie ein flacher Farbkreis, "leuchtet" selbst.
    Material matAmbient = { {0.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0.0f, 0.0f}; // Cyan
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
    Vector3df viewDir = -1.0f * ray.direction; // Blickrichtung (zum Auge zurück)

    // Startfarbe ist Schwarz
    Vector3df finalColor = {0,0,0};

    // Lichtquelle (Weißes Punktlicht)
    Vector3df lightPos = {5.0f, 15.0f, 10.0f}; 
    Vector3df lightDir = lightPos - hitPoint;
    float distToLight = lightDir.length(); // Optional für Abnahme der Helligkeit
    lightDir.normalize();

    // A. AMBIENT (Grundhelligkeit)
    // Farbe = Objektfarbe * Ambient-Stärke
    finalColor += (mat.k_ambient * mat.color);

    // B. Diffuser Anteil (Lambert)
    // Farbe += MaterialFarbe * k_diffuse * (Normal . LightDir)
    float NdotL = normal * lightDir; // Skalarprodukt aus math.h
    if (NdotL > 0) {
        finalColor += NdotL * (mat.k_diffuse * mat.color);

        // [Neu] C. SPECULAR (Phong - Weißer Glanzpunkt)
        // Nur berechnen, wenn Material glänzend ist (shininess > 0)
        // Das ist wichtig für "weißes Licht": Der Glanzpunkt hat die Farbe des Lichts (Weiß),
        // NICHT die Farbe des Objekts.
        if (mat.shininess > 0.0f) {
            // Reflektionsrichtung des Lichts
            Vector3df reflectDirLight = (-1.0f * lightDir).get_reflective(normal);
            reflectDirLight.normalize();

            // Wie stark schaut die Kamera in die Reflektion des Lichts?
            float specAngle = viewDir * reflectDirLight;
            if (specAngle > 0.0f) {
                // Potenzfunktion macht den Punkt klein und scharf
                float specularFactor = std::pow(specAngle, mat.shininess);
                
                // Glanzpunkt ist Weiß {1.0, 1.0, 1.0}, daher addieren wir einfach den Faktor
                Vector3df whiteLight = {1.0f, 1.0f, 1.0f};
                finalColor += (specularFactor * 0.5f) * whiteLight; // 0.5f als Stärke des Glanzes
            }
        }
    }

    // --- SCHATTEN TEST (Shadow Ray) ---
    bool inShadow = false;

    // Wir starten den Schattenstrahl minimal verschoben in Richtung Licht, 
    // um "Shadow Acne" (Selbstschattierung) zu vermeiden.
    Ray3df shadowRay = { hitPoint + (0.01f * lightDir), lightDir };

    for (const auto& obj : scene) {
        Intersection_Context<float, 3u> shadowCtx;
        // Prüfen ob Objekt im Weg steht
        if (obj.sphere.intersects(shadowRay, shadowCtx)) {
            // Ein Objekt wirft nur Schatten, wenn:
            // 1. Es VOR der Lampe ist (t < distToLight)
            // 2. Es nicht hinter uns ist (t > 0.001)
            if (shadowCtx.t > 0.001f && shadowCtx.t < distToLight) {
                inShadow = true;
                break; // Ein Treffer reicht, wir sind im Schatten
            }
        }
    }


    // B. & C. DIFFUSE + SPECULAR (Nur wenn NICHT im Schatten)
    if (!inShadow) {
        
        // B. Diffus
        float NdotL = normal * lightDir; 
        if (NdotL > 0) {
            finalColor += NdotL * (mat.k_diffuse * mat.color);

            // C. Specular (Glanzpunkt)
            if (mat.shininess > 0.0f) {
                Vector3df reflectDirLight = (-1.0f * lightDir).get_reflective(normal);
                reflectDirLight.normalize();

                float specAngle = viewDir * reflectDirLight;
                if (specAngle > 0.0f) {
                    float specularFactor = std::pow(specAngle, mat.shininess);
                    Vector3df whiteLight = {1.0f, 1.0f, 1.0f};
                    finalColor += (specularFactor * 0.5f) * whiteLight; 
                }
            }
        }
    }

    // C. Reflektiver Anteil (Spiegelung)
    // Das ist für den "Spiegel-Effekt", wo man andere Objekte sieht
    if (mat.k_reflective > 0.0f) {
        Vector3df reflectDir = ray.direction.get_reflective(normal);
        reflectDir.normalize();

        // Offset (0.01), damit der Strahl nicht im eigenen Objekt startet
        Ray3df reflectRay = { hitPoint + (0.01f * reflectDir), reflectDir };

        Vector3df reflectedColor = trace(reflectRay, scene, depth + 1);
        
        // Farbe mischen: Bestehende Farbe + Reflektion
        finalColor += (mat.k_reflective * reflectedColor);
    }

    return finalColor;
}


// --- Main: Nutzung ---
int main() {
    // 1. Bildschirm definieren
    const int image_width = 1024;
    const int image_height = 1024;
    Screen screen(image_width, image_height);

    // 2. Kamera aufsetzen
    Vector3df eye({0.0f, 5.0f, 20.0f});   // Kamera Position
    Vector3df lookDir({0.0f, -0.1f, -1.0f}); // Blickrichtung (leicht nach unten)
    Vector3df worldUp({0.0f, 1.0f, 0.0f});
    float dist_to_focus = 10.0f;

    float aspectRatio = float(image_width) / image_height;
    
    float abstand = 10.0f;     // Das Bild schwebt 10 Einheiten vor der Kamera
    float pixelGroesse = 0.01f; // Ein Pixel ist 1cm groß (bei 1024px ist das Bild ca 10m breit)

    Camera cam(eye, lookDir, worldUp, abstand, pixelGroesse, image_width, image_height);

    std::vector<SceneObject> scene = createScene();

    // 3. Rendern (Ray Generation Loop)
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            Ray ray = cam.getRay(i, j);

            Vector3df pixelColor = trace(ray, scene, 0);


            pixelColor[0] = std::min(pixelColor[0], 1.0f);
            pixelColor[1] = std::min(pixelColor[1], 1.0f);
            pixelColor[2] = std::min(pixelColor[2], 1.0f);

            screen.setPixel(i, j, pixelColor);
        }
    }

    screen.saveToPPM("output.ppm");
    return 0;
}