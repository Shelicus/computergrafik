#include "opengl_renderer.h"
#include <cassert>
#include <span>
#include <utility>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Original Vektor-Daten aus dem alten Code ---
std::vector<Vector2df> asteroid_vec_1 = {
  Vector2df{ 0, -12},
  Vector2df{16, -24},
  Vector2df{32, -12},
  Vector2df{24, 0},
  Vector2df{32, 12},
  Vector2df{8, 24}, 
  Vector2df{-16, 24}, 
  Vector2df{-32, 12}, 
  Vector2df{-32, -12}, 
  Vector2df{-16, -24},
  Vector2df{0, -12}
};

std::vector<Vector2df> asteroid_vec_4 = {
  Vector2df{-16, 0}, 
  Vector2df{-32, 6}, 
  Vector2df{-16, 24}, 
  Vector2df{0, 6}, 
  Vector2df{0, 24}, 
  Vector2df{16, 24},
  Vector2df{32, 6}, 
  Vector2df{32, 6}, 
  Vector2df{16, -24}, 
  Vector2df{-8, -24}, 
  Vector2df{-32, -6}
};

std::vector<Vector2df> asteroid_vec_3 = {
  Vector2df{6, -6},
  Vector2df{6, -6},
  Vector2df{32, -12},
  Vector2df{16, -24}, 
  Vector2df{0, -16}, 
  Vector2df{-16, -24}, 
  Vector2df{-24, -12},
  Vector2df{-16, -0}, 
  Vector2df{-32, 12}, 
  Vector2df{-16, 24}, 
  Vector2df{-8, 16}, 
  Vector2df{16, 24}, 
  Vector2df{32, 6}, 
  Vector2df{16, -6},};

  std::vector<Vector2df> asteroid_vec_2 = {
  Vector2df{8,0}, 
  Vector2df{32,-6}, 
  Vector2df{32, -12}, 
  Vector2df{8, -24}, 
  Vector2df{-16, -24}, 
  Vector2df{-8, -12}, 
  Vector2df{-32, -12}, 
  Vector2df{-32, 12}, 
  Vector2df{-16, 24}, 
  Vector2df{8, 16}, 
  Vector2df{16, 24}, 
  Vector2df{32, 12}, 
  Vector2df{8, 0}
};

// Hilfsdaten für Ziffern
std::vector<Vector2df> raw_digit_0 = { {0,-8}, {4,-8}, {4,0}, {0,0}, {0, -8} };
std::vector<Vector2df> raw_digit_1 = { {4,0}, {4,-8} };
std::vector<Vector2df> raw_digit_2 = { {0,-8}, {4,-8}, {4,-4}, {0,-4}, {0,0}, {4,0}  };
std::vector<Vector2df> raw_digit_3 = { {0,0}, {4, 0}, {4,-4}, {0,-4}, {4,-4}, {4, -8}, {0, -8}  };
std::vector<Vector2df> raw_digit_4 = { {4,0}, {4,-8}, {4,-4}, {0,-4}, {0,-8}  };
std::vector<Vector2df> raw_digit_5 = { {0,0}, {4,0}, {4,-4}, {0,-4}, {0,-8}, {4, -8}  };
std::vector<Vector2df> raw_digit_6 = { {0,-8}, {0,0}, {4,0}, {4,-4}, {0,-4} };
std::vector<Vector2df> raw_digit_7 = { {0,-8}, {4,-8}, {4,0} };
std::vector<Vector2df> raw_digit_8 = { {0,-8}, {4,-8}, {4,0}, {0,0}, {0,-8}, {0, -4}, {4, -4} };
std::vector<Vector2df> raw_digit_9 = { {4, 0}, {4,-8}, {0,-8}, {0, -4}, {4, -4} };

std::vector<std::vector<Vector2df>*> raw_digits = {
    &raw_digit_0, &raw_digit_1, &raw_digit_2, &raw_digit_3, &raw_digit_4,
    &raw_digit_5, &raw_digit_6, &raw_digit_7, &raw_digit_8, &raw_digit_9
};

