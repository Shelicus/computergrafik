#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "renderer.h"
#include <GL/glew.h>
#include <SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include "physics.h"
#include "matrix.h"
#include "game.h"
#include "debug.h"
#include "wavefront.h" 

using SquareMatrix4df = SquareMatrix<float, 4>;

class OpenGLView {
private:
  GLuint vao;
  unsigned int shaderProgram;
  size_t vertices_count; 
  GLuint mode;

public:
  OpenGLView(GLuint vbo, unsigned int shaderProgram, size_t vertices_count, GLuint mode);
  virtual ~OpenGLView();
  virtual void render(SquareMatrix4df & matrice);
};

class TypedBodyView : public OpenGLView {
private:
  TypedBody * typed_body;
  float scale;
  float rotation_offset;
  float rotation;
  bool tilt_view;
  std::function<bool()> draw;
  std::function<void(TypedBodyView *)> modify;

  SquareMatrix4df create_object_transformation(Vector2df direction, float angle, float scale);

public:
  TypedBodyView(TypedBody * typed_body, GLuint vbo, unsigned int shaderProgram, size_t vertices_count, float scale, GLuint mode = GL_TRIANGLES,
               std::function<bool()> draw = []() -> bool {return true;},
               std::function<void(TypedBodyView *)> modify = [](TypedBodyView *) -> void {},
               float rotation = 0.0f, bool titl = false);

  void render(SquareMatrix4df & world) override;
  TypedBody * get_typed_body();
  void set_scale(float scale);
};

class OpenGLRenderer : public Renderer {
private:
  SDL_Window * window = nullptr;
  SDL_GLContext context;
  std::string title;
  int window_width;
  int window_height;

  // Shader und VBOs
  unsigned int shaderProgram;
  GLuint * vbos = nullptr;
  
  // Container für die geladenen Vertex-Daten
  std::vector<std::vector<float>> loaded_obj_data;

  // Views
  std::vector< std::unique_ptr<TypedBodyView> > views;
  std::unique_ptr<OpenGLView> spaceship_view;
  std::unique_ptr<OpenGLView> digit_views[10];
  
  // Indizes für die geladenen Objekte im vbos Array
  enum VboIndex {
      SHIP = 0,
      FLAME, 
      TORPEDO,
      SAUCER,
      ASTEROID_VEC_1, // Original Vektor Typ 1
      ASTEROID_VEC_2, // Original Vektor Typ 2
      ASTEROID_VEC_3, // Original Vektor Typ 3
      ASTEROID_OBJ,   // Wavefront OBJ Typ (Ersetzt Typ 4)
      DEBRIS_SHAPE,
      DIGIT_START 
  };

  void create_shader_programs();
  void createVbos();
  
  // Hilfsfunktionen
  void load_obj_file(const std::string& filename, std::vector<float>& target_container);
  void convert_vector_shape(const std::vector<Vector2df>& shape, std::vector<float>& target_container);
  std::vector<float> create_vertices_from_importer(WavefrontImporter& wi);

  void create(Spaceship * ship, std::vector< std::unique_ptr<TypedBodyView> > & views);
  void create(Torpedo * torpedo, std::vector< std::unique_ptr<TypedBodyView> > & views);
  void create(Asteroid * asteroid, std::vector< std::unique_ptr<TypedBodyView> > & views);
  void create(Saucer * saucer, std::vector< std::unique_ptr<TypedBodyView> > & views);
  void create(SpaceshipDebris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views);
  void create(Debris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views);

  void createSpaceShipView();
  void createDigitViews();

  void renderFreeShips(SquareMatrix4df & matrice);
  void renderScore(SquareMatrix4df & matrice);

public:
  OpenGLRenderer(Game & game, std::string title, int window_width = 1024, int window_height = 768)
    : Renderer(game), title(title), window_width(window_width), window_height(window_height) { }
  
  virtual ~OpenGLRenderer() { }

  bool init() override;
  void render() override;
  void exit() override;
};

#endif