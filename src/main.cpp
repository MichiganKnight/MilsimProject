#include <SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string LoadFileToString(const std::string &path) {
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error("Failed to Open Shader File: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint CompileShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compile error: " << info << std::endl;
    }
    return shader;
}

GLuint CreateShaderProgramFromFiles(const std::string &verPath, const std::string &fragPath) {
    std::string vertSrc = LoadFileToString(verPath);
    std::string fragSrc = LoadFileToString(fragPath);

    auto compile = [](GLenum type, const char *src) -> GLuint {
        GLuint s = glCreateShader(type);

        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);

        GLint ok;

        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);

        if (!ok) {
            char log[512];
            glGetShaderInfoLog(s, 512, nullptr, log);
            throw std::runtime_error("Shader Compile Error:\n" + std::string(log));
        }

        return s;
    };

    GLuint vert = compile(GL_VERTEX_SHADER, vertSrc.c_str());
    GLuint frag = compile(GL_FRAGMENT_SHADER, fragSrc.c_str());

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);

    if (!ok) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        throw std::runtime_error("Program Link Error: " + std::string(log));
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    return program;
}

float cubeVertices[] = {
    // positions          // texcoords
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,

    0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("Milsim FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
                                          SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(1);

    GLuint shader = 0;
    try {
        shader = CreateShaderProgramFromFiles("shaders/basic.vert",
                                              "shaders/basic.frag");
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << '\n';
        SDL_Quit();
        return 1;
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, channels;
    unsigned char *data = stbi_load("/textures/wall.jpg", &w, &h, &channels, 0);
    if (data) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture\n";
    }
    stbi_image_free(data);

    glm::vec3 camPos = {0, 0, 5}, camFront = {0, 0, -1}, camUp = {0, 1, 0};
    float yaw = -90.0f, pitch = 0.0f, lastX = 400, lastY = 300, deltaTime = 0, lastFrame = 0;
    bool firstMouse = true, running = true;

    SDL_Event e;
    while (running) {
        float now = SDL_GetTicks() / 1000.0f;
        deltaTime = now - lastFrame;
        lastFrame = now;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_MOUSEMOTION) {
                float x = e.motion.xrel, y = e.motion.yrel;
                float sens = 0.1f;
                yaw += x * sens;
                pitch -= y * sens;
                if (pitch > 89.0f) pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;
                glm::vec3 dir;
                dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                dir.y = sin(glm::radians(pitch));
                dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                camFront = glm::normalize(dir);
            }
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        float speed = 2.5f * deltaTime;
        if (keys[SDL_SCANCODE_W]) camPos += camFront * speed;
        if (keys[SDL_SCANCODE_S]) camPos -= camFront * speed;
        if (keys[SDL_SCANCODE_A]) camPos -= glm::normalize(glm::cross(camFront, camUp)) * speed;
        if (keys[SDL_SCANCODE_D]) camPos += glm::normalize(glm::cross(camFront, camUp)) * speed;

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(shader);
        glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.f);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);

        glm::mat4 model = glm::mat4(1.0f);
        // Floor
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 0.1f, 10.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Walls
        auto wall = [&](float x, float z) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x, 1.5f, z));
            model = glm::scale(model, glm::vec3(0.1f, 3.0f, 10.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        };
        wall(-5.0f, 0.0f); // left
        wall(5.0f, 0.0f); // right

        // Ceiling
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 0.1f, 10.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