// Material-Definition
struct DefaultMaterial {
    float ambient[3] = {1.0f, 1.0f, 1.0f};
};
static DefaultMaterial default_mat;

// --- Implementation OpenGLView ---

OpenGLView::OpenGLView(GLuint vbo, unsigned int shaderProgram, size_t vertices_count, GLuint mode)
  : shaderProgram(shaderProgram), vertices_count(vertices_count), mode(mode) {
  
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  size_t stride = 9 * sizeof(float);

  // Location 0: Position (vec3)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(0);

  // Location 1: Color (vec3)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Location 2: Normal (vec3)
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0); 
}

OpenGLView::~OpenGLView() {
  glDeleteVertexArrays(1, &vao);
}

void OpenGLView::render( SquareMatrix<float,4> & matrice) {
  glBindVertexArray(vao);
  glUseProgram(shaderProgram);
  unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &matrice[0][0] );
  glDrawArrays(mode, 0, vertices_count );
}

// --- Implementation TypedBodyView ---


TypedBodyView::TypedBodyView(TypedBody * typed_body, GLuint vbo, unsigned int shaderProgram, size_t vertices_count, float scale, GLuint mode,
             std::function<bool()> draw, std::function<void(TypedBodyView *)> modify, float rotation, bool tilt)
      : OpenGLView(vbo, shaderProgram, vertices_count, mode),  
        typed_body(typed_body), 
        scale(scale), 
        rotation(rotation), 
        draw(draw), 
        modify(modify), tilt_view(tilt) {
}

SquareMatrix4df TypedBodyView::create_object_transformation(Vector2df direction, float angle, float scale) {
  SquareMatrix4df translation = { {1.0f,        0.0f,          0.0f, 0.0f},
                                  {0.0f,        1.0f,          0.0f, 0.0f},
                                  {0.0f,        0.0f,          1.0f, 0.0f},
                                  {direction[0], direction[1], 0.0f, 1.0f}
                                };
  

  float final_angle = angle + rotation;

  float c = std::cos(final_angle);
  float s = std::sin(final_angle);

  SquareMatrix4df correction_x;
  correction_x = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
  if (this->tilt_view) {
      float cr = std::cos(M_PI/2.0 );
      float sr = std::sin(M_PI/2.0 );
      correction_x = { 
          { 1.0f, 0.0f, 0.0f, 0.0f},
          { 0.0f,   cr,   sr, 0.0f},
          { 0.0f,  -sr,   cr, 0.0f},
          { 0.0f, 0.0f, 0.0f, 1.0f}
      };
  }
  
  SquareMatrix4df rotation_mat = { { c,    s,    0.0f, 0.0f},
                               {-s,    c,    0.0f, 0.0f},
                               { 0.0f, 0.0f, 1.0f, 0.0f},
                               { 0.0f, 0.0f, 0.0f, 1.0f}
                             };
                             
  SquareMatrix4df scaling = { { scale, 0.0f, 0.0f, 0.0f},
                              { 0.0f, scale, 0.0f, 0.0f},
                              { 0.0f, 0.0f, scale, 0.0f},
                              { 0.0f, 0.0f, 0.0f, 1.0f}
                            };                                 

  return translation * rotation_mat * correction_x * scaling;
}

void TypedBodyView::render( SquareMatrix<float,4> & world) {
  if ( draw() ) {
    modify(this);
    auto transform = world * create_object_transformation(typed_body->get_position(), typed_body->get_angle(), scale);
    OpenGLView::render(transform);
  }
}

TypedBody * TypedBodyView::get_typed_body() {
 return typed_body;
}

void TypedBodyView::set_scale(float scale) {
 this->scale = scale;
}

// --- Implementation OpenGLRenderer ---

