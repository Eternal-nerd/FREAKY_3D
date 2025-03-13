#include <SDL3/SDL_main.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "engine.h"

// Program entry point
int main(int argv, char** args) {
    std::cout << "main function invocation\n";

    Engine eng;

    try {
        eng.run();
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::cout << "main function exit failure\n";
        return EXIT_FAILURE;
    }

    std::cout << "main function exit success\n";
    return EXIT_SUCCESS;
}
