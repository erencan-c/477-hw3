#ifndef __GRID_HPP__
#define __GRID_HPP__

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "wrapper.h"

const double CAMERA_LEFT = -10.0;
const double CAMERA_RIGHT = 10.0;
const double CAMERA_TOP = 10.0;
const double CAMERA_BOT = -10.0;

enum BunnyColor {
    NO_BUNNY,
    BCOLOR_RED,
    BCOLOR_GREEN,
    BCOLOR_BLUE,
    BCOLOR_PURPLE,
    BCOLOR_YELLOW,
};

const std::array<glm::vec3, 6> bunny_colors{glm::vec3{0}, glm::vec3{1,0,0}, glm::vec3{0,1,0}, glm::vec3{0,0,1}, glm::vec3{1,0,1}, glm::vec3{1,1,0}};



struct Bunny {
    BunnyColor color;
    glm::ivec2 pos;
    glm::ivec2 from;
    bool falling;
    bool growing;
    double t;
    double growth_t;

    Bunny() {
        color = NO_BUNNY;
        pos = glm::ivec2(0);
        from = glm::ivec2(0);
        falling = false;
        growing = false;
        t = 0;
        growth_t = 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const Bunny& bunny);
};

struct Grid {
private:
    bool is_matched(int x, int y, bool count = true);
public:
    int width;
    int height;
    int row;
    int col;
    glm::ivec2 hovered;
    std::vector<Bunny> grid;
    ModelData model;
    double fall_duration;
    double growth_duration = 50;
    float horizontal_distance;
    float vertical_distance;

    Grid();
    Grid(const int w, const int h, const int r, const int c, const ModelData m);
    void render(const float angle, Shader& shader);
    void destroy(int x, int y);
    bool fill();
    bool match();
    void animate();
};

#endif