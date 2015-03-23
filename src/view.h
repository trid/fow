#include <string>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#define GLEW_STATIC

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>

using std::string;

struct Vertex {
    float x, y, z;
    float u, v;
};

class View {
private:
    View();
    GLuint loadTexture(const string &path);
    void loadShaders();

    int windowWidth = 800;
    int windowHeight = 600;

    SDL_Window* window;
    SDL_GLContext glContext;

    GLuint backgroundTexture;
    GLuint vertexBuffer;
    GLuint indicesBuffer;
    GLuint shaderProgram;
public:
    static View& getInstance() {
        static View view;
        return view;
    }
    void draw();
};