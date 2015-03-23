#include "view.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

View::View() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "SDL_Init error" << endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    glContext = SDL_GL_CreateContext(window);

    //Enable OpenGL dynamics
    glewExperimental = GL_TRUE;
    if (GLenum i = glewInit()) {
        cout << glewGetErrorString(i) << endl;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //Creating screen surface
    Vertex vertices[4] = {{-1, -1, -1, 1, 1}, {1, -1, -1, 0, 1}, {-1, 1, -1, 1, 0}, {1, 1, -1, 0, 0}};

    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STATIC_DRAW);

    GLint indices[6] = { 0, 1, 2, 2, 1, 3};
    glGenBuffers(1, &indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &indices, GL_STATIC_DRAW);

    //Loading background grass texture
    backgroundTexture = loadTexture("res/img/grass.jpg");

    // Shader loading is pretty long, so it's moved to other function
    loadShaders();

    //Black is just classic!
    glClearColor(0, 0, 0, 0);
}

void View::loadShaders() {
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

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);

    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);
}

GLuint View::loadTexture(const string &path) {
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

void View::draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    const char* err = SDL_GetError();
    GLint textureId = glGetUniformLocation(shaderProgram, "uTextureSampler");

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
}