std::vector<float> OpenGLRenderer::create_vertices_from_importer(WavefrontImporter & wi) {
  std::vector<float> vertices;
  
  for (Face face : wi.get_faces() ) {
    for (ReferenceGroup group : face.reference_groups ) {
      // Vertex (x, y, z)
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( group.vertice[i]);
      }
      // Normal (nx, ny, nz)
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( group.normal[i] );
      }
      // Color (r, g, b)
      if (face.material == nullptr) {
          for (size_t i = 0; i < 3; i++) vertices.push_back(default_mat.ambient[i]);
      } else {
          for (size_t i = 0; i < 3; i++) {
            vertices.push_back( face.material->ambient[i]);
          }
      }
    } 
  }
  return vertices;
}

void OpenGLRenderer::load_obj_file(const std::string& filename, std::vector<float>& target_container) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        // Fallback Triangle
        target_container = { 
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
             0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f
        };
        return;
    }
    WavefrontImporter wi(in);
    wi.parse();
    target_container = create_vertices_from_importer(wi);
    std::cout << "Loaded " << filename << " with " << target_container.size() / 9 << " vertices." << std::endl;
}

void OpenGLRenderer::convert_vector_shape(const std::vector<Vector2df>& shape, std::vector<float>& target_container) {
    for(const auto& p : shape) {
         // Position (x, y, 0)
         target_container.push_back(p[0]); 
         target_container.push_back(p[1]); 
         target_container.push_back(0.0f);
         // Normal (0, 0, 1)
         target_container.push_back(0.0f); 
         target_container.push_back(0.0f); 
         target_container.push_back(1.0f);
         // Color (1, 1, 1)
         target_container.push_back(1.0f); 
         target_container.push_back(1.0f); 
         target_container.push_back(1.0f);
     }
}

void OpenGLRenderer::createVbos() {
 loaded_obj_data.resize(DIGIT_START + 10);
 
 load_obj_file("roket.obj", loaded_obj_data[SHIP]);
 load_obj_file("flame.obj", loaded_obj_data[FLAME]); 
 load_obj_file("torpedo.obj", loaded_obj_data[TORPEDO]);
 load_obj_file("ufo.obj", loaded_obj_data[SAUCER]);
 
 convert_vector_shape(asteroid_vec_1, loaded_obj_data[ASTEROID_VEC_1]);
 convert_vector_shape(asteroid_vec_2, loaded_obj_data[ASTEROID_VEC_2]);
 convert_vector_shape(asteroid_vec_3, loaded_obj_data[ASTEROID_VEC_3]);
 
 load_obj_file("Asteroid.obj", loaded_obj_data[ASTEROID_OBJ]); 

 loaded_obj_data[DEBRIS_SHAPE] = loaded_obj_data[ASTEROID_OBJ];

 for (size_t i = 0; i < 10; i++) {
     convert_vector_shape(*raw_digits[i], loaded_obj_data[DIGIT_START + i]);
 }

 vbos = new GLuint[loaded_obj_data.size()];
 glGenBuffers(loaded_obj_data.size(), vbos);

 for (size_t i = 0; i < loaded_obj_data.size(); i++) {
   glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
   glBufferData(GL_ARRAY_BUFFER, loaded_obj_data[i].size() * sizeof(float), loaded_obj_data[i].data(), GL_STATIC_DRAW);
 }
}

void OpenGLRenderer::create_shader_programs() {
  const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n" 
    "layout (location = 1) in vec3 incolor;\n"
    "layout (location = 2) in vec3 innormal;\n"
    "out vec3 color;\n"
    "uniform mat4 model;\n" 
    "void main()\n"
    "{\n"
    "   gl_Position = model * vec4(position, 1.0);\n"
    "   vec3 normWorld = normalize((model * vec4(innormal, 0.0)).xyz);\n"
    "   vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0));\n"
    "   float diff = max(dot(normWorld, lightDir), 0.0);\n"
    "   vec3 lighting = incolor * (0.3 + 0.7 * diff);\n"
    "   color = lighting;\n"
    "}\0";

  const char *fragmentShaderSource = "#version 330 core\n"
  "out vec4 outColor;\n"
  "in vec3 color;\n"
  "void main () {\n"
  "  outColor = vec4(color, 1.0);\n"
  "}\n\0";
  
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "vertex shader compilation failed: " << infoLog << std::endl;
        throw std::runtime_error("Shader compilation failed");
    }
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "fragment shader compilation failed: " << infoLog << std::endl;
        throw std::runtime_error("Shader compilation failed");
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "linking shader programs failed: " << infoLog << std::endl;
        throw std::runtime_error("Shader linking failed");
    }
}

