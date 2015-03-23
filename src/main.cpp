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
#include "view.h"

using namespace std;


int main(int argc, char* argv[]) {
    View& view = View::getInstance();

    while (true) {
        view.draw();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return 0;
            }
        }
    }

    return 0;
}