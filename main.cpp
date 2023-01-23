#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h> 
#include <GLFW/glfw3.h> 
#include <ctime>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "wrapper.h"
#include "grid.h"
#include "text.h"

using namespace std;

[[unused]] std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << "[" << v.x << "," << v.y << "," << v.z << "]";
}

Grid gGrid;
bool is_animating = false;
long score = 0;
long dscore = 0;
long moves = 0;

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    } else if(key == GLFW_KEY_R and action == GLFW_PRESS) {
        gGrid = Grid(gGrid.width, gGrid.height, gGrid.row, gGrid.col, gGrid.model);
        score = 0;
        moves = 0;
    }
}

void resize(GLFWwindow* window, int width, int height) {
    gGrid.width = width;
    gGrid.height = height;
    glViewport(0, 0, width, height);
}

void mouse_moved(GLFWwindow* window, double xpos, double ypos) {
    xpos = std::min(std::max(0.0, xpos), (double)640);
    ypos = std::min(std::max(0.0, ypos), (double)600);
    const auto model_size = gGrid.model.size;
    const auto model_offset = gGrid.model.offset;
    const auto rectangler = glm::vec3(1.0f)/std::max(std::max(model_size.x, model_size.y), model_size.z);
    const auto scale = glm::vec3(glm::vec2(std::min(gGrid.horizontal_distance, gGrid.vertical_distance)), 1.0f)*rectangler;
    Bunny* nearest = &gGrid.grid[0];
    auto min_dist = INFINITY;
    ypos = (600 - ypos);
    for(int y = 1; y < gGrid.row; y++) {
        for(int x = 0; x < gGrid.col; x++) {
            auto& element = gGrid.grid[y*gGrid.col+x];
            float render_x = CAMERA_LEFT + x*gGrid.horizontal_distance;
            float render_y = CAMERA_TOP - (y)*gGrid.vertical_distance - 0.05*element.t;
            const auto grid_pos = glm::vec3(render_x, render_y, 0.0f);
            const auto offset_corrector = glm::vec3(model_offset.x, -model_size.y+model_offset.y, model_offset.z);

            const auto mvp = glm::ortho(-10.0, 10.0, -10.0, 10.0, -20.0, 20.0);
            const auto gl_scr_pos = mvp * (glm::vec4(grid_pos + glm::vec3(gGrid.horizontal_distance/2.0, gGrid.vertical_distance/2.0, 0.0), 1.0));
            const auto scr_pos = (glm::dvec4(640, 600, 0, 0) * (gl_scr_pos/2.0)) + glm::dvec4(320, 300, 0, 0);
            // std::cout << scr_pos << " " << glm::vec3(xpos, ypos, 0.0) << "\n";
            if(glm::distance(glm::dvec2(scr_pos), glm::dvec2(xpos, ypos)) < min_dist) {
                min_dist = glm::distance(glm::dvec2(scr_pos), glm::dvec2(xpos, ypos));
                nearest = &element;
            }
        }
    }
    // std::cout << "\n";
    gGrid.hovered = nearest->pos;
}

void mouse_clicked(GLFWwindow* window, int button, int action, int mods) {
    if(not is_animating) {
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            if(gGrid.hovered.x >= 0 and gGrid.hovered.y >= 1 and gGrid.hovered.x < gGrid.col and gGrid.hovered.y < gGrid.row) {
                gGrid.destroy(gGrid.hovered.x, gGrid.hovered.y);
                moves += 1;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    const char* error = NULL;
    if (!glfwInit()) {
        glfwGetError(&error);
        fprintf(stderr, "Error: %s\n", error);
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int width = 640, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "CENG477 - THE3 - Rabbit Crush", NULL, NULL);

    if (!window) {
        glfwGetError(&error);
        fprintf(stderr, "Error: %s\n", error);
        return EXIT_FAILURE;
    }


    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-10.0, 10.0, -10.0, 10.0, -20, 20);

    std::srand(std::time(nullptr));
    ModelData model = load_model(argv[3]);
    Shader shader("example_diffuse.shader");
    gGrid = Grid(width, height-54, std::strtol(argv[1], nullptr, 10)+1, std::strtol(argv[2], nullptr, 10), model);
    gGrid.match();

    FontData font = load_font("./LiberationSans.ttf");
    Shader text_shader("example_text.shader");

    glfwSetKeyCallback(window, keyboard);
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetCursorPosCallback(window, mouse_moved);
    glfwSetMouseButtonCallback(window, mouse_clicked);

    float angle = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gGrid.render(angle, shader);

        angle += 0.5f;

        gGrid.animate();

        const int text_len = std::snprintf(nullptr, 0, "Moves: %ld Score: %ld", moves, score);
        char* text = new char[text_len+1];
        std::sprintf(text, "Moves: %ld Score: %ld", moves, score);

        const float text_size = std::min(0.5f/gGrid.row, 0.03f);
        // std::cout << text_size << "\n";

        float next = draw_text(font, text_shader, text, glm::vec2(-10, -10), text_size, glm::vec3(1,1,0));
        if(is_animating) {
            draw_text(font, text_shader, " WAIT", glm::vec2(-10+next, -10), text_size, glm::vec3(1,0,0));
        } else {
            draw_text(font, text_shader, " MOVE", glm::vec2(-10+next, -10), text_size, glm::vec3(0,1,0));
        }
        delete[] text;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}