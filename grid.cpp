#include "grid.h"
#include "wrapper.h"

#include <iostream>

extern bool is_animating;
extern long score;
extern long moves;
extern long dscore;

#define AT(x,y) ((y)*col + (x))

std::ostream& operator<<(std::ostream& os, const glm::ivec2& vec) {
    return os << "[" << vec.x << "," << vec.y << "]";
}

std::ostream& operator<<(std::ostream& os, const Bunny& bunny) {
    return os << "Bunny(" << bunny.color << ", " << bunny.pos << ", " << bunny.falling << ", " << bunny.growing << ", " << bunny.t << ", " << bunny.growth_t << ")";
}

Grid::Grid() {

}

Grid::Grid(const int w, const int h, const int r, const int c, const ModelData m) {
    width = w;
    height = h;
    row = r;
    col = c;
    model = m;
    fall_duration = ((CAMERA_TOP-CAMERA_BOT)*20.0)/row;
    grid.resize(r*c, Bunny());
    vertical_distance = (CAMERA_TOP-CAMERA_BOT)/row;
    horizontal_distance = (CAMERA_RIGHT-CAMERA_LEFT)/col;
    for(size_t i = col; i < grid.size(); i++) {
        bool has_matched = true;
        while(has_matched) {
            grid[i].color = (BunnyColor)((rand() % BCOLOR_YELLOW) + 1);
            has_matched = is_matched(i % col, i / col, false);
        }
        grid[i].pos = glm::ivec2(i % col, i / col);
    }
} 

void Grid::render(const float angle, Shader& shader) {
    // static bool has_out = false;
    const auto model_size = model.size;
    const auto model_offset = model.offset;
    const auto rectangler = glm::vec3(1.0f)/std::max(std::max(model_size.x, model_size.y), model_size.z);
    const auto scale = glm::vec3(glm::vec2(std::min(horizontal_distance, vertical_distance)), 1.0f)*rectangler;

    for(int y = 0; y < row; y++) {
        for(int x = 0; x < col; x++) {
            auto& element = grid[y*col+x];
            // std::cout << element.color << element.pos << element.falling << " ";
            if(grid[y*col+x].color) {
                float render_x = CAMERA_LEFT + x*horizontal_distance;
                float render_y = CAMERA_TOP - (y-1)*vertical_distance - 0.05*element.t;
                const auto grid_pos = glm::vec3(render_x, render_y, 0.0f);
                const auto offset_corrector = glm::vec3(model_offset.x, -model_size.y+model_offset.y, model_offset.z);
                draw_model(
                    model,
                    shader,
                    grid_pos + (offset_corrector)*scale /* + glm::vec3(std::max(vertical_distance, horizontal_distance), 0, 0) */,
                    angle,
                    scale * glm::vec3(1.0 + element.growth_t/100.0),
                    (glm::ivec2(x,y) == hovered ? glm::clamp(bunny_colors[grid[y*col+x].color]*0.70f + glm::vec3(0.35f), 0.0f, 1.0f) : bunny_colors[grid[y*col+x].color])
                );
            }
        }
        // std::cout << "\n";
    }
    // std::cout << score << " " << moves << "\n";
    // has_out = true;
}

void Grid::destroy(int x, int y) {
    // grid[AT(x,y)].color = NO_BUNNY;
    grid[AT(x,y)].growing = true;
    grid[AT(x,y)].growth_t = 0;
}

bool Grid::fill() {
    bool has_filled = false;
    auto gap_counts = std::vector<int>(col);
    for(int y = row-1; y > 0; y--) {
        for(int x = 0; x < col; x++) {
            if(y == 0) std::cout << "OOO\n";
            // std::cout << grid[(row-y-1)*col + x].color << " ";
            if(grid[y*col + x].color == NO_BUNNY) {
                gap_counts[x] += 1;
                has_filled = true;
                for(int i = y-1; i > 0; i--) {
                    grid[i*col + x].t = 0;
                    grid[i*col + x].falling = true;
                }
            }
        }
        // std::cout << "\n";
    }
    for(int x = 0; x < col; x++) {
        if(gap_counts[x] != 0) {
            grid[x] = Bunny();
            grid[x].color = (BunnyColor)((rand() % BCOLOR_YELLOW) + 1);
            grid[x].pos = glm::ivec2(x, 0);
            grid[x].falling = true;
            grid[x].t = 0;
        } else {
            grid[x].color = NO_BUNNY;
        }
    }
    // std::cout << "\n";
    return has_filled;
}

bool Grid::is_matched(int x, int y, bool count) {
    int horizontal_neighbors = 1;
    int vertical_neighbors = 1;
    for(int i = y+1; i < row and grid[AT(x,y)].color == grid[AT(x,i)].color; i++) {
        vertical_neighbors += 1;
    }
    for(int i = y-1; i > 0 and grid[AT(x,y)].color == grid[AT(x,i)].color; i--) {
        vertical_neighbors += 1;
    }
    for(int i = x+1; i < col and grid[AT(x,y)].color == grid[AT(i,y)].color; i++) {
        horizontal_neighbors += 1;
    }
    for(int i = x-1; i >= 0 and grid[AT(x,y)].color == grid[AT(i,y)].color; i--) {
        horizontal_neighbors += 1;
    }
    // std::cout << x << " " << y << ": " << horizontal_neighbors << " " << vertical_neighbors << "\n";
    bool match = horizontal_neighbors >= 3 or vertical_neighbors >= 3;
    if(count and match) {
        dscore += 1;
    }
    return match;
}

bool Grid::match() {
    const bool output = false;
    bool has_destroyed = false;
    auto backup = grid;
    for(int y = 1; y < row; y++) {
        for(int x = 0; x < col; x++) {
            // std::cout << x << ", " << y << "\n";
            if(is_matched(x, y)) {
                backup[AT(x,y)].growing = true;
                backup[AT(x,y)].growth_t = 0;
                has_destroyed = true;
            }
        }
    }
    if(output) std::cout << "match:\n";
    for(int y = 0; y < row; y++) {
        for(int x = 0; x < col; x++) {
            if(output) std::cout << backup[AT(x,y)].color << " ";
        }
        if(output) std::cout << "\n";
    }
    if(output) std::cout << "\n";
    if(has_destroyed) {
        grid = backup;
    }
    return has_destroyed;
}

#include <memory>

void Grid::animate() {
    bool has_grown = false;
    bool fall_finished = true;
    for(int y = row-1; y >= 0; y--) {
        for(int x = 0; x < col; x++) {
            auto& element = grid[AT(x,y)];
            is_animating = true;
            if(element.growing) {
                has_grown = true;
                element.growth_t += 1;
                if(element.growth_t >= growth_duration) {
                    element.growth_t = 0;
                    element.growing = false;
                    element.color = NO_BUNNY;
                    score += dscore;
                    dscore = 0;
                }
            } else if(element.falling) {
                element.t += 1;
                if(element.t >= fall_duration) {
                    element.t = 0;
                    element.falling = false;
                    element.pos.y += 1;
                    grid[AT(x,y+1)] = element;
                } else {
                    fall_finished = false;
                }
            }
        }
    }
    // std::cout << "anim: " << is_animating << "\n";
    // std::printf("Zort: %d %d\n", has_grown, fall_finished);
    if(not has_grown and fall_finished) {
        bool has_gap = fill();
        // std::cout << "bort: " << has_gap << "\n";
        if(not has_gap) {
            is_animating = match();
            // std::cout << "animate: " << is_animating << "\n";
        }
    }
}

#undef AT
