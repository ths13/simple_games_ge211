#include <ge211.h>

#include <vector>

using namespace ge211;
using namespace std;

// MODEL CONSTANTS

Dimensions const scene_dimensions{1024, 768};
int const min_x_coord{0};
int const max_x_coord{1024};
int const min_y_coord{0};
int const max_y_coord{768};

//maximum amount of food at once on screen
int const max_food{3};

// MODEL DATA DEFINITIONS

struct Food {
    using Position = Basic_position<int>;

    vector<Position> locs;

    static Position random(
            Random&,
            int min_x, int max_x,
            int min_y, int max_y);
};

struct Snake {
    using Position = Basic_position<int>;

    enum class Direction { up, left, down, right };

    Direction dir = Snake::Direction::left;
    vector<Position> segments;

    static Position random(
            Random&,
            int min_x, int max_x,
            int min_y, int max_y);
    void grow();
    void update();

};

struct Model {
    Food food;
    Snake snake;

    void add_random_food(Random&);
    void add_snake_start(Random&);
    void update();
};

// VIEW DATA DEFINITIONS

struct View {
    Circle_sprite food_sprite{5, Color::medium_magenta()};
    Circle_sprite snake_sprite{5, Color::medium_green()};
};

struct Simple_snake : Abstract_game {

    // Model
    Model model;

    // View
    View view;
    Dimensions initial_window_dimensions() const override;
    void draw(Sprite_set& sprites) override;

    // Controller
    bool game_start{true};
    bool is_growing{false};
    bool is_paused{false};
    void on_key(Key key) override;
    void on_frame(double dt) override;
};

int main() {
    Simple_snake{}.run();
}

// FUNCTION DEFINITIONS FOR MODEL

void Model::add_random_food(Random& rng) {
    while (food.locs.size() < max_food) {
        food.locs.push_back(Food::random(rng, min_x_coord, max_x_coord,
                                         min_y_coord, max_y_coord));
    }
}

void Model::add_snake_start(Random& rng) {
    snake.segments.push_back(Snake::random(rng, min_x_coord, max_x_coord,
                                           min_y_coord, max_y_coord));
}

void Model::update() {
    snake.update();
}

void Snake::update() {

    Basic_position<int> new_head = segments.front();

    switch (dir) {
        case Direction::down:
            segments.pop_back();
            new_head.y += 3;
            segments.insert(segments.begin(), new_head);
            break;
        case Direction::left:
            segments.pop_back();
            new_head.x -= 3;
            segments.insert(segments.begin(), new_head);
            break;
        case Direction::up:
            segments.pop_back();
            new_head.y -= 3;
            segments.insert(segments.begin(), new_head);
            break;
        case Direction::right:
            segments.pop_back();
            new_head.x += 3;
            segments.insert(segments.begin(), new_head);
            break;
    }
}

void Snake::grow() {
    if (is_growing) {
        Basic_position<int> new_head = segments.back();
        update();
        segments.push_back(new_head);
    }
}

Basic_position<int> Snake::random(
        Random& rng,
        int min_x, int max_x,
        int min_y, int max_y)
{
    int x_pos = rng.between(min_x, max_x);
    int y_pos = rng.between(min_y, max_y);
    Position pos{x_pos, y_pos};
    return pos;
}

Basic_position<int> Food::random(Random& rng,
                                 int min_x, int max_x,
                                 int min_y, int max_y)
{
    int x_pos = rng.between(min_x, max_x);
    int y_pos = rng.between(min_y, max_y);
    Position pos{x_pos, y_pos};
    return pos;
}

// FUNCTION DEFINITIONS FOR VIEW

Dimensions Simple_snake::initial_window_dimensions() const {
    return scene_dimensions;
}

void Simple_snake::draw(Sprite_set &sprites) {
    for (Basic_position<int> const& loc : model.food.locs) {
        sprites.add_sprite(view.food_sprite, loc);
    }
    for (Basic_position<int> const& pos : model.snake.segments) {
        sprites.add_sprite(view.snake_sprite, pos);
    }
}

// FUNCTION DEFINITIONS FOR CONTROLLER

void Simple_snake::on_key(Key key) {

    Snake::Direction cur_dir = model.snake.dir;

    if (key == Key::code('q')) {
        quit();
    } else if (key == Key::code('f')) {
        get_window().set_fullscreen(!get_window().get_fullscreen());
    } else if (key == Key::code('p')) {
        is_paused = !is_paused;
    } else if (key == Key::up() && cur_dir != Snake::Direction::down) {
        model.snake.dir = Snake::Direction::up;
    } else if (key == Key::down() && cur_dir != Snake::Direction::up) {
        model.snake.dir = Snake::Direction::down;
    } else if (key == Key::left() && cur_dir != Snake::Direction::right) {
        model.snake.dir = Snake::Direction::left;
    } else if (key == Key::right() && cur_dir != Snake::Direction::left) {
        model.snake.dir = Snake::Direction::right;
    }

    //temporary way to test how the snake grows
    if (key == Key::code('g')) {
        model.snake.grow();
        is_growing = !is_growing;
    }

}

void Simple_snake::on_frame(double dt)
{
    //currently not using `dt` in updating

    //initialize the game state
    if (game_start) {
        model.add_random_food(get_random());
        model.add_snake_start(get_random());
        game_start = !game_start;
    }
    if (!is_paused)
        model.update();
}
