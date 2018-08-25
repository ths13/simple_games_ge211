#include <ge211.h>

using namespace ge211;
using namespace std;

// MODEL CONSTANTS

Dimensions const scene_dimensions{1024, 768};
//double const min_x_coord{0};
//double const max_x_coord{1024};
//double const min_y_coord{0};
//double const max_y_coord{768};

// MODEL DATA DEFINITIONS

struct Snake {
    using Position = Basic_position<double>;

    Position body{10,10};
    //vector<Position> body;

//    static Snake random(
//            Random&,
//            double min_x, double max_x,
//            double min_y, double max_y);
};

struct Model {
    Snake snake;

//    void add_random(Random&);
};

// MAIN STRUCT AND FUNCTION

struct Simple_snake : Abstract_game {

    // Model
    Model model;

    // View
    Dimensions initial_window_dimensions() const override;
    void draw(Sprite_set& sprites) override;

//    model.add_random(get_random());
};

int main() {
    Simple_snake{}.run();
}

// FUNCTION DEFINITIONS FOR MODEL

//Snake Snake::random(Random& rng,
//                    double min_x, double max_x,
//                    double min_y, double max_y) {
//    double x_coord = rng.between(min_x, max_x);
//    double y_coord = rng.between(min_y, max_y);
//    Position pos{x_coord, y_coord};
//    return {pos};
//}

//void Model::add_random(Random& rng) {
//    snake = Snake::random(rng, min_x_coord, max_x_coord, min_y_coord, max_y_coord);
//}

// FUNCTION DEFINITIONS FOR VIEW

Dimensions Simple_snake::initial_window_dimensions() const {
    return scene_dimensions;
}

void Simple_snake::draw(Sprite_set& sprites)
{
    Circle_sprite snake_sprite{10, Color::medium_cyan()};
    sprites.add_sprite(snake_sprite, model.snake.body.into<int>());
}