void OpenGLRenderer::create(Spaceship * ship, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  float scale = 3.0f;
  
  float rotation = 2*static_cast<float>(M_PI) / 2.0f;

  views.push_back(std::make_unique<TypedBodyView>(
                  ship, 
                  vbos[SHIP], 
                  shaderProgram, 
                  loaded_obj_data[SHIP].size() / 9, 
                  scale, 
                  GL_TRIANGLES,
                  [ship]() -> bool {return ! ship->is_in_hyperspace();},
                  [](TypedBodyView *) -> void {},
                  rotation 
                 ));   
}

void OpenGLRenderer::create(Saucer * saucer, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  float scale = (saucer->get_size() == 0) ? 8.0f : 15.0f;
  float rotation = 2*-static_cast<float>(M_PI) / 2.0f;
  views.push_back(std::make_unique<TypedBodyView>(saucer, vbos[SAUCER], shaderProgram, loaded_obj_data[SAUCER].size() / 9, scale, GL_TRIANGLES, 
                  []() -> bool {return true;}, 
                  [](TypedBodyView *) -> void {}, 
                  rotation,true));   
}

void OpenGLRenderer::create(Torpedo * torpedo, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  float rotation = -static_cast<float>(M_PI) / 2.0f;
  views.push_back(std::make_unique<TypedBodyView>(torpedo, vbos[TORPEDO], shaderProgram, loaded_obj_data[TORPEDO].size() / 9, 1.0f, GL_TRIANGLES, 
                  []() -> bool {return true;}, 
                  [](TypedBodyView *) -> void {}, 
                  rotation)); 
}

void OpenGLRenderer::create(Asteroid * asteroid, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  float scale_factor = (asteroid->get_size() == 3 ? 1.0f : ( asteroid->get_size() == 2 ? 0.5f : 0.25f ));
  int type = asteroid->get_rock_type(); 
  if (type == 3) {
      float rotation = static_cast<float>(M_PI);
      views.push_back(std::make_unique<TypedBodyView>(asteroid, vbos[ASTEROID_OBJ], shaderProgram, loaded_obj_data[ASTEROID_OBJ].size() / 9, scale_factor * 18.0f, GL_TRIANGLES, 
                      []() -> bool {return true;}, 
                      [](TypedBodyView *) -> void {}, 
                      rotation)); 
  } else {
      GLuint vbo_index = ASTEROID_VEC_1 + type;
      if (type > 2) vbo_index = ASTEROID_VEC_1;

      // Vektor Modell - Rotation 0.0f
      views.push_back(std::make_unique<TypedBodyView>(asteroid, vbos[vbo_index], shaderProgram, loaded_obj_data[vbo_index].size() / 9, scale_factor, GL_LINE_LOOP, 
                      []() -> bool {return true;}, 
                      [](TypedBodyView *) -> void {}, 
                      0.0f)); 
  }
}

void OpenGLRenderer::create(SpaceshipDebris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  float obj_size_factor = 5.0f; 
  views.push_back(std::make_unique<TypedBodyView>(debris, vbos[FLAME], shaderProgram, loaded_obj_data[FLAME].size() / 9, 0.1f, GL_TRIANGLES,
            []() -> bool {return true;},
            [debris, obj_size_factor](TypedBodyView * view) -> void { 
                view->set_scale( obj_size_factor * (SpaceshipDebris::TIME_TO_DELETE - debris->get_time_to_delete()));
            },
            0.0f)); 
}

