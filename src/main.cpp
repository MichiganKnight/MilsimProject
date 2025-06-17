#include <glad/glad.h>
#include <SDL.h>
#include <iostream>

int main(int argc, char* args[]) {
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Could Not Be Initialized. SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create a Window Centered on the Screen
    SDL_Window* window = SDL_CreateWindow("Milsim FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    if (!glContext) {
        std::cerr << "Failed to Create OpenGL Context. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to Initialize GLAD. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // Enable VSync

    if (!window) {
        std::cerr << "Window Could Not Be Created. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Main Loop Flag
    bool running = true;

    // SDL Event Handler
    SDL_Event e;

    while (running) {
        // Handle Queued Events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false; // Exit Loop When Window is Closed
            }
        }

        // Render Logic
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap Buffers
        SDL_GL_SwapWindow(window);

        // Delay to Avoid Maxing Out CPU
        SDL_Delay(16); // ~60 FPS Cap (1000ms / 60 ≈ 16)
    }

    // Clean Up and Close Window
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}