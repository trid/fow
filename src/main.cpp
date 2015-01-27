#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#define GLEW_STATIC

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

#include <string>
#include <fstream>
#include <complex>
#include <io.h>

using namespace std;

struct Vertex {
    float x, y, z;
    float u, v;
};

GLuint createTexture(const string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    int pitch = surface->pitch;
    int pixelSize = surface->format->BytesPerPixel;
    int pixel = ((int*)surface->pixels)[0];
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);


    return textureId;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "SDL_Init error" << endl;
        return 0;
    }
    int windowWidth = 800;
    int windowHeight = 600;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    if (GLenum i = glewInit()) {
        cout << glewGetErrorString(i) << endl;
    }

    Vertex vertices[4] = {{-1, -1, -1, 1, 1}, {1, -1, -1, 0, 1}, {-1, 1, -1, 1, 0}, {1, 1, -1, 0, 0}};

    //Fuck. I really wanted to use SDL render and textures, but look like it's pretty glitchy
    //GLuint background = createTexture("res/img/grass.jpg");
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "res/img/grass.jpg");


    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);

    //Shader loading. I'm fucking lazy
    std::ifstream vertexShaderFile("res/shaders/shader.vs");
    string vertexShaderData((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
    std::ifstream fragmentShaderFile("res/shaders/shader.fs");
    string fragmentShaderData((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());

    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    char const *cstrVertexShaderData = vertexShaderData.c_str();
    char const *cstrFragmentShaderData = fragmentShaderData.c_str();

    GLint result;
    GLint infoLogLength;

    glShaderSource(vertexShaderId, 1, &cstrVertexShaderData, NULL);
    glCompileShader(vertexShaderId);
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    char* log = new char[infoLogLength];
    glGetShaderInfoLog(vertexShaderId, infoLogLength, nullptr, log);

    glShaderSource(fragmentShaderId, 1, &cstrFragmentShaderData, NULL);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShaderId);
    glAttachShader(program, fragmentShaderId);

    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STATIC_DRAW);

    GLuint indicesBuffer;
    GLint indices[6] = { 0, 1, 2, 2, 1, 3};
    glGenBuffers(1, &indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &indices, GL_STATIC_DRAW);

    glLinkProgram(program);

    glUseProgram(program);

    glClearColor(0, 0, 0, 0);

    while (true) {
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, background);
        SDL_GL_BindTexture(backgroundTexture, nullptr, nullptr);
        const char* err = SDL_GetError();
        GLint textureId = glGetUniformLocation(program, "uTextureSampler");

        glUniform1i(textureId, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        float* ptr = 0;
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr + 3);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(0);

        SDL_GL_SwapWindow(window);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return 0;
            }
        }
    }

    return 0;
}