void OpenGLRenderer::create(Debris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  float obj_size_factor = 0.2f;
  views.push_back(std::make_unique<TypedBodyView>(debris, vbos[FLAME], shaderProgram, loaded_obj_data[FLAME].size() / 9, 0.1f, GL_TRIANGLES,
            []() -> bool {return true;},
            [debris, obj_size_factor](TypedBodyView * view) -> void { 
                view->set_scale(obj_size_factor * (Debris::TIME_TO_DELETE - debris->get_time_to_delete()));
            },
            0.0f)); 
}

void OpenGLRenderer::createSpaceShipView() {
  spaceship_view = std::make_unique<OpenGLView>(vbos[SHIP], shaderProgram, loaded_obj_data[SHIP].size() / 9, GL_TRIANGLES);
}

void OpenGLRenderer::createDigitViews() {
  for (size_t i = 0; i < 10; i++ ) {
    digit_views[i] = std::make_unique<OpenGLView>(vbos[DIGIT_START + i], shaderProgram, loaded_obj_data[DIGIT_START + i].size() / 9, GL_LINE_STRIP);
  }
}

void OpenGLRenderer::renderFreeShips(SquareMatrix4df & matrice) {
  constexpr float FREE_SHIP_X = 128;
  constexpr float FREE_SHIP_Y = 64;
  const float PIf = static_cast<float> ( M_PI );
  Vector2df position = {FREE_SHIP_X, FREE_SHIP_Y};
  float angle = static_cast<float>(M_PI) / 2.0f;
  float c = std::cos(angle);
  float s = std::sin(angle);
  SquareMatrix4df rotation = {   { c,    s,    0.0f, 0.0f},
      {-s,    c,    0.0f, 0.0f},
      { 0.0f, 0.0f, 1.0f, 0.0f},
      { 0.0f, 0.0f, 0.0f, 1.0f}
  };
  SquareMatrix4df scale = { {3.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 3.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} };

  for (int i = 0; i < game.get_no_of_ships(); i++) {
    SquareMatrix4df  translation= { {1.0f,        0.0f,         0.0f, 0.0f},
                                    {0.0f,        1.0f,         0.0f, 0.0f},
                                    {0.0f,        0.0f,         1.0f, 0.0f},
                                    {position[0], position[1],  0.0f, 1.0f} };
    SquareMatrix4df render_matrice = matrice * translation * rotation * scale;
    spaceship_view->render( render_matrice );
    position[0] += 20.0;
  }
}

void OpenGLRenderer::renderScore(SquareMatrix4df & matrice) {
  constexpr float SCORE_X = 128 - 48;
  constexpr float SCORE_Y = 48 - 4;
  
  long long score = game.get_score();
  int no_of_digits = 0;
  if (score > 0) {
    no_of_digits = std::trunc( std::log10( score ) ) + 1;
  }

  Vector2df position = {SCORE_X + 20.0f * no_of_digits,  SCORE_Y};  
  do {
    int d = score % 10;
    score /= 10;
    SquareMatrix4df scale_translation= { {4.0f,        0.0f,         0.0f, 0.0f},
                                         {0.0f,        4.0f,         0.0f, 0.0f},
                                         {0.0f,        0.0f,         1.0f, 0.0f},
                                         {position[0], position[1],  0.0f, 1.0f} };
    SquareMatrix4df render_matrice = matrice * scale_translation;
    digit_views[d]->render( render_matrice );
    no_of_digits--;
    position[0] -= 20;

  } while (no_of_digits > 0);
}


bool OpenGLRenderer::init() {
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    std::cerr << "Could not initialize SDL. SDLError: " << SDL_GetError() << std::endl;
  } else {
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if( window == nullptr ) {
      std::cerr << "Could not create Window. SDLError: " << SDL_GetError() << std::endl;
    } else {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

      context = SDL_GL_CreateContext(window);
      
      GLenum err = glewInit();
      if (GLEW_OK != err) {
        std::cerr << "Could not initialize Glew. Glew error: " << glewGetErrorString(err) << std::endl;
        return false;
      }

      SDL_GL_SetSwapInterval(1);
      
      glEnable(GL_DEPTH_TEST);
      glFrontFace(GL_CW);

      create_shader_programs();
      createVbos();
      createSpaceShipView();
      createDigitViews();
      return true;
    }
  }
  return false;
}

static Vector2df tile_positions [] = {
                         {0.0f, 0.0f},
                         {1024.0f, 0.0f},
                         {1024.0f, 768.0f},
                         {1024.0f, -768.0f},
                         {-1024.0f, 0.0f},
                         {-1024.0f, 768.0f},
                         {-1024.0f, -768.0f},
                         {0.0f, 768.0f},
                         {0.0f, -768.0f} };

void OpenGLRenderer::render() {
  SquareMatrix4df world_transformation =
                         SquareMatrix4df{
                           { 2.0f / 1024.0f,           0.0f,            0.0f,  0.0f},
                           {       0.0f,     -2.0f / 768.0f,            0.0f,  0.0f},
                           {       0.0f,               0.0f,  2.0f / 1024.0f,  0.0f},
                           {      -1.0f,               1.0f,           0.0f,  1.0f}
                         };

  SquareMatrix4df view_transformation = world_transformation;

  if (game.ship_exists()) {
    TypedBody* ship = game.get_ship(); 
    Vector2df ship_pos = ship->get_position();

    float dx = 512.0f - ship_pos[0];
    float dy = 384.0f - ship_pos[1];

    SquareMatrix4df camera_translation = { 
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {dx,   dy,   0.0f, 1.0f}
    };

    view_transformation = world_transformation * camera_translation;
  }

  glClearColor ( 0.0, 0.0, 0.0, 1.0 );
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  std::erase_if(views, []( std::unique_ptr<TypedBodyView> & view) { return view->get_typed_body()->is_marked_for_deletion();}); 

  auto new_bodies = game.get_physics().get_recently_added_bodies();
  for (Body2df * body : new_bodies) {
    assert(body != nullptr);
    TypedBody * typed_body = static_cast<TypedBody *>(body);
    auto type = typed_body->get_type();
    
    if (type == BodyType::spaceship) {
      create( static_cast<Spaceship *>(typed_body), views );
    } else if (type == BodyType::torpedo ) {
      create( static_cast<Torpedo *>(typed_body), views );
    } else  if (type == BodyType::asteroid) {
      create( static_cast<Asteroid *>(typed_body), views );
    } else if (type == BodyType::saucer) {
      create( static_cast<Saucer *>(typed_body), views );
    } else if (type == BodyType::spaceship_debris ) {
      create( static_cast<SpaceshipDebris *>(typed_body), views );
    } else if (type == BodyType::debris) {
      create( static_cast<Debris *>(typed_body), views );
    }
  }

  for (auto & view : views) {
    for (const auto & tile_pos : tile_positions) {
      SquareMatrix4df tile_translation = { 
          {1.0f, 0.0f, 0.0f, 0.0f},
          {0.0f, 1.0f, 0.0f, 0.0f},
          {0.0f, 0.0f, 1.0f, 0.0f},
          {tile_pos[0], tile_pos[1], 0.0f, 1.0f}
      };
      SquareMatrix4df final_transform = view_transformation * tile_translation;

      view->render( final_transform);
    }
  }
  
  renderFreeShips(world_transformation);
  renderScore(world_transformation);

  SDL_GL_SwapWindow(window);
}

void OpenGLRenderer::exit() {
  views.clear();
  spaceship_view.reset();
  for(int i=0; i<10; ++i) digit_views[i].reset();

  if (vbos) {
    glDeleteBuffers(DIGIT_START + 10, vbos);
    delete[] vbos;
  }
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow( window );
  SDL_Quit();